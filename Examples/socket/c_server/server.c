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


void error(char *msg)
{
    perror(msg);
    exit(1);
}


int socketCreateAndConnect(void){
	
	int sockfd, new_fd; //listen on sock_fd, new connection on new_fd

	struct sockaddr_storage their_addr; //connector's address information
	int yes=1;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; //don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; //TCP stream sockets
	hints.ai_flags = AI_PASSIVE;	 //fill in my IP for me
	
	int status;
	if((status = getaddrinfo(NULL, "12345", &hints, &servinfo)) != 0){
		fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(status));
		return 1;
	}

	//loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p= p->ai_next) {
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			perror("server: socket");
			continue;
		}
		
		//lose the pesky "Address already in use" error message
		if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
			perror("setsockopt");
			exit(1);
		}

		if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
			close(sockfd);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo);
	if(p == NULL){
		fprintf(stderr,"server: failed to bind\n");
		exit(1);
	}	


	if(listen(sockfd, 5) == -1){
		perror("listen");
		exit(1);
	}	


	printf("server: waiting for connection..\n");
	socklen_t sin_size = sizeof(their_addr);
	new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
	if(new_fd == -1){
		perror("accept");
		exit(1);
	}

	return new_fd;	
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


int sendAll(int sockfd, char *buf, int *len){
	int total = 0;
	int bytesLeft = *len;
	int n;

	while(total < *len){
		n = send(sockfd, buf+total, bytesLeft, 0);
		if(n == -1){ break; }
		total += n;
		bytesLeft -= n;
	}

	*len = total; 
	return (n==-1)?-1:0;
}

int recvAll(int sockfd, char *buf, int *len){
	int total = 0;
	int bytesLeft =  *len;
	int n; 

	while(total < *len){
		n = recv(sockfd, buf+total, bytesLeft, 0);
		if(n == -1 || n == 0){ break; }
		total += n;
		bytesLeft -= n;
	}
	*len = total;
	return n;
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
