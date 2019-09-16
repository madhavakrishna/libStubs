package main

import (
	"fmt"
)

func main() {
	fmt.Println("vim-go")
	sockfd := socketCreateAndConnectToClient("12345")
	sendAll(sockfd, []byte("Madhava Krishna"))
	x := make([]byte, 32)
	recvAll(sockfd, x)
	fmt.Println(string(x))
}
