/*
  ==============================================================================

    SynthVoice.h
    Created: 4 Oct 2021 1:57:03am
    Author:  robert

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "SynthSound.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
	SynthVoice();

    bool canPlaySound(juce::SynthesiserSound* sound) override;
    
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    
    void stopNote(float velocity, bool allowTailOff) override;

    void pitchWheelMoved(int newPitchWheelValue) override;
    
    void controllerMoved(int controllerNumber, int newControllerValue) override;
   
    void renderNextBlock(juce::AudioBuffer <float> &outputBuffer, int startSample, int numSamples) override;

    void genFilter();
    
	int getOrder() { return order; }
	float getCutoff() { return cutoff; }

    void setLevel(float newLevel);
    void setMode(int newMode);
	void setOrder(int newOrder);
	void setCutoff(float newCutoff);
    
private:
    juce::Random random;
    float level;
    int mode;
	int order;
	float cutoff, fc; // cutoff(Hz), fc = cutoff/sampleRate

	std::vector <float> h;  // impulse response
	std::vector <float> x;  // input signal
	std::vector <float> window;
};
