package ipc

import (
	"C"
	"github.com/fabiokung/shm"
	"io"
	"net"
	"os"
	"syscall"
)

func SocketCreateAndConnectToClient(port string) (conn net.Conn) {
	// listen on all interfaces
	ln, err := net.Listen("tcp", ":"+port)
	if err != nil {
		panic("server: Listen!!")
	}
	// accept connection on port
	connTmp, err := ln.Accept()
	if err != nil {
		panic("server: Accept!!")
	}
	conn = connTmp
	return
}

func SocketDialAndConnectToServer(ip string, port string) (conn net.Conn) {
	addr := ip + ":" + port
	// connect to this socket
	connTmp, err := net.Dial("tcp", addr)
	if err != nil {
		panic("Client: Dail!!")
	}
	conn = connTmp
	return
}

func SendAll(conn net.Conn, b []byte) (n int, err error) {
	n, err = conn.Write(b)
	return
}

func RecvAll(conn net.Conn, b []byte) (n int, err error) {
	n, err = io.ReadFull(conn, b)
	return
}

//------ POSIX Shared Memory ----------------------
func CreateShMem(fName string, len uint32) (fd *os.File) {
	//NOTE: /dev/shm/fName file-path
	fd, err := shm.Open(fName, os.O_RDWR|os.O_CREATE, 0600)
	if err != nil {
		panic(err)
	}
	fdUnix := fd.Fd()
	_, _, e1 := syscall.Syscall(syscall.SYS_FTRUNCATE, fdUnix, uintptr(len), 0)
	if e1 != 0 {
		panic(e1)
	}
	return
}

func DestroyShMem(fd *os.File) {
	//TODO: Equivalent on the device side
	shm.Unlink(fd.Name())
	fd.Close()
}

//----------------------------------------------------------
