/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "ui/LM_slider.h"
#include "ui/dataDisplayer.h"
#include "ui/eqDisplayer.h"
#include "ui/eqZeroPole.h"
#include "dsp/biquad.h"
#include "dsp/eq.h"
//==============================================================================
/**
*/
class LmeqAudioProcessorEditor : public juce::AudioProcessorEditor, juce::Timer
{
public:
	LmeqAudioProcessorEditor(LmeqAudioProcessor&);
	~LmeqAudioProcessorEditor() override;

	//==============================================================================
	void paint(juce::Graphics&) override;
	void resized() override;
	void timerCallback() override;//注意，这里面不定期会给fft填东西。

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	LmeqAudioProcessor& audioProcessor;

	LMEQDisplayer displayEQ;
	LMEQZeroPole displayZeroPole;

	juce::ComponentBoundsConstrainer constrainer;  // 用于设置宽高比例
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LmeqAudioProcessorEditor)
};

