#ifndef FFTCOMMON
#define FFTCOMMON
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define N 256
#define log2N 8

#define PI 3.1415926535

typedef struct Complex_t{
  float rel;    
  float img;
} Complex;

void genTwiddle(void);
void fftNR2_reuse(Complex *x, Complex *y);

Complex getTwiddle(int i, int size);

void printlnComplex(char* s, Complex x);

typedef union floatint_t{
  float f;
  int   i;
}floatint;

typedef struct ComplexInHex_t{
  int rel;
  int img;
}ComplexInHex;

ComplexInHex convert2Hex(Complex x);

Complex Complex_add(Complex a, Complex b);

Complex Complex_sub(Complex a, Complex b);

Complex Complex_mul(Complex a, Complex b);

void fft2(Complex *x0, Complex *x1);


void fft4(Complex *x0, Complex *x1, Complex *x2, Complex *x3);

unsigned int reverse(register unsigned int x, unsigned int size);

unsigned int twiddleSelect(unsigned int index, unsigned int stage, unsigned int log2fftSize);

unsigned int producer2Consumer(unsigned int i, unsigned int j, unsigned int size);


#endif







