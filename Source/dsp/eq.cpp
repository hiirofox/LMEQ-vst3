#include "eq.h"

void LMEQInit(LMEQ* eq, int nodeN)
{
	for (int i = 0; i < MaxEQNodeNum; ++i)
	{
		biquadInit(&eq->nodes[i]);
	}
	eq->nodeN = nodeN;
}

complex_f32_t LMEQGetFreqResponse(LMEQ* eq, float w)
{
	complex_f32_t dat = complex_f32_t{ 1,0 };
	for (int i = 0; i < eq->nodeN; ++i)//�˲����������ڴ��ݺ������
	{
		dat = cmul(dat, biquadGetFreqResponse(&eq->nodes[i], w));
	}
	return dat;
}


float LMEQProcess(LMEQ* eq, float input)
{
	for (int i = 0; i < eq->nodeN; ++i)
	{
		input = biquadProcess(&eq->nodes[i], input);
	}
	return input;
}

StereoSignal LMEQProcessStereo(LMEQ* eq, StereoSignal input)
{
	for (int i = 0; i < eq->nodeN; ++i)
	{
		input = biquadProcessStereo(&eq->nodes[i], input);
	}
	return input;
}

float FR_FreqWarping(float linear_f)
{
	return atanf(expf((linear_f - 1.0) * 6.0) * (M_PI / 2)) / (M_PI / 2);
}

float LMEQGetFreqGain(LMEQ* eq, float normFreq)
{
	complex_f32_t fr = LMEQGetFreqResponse(eq, normFreq);
	float gain = sqrtf(fr.re * fr.re + fr.im * fr.im);
	return log10f(gain) * 10.0;
}

void LMEQSetNodeN(LMEQ* eq, int nodeN)
{
	eq->nodeN = nodeN;
}

void LMEQSetFilter(LMEQ* eq, int nodeID, float type, float ctof, float reso, float gain)
{
	//����Ҫע�⣬ctof��reso�������Եģ���Ҫ���ⲿ�ĳ�ָ��
	//��Ҫע�⣬ctof��ָ�������FR_FreqWarping���ģ�����׼ȷһ�㡣resoʹ��expf������

	if (type < 0 || type >= MaxEQNodeNum)return;

	if (type == 1) biquadApplyLPF(&eq->nodes[nodeID], ctof, reso, gain);
	if (type == 2) biquadApplyHPF(&eq->nodes[nodeID], ctof, reso, gain);
	if (type == 3) biquadApplyPeaking(&eq->nodes[nodeID], ctof, reso, gain);
	if (type == 4) biquadApplyLowShelf(&eq->nodes[nodeID], ctof, reso, gain);
	if (type == 5) biquadApplyHighShelf(&eq->nodes[nodeID], ctof, reso, gain);
}

void LMEQProcessBlockStereo(LMEQ* eq, StereoSignal* in, StereoSignal* out, int numSamples)
{
	StereoSignal tmpv = { 0,0 };
	for (int j = 0; j < numSamples; ++j)
	{
		tmpv = in[j];
		for (int i = 0; i < eq->nodeN; ++i)
		{
			tmpv = biquadProcessStereo(&eq->nodes[i], tmpv);
		}
		out[j] = tmpv;
	}
}

void LMEQProcessBlock(LMEQ* eq, float* in, float* out, int numSamples)
{
	float tmpv = 0;
	for (int j = 0; j < numSamples; ++j)
	{
		tmpv = in[j];
		for (int i = 0; i < eq->nodeN; ++i)
		{
			tmpv = biquadProcess(&eq->nodes[i], tmpv);
		}
		out[j] = tmpv;
	}
}
