// Reference: Beej's Guide to Network Programming

#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <error.h>

#define DEBUG 1
#define dbgprintf(...) if(DEBUG) { printf( __VA_ARGS__ ); }

// get sockaddr, IPv4 or IPv6
void *get_in_addr(struct sockaddr *sa){
	if(sa->sa_family == AF_INET){
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}
	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/* A simple client in the internet domain using TCP
   The ip address and port number are passed as arguments */
int socketDailAndConnectToServer(char *ip, char *port){
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int status;
	char s[INET6_ADDRSTRLEN];

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; //don't care IPv4 or IPv6
	hints.ai_socktype  =SOCK_STREAM; //TCP stream socket

	if((status = getaddrinfo(ip, port, &hints, &servinfo)) != 0){
		fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(status));
		return -1;
	}

	//loop through all the results and connect to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
			perror("client: socket");
			continue;
		}
		if(connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
			close(sockfd);
			perror("client: connect");
			continue;
		}
		break;
	}

	if(p == NULL){
		fprintf(stderr, "client: failed to connect\n");
		return -2;
	}

	inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof(s));
	dbgprintf("client: connecting to %s\n", s);
	freeaddrinfo(servinfo); //all done with this structure

	return sockfd;
}

/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
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

int checkRecvRdy(int rdFd){
	fd_set read_fds;
	FD_ZERO(&read_fds);
	FD_SET(rdFd, &read_fds);

	struct timeval tv; //return immediately
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	if(select(rdFd+1, &read_fds, NULL, NULL, &tv) == -1){
		perror("select");
		exit(4);
	}

	if(FD_ISSET(rdFd, &read_fds)){
		return 1;
	} else {
		return 0;
	}
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
        int n=0;

        while(total < *len){
                n = recv(sockfd, buf+total, bytesLeft, 0);
                if(n <= 0){ break; } //0: socket closed
                total += n;
                bytesLeft -= n;
        }
        *len = total;
        return n;
}

unsigned long long bytes2longlong(char in[8]){
        //NOTE: Little Endian
        unsigned long long r;
        memcpy(&r, in, 8);
        return r;
}

void longlong2bytes(unsigned long long in, char out[8]){
        //NOTE: Little Endian
        memcpy(out, &in, 8);
}

