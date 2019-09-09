int socketCreateAndConnectToClient(const char*);

int hex2dec(char x);
int hex2int(char *x);

int sendAll(int sockfd, char *buf, int *len);
int recvAll(int sockfd, char *buf, int *len);
