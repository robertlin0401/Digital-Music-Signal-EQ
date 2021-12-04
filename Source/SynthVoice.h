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
    
	int getMode() { return mode; }
	int getOrder() { return order; }
	float getF1() { return f1; }
	float getF2() { return f2; }
	float getQ() { return Q; }
	float getGain() { return gain; }

    void setLevel(float newLevel);
    void setMode(int newMode);
	void setOrder(int newOrder);
	void setF1(float newF1);
	void setF2(float newF2);
	void setQ(float newQ);
	void setGain(float newGain);
    
private:
    void genFilter();
    void genAllPass();
    void genLowPass();
    void genHighPass();
    void genBandPass();
    void genOthers();
    
    float level;
    int mode;
	int order;
	float f1, f2;
    float Q;
    float gain;

    juce::Random random;

	std::vector <float> h1, h2;  // impulse response
	std::vector <float> x;       // input signal
	std::vector <float> y;       // output signal
	std::vector <float> w;       // window function
};
