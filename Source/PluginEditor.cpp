/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
LmeqAudioProcessorEditor::LmeqAudioProcessorEditor(LmeqAudioProcessor& p)
	: AudioProcessorEditor(&p), audioProcessor(p)
{
	// Make sure that before the constructor has finished, you've set the
	// editor's size to whatever you need it to be.
	setResizable(true, true); // 允许窗口调整大小

	//setResizeLimits(64 * 11, 64 * 5, 10000, 10000); // 设置最小宽高为300x200，最大宽高为800x600
	setSize(64 * 11, 64 * 4 + 64 + 48 * 2);

	//constrainer.setFixedAspectRatio(11.0 / 4.0);  // 设置为16:9比例
	//setConstrainer(&constrainer);  // 绑定窗口的宽高限制


	if (audioProcessor.isEQInit == 0)
	{
		displayEQ.init(&audioProcessor.eq, 8);
		audioProcessor.isEQInit = 1;
	}
	else
	{
		displayEQ.setData(&audioProcessor.eq, 8);
	}
	//displayZeroPole.init(&audioProcessor.eq);
	addAndMakeVisible(displayEQ);
	//addAndMakeVisible(displayZeroPole);

	//B_LINEAR.setName("Linear");
	//B_LINEAR.setClickedCallback([&]() {
	/*	if (B_LINEAR.getButtonState())
		{
			audioProcessor.isEnableLinear = 1;
			DBG("button on.");
		}
		else
		{
			audioProcessor.isEnableLinear = 0;
			DBG("button off.");
		}
		});*/

		//addAndMakeVisible(B_LINEAR);

		//S_TotalGain.setText("GAIN");
		//S_TotalGain.ParamLink(audioProcessor.GetParams(), "gain");
		//addAndMakeVisible(S_TotalGain);

	startTimerHz(30);

}

LmeqAudioProcessorEditor::~LmeqAudioProcessorEditor()
{
}

//==============================================================================
void LmeqAudioProcessorEditor::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colour(0x00, 0x00, 0x00));

	g.fillAll();
	g.setFont(juce::Font("FIXEDSYS", 16.0, 1));
	g.setColour(juce::Colour(0xffffffff));;

	int w = getBounds().getWidth(), h = getBounds().getHeight();
	juce::String str = audioProcessor.isEnableLinear ?
		"Mode=Linear (Switch modes by pressing the middle mouse button.)" :
		"Mode=Biquad (Switch modes by pressing the middle mouse button.)";

	g.drawText(str, 48, 16, w, 32, 1);
	//g.drawText("L-MODEL Equaliser V1.0", 48, 0, w, 16,1);
}

void LmeqAudioProcessorEditor::resized()
{
	juce::Rectangle<int> bound = getBounds();
	int x = bound.getX(), y = bound.getY(), w = bound.getWidth(), h = bound.getHeight();

	//displayEQ.setBounds(juce::Rectangle<int>::leftTopRightBottom(48, 48, getBounds().getWidth() - getBounds().getHeight() + 64 + 48 * 2, getBounds().getHeight() - 48 - 64 - 48 * 2));
	//displayZeroPole.setBounds(juce::Rectangle<int>::leftTopRightBottom(getBounds().getWidth() - getBounds().getHeight() + 64, 48, getBounds().getWidth() - 48, getBounds().getHeight() - 48 - 64 - 48 * 2));
	auto convXY = juce::Rectangle<int>::leftTopRightBottom;
	displayEQ.setBounds(convXY(48, 48, w - 48, h - 48));
	//B_LINEAR.setBounds(w - 64 - 48, h - 48 - 64, 64, 32);

	//S_TotalGain.setBounds(32, h - 64 - 64, 64, 64);
}

#define MaxFRDatLen (32768)
void LmeqAudioProcessorEditor::timerCallback()
{
	displayEQ.fillFFTApplyWindow(audioProcessor.fftData, audioProcessor.fftPos);
	displayZeroPole.setActivtyNodeNum(displayEQ.getActiveNodeNum());
	audioProcessor.fftPos = 0;//归零一下pos

	//if (audioProcessor.isEnableLinear)
	//{
	//	audioProcessor.linearEQ.updata();//更新一下LinearEQ!!!
	//}

	audioProcessor.isEnableLinear = displayEQ.getLinearMode();
	repaint();
}
