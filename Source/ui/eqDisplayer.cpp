#include "eqDisplayer.h"


LMEQDisplayer::LMEQDisplayer()
{
	memset(fftDisplayData, 0, sizeof(fftDisplayData));
	memset(fftData, 0, sizeof(fftData));
}

void LMEQDisplayer::init(LMEQ* eq, int nodeN)
{
	this->eq = eq;
	eq->nodeN = nodeN;
	//LMEQInit(eq, nodeN);
	eq->nodes[0].type = 4;
	eq->nodes[nodeN - 1].type = 5;
	for (int i = 1; i < nodeN - 1; ++i)
		eq->nodes[i].type = 3;
	for (int i = 0; i < nodeN; ++i)
	{
		eq->nodePosX[i] = (float)(i + 1) / (nodeN + 1);
		eq->nodePosY[i] = 0.5;
		eq->wheelValue[i] = 0.25;
		//eq->nodes[i].ctof = (float)(i + 1) / (nodeN + 1);
		//eq->nodes[i].reso = 0.707;
		//eq->nodes[i].gain = 1.0;
	}
	updataEQ();
}
void LMEQDisplayer::setData(LMEQ* eq, int nodeN)
{
	this->eq = eq;
	eq->nodeN = nodeN;
	//LMEQInit(eq, nodeN);
	updataEQ();
}

void LMEQDisplayer::paint(juce::Graphics& g)
{
	juce::Rectangle<int> bound = getBounds();
	int w = bound.getWidth(), h = bound.getHeight();

	if (eq == NULL)return;

	const int gridLineNum = 6;
	const int gridLineNum2 = 12;
	for (int i = 0; i < gridLineNum; ++i) {
		float pos = (float)(i + 1) / (gridLineNum);
		for (int j = 0; j < gridLineNum2; ++j)//x轴
		{
			float posx = pos - (expf(j * 0.15) * freqRange / 6.0 - 1) * 0.05;
			posx = atanf(posx * M_PI / 2.0);//补偿
			int col = (gridLineNum2 - j + 1) * 180 / 3 / (gridLineNum2 + 1);
			g.setColour(juce::Colour(col, col, col));
			g.drawLine(posx * w, 0, posx * w, h, 1);
		}
		g.setColour(juce::Colour(0xaa777777));//y轴
		g.drawLine(0, pos * h, w, pos * h, 1);
	}

	//此处绘制频谱
	g.setColour(juce::Colour(0x5500ffff));
	float lasty = 0;
	for (int i = 1; i < MaxFFTLength; ++i)
	{
		float y = h - fftDisplayData[i] * h;
		if (y > h)y = h;
		else if (y < 0)y = 0;
		g.drawLine((float)(i - 1) / MaxFFTLength * w, lasty, (float)i / MaxFFTLength * w, y);
		lasty = y;
	}

	//绘制总的频响
	g.setColour(juce::Colour(0x77, 0x77, 0xff));
	lasty = 0;
	int datlen = accuracy * w;
	for (int i = 1; i < datlen; ++i)
	{
		float x = FR_FreqWarping((float)i / datlen, freqRange);
		complex_f32_t fr = LMEQGetFreqResponse(eq, x * M_PI);
		float r = sqrtf(fr.re * fr.re + fr.im * fr.im);
		r = logf(r);

		float y = h / 2 - r * h * yZoom;
		g.drawLine((float)(i - 1) / datlen * w, lasty, (float)i / datlen * w, y, 3.0);
		lasty = y;
	}

	//绘制选中的频响
	if (activtyNum != -1)//如果选中，就画一下那个传递函数
	{
		g.setColour(juce::Colour(0xaa77ff77));
		lasty = 0;
		for (int i = 1; i < datlen; ++i)
		{
			float x = FR_FreqWarping((float)i / datlen, freqRange);
			complex_f32_t fr = biquadGetFreqResponse(&eq->nodes[activtyNum], x * M_PI);
			float r = sqrtf(fr.re * fr.re + fr.im * fr.im);
			r = logf(r);

			float y = h / 2 - r * h * yZoom;
			g.drawLine((float)(i - 1) / datlen * w, lasty, (float)i / datlen * w, y, 1.0);
			lasty = y;
		}
	}

	//画拖动的环环
	for (int i = 0; i < eq->nodeN; ++i)//画一下控件
	{
		int x = eq->nodePosX[i] * w;
		int y = eq->nodePosY[i] * h;
		if (eq->nodes[i].type != 3)
		{
			g.setColour(juce::Colour(0xffff00ff));
		}
		else
		{
			g.setColour(juce::Colour(0xff00ff00));
		}
		g.drawEllipse(x - 6, y - 6, 12, 12, 2);
	}

	g.setColour(juce::Colour(0x00, 0xff, 0x00));
	g.drawLine(0, 0, w, 0, 3);
	g.drawLine(0, 0, 0, h, 3);
	g.drawLine(w, h, w, 0, 3);
	g.drawLine(w, h, 0, h, 3);
}

