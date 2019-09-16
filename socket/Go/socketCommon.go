package main

import (
	"C"
	"github.com/fabiokung/shm"
	"io"
	"net"
	"os"
	"syscall"
)

func socketCreateAndConnectToClient(port string) (conn net.Conn) {
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

func socketDialAndConnectToServer(ip string, port string) (conn net.Conn) {
	addr := ip + ":" + port
	// connect to this socket
	connTmp, err := net.Dial("tcp", addr)
	if err != nil {
		panic("Client: Dail!!")
	}
	conn = connTmp
	return
}

func sendAll(conn net.Conn, b []byte) (n int, err error) {
	n, err = conn.Write(b)
	return
}

func recvAll(conn net.Conn, b []byte) (n int, err error) {
	n, err = io.ReadFull(conn, b)
	return
}

//------ POSIX Shared Memory ----------------------
func createShMem(fName string, len int) (fd *os.File) {
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

func destroyShMem(fd *os.File) {
	//TODO: Equivalent on the device side
	shm.Unlink(fd.Name())
	fd.Close()
}

//----------------------------------------------------------
