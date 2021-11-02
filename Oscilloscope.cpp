/*
  ==============================================================================

    Oscilloscope.cpp
    Created: 17 Oct 2021 1:16:28pm
    Author:  robert

  ==============================================================================
*/

#include <JuceHeader.h>
#include "Oscilloscope.h"

Oscilloscope::Oscilloscope(MyAudioProcessor &p):
    audioProcessor(p),
    waveform(p.getSingleChannelSampleFifo())
{
    pathBuffer.setSize(1, 1024);
    startTimerHz(30);
}

Oscilloscope::~Oscilloscope()
{

}

void Oscilloscope::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colour(50, 50, 50));
    g.setColour(juce::Colours::grey);
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 3, 3);
    g.setColour(juce::Colours::white);
    g.strokePath(waveformPath, juce::PathStrokeType(1.f));
}

void Oscilloscope::resized()
{

}

void Oscilloscope::timerCallback()
{
    generateWaveform();
    repaint();
}

void Oscilloscope::generateWaveform()
{
    juce::AudioBuffer<float> temp;
    while (waveform.getNumCompleteBuffersAvailable() > 0) {
        if (waveform.getAudioBuffer(temp)) {
            auto size = temp.getNumSamples();
            juce::FloatVectorOperations::copy(
                pathBuffer.getWritePointer(0, 0),
                pathBuffer.getReadPointer(0, size),
                pathBuffer.getNumSamples() - size
            );
            juce::FloatVectorOperations::copy(
                pathBuffer.getWritePointer(0, pathBuffer.getNumSamples() - size),
                temp.getReadPointer(0, 0),
                size
            );
            pathProducer.generatePath(pathBuffer, getLocalBounds().toFloat());
        }
    }
    while (pathProducer.getNumPathsAvailable() > 0) {
        pathProducer.getPath(waveformPath);
    }
}
