#!/usr/bin/python3

import socket
import struct

def float2hexIEEE754(f):
  tmp = hex(struct.unpack('<I',struct.pack('<f',f))[0])
  return tmp 

def hexIEEE7542float(x):	
  tmp = struct.unpack('<f',struct.pack('<I',int(x,16)))[0]
  return tmp #float value

def outSourceFFT(y,n):
	senddata = ""
	for i in y :
		tmp = float2hexIEEE754(i)[2:]
		senddata = senddata + (8-len(tmp))*'0'+tmp
		
	clientsocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	clientsocket.connect(('localhost',12345))
	#converting python string to byte-code string
	data = senddata.encode()
	clientsocket.send(data)

	# receive FFT Output
	recvdata = ""
	bytes_recd = 0
	FFTOUTSZ = n*8*2
	while bytes_recd < FFTOUTSZ :
		tmp = clientsocket.recv(FFTOUTSZ-bytes_recd).decode()
		if tmp == "":
			raise RuntimeError("socket connection broken")
		bytes_recd = bytes_recd + len(tmp)
		recvdata = recvdata + tmp

	i = 0
	fft = []
	for i in range(0,n):	
		k = 2*i
		x = recvdata[k*8:(k+1)*8]
		y = recvdata[(k+1)*8:(k+2)*8]
		cmplx = hexIEEE7542float(x)+1j*hexIEEE7542float(y)
		fft.append(cmplx)

	return fft	


if __name__ == "__main__" :
	N = 256
	y = [0]*N
	x = outSourceFFT(y,N)	
