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
 * Debug API *
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
**********************************************************/

SynthVoice::SynthVoice()
{
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
    for (int i = startSample; i < (startSample + numSamples); i++) {
        value = random.nextFloat() * 0.25f - 0.125f;
        value *= level;
        x.erase(x.begin());
        x.push_back(value);

        value = 0.0;
        for (int j = 0; j < x.size(); j++)
            value += x.at(j) * h1.at(j);
        for (int j = 0; j < y.size(); j++)
            value -= y.at(j) * h2.at(j);

        y.erase(y.begin());
        y.push_back(value);

        outputBuffer.addSample(0, i, value);
        outputBuffer.addSample(1, i, value);
    }
}

void SynthVoice::genFilter()
{
    /* clear vectors */
    h1.clear();
    h2.clear();
    x.clear();
    y.clear();
    
    /* generate corresponding filter */
    switch (mode) {
        case 0:
            genAllPass();
            break;
        case 1:
            genLowPass();
            break;
        case 2:
            genHighPass();
            break;
        case 3:
            genBandPass();
            break;
    }

}

void SynthVoice::genAllPass()
{
	/* initialize the first few input signal to 0 */
	for (int i = 0; i < order; i++)
		x.push_back(0);

    y.push_back(0);
    h2.push_back(0);

    /* Construct impulse response of All Pass Filter (FIR) */
    h1.push_back(1.0f);
    for (int i = 1; i < order; i++)
        h1.push_back(0.0f);
}

void SynthVoice::genLowPass()
{
	/* initialize the first few input signal to 0 */
	for (int i = 0; i < order; i++)
		x.push_back(0);

    y.push_back(0);
    h2.push_back(0);

    /* Construct impulse response of Low Pass Filter (FIR) */
	auto fc = f1 / getSampleRate();
	float impulse_response_sum = 0.0f;

	for (int i = -(order - 1) / 2; i <= order / 2; i++) {
		if (i != 0)
			h1.push_back(sin(2 * fc * i * MY_PI) / (2 * fc * i * MY_PI));
		else
			h1.push_back(1.0);
	}

	/* Windowed-Sinc Filter */
	for (int i = 0; i < order; i++) {
		h1.at(i) = h1.at(i) * w.at(i);
		impulse_response_sum += h1.at(i);
	}

	/* Normalized windowed-sinc filter */
	for (int i = 0; i < order; i++)
		h1.at(i) /= impulse_response_sum;
}

void SynthVoice::genHighPass()
{
	/* initialize the first few input signal to 0 */
	for (int i = 0; i < order; i++)
		x.push_back(0);

    y.push_back(0);
    h2.push_back(0);

    /* Construct impulse response of Low Pass Filter (FIR) first */
	auto fc = f1 / getSampleRate();
	float impulse_response_sum = 0.0f;

	for (int i = -(order - 1) / 2; i <= order / 2; i++) {
		if (i != 0)
			h1.push_back(sin(2 * fc * i * MY_PI) / (2 * fc * i * MY_PI));
		else
			h1.push_back(1.0);
	}

	/* Windowed-Sinc Filter */
	for (int i = 0; i < order; i++) {
		h1.at(i) = h1.at(i) * w.at(i);
		impulse_response_sum += h1.at(i);
	}

    /*for (int i = 0; i < order; ++i) {
        OutputDebugPrintf("(%d, %f)", i - order / 2, h.at(i));
        if (i == order - 1)
            OutputDebugPrintf("\n");
        else
            OutputDebugPrintf(", ");
    }*/

	/* Normalized windowed-sinc filter */
	for (int i = 0; i < order; i++)
		h1.at(i) /= impulse_response_sum;

	/* Then using spectral inversion to convert it into a high-pass one. */
	for (int i = 0; i < order; i++)
		h1.at(i) *= -1;
	h1.at(order / 2) += 1;
}

void SynthVoice::genBandPass()
{
	/* initialize the first few input signal to 0 */
	for (int i = 0; i < 2 * order - 1; i++)
		x.push_back(0);

    y.push_back(0);
    h2.push_back(0);

	/* Construct impulse response of two Low Pass Filter (FIR) first */
	std::vector<float> low, high;
	auto fh = f1 / getSampleRate();
	auto fl = f2 / getSampleRate();
	float impulse_response_sum_LPF = 0.0f;
	float impulse_response_sum_HPF = 0.0f;

	for (int i = -(order - 1) / 2; i <= order / 2; i++) {
		if (i != 0) {
			low.push_back (sin(2 * fl * i * MY_PI) / (2 * fl * i * MY_PI));
			high.push_back(sin(2 * fh * i * MY_PI) / (2 * fh * i * MY_PI));
		} else {
			low.push_back(1.0);
			high.push_back(1.0);
		}
	}

	/* Windowed-Sinc Filter */
	for (int i = 0; i < order; i++) {
		low.at(i)  *= w.at(i);
		high.at(i) *= w.at(i);
		impulse_response_sum_LPF += low.at(i);
		impulse_response_sum_HPF += high.at(i);
	}

	/* Normalized windowed-sinc filter */
	for (int i = 0; i < order; i++) {
		low.at(i)  /= impulse_response_sum_LPF;
		high.at(i) /= impulse_response_sum_HPF;
	}

	/* Using spectral inversion to convert LPF into a high-pass one. */
	for (int i = 0; i < order; i++)
		high.at(i) *= -1;
	high.at(order / 2) += 1;

	/* Create a BPF by convolving the two filters.(LPF and HPF) */
	for (int i = 0; i < 2 * order - 1; i++)
		h1.push_back(0.0);

	for (int i = 0; i < 2 * order - 1; i++) {
		const int jmn = (i >= order - 1) ? i - (order - 1) : 0;
		const int jmx = (i >= order - 1) ? order - 1       : i;
		for (int j = jmn; j <= jmx; j++) {
			h1.at(i) += (low.at(j) * high.at(i - j));
		}
	}
}

void SynthVoice::setLevel(float newLevel)
{
    level = newLevel;
}

void SynthVoice::setMode(int newMode)
{
    mode = newMode;
    genFilter();
}

void SynthVoice::setOrder(int newOrder)
{
	order = newOrder;

	/* blackman window */
    w.clear();
	for (int i = 0; i < order; i++)
		w.push_back(0.42 - 0.5 * cos(2 * MY_PI * i / (order - 1)) + 0.08 * cos(4 * MY_PI * i / (order - 1)));

    genFilter();
}

void SynthVoice::setF1(float newF1)
{
	f1 = newF1;
    if (mode != 0)
        genFilter();
}

void SynthVoice::setF2(float newF2)
{
    f2 = newF2;
    if (mode == 3)
        genFilter();
}