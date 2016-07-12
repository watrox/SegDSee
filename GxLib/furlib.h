//---------------------------------------------------------------------------

#ifndef FurlibH
#define FurlibH
//---------------------------------------------------------------------------


#ifdef _MSC_VER
    #define _USE_MATH_DEFINES
    #include <cmath>
#endif

#include <stdlib.h>


int BandPass(float *in,float *out,int hz1,int hz2,int hz3,int hz4,
			int window,int pad);

int Convolution( float* inp, float* out, float* flt, int datl, int fltl);

int Spectrum1( float* inp, float* out, float si, int nn);

int PhaseRotate(float *in, float *out, int degrees);


int Spectrum(float* in, int* out, int StartFreq, int EndFreq);


void FastFourierTransformation(float* cxxr, float* cxxi, int lx, int isign);


int HilbertTransformation(  int nn, float inp[], float outr[], float outi[]);

void  Agc      (float *data, int iwagc, int nt);
float MaxAmp   (float* data, int nt);
void  Normalize(float* data, float mx, int nt);

int BandPassFilter( float* inp, float* out,
                    float f1, float f2, float f3, float f4,
                    float si, int nn);

int LowPassFilter( float* inp, float* out,
                   float f3, float f4,
                   float si, int nn);

float hsin(float x);
float lsin(float x);
float hcos(float x);
float lcos(float x);


//---------------------------------------------------------------------------


#endif
