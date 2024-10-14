#pragma once

#include <JuceHeader.h>
#include <vector>

#define MaxDataLen 512
class DataDisplayer :public juce::Component, juce::Timer
{
private:
	struct LineData
	{
		LineData();
		std::vector<float> data;

		int displayMode;
		juce::Colour col;
		float minv, maxv;
		float* dataPtr = NULL;
	};
	int pos;
	std::vector<LineData> lines;
	float zoom;

public:
	DataDisplayer();
	void paint(juce::Graphics&) override;
	void resized() override;
	void timerCallback() override;
	void setTimer(int Hz);
	void addLine(int id, int displayMode, float minv, float maxv, juce::Colour col);
	void setLine(int id, float data);
	void setLineAutoUpdata(int id, float* dataPtr);
	void nextLine();
};

class Oscillator :public juce::Component
{
private:
	float dat[10000];
	int datlen = 0;
	int mode;
	float yZoom;
public:
	void paint(juce::Graphics&) override;
	void resized() override;
	void setData(float* dat, int datlen);
	void setDisplayMode(int mode = 0);//0线性，1对数
	void setYZoom(float yZoom = 1.0);
};