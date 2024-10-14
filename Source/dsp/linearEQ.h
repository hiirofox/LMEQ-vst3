#pragma once

#include <JuceHeader.h>
#include "eq.h"
#include "complex.h"
#include "fft.h"

#define FFT_Length 1024//fir_length = fft_length*2 ÒòÎªÒª¶Ô³Æ
class LinearEQ
{
private:
	LMEQ* iir = NULL;
	complex_f32_t dat[FFT_Length];//H(z)
	float convDat[FFT_Length * 2];
	float dlyDatL[FFT_Length * 2];
	float dlyDatR[FFT_Length * 2];
	int pos = 0;
	float firSum = 0;
	float lpFirSum = 0;
public:
	LinearEQ();
	void init(LMEQ* eq);
	void updata();
	void proc(const float* inl, const float* inr, float* outl, float* outr, int numSample);
};