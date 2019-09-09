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

#define DEBUG 0
#define dbgprintf(...) if(DEBUG) { printf( __VA_ARGS__ ); }

int socketCreateAndConnectToClient(const char *port){
	
	int sockfd, new_fd; //listen on sock_fd, new connection on new_fd

	struct sockaddr_storage their_addr; //connector's address information
	int yes=1;

	struct addrinfo hints, *servinfo, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; //don't care IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; //TCP stream sockets
	hints.ai_flags = AI_PASSIVE;	 //fill in my IP for me
	
	int status;
	if((status = getaddrinfo(NULL, port, &hints, &servinfo)) != 0){
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

	dbgprintf("server: waiting for connection..\n");
	socklen_t sin_size = sizeof(their_addr);
	new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
	if(new_fd == -1){
		perror("accept");
		exit(1);
	}

	return new_fd;	
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
