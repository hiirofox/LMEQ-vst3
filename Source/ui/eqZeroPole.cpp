#include "eqZeroPole.h"

void LMEQZeroPole::drawZeroPoint(juce::Graphics& g, int x, int y, float thickness)
{
	g.drawEllipse(x - 4, y - 4, 8, 8, thickness);
}

void LMEQZeroPole::drawPolePoint(juce::Graphics& g, int x, int y, float thickness)
{
	g.drawLine(x - 4, y - 4, x + 4, y + 4, thickness);
	g.drawLine(x - 4, y + 4, x + 4, y - 4, thickness);
}

LMEQZeroPole::LMEQZeroPole()
{

}

void LMEQZeroPole::init(LMEQ* eq)
{
	this->eq = eq;
}

void LMEQZeroPole::paint(juce::Graphics& g)
{
	int w = getBounds().getWidth();
	int h = getBounds().getHeight();
	float midw = 0, midh = 0;
	int r = w;
	if (r > h)
	{
		r = h;
		midw = (w - h) / 2;
	}
	else
	{
		midh = (h - w) / 2;
	}
	r /= 2;

	g.setColour(juce::Colour(0xff00ff00));
	g.drawEllipse(midw * 2, midh * 2, r * 2, r * 2, 2.0);

	if (eq == NULL)return;

	for (int i = 0; i < eq->nodeN; ++i)
	{
		complex_f32_t z1 = { 0,0 };//零点
		complex_f32_t z2 = { 0,0 };
		float delta = eq->nodes[i].b1 * eq->nodes[i].b1 - 4.0 * eq->nodes[i].b0 * eq->nodes[i].b2;//b^2-4ac
		z1.re += -eq->nodes[i].b1;
		z2.re += -eq->nodes[i].b1;
		if (delta >= 0)
		{
			z1.re += sqrtf(delta);
			z2.re -= sqrtf(delta);
			z1 = cmulreal(z1, 1.0 / (2.0 * eq->nodes[i].b0));
			z2 = cmulreal(z2, 1.0 / (2.0 * eq->nodes[i].b0));
		}
		else
		{
			z1.im += sqrtf(-delta);
			z2.im -= sqrtf(-delta);
			z1 = cmulreal(z1, 1.0 / (2.0 * eq->nodes[i].b0));
			z2 = cmulreal(z2, 1.0 / (2.0 * eq->nodes[i].b0));
		}

		complex_f32_t p1 = { 0,0 };//极点
		complex_f32_t p2 = { 0,0 };
		delta = eq->nodes[i].a1 * eq->nodes[i].a1 - 4.0 * eq->nodes[i].a2;//b^2-4ac
		p1.re += -eq->nodes[i].a1;
		p2.re += -eq->nodes[i].a1;
		if (delta >= 0)
		{
			p1.re += sqrtf(delta);
			p2.re -= sqrtf(delta);
			p1 = cmulreal(p1, 0.5);
			p2 = cmulreal(p2, 0.5);
		}
		else
		{
			p1.im += sqrtf(-delta);
			p2.im -= sqrtf(-delta);
			p1 = cmulreal(p1, 0.5);
			p2 = cmulreal(p2, 0.5);
		}

		z1 = cmulreal(z1, r);
		z2 = cmulreal(z2, r);
		p1 = cmulreal(p1, r);
		p2 = cmulreal(p2, r);
		z1 = FR_OmegaWarping(z1);
		z2 = FR_OmegaWarping(z2);
		p1 = FR_OmegaWarping(p1);
		p2 = FR_OmegaWarping(p2);


		float thickness = 1.0;
		if (activtyNum == i || activtyNum == -1)
		{
			g.setColour(juce::Colour(0xff7777ff));
			thickness = 2.0;
		}
		else
		{
			g.setColour(juce::Colour(0x337777ff));
		}
		drawZeroPoint(g, z1.re + w / 2 + midw, h / 2 - z1.im + midh, thickness);
		drawZeroPoint(g, z2.re + w / 2 + midw, h / 2 - z2.im + midh, thickness);
		drawPolePoint(g, p1.re + w / 2 + midw, h / 2 - p1.im + midh, thickness);
		drawPolePoint(g, p2.re + w / 2 + midw, h / 2 - p2.im + midh, thickness);
	}
}

void LMEQZeroPole::resized()
{
}

void LMEQZeroPole::setActivtyNodeNum(int num)
{
	this->activtyNum = num;
}

complex_f32_t FR_OmegaWarping(complex_f32_t z)//不会写
{
	return z;
}
