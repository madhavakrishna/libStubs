package main

import (
	"fmt"
)

func main() {
	fmt.Println("vim-go")
	//sockfd := socketCreateAndConnectToClient("12345")
	sockfd := socketDialAndConnectToServer("127.0.0.1", "12345")
	str := make([]byte, 15)
	recvAll(sockfd, str)
	fmt.Println(string(str))

	x := make([]byte, 32)
	for i := 0; i < 32; i++ {
		x[i] = 'a' + byte(i)
	}
	sendAll(sockfd, x)
}
