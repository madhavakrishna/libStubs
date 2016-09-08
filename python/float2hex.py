#!/usr/bin/python3

import struct

def float2hex(f):
  tmp = hex(struct.unpack('<I',struct.pack('<f',f))[0])
  return tmp 

def hex2float(x):  
  tmp = struct.unpack('<f',struct.pack('<I',int(x,16)))[0]
  return tmp #float value


if __name__ == "__main__":
	print("hex2float: "+str(hex2float("cbded000")))
	print("float2hex: "+float2hex(1024.1024))

