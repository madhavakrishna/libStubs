package main

import (
	"./ipc"
	"fmt"
)

func main() {
	fmt.Println("vim-go")
	sockfd := ipc.SocketCreateAndConnectToClient("12345")
	ipc.SendAll(sockfd, []byte("Madhava Krishna"))
	x := make([]byte, 32)
	ipc.RecvAll(sockfd, x)
	fmt.Println(string(x))
}
