#include<stdio.h>
#include "fftCommon.h"

Complex getTwiddle(int i, int size){
  float phi = (-2*PI/size)*i;
  Complex w;
  w.rel = cos(phi);
  w.img = sin(phi);
  return w;
}


void printlnComplex(char* s, Complex x){
  printf("%-10s(%09.3f,%09.3fj)\n",s,x.rel,x.img);
}

ComplexInHex convert2Hex(Complex x){
  ComplexInHex returnData;
  floatint rel;
  rel.f = x.rel;
  returnData.rel = rel.i;
  floatint img;
  img.f = x.img;
  returnData.img = img.i;
  return returnData;
}

Complex Complex_add(Complex a, Complex b){
  Complex returnData;
  returnData.rel  = a.rel + b.rel;
  returnData.img  = a.img + b.img;
  return returnData;
}

Complex Complex_sub(Complex a, Complex b){
  Complex returnData;
  returnData.rel = a.rel - b.rel;
  returnData.img = a.img - b.img;
  return returnData;
}

Complex Complex_mul(Complex a, Complex b){
  Complex returnData; 
  returnData.rel = (a.rel*b.rel) - (a.img*b.img); 
  returnData.img = (a.rel*b.img) + (a.img*b.rel);
  return returnData; 
}

void fft2(Complex *x0, Complex *x1){
  Complex tmp0 = *x0;  
  Complex tmp1 = *x1;
  *x0 = Complex_add(tmp0, tmp1);
  *x1 = Complex_sub(tmp0, tmp1);
}


void fft4(Complex *x0, Complex *x1, Complex *x2, Complex *x3){
  fft2(x0,x2);
  fft2(x1,x3);
  Complex w_0_4 = getTwiddle(0,4);
  Complex w_1_4 = getTwiddle(1,4);
  Complex tmp0 = *x0;
  Complex tmp2 = *x2;
  Complex tmp1 = Complex_mul(*x1, w_0_4);
  Complex tmp3 = Complex_mul(*x3, w_1_4);
  *x0 = Complex_add(tmp0,tmp1);
  *x1 = Complex_add(tmp2,tmp3); 
  *x2 = Complex_sub(tmp0,tmp1);
  *x3 = Complex_sub(tmp2,tmp3);
}

unsigned int reverse(register unsigned int x, unsigned int size) {
    x = (((x & 0xaaaaaaaa) >> 1) | ((x & 0x55555555) << 1));
    x = (((x & 0xcccccccc) >> 2) | ((x & 0x33333333) << 2));
    x = (((x & 0xf0f0f0f0) >> 4) | ((x & 0x0f0f0f0f) << 4));
    x = (((x & 0xff00ff00) >> 8) | ((x & 0x00ff00ff) << 8));
    return ((x >> 16) | (x << 16)) >> (32-size);
}

unsigned int twiddleSelect(unsigned int index, unsigned int stage, unsigned int log2fftSize){
	return (index >> (log2fftSize-stage)) << (log2fftSize - stage);
}

unsigned int producer2Consumer(unsigned int i, unsigned int j, unsigned int size){
	int tmp = i*2+j;
	int msb = tmp/2;
	int lsb = tmp%2;
	return msb | (lsb * size/2);
}

