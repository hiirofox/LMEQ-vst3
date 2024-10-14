#include "dataDisplayer.h"

DataDisplayer::LineData::LineData()
{
	data.resize(MaxDataLen);
}


DataDisplayer::DataDisplayer()
{
	zoom = 1.0;
	lines.resize(10);
	pos = 0;
}

void DataDisplayer::paint(juce::Graphics& g)
{
	juce::Rectangle<int> bound = getBounds();
	int w = bound.getWidth(), h = bound.getHeight();

	g.setColour(juce::Colour(0x55ffffff));
	for (int i = 0; i < w; ++i)
	{
		if ((int)(zoom * (i + pos)) % 32 == 0)
			g.drawLine(i, 0, i, h);
	}

	for (auto line : lines)
	{
		g.setColour(line.col);
		if (line.displayMode == 0)
		{
			float k = (float)h / (line.maxv - line.minv);
			for (int i = 1; i < w; ++i)
			{
				int lastpos = (i - w - 1) * zoom + pos;
				int nowpos = (i - w) * zoom + pos;
				while (lastpos < 0)lastpos += MaxDataLen;
				while (nowpos < 0)nowpos += MaxDataLen;
				lastpos = lastpos % MaxDataLen;
				nowpos = nowpos % MaxDataLen;

				int v1 = k * (line.data[lastpos] - line.minv);
				int v2 = k * (line.data[nowpos] - line.minv);
				if (v1 < 0)v1 = 0;
				if (v1 > h)v1 = h;
				if (v2 < 0)v2 = 0;
				if (v2 > h)v2 = h;
				g.drawLine(i - 1, h - v1, i, h - v2);
			}
		}
		else if (line.displayMode == 1)
		{
			g.setColour(line.col);
			for (int i = 0; i < w; ++i)
			{
				int nowpos = (i - w) * zoom + pos;
				while (nowpos < 0)nowpos += MaxDataLen;
				nowpos = nowpos % MaxDataLen;
				if (line.data[nowpos] <= 0)
					g.drawLine(i, h, i, 0);
			}
		}
	}



	g.setColour(juce::Colour(0x00, 0xff, 0x00));
	g.drawLine(0, 0, w, 0, 3);
	g.drawLine(0, 0, 0, h, 3);
	g.drawLine(w, h, w, 0, 3);
	g.drawLine(w, h, 0, h, 3);
}

void DataDisplayer::resized()
{
}

void DataDisplayer::timerCallback()
{
	for (auto& line : lines)
	{
		if (line.dataPtr != NULL)
		{
			line.data[pos] = *line.dataPtr;
		}
	}
	nextLine();
}

void DataDisplayer::setTimer(int Hz)
{
	startTimerHz(Hz);
}

void DataDisplayer::addLine(int id, int displayMode, float minv, float maxv, juce::Colour col)
{
	if (lines.size() <= id)
	{
		lines.resize(id + 1);
	}
	lines[id].displayMode = displayMode;
	lines[id].minv = minv;
	lines[id].maxv = maxv;
	lines[id].col = col;

}

void DataDisplayer::setLine(int id, float data)
{
	lines[id].data[pos] = data;
}

void DataDisplayer::setLineAutoUpdata(int id, float* dataPtr)
{
	lines[id].dataPtr = dataPtr;
}


void DataDisplayer::nextLine()
{
	pos++;
	if (pos >= MaxDataLen)pos = 0;
}



void Oscillator::paint(juce::Graphics& g)
{
	juce::Rectangle<int> bound = getBounds();
	int w = bound.getWidth(), h = bound.getHeight();

	if (dat == NULL || datlen == 0)return;

	g.setColour(juce::Colour(0x77, 0x77, 0xff));
	for (int i = 1; i < datlen; ++i)
	{
		float x1 = (float)(i - 1) / datlen * w;
		float x2 = (float)i / datlen * w;
		float y1 = h / 2 - dat[i - 1] * yZoom * h;
		float y2 = h / 2 - dat[i] * yZoom * h;
		if (y1 < 0)y1 = 0;
		if (y1 > h)y1 = h;
		if (y2 < 0)y2 = 0;
		if (y2 > h)y2 = h;
		if (x1 < 0)x1 = 0;
		if (x1 > w)x1 = w;
		if (x2 < 0)x2 = 0;
		if (x2 > w)x2 = w;
		if (isnan(y1))y1 = 0;
		if (isnan(y2))y2 = 0;

		g.drawLine(x1, y1, x2, y2, 3.0);
	}

	g.setColour(juce::Colour(0x00, 0xff, 0x00));
	g.drawLine(0, 0, w, 0, 3);
	g.drawLine(0, 0, 0, h, 3);
	g.drawLine(w, h, w, 0, 3);
	g.drawLine(w, h, 0, h, 3);
}

void Oscillator::resized()
{
}

void Oscillator::setData(float* dat, int datlen)
{
	this->datlen = datlen;
	for (int i = 0; i < datlen; ++i)
	{
		this->dat[i] = dat[i];
	}
}

void Oscillator::setDisplayMode(int mode)
{
	this->mode = mode;
}

void Oscillator::setYZoom(float yZoom)
{
	this->yZoom = yZoom;
}

