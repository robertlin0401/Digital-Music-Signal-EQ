/*
  ==============================================================================

    Oscilloscope.h
    Created: 17 Oct 2021 1:16:28pm
    Author:  robert

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "FIFO.h"

class Oscilloscope : public juce::Component, public juce::Timer
{
public:
    Oscilloscope(MyAudioProcessor &);
    ~Oscilloscope() override;

    void paint(juce::Graphics &) override;
    void resized() override;
    void timerCallback() override;

private:
    MyAudioProcessor &audioProcessor;
    SingleChannelSampleFifo<juce::AudioBuffer<float>> &waveform;
    juce::AudioBuffer<float> pathBuffer;
    PathProducer<juce::Path> pathProducer;
    juce::Path waveformPath;

    void generateWaveform();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Oscilloscope);
};
