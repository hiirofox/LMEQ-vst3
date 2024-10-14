/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "dsp/biquad.h"
#include "dsp/eq.h"
#include "dsp/linearEQ.h"

#define MaxFFTLength 2048

//==============================================================================
/**
*/
class LmeqAudioProcessor : public juce::AudioProcessor,juce::Timer
{
public:


	//==============================================================================
	LmeqAudioProcessor();
	~LmeqAudioProcessor() override;

	//==============================================================================
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

	//==============================================================================
	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;

	//==============================================================================
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;

	//==============================================================================
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram(int index) override;
	const juce::String getProgramName(int index) override;
	void changeProgramName(int index, const juce::String& newName) override;

	//==============================================================================
	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

	//==============================================================================
	juce::AudioProcessorValueTreeState& GetParams()
	{
		return Params;
	}

	LMEQ eq;
	LinearEQ linearEQ;
	void timerCallback() override;//����linearEQ��
	int isEQInit = 0;
	float fftData[MaxFFTLength];
	int fftPos = 0;

	int isEnableLinear = 0;

	int procLoopCount = 0;
private:
	//Synth Param
	static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
	juce::AudioProcessorValueTreeState Params{ *this, nullptr, "Parameters", createParameterLayout() };
	//==============================================================================
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LmeqAudioProcessor)
};