void LMEQDisplayer::resized()
{
}

void LMEQDisplayer::setYZoom(float yZoom)
{
	this->yZoom = yZoom;
}

void LMEQDisplayer::setAccuracy(float accuracy)
{
	this->accuracy = accuracy;
}

void LMEQDisplayer::setFreqRange(float freqRange)
{
	this->freqRange = freqRange;
}

void LMEQDisplayer::fillFFTApplyWindow(float* data, int length)/////////////////////
{
	for (int i = 0; i < MaxFFTLength; ++i)//清空一下
	{
		fftData[i].re = 0;
		fftData[i].im = 0;
	}
	if (length > MaxFFTLength)length = MaxFFTLength;
	for (int i = 0; i < length; ++i)
	{
		fftData[i].re = data[i] * (1.0 - cosf((float)i / length * 2.0 * M_PI)) * 5.0;//加余弦窗防止频谱泄露
		fftData[i].im = 0;
	}
	fft_f32(fftData, MaxFFTLength, 1.0);
	//现在是频域了
	for (int i = 0; i < MaxFFTLength; ++i)//拷贝一下数据
	{
		fftData[i].re = logf(sqrtf(fftData[i].re * fftData[i].re + fftData[i].im * fftData[i].im) + 0.00000000000001f) * 0.15 + 0.25;//把幅度信息保留到实部
	}
	for (int i = 0; i < MaxFFTLength; ++i)
	{
		int pos2 = FR_FreqWarping((float)i / MaxFFTLength, freqRange) * MaxFFTLength;
		if (pos2 > MaxFFTLength)pos2 = MaxFFTLength;
		else if (pos2 < 0)pos2 = 0;
		fftDisplayData[i] += 0.5 * (fftData[pos2 / 2].re - fftDisplayData[i]);//给频谱一点滤波
	}
	for (int i = 2; i < MaxFFTLength - 2; ++i)//卷积滤波过渡一下
	{
		fftDisplayData[i] = fftDisplayData[i - 2] + fftDisplayData[i - 1] + fftDisplayData[i] + fftDisplayData[i + 1] + fftDisplayData[i + 2];
		fftDisplayData[i] /= 5.0;
	}
}

void LMEQDisplayer::mouseDown(const juce::MouseEvent& event)
{
	activtyNum = -1;
	for (int i = 0; i < eq->nodeN; ++i)
	{
		float dx = event.position.getX() - eq->nodePosX[i] * getBounds().getWidth();
		float dy = event.position.getY() - eq->nodePosY[i] * getBounds().getHeight();
		if (dx * dx + dy * dy <= 36)
		{
			isNodeActivty = 1;
			activtyNum = i;
			break;
		}
	}
	if (isNodeActivty)
	{
		//lastMousePosition = event.source.getScreenPosition();
		setMouseCursor(juce::MouseCursor::NoCursor);
	}

	if (event.mods.isMiddleButtonDown())
	{
		isEnableLinear = !isEnableLinear;
	}
}

