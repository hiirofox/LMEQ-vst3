#pragma once

#include <JuceHeader.h>
#include "../dsp/eq.h"
#include "../dsp/complex.h"
#include "eqDisplayer.h"//FR_FreqWarping

complex_f32_t FR_OmegaWarping(complex_f32_t w);

class LMEQZeroPole :public juce::Component
{
private:
	LMEQ* eq = NULL;
	void drawZeroPoint(juce::Graphics& g, int x, int y, float thickness);
	void drawPolePoint(juce::Graphics& g, int x, int y, float thickness);
	int activtyNum = -1;
public:
	LMEQZeroPole();
	void init(LMEQ* eq);
	void paint(juce::Graphics&) override;
	void resized() override;
	void setActivtyNodeNum(int num);
};