#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include"fftCommon.h"

Complex twiddle[N/2];

void fftNR2_reuse(Complex *x, Complex *y){
	int i,j;	

	Complex stageEven[N/2][2];
	Complex stageOdd[N/2][2];

	int stageId = 1;

	//---- Input bit-reversal
	for(i=0;i<N/2;i++){
		for(j=0;j<2;j++){
			stageOdd[i][j] = x[reverse(i*2+j,log2N)];
		}
	}

	//---- Stage
	for(stageId = 1 ;N >> stageId != 0; stageId = stageId + 1){
		if(stageId%2 == 1){
			for(i=0;i<N/2;i++){
				int tSel = twiddleSelect(i,stageId,log2N);
				stageOdd[i][1] = Complex_mul(stageOdd[i][1],twiddle[tSel]);
				fft2(&stageOdd[i][0],&stageOdd[i][1]);
				int tmp0 = producer2Consumer(i,0,N);
				int tmp1 = producer2Consumer(i,1,N);
				stageEven[tmp0/2][tmp0%2] = stageOdd[i][0];
				stageEven[tmp1/2][tmp1%2] = stageOdd[i][1];
			}
		} else {
			for(i=0;i<N/2;i++){
				int tSel = twiddleSelect(i,stageId,log2N);
				stageEven[i][1] = Complex_mul(stageEven[i][1],twiddle[tSel]);
				fft2(&stageEven[i][0],&stageEven[i][1]);
				int tmp0 = producer2Consumer(i,0,N);
				int tmp1 = producer2Consumer(i,1,N);
				stageOdd[tmp0/2][tmp0%2] = stageEven[i][0];
				stageOdd[tmp1/2][tmp1%2] = stageEven[i][1];
			}
		}
	}

	//---- Output
	if(stageId%2 == 0){
		for(i=0;i<N/2;i++){
			for(j=0;j<2;j++){
				y[i*2+j] = stageEven[i][j];
			}	
		}
	} else {
		for(i=0;i<N/2;i++){
			for(j=0;j<2;j++){
				y[i*2+j] = stageOdd[i][j];
			}	
		}
	}
}


void getInput(Complex *x){
	int i;
	
  for(i=0; i<N ;i++){
    x[i].rel = (float) i ; x[i].img = 0;
  }

}


void genTwiddle(void){
	int i;
	for(i=0; i<N/2 ;i++){
		twiddle[i] = getTwiddle(i,N) ;
    char str[100];
    sprintf(str,"W(%02d,%02d): ",i,N);
    printlnComplex(str,twiddle[i]);
	}
}

/*
int main(){
  int i;
 	
	Complex x[N];	
	Complex y[N];	

	genTwiddle();
	getInput(x);
	
  fftNR2_reuse(x,y);	
  for(i=0; i<N; i++){
    char str[100];
    sprintf(str,"y(%0d): ",i);
    printlnComplex(str,y[i]);
  }
  return 0; 
} 
*/

