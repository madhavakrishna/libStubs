/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <strings.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#include "fftCommon.h"


void error(char *msg)
{
    perror(msg);
    exit(1);
}


int socketCreateAndConnect(void){
  int sockfd, newsockfd, portno, clilen;
  struct sockaddr_in serv_addr, cli_addr;
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) 
     error("ERROR opening socket");
  bzero((char *) &serv_addr, sizeof(serv_addr));
  portno = atoi("12345");
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);
  if (bind(sockfd, (struct sockaddr *) &serv_addr,
           sizeof(serv_addr)) < 0) 
           error("ERROR on binding");
  listen(sockfd,5);
  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
  if (newsockfd < 0) 
       error("ERROR on accept");
	return newsockfd;
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


int hex2dec(char x){
	if(x >= '0' && x <= '9'){
		return (x - '0');
	} else if(x >= 'A' && x <= 'F'){
		return (x - 'A') + 10;
	} else if(x >= 'a' && x <= 'f'){
		return (x - 'a') + 10;
	}
}

int hex2int(char *x){
	int i,r=0;
	for(i=0;i<8;i++){
		r = r + (hex2dec(x[i]) << (4*(7-i)));
	}
	return r;	
}



void recvFFTInput(int sockfd, Complex *x){
	int BUFSZ = N*8;
	char buffer[BUFSZ];
	bzero(buffer,BUFSZ);
	int n = 0;
	n = read(sockfd,buffer,BUFSZ);
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
	n = write(sockfd,buffer,BUFSZ);
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
	int newsockfd = socketCreateAndConnect();
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
