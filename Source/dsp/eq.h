#pragma once

#include <math.h>
#include "biquad.h"

#define MaxEQNodeNum 32
typedef struct
{
	Biquad nodes[MaxEQNodeNum];
	int nodeN;

	//如果你只用这个库里的东西，就把下面这三个数组注释掉
	//我想直接通过这个结构体来做数据持久化的（配合juce）
	float nodePosX[MaxEQNodeNum];//先把显示控件的信息记录，到时候再转换成滤波器的参数
	float nodePosY[MaxEQNodeNum];
	float wheelValue[MaxEQNodeNum];
}LMEQ;

void LMEQInit(LMEQ* eq, int nodeN);
complex_f32_t LMEQGetFreqResponse(LMEQ* eq, float w);//获取整个EQ的频响
float LMEQProcess(LMEQ* eq, float input);
StereoSignal LMEQProcessStereo(LMEQ* eq, StereoSignal input);


//下面是显示部分
float FR_FreqWarping(float linear_f);	//归一化频率修正函数

float LMEQGetFreqGain(LMEQ* eq, float normFreq);	//进去的是归一化坐标，经过修正后，出来的是增益(单位是db)

void LMEQSetNodeN(LMEQ* eq, int nodeN);			//设置滤波器的个数
void LMEQSetFilter(LMEQ* eq, int nodeID, float type,
	float ctof, float reso, float gain);			//更改某滤波器的参数，注意这个频率也是归一化的
void LMEQProcessBlockStereo(LMEQ* eq,
	StereoSignal* in, StereoSignal* out, int numSamples);//处理一块数据，StereoSignal是双声道的
void LMEQProcessBlock(LMEQ* eq,
	float* in, float* out, int numSamples);//处理一块数据，StereoSignal是双声道的