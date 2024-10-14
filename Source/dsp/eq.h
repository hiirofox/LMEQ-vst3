#pragma once

#include <math.h>
#include "biquad.h"

#define MaxEQNodeNum 32
typedef struct
{
	Biquad nodes[MaxEQNodeNum];
	int nodeN;

	//�����ֻ���������Ķ������Ͱ���������������ע�͵�
	//����ֱ��ͨ������ṹ���������ݳ־û��ģ����juce��
	float nodePosX[MaxEQNodeNum];//�Ȱ���ʾ�ؼ�����Ϣ��¼����ʱ����ת�����˲����Ĳ���
	float nodePosY[MaxEQNodeNum];
	float wheelValue[MaxEQNodeNum];
}LMEQ;

void LMEQInit(LMEQ* eq, int nodeN);
complex_f32_t LMEQGetFreqResponse(LMEQ* eq, float w);//��ȡ����EQ��Ƶ��
float LMEQProcess(LMEQ* eq, float input);
StereoSignal LMEQProcessStereo(LMEQ* eq, StereoSignal input);


//��������ʾ����
float FR_FreqWarping(float linear_f);	//��һ��Ƶ����������

float LMEQGetFreqGain(LMEQ* eq, float normFreq);	//��ȥ���ǹ�һ�����꣬���������󣬳�����������(��λ��db)

void LMEQSetNodeN(LMEQ* eq, int nodeN);			//�����˲����ĸ���
void LMEQSetFilter(LMEQ* eq, int nodeID, float type,
	float ctof, float reso, float gain);			//����ĳ�˲����Ĳ�����ע�����Ƶ��Ҳ�ǹ�һ����
void LMEQProcessBlockStereo(LMEQ* eq,
	StereoSignal* in, StereoSignal* out, int numSamples);//����һ�����ݣ�StereoSignal��˫������
void LMEQProcessBlock(LMEQ* eq,
	float* in, float* out, int numSamples);//����һ�����ݣ�StereoSignal��˫������