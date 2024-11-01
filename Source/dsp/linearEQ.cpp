#include "linearEQ.h"

LinearEQ::LinearEQ()
{
	for (int i = 0; i < FFT_Length; ++i)
	{
		dat[i].re = dat[i].im = 0;
	}
	for (int i = 0; i < FFT_Length * 2; ++i)
	{
		convDat[i] = 0;
	}
	for (int i = 0; i < FFT_Length * 2; ++i)
	{
		dlyDatL[i] = 0;
		dlyDatR[i] = 0;
	}
}

void LinearEQ::init(LMEQ* eq)
{
	this->iir = eq;
}

void LinearEQ::updata()
{
	if (iir == NULL)return;
	for (int i = 0; i < FFT_Length >> 1; ++i)
	{
		complex_f32_t v = LMEQGetFreqResponse(iir, (float)i / FFT_Length * 2.0 * M_PI);
		dat[i].re = sqrtf(v.re * v.re + v.im * v.im);
		dat[i].im = 0;
		dat[FFT_Length - i - 1] = dat[i];
	}
	fft_f32(dat, FFT_Length, -1);//ifft
	for (int i = 0; i < FFT_Length; ++i)
	{
		dat[i].re *= (cosf((float)i / FFT_Length * M_PI) + 1.0) * 0.5;
	}
	for (int i = 0; i < FFT_Length; ++i)
	{
		convDat[FFT_Length + i - 1] = dat[i].re;
		convDat[FFT_Length - i - 1] = dat[i].re;
	}
	firSum = 0;
	for (int i = 0; i < FFT_Length * 2; ++i)// to unit
	{
		firSum += convDat[i] * convDat[i];
	}
	firSum = 1.0 / sqrtf(firSum);
}

void LinearEQ::proc(const float* inl, const float* inr, float* outl, float* outr, int numSample)
{
	int len = FFT_Length * 2;
	for (int i = 0; i < numSample; ++i)
	{
		float suml = 0, sumr = 0;
		dlyDatL[pos] = inl[i];
		dlyDatR[pos] = inr[i];
		for (int j = 0, k = pos; j < len; ++j)
		{
			suml += dlyDatL[k] * convDat[j];
			sumr += dlyDatR[k] * convDat[j];
			k++;
			if (k >= len)k = 0;
		}
		pos++;
		if (pos >= len)pos = 0;

		lpFirSum += 0.05 * (firSum - lpFirSum);
		outl[i] = suml * lpFirSum;
		outr[i] = sumr * lpFirSum;
	}
}

LinearEQ_FFT::LinearEQ_FFT()
{
	for (int i = 0; i < FFTFilterSize; ++i)
	{
		window[i] = 0.5 - 0.5 * cosf((float)i / FFTFilterSize * 2.0 * M_PI);
	}
	FFTFilterInit(&fftfilt, window);
}

void LinearEQ_FFT::init(LMEQ* eq)
{
	this->iir = eq;
}

void LinearEQ_FFT::updata()
{
	for (int i = 0; i < FFTFilterSize >> 1; ++i)
	{
		complex_f32_t v = LMEQGetFreqResponse(iir, (float)i / FFT_Length * 2.0 * M_PI);
		fftfilt.core[i].re = sqrtf(v.re * v.re + v.im * v.im);
		fftfilt.core[i].im = 0;
	}
}

void LinearEQ_FFT::proc(const float* inl, const float* inr, float* outl, float* outr, int numSample)
{
	FFTFilterProcStereo(&fftfilt, inl, inr, outl, outr, numSample);
}

