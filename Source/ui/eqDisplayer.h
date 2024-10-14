#pragma once

#include <JuceHeader.h>
#include "../dsp/eq.h"
#include "../dsp/fft.h"

#define MaxFFTLength 2048

float FR_FreqWarping(float linear_f,float freqRange);//������ת�����׵Ĳ�������

class LMEQDisplayer :public juce::Component
{
private:
	LMEQ* eq = NULL;//��EQָ���
	float yZoom = 0.144, accuracy = 0.75;
	int isNodeActivty = 0, activtyNum = -1;

	void updataEQ();
	
	complex_f32_t fftData[MaxFFTLength];//���Ƶ�����
	float fftDisplayData[MaxFFTLength];//��������ʾ�ģ������һ���˲�����
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

	void mouseDown(const juce::MouseEvent& event) override; //��д��갴�µĻص�
	void mouseDrag(const juce::MouseEvent& event) override; // ��д����϶��Ļص�
	void mouseUp(const juce::MouseEvent& event) override; // ��д����ͷŵĻص�
	//void mouseMove(const juce::MouseEvent& event) override; // ��д����ƶ��Ļص�
	void mouseDoubleClick(const juce::MouseEvent& event) override; // ��д���˫���Ļص�
	void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;//������

	int getLinearMode();

	int getActiveNodeNum();
};