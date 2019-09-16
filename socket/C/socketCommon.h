// Reference: Beej's Guide to Network Programming

/* A simple client in the internet domain using TCP
   The ip address and port number are passed as arguments */
int socketDailAndConnectToServer(char *ip, char *port);

/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
int socketCreateAndConnectToClient(const char *port);

int checkRecvRdy(int rdFd);

int sendAll(int sockfd, char *buf, int *len);

int recvAll(int sockfd, char *buf, int *len);

unsigned long long bytes2longlong(char in[8]);

void longlong2bytes(unsigned long long in, char out[8]);
