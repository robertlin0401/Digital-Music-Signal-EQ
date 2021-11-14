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
# define MY_PI 3.14159265358979323846

SynthVoice::SynthVoice() {

	// Construct impulse response
	cutoff = 5000.0;
	fc = cutoff / getSampleRate();
	impulse_response_sum = 0.0;

	for (int i = -15; i < 16; i++)
	{
		if (i != 0) {
			impulse_response =  sin(2*fc*i*MY_PI) / (2*fc*i*MY_PI);
			h.push_back(impulse_response);
			impulse_response_sum += impulse_response;
		}
		else {
			h.push_back(1.0);
			impulse_response_sum += 1.0;
		}
	}

	// Initial the first few input signal to 0
	for (int i = 0; i < 31; i++) {
		x.push_back(0);		
	}
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    noteMidiNumber = midiNoteNumber;
    frequency = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber);
    currentAngle = 0.f;
    angleIncrement = frequency / getSampleRate() * juce::MathConstants<float>::twoPi;
    tailOff = 0.0;
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    if (allowTailOff) {
        if (tailOff == 0.0)
            tailOff = 1.0;
    } else {
        clearCurrentNote();
        level = 0;
        currentAngle = 0.f;
    }
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
			for (int j = 0; j < 31; j++)
			{
				value += x.at(j) * h.at(j);
			}
			value /= impulse_response_sum;
            outputBuffer.addSample(0, i, value);
            outputBuffer.addSample(1, i, value);
        }
}

void SynthVoice::setLevel(float newLevel)
{
    level = newLevel;
}

void SynthVoice::setMode(int newMode)
{
    mode = newMode;
}
