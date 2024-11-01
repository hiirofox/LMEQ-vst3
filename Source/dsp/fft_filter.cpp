#include "fft_filter.h"
#define M_PI 3.1415926535897932384626

void FFTFilterInit(FFTFilter* dat, float* window)
{
	dat->window = window;
	dat->pos = 0;
	dat->posout = 0;
	memset(dat->inbufl, 0, sizeof(float) * FFTFilterSize);
	memset(dat->outbufl, 0, sizeof(float) * FFTFilterSize * 2);
	memset(dat->inbufr, 0, sizeof(float) * FFTFilterSize);
	memset(dat->outbufr, 0, sizeof(float) * FFTFilterSize * 2);
	memset(dat->fftdatl, 0, sizeof(complex_f32_t) * FFTFilterSize);
	memset(dat->fftdatr, 0, sizeof(complex_f32_t) * FFTFilterSize);
	memset(dat->core, 0, sizeof(complex_f32_t) * FFTFilterSize / 2);
}

void FFTFilterProc(FFTFilter* dat, const float* in, float* out, float numSamples)
{
	auto& inbufl = dat->inbufl;
	auto& outbufl = dat->outbufl;
	auto& inbufr = dat->inbufr;
	auto& outbufr = dat->outbufr;
	auto& core = dat->core;
	auto& window = dat->window;
	auto& fftdatl = dat->fftdatl;
	auto& fftdatr = dat->fftdatr;
	auto& pos = dat->pos;
	auto& posout = dat->posout;
	for (int i = 0; i < numSamples; ++i)
	{
		inbufl[pos] = in[i];
		out[i] = outbufl[posout];
		pos++;
		posout++;
		if (pos >= FFTFilterSize)pos = 0;
		if (posout >= FFTFilterSize * 2)posout = 0;
		if (pos % (FFTFilterSize / 4) == 0)//填充完了！
		{//然后就往fftdat里面灌，然后加窗，然后fft，然后乘频响，然后ifft，然后加窗，然后叠加回outbuf
			for (int j = 0; j < FFTFilterSize; ++j)//灌
			{
				fftdatl[j].re = inbufl[(pos + j) % FFTFilterSize] * window[j];
				fftdatl[j].im = 0;
			}
			fft_f32(fftdatl, FFTFilterSize, 1);//fft
			for (int j = 0; j < FFTFilterSize / 2; ++j)
			{
				fftdatl[j] = cmul(fftdatl[j], core[j]);
				fftdatl[FFTFilterSize - 1 - j] = cmul(fftdatl[FFTFilterSize - 1 - j], core[j]);
			}
			fft_f32(fftdatl, FFTFilterSize, -1);//ifft
			for (int j = 0; j < FFTFilterSize; ++j)//加窗
			{
				fftdatl[j].re *= window[j];
			}
			int pos2 = posout;
			for (int j = 0; j < FFTFilterSize; ++j)
			{
				pos2++;
				outbufl[pos2 % (FFTFilterSize * 2)] += fftdatl[j].re / 1.5 / FFTFilterSize;//我记得增益是1.5
			}
			for (int j = 0; j < FFTFilterSize / 4; ++j)
			{
				pos2++;
				outbufl[pos2 % (FFTFilterSize * 2)] = 0;//这里要清零一下，要不然就真一直加了
			}
		}
	}
}

void FFTFilterProcStereo(FFTFilter* dat, const float* inl, const float* inr, float* outl, float* outr, float numSamples)
{
	auto& inbufl = dat->inbufl;
	auto& outbufl = dat->outbufl;
	auto& inbufr = dat->inbufr;
	auto& outbufr = dat->outbufr;
	auto& core = dat->core;
	auto& window = dat->window;
	auto& fftdatl = dat->fftdatl;
	auto& fftdatr = dat->fftdatr;
	auto& pos = dat->pos;
	auto& posout = dat->posout;
	for (int i = 0; i < numSamples; ++i)
	{
		inbufl[pos] = inl[i];
		inbufr[pos] = inr[i];
		outl[i] = outbufl[posout];
		outr[i] = outbufr[posout];
		pos++;
		posout++;
		if (pos >= FFTFilterSize)pos = 0;
		if (posout >= FFTFilterSize * 2)posout = 0;
		if (pos % (FFTFilterSize / 4) == 0)//填充完了！
		{//然后就往fftdat里面灌，然后加窗，然后fft，然后乘频响，然后ifft，然后加窗，然后叠加回outbuf
			for (int j = 0; j < FFTFilterSize; ++j)//灌
			{
				int k = (pos + j) % FFTFilterSize;
				fftdatl[j].re = inbufl[k] * window[j];
				fftdatl[j].im = 0;
				fftdatr[j].re = inbufr[k] * window[j];
				fftdatr[j].im = 0;
			}
			fft_f32(fftdatl, FFTFilterSize, 1);//fft
			fft_f32(fftdatr, FFTFilterSize, 1);
			for (int j = 0; j < FFTFilterSize / 2; ++j)
			{
				fftdatl[j] = cmul(fftdatl[j], core[j]);
				fftdatl[FFTFilterSize - 1 - j] = cmul(fftdatl[FFTFilterSize - 1 - j], core[j]);
				fftdatr[j] = cmul(fftdatr[j], core[j]);
				fftdatr[FFTFilterSize - 1 - j] = cmul(fftdatr[FFTFilterSize - 1 - j], core[j]);
			}
			fft_f32(fftdatl, FFTFilterSize, -1);//ifft
			fft_f32(fftdatr, FFTFilterSize, -1);
			for (int j = 0; j < FFTFilterSize; ++j)//加窗
			{
				fftdatl[j].re *= window[j];
				fftdatr[j].re *= window[j];
			}
			int pos2 = posout;
			for (int j = 0; j < FFTFilterSize; ++j)
			{
				pos2++;
				int k = pos2 % (FFTFilterSize * 2);
				outbufl[k] += fftdatl[j].re / 1.5 / FFTFilterSize;//我记得增益是1.5
				outbufr[k] += fftdatr[j].re / 1.5 / FFTFilterSize;
			}
			for (int j = 0; j < FFTFilterSize / 4; ++j)
			{
				pos2++;
				int k = pos2 % (FFTFilterSize * 2);
				outbufl[k] = 0;//这里要清零一下，要不然就真一直加了
				outbufr[k] = 0;
			}
		}
	}
}

void FFTFilterApplyBPF(FFTFilter* dat, float freql, float freqr)
{
	for (int i = 0; i < FFTFilterSize / 2; ++i)
	{
		dat->core[i].re = 0;
		dat->core[i].im = 0;
	}
	if (freql > freqr) return;
	int l = freql * (FFTFilterSize / 2 - 2);//??
	int r = freqr * (FFTFilterSize / 2 - 2) + 1;

	dat->core[l].re = 1.0 - freql + (int)freql;
	dat->core[l].im = 0;
	dat->core[r].re = freql - (int)freql;
	dat->core[r].im = 0;

	for (int i = l + 1; i <= r - 1; ++i)
	{
		dat->core[i].re = 1.0;
		dat->core[i].im = 0.0;
	}
}

