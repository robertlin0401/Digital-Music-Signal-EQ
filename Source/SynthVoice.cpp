/*
  ==============================================================================

    SynthVoice.cpp
    Created: 4 Oct 2021 1:57:03am
    Author:  robert

  ==============================================================================
*/

#include "SynthVoice.h"

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
