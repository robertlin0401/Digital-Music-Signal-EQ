/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

MyAudioProcessorEditor::MyAudioProcessorEditor(MyAudioProcessor& p)
    : AudioProcessorEditor(&p), 
      audioProcessor(p),
      sliderContainer(p),
      waveform(p),
      spectrum(p)
{
    setSize(350, 600);

    comboBox.addItem("sine", 1);
    comboBox.addItem("square", 2);
    comboBox.addItem("triangle", 3);
    comboBox.addItem("sawtooth", 4);
    modeComboBoxAttachment.reset(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(
        audioProcessor.tree, "mode", comboBox
    ));

    for (auto &subcomponent : subcomponents) {
        addAndMakeVisible(subcomponent);
    }
}

MyAudioProcessorEditor::~MyAudioProcessorEditor()
{
}

void MyAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Our component is opaque, so we must completely fill the background with a solid colour
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MyAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::column;
    flexBox.items.add(juce::FlexItem(sliderContainer).withFlex(2.0f));
    flexBox.items.add(juce::FlexItem(comboBox).withFlex(0.5f));
    flexBox.items.add(juce::FlexItem(waveform).withFlex(2.0f));
    flexBox.items.add(juce::FlexItem(spectrum).withFlex(3.0f));
    flexBox.performLayout(area.reduced(10));
    for (auto &subcomponent : subcomponents) {
        subcomponent->setBounds(subcomponent->getBounds().reduced(3));
    }
}
