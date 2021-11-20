/*
  ==============================================================================

    SynthVoice.cpp
    Created: 4 Oct 2021 1:57:03am
    Author:  robert

  ==============================================================================
*/

#include "SynthVoice.h"
#include <vector>
#include <cmath>
#define MY_PI 3.14159265358979323846

/***********************************************************
 * Debug API */
#include "atlbase.h"
#include "atlstr.h"
void OutputDebugPrintf(const char *strOutputString, ...)
{
    char strBuffer[4096] = {0};
    va_list vlArgs;
    va_start(vlArgs, strOutputString);
    _vsnprintf_s(strBuffer, sizeof(strBuffer) - 1, strOutputString, vlArgs);
    va_end(vlArgs);
    OutputDebugString(strBuffer);
}
/**********************************************************/

SynthVoice::SynthVoice()
{
	cutoff = 5000.0f;
	order = 15;

    genFilter();
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    // frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    // tailOff = 0.0;
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    // if (allowTailOff) {
    //     if (tailOff == 0.0)
    //         tailOff = 1.0;
    // } else {
    //     clearCurrentNote();
    // }
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
    // handle pitch wheel moved midi event
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
    // handle midi control change
}

void SynthVoice::renderNextBlock(juce::AudioBuffer <float> &outputBuffer, int startSample, int numSamples)
{
    float value;
    if (mode == 0) // without filter
        for (int i = startSample; i < (startSample + numSamples); i++) {
            value = random.nextFloat() * 0.25f - 0.125f;
            value *= level;
            outputBuffer.addSample(0, i, value);
            outputBuffer.addSample(1, i, value);
        }
    else if (mode == 1) // with filter
        for (int i = startSample; i < (startSample + numSamples); i++) {
            value = random.nextFloat() * 0.25f - 0.125f;
            value *= level;
			x.erase(x.begin());
			x.push_back(value);

			value = 0.0;
			for (int j = 0; j < order; j++)
				value += x.at(j) * h.at(j);
            
            outputBuffer.addSample(0, i, value);
            outputBuffer.addSample(1, i, value);
        }
}

void SynthVoice::genFilter()
{
    // clear vectors
    x.clear();
    h.clear();
    window.clear();

	// Initialize the first few input signal to 0
	for (int i = 0; i < order; i++)
		x.push_back(0);
    
	// Construct impulse response of Low Pass Filter (FIR)
	fc = cutoff / getSampleRate();
	float impulse_response_sum = 0.0f;

	for (int i = -(order - 1) / 2; i <= order / 2; i++) {
		if (i != 0)
			h.push_back(sin(2 * fc * i * MY_PI) / (2 * fc * i * MY_PI));
		else
			h.push_back(1.0);
	}

	// Construct a Blackman Window
	for (int i = 0; i < order; i++)
		window.push_back(0.42 - 0.5 * cos(2 * MY_PI * i / (order - 1)) + 0.08 * cos(4 * MY_PI * i / (order - 1))) ;

	// Windowed-Sinc Filter
	for (int i = 0; i < order; i++) {
		h.at(i) = h.at(i) * window.at(i);
		impulse_response_sum += h.at(i);
	}

	// Normalized windowed-sinc filter
	for (int i = 0; i < order; i++)
		h.at(i) /= impulse_response_sum;
}

void SynthVoice::setLevel(float newLevel)
{
    level = newLevel;
}

void SynthVoice::setMode(int newMode)
{
    mode = newMode;
}

void SynthVoice::setOrder(int newOrder)
{
	order = newOrder;
    genFilter();
}

void SynthVoice::setCutoff(float newCutoff)
{
	cutoff = newCutoff;
    genFilter();
}
