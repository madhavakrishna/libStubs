/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "fftCommon.h"
#include "serverCommon.h"

void error(char *msg)
{
    perror(msg);
    exit(1);
}


typedef union IntOrFloat{
	int i;
	float f;
} intRfloat;

float int2float(int x){
	intRfloat tmp;
	tmp.i = x;
	return tmp.f;
}

int float2int(float x){
	intRfloat tmp;
	tmp.f = x;
	return tmp.i; 
}

void recvFFTInput(int sockfd, Complex *x){
	int BUFSZ = N*8;
	char buffer[BUFSZ];
	bzero(buffer,BUFSZ);
	int n = 0;
	n = recvAll(sockfd,buffer,&BUFSZ);
	if(n<0) error("ERROR reading from socket");
	int i=0;
	for(i=0; i< N ; i++){
		char y[8];
		int j;
		for(j=0;j<8;j++){ y[j] = buffer[i*8+j]; }
		x[i].rel = int2float(hex2int(y));
		//printf("%s- %d - %f\n",y,hex2int(y),x[i].rel);		
		x[i].img  = 0;		
	}
}

void sendFFTOutput(int sockfd, Complex *x){
	int BUFSZ = N*8*2;
	char buffer[BUFSZ];
	int i;
	for(i=0; i<256; i++){
		char rel[10], img[10];
		sprintf(rel,"%08x",float2int(x[i].rel));	
		sprintf(img,"%08x",float2int(x[i].img));
		int j;
		int k = i*2*8;
		for(j=0;j<8;j++){
			buffer[k+j] 	= rel[j];
			buffer[k+8+j]	= img[j];
		}
	}
	int n=0;
	n = sendAll(sockfd,buffer,&BUFSZ);
	if(n<0) error("ERROR writing to socket");
}


void doFFT(Complex *x, Complex *y){
	int i;
	for(i=0; i<N; i++){
		y[i].rel = x[i].rel;	
		y[i].img  = x[i].img;
	}
}

int main()
{
	int newsockfd = socketCreateAndConnectToClient("12345");
	Complex x[N],y[N];
	
	genTwiddle();
	recvFFTInput(newsockfd,x);
	fftNR2_reuse(x,y);
	int i;
	//for(i=0; i<N ;i++){
  //  char str[100];
  //  sprintf(str,"y(%02d,%02d): ",i,N);
  //  printlnComplex(str,y[i]);
  //}

	sendFFTOutput(newsockfd,y);	
}