void LMEQDisplayer::mouseUp(const juce::MouseEvent& event)
{
	if (isNodeActivty)
	{
		//juce::Desktop::getInstance().getMainMouseSource().setScreenPosition(lastMousePosition);
		setMouseCursor(juce::MouseCursor::NormalCursor);
	}
	isNodeActivty = 0;
}
void LMEQDisplayer::mouseDrag(const juce::MouseEvent& event)
{
	if (event.mods.isLeftButtonDown())
	{
		//左键拖动
		if (isNodeActivty)
		{
			float x = event.position.getX();
			float y = event.position.getY();
			if (x < 0)x = 0;
			if (x > getBounds().getWidth())x = getBounds().getWidth();
			if (y < 0)y = 0;
			if (y > getBounds().getHeight())y = getBounds().getHeight();
			eq->nodePosX[activtyNum] = x / getBounds().getWidth();
			eq->nodePosY[activtyNum] = y / getBounds().getHeight();
			updataEQ();
		}
	}
}
void LMEQDisplayer::mouseDoubleClick(const juce::MouseEvent& event)
{
	if (event.mods.isLeftButtonDown())//左键双击就重置滤波器
	{
		for (int i = 0; i < eq->nodeN; ++i)
		{
			float dx = event.position.getX() - eq->nodePosX[i] * getBounds().getWidth();
			float dy = event.position.getY() - eq->nodePosY[i] * getBounds().getHeight();
			if (dx * dx + dy * dy <= 36)
			{
				eq->nodePosX[i] = (float)(i + 1) / (eq->nodeN + 1);
				eq->nodePosY[i] = 0.5;
				eq->wheelValue[i] = 0.25;
				updataEQ();
				break;
			}
		}
	}
	else if (event.mods.isRightButtonDown())//右键双击只重置reso
	{
		for (int i = 0; i < eq->nodeN; ++i)
		{
			float dx = event.position.getX() - eq->nodePosX[i] * getBounds().getWidth();
			float dy = event.position.getY() - eq->nodePosY[i] * getBounds().getHeight();
			if (dx * dx + dy * dy <= 36)
			{
				eq->wheelValue[i] = 0.25;
				updataEQ();
				break;
			}
		}
	}
}
void LMEQDisplayer::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
	int isUpdata = 0;
	for (int i = 0; i < eq->nodeN; ++i)
	{
		float dx = event.position.getX() - eq->nodePosX[i] * getBounds().getWidth();
		float dy = event.position.getY() - eq->nodePosY[i] * getBounds().getHeight();
		if (dx * dx + dy * dy <= 36)
		{
			eq->wheelValue[i] += wheel.deltaY * 0.1;
			if (eq->wheelValue[i] < 0)eq->wheelValue[i] = 0;
			if (eq->wheelValue[i] > 1)eq->wheelValue[i] = 1;
			isUpdata = 1;
			break;
		}
	}
	if (isUpdata)
	{
		updataEQ();
	}
}

int LMEQDisplayer::getLinearMode()
{
	return isEnableLinear;
}

int LMEQDisplayer::getActiveNodeNum()
{
	return activtyNum;
}

void LMEQDisplayer::updataEQ()
{
	//首先把控件pos转换成biquad的ctof，reso和gain
	for (int i = 0; i < eq->nodeN; ++i)
	{
		eq->nodes[i].ctof = FR_FreqWarping(eq->nodePosX[i], freqRange);
		if (eq->nodes[i].type == 1) eq->nodes[i].gain = exp((0.5 - eq->nodePosY[i]) * 3.5);
		if (eq->nodes[i].type == 2) eq->nodes[i].gain = exp((0.5 - eq->nodePosY[i]) * 3.5);
		if (eq->nodes[i].type == 3) eq->nodes[i].gain = exp((0.5 - eq->nodePosY[i]) * 3.5);
		if (eq->nodes[i].type == 4) eq->nodes[i].gain = exp((0.5 - eq->nodePosY[i]) * 3.5);
		if (eq->nodes[i].type == 5) eq->nodes[i].gain = exp((0.5 - eq->nodePosY[i]) * 3.5);
		eq->nodes[i].reso = exp((eq->wheelValue[i] - 0.25) * 5.0) * 0.707;
	}
	//然后再更新biquad的系数
	for (int i = 0; i < eq->nodeN; ++i)
	{
		if (eq->nodes[i].type == 1) biquadApplyLPF(&eq->nodes[i], eq->nodes[i].ctof, eq->nodes[i].reso, eq->nodes[i].gain);
		if (eq->nodes[i].type == 2) biquadApplyHPF(&eq->nodes[i], eq->nodes[i].ctof, eq->nodes[i].reso, eq->nodes[i].gain);
		if (eq->nodes[i].type == 3) biquadApplyPeaking(&eq->nodes[i], eq->nodes[i].ctof, eq->nodes[i].reso, eq->nodes[i].gain);
		if (eq->nodes[i].type == 4) biquadApplyLowShelf(&eq->nodes[i], eq->nodes[i].ctof, eq->nodes[i].reso, eq->nodes[i].gain);
		if (eq->nodes[i].type == 5) biquadApplyHighShelf(&eq->nodes[i], eq->nodes[i].ctof, eq->nodes[i].reso, eq->nodes[i].gain);
	}
}
/*
const float FRFixConst = M_PI / 2;
float FR_FreqWarping(float linear_f, float freqRange)
{
	return atanf(expf((linear_f - 1.0) * freqRange) * FRFixConst) / FRFixConst;
}
*/
const float f_min = 24.0;       // 最小频率
const float f_max = 24000.0;    // 最大频率
const float x_max = 1.0;      // 横坐标最大值
const float gamma = 0.8;
float FR_FreqWarping(float x, float freqRange) {
	float ratio = f_max / f_min; // 频率比率
	float normalized_x = x / x_max;// 调整映射函数，引入 gamma 参数
	float frequency = f_min * pow(ratio, pow(normalized_x, gamma));
	return frequency / 48000.0;
}