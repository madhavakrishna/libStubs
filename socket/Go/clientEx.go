package main

import (
	"./ipc"
	"fmt"
)

func main() {
	fmt.Println("vim-go")
	//sockfd := socketCreateAndConnectToClient("12345")
	sockfd := ipc.SocketDialAndConnectToServer("127.0.0.1", "12345")
	str := make([]byte, 15)
	ipc.RecvAll(sockfd, str)
	fmt.Println(string(str))

	x := make([]byte, 32)
	for i := 0; i < 32; i++ {
		x[i] = 'a' + byte(i)
	}
	ipc.SendAll(sockfd, x)
}
