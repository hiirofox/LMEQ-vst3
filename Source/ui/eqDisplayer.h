#pragma once

#include <JuceHeader.h>
#include "../dsp/eq.h"
#include "../dsp/fft.h"

#define MaxFFTLength 2048

float FR_FreqWarping(float linear_f,float freqRange);//线性谱转对数谱的补偿函数

class LMEQDisplayer :public juce::Component
{
private:
	LMEQ* eq = NULL;//存EQ指针的
	float yZoom = 0.144, accuracy = 0.75;
	int isNodeActivty = 0, activtyNum = -1;

	void updataEQ();
	
	complex_f32_t fftData[MaxFFTLength];//变成频域的了
	float fftDisplayData[MaxFFTLength];//存用来显示的，本身带一点滤波功能
	int fftPos = 0;

	float freqRange = 6.0;

	int isEnableLinear = 0;
public:
	LMEQDisplayer();
	void init(LMEQ* eq, int nodeN);
	void setData(LMEQ* eq, int nodeN);
	void paint(juce::Graphics&) override;
	void resized() override;
	void setYZoom(float yZoom = 0.1);
	void setAccuracy(float accuracy = 0.75);

	void setFreqRange(float freqRange);

	void fillFFTApplyWindow(float *data,int length);

	void mouseDown(const juce::MouseEvent& event) override; //重写鼠标按下的回调
	void mouseDrag(const juce::MouseEvent& event) override; // 重写鼠标拖动的回调
	void mouseUp(const juce::MouseEvent& event) override; // 重写鼠标释放的回调
	//void mouseMove(const juce::MouseEvent& event) override; // 重写鼠标移动的回调
	void mouseDoubleClick(const juce::MouseEvent& event) override; // 重写鼠标双击的回调
	void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;//鼠标滚轮

	int getLinearMode();

	int getActiveNodeNum();
};