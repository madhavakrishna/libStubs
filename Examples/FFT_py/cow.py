#!/usr/bin/python3
import scipy.fftpack
from scipy.io import wavfile
import numpy as np
import matplotlib.pyplot as plt
import math


# Number of samplepoints -- FFT size
N = 1024
# sampling frequency : fs
# maximum frequency that can be measured "fs/2" Nyquist's theorem
fs= 9000 #Hz 
#fs=Fs
# sampling time period = 1/fs 
T = 1/fs 
# length of the signal in time
L = N*T 


def downSampling(fsIn,fsOut,dataIn,N):
	dataOut = []
	dataOut.append(dataIn[0])
	assert fsIn >= fsOut, "Original sampling is lessthan required"
	r = fsIn/fsOut	
	for j in range(1,N):
		d,i = math.modf(r*j)		
		if(int(d) == 0):	
			dataOut.append(dataIn[int(i)])
		else :
			d1,i1 = math.modf(r*(j+1))
			n = dataIn[int(i)]
			m = dataIn[int(i1)]
			p = (n+m)/2
			dataOut.append(p)		

	return dataOut


def inputData():
	Fs,audstrm = wavfile.read('cow.wav')
	# select any one channel of audio stream
	actual_N = len(audstrm.T[0]) 
	actual_t = np.linspace(0.0,actual_N/Fs,actual_N)
	# input signal - downSampled
	downSampled = np.array(downSampling(Fs,fs,audstrm.T[0],N))
	#print(downSampled)

	# 16-bit data, normalized [-1,1]
	y = (downSampled/(2**16))*2
	return y


def doFFT(y):
	return scipy.fftpack.fft(y)

def doIFFT(y):
	return scipy.fftpack.ifft(y)

	
def doPlot(y,yfft,yCnstr):	

	fig, (ploty,plotfft,plotyCnstr) = plt.subplots(nrows=1,ncols=3,sharex=False,sharey=False)

	# Vector with sampling time-stamps
	t = np.linspace(0.0,L,N) # input time-scale
	# Vector with fft-output frequency scale
	feqScale = np.array(list(range(0,int(N/2)))).astype(float) * fs/N
	# yfabs[0] gives the DC component of the input signal
	# If the FFT is input is just real numbers (true for all real world apps.)
	#   second half of fft-out is complex conjugate of first half of the fft-out
	#<VVI> Why are we scaling the output ??
	yfftabs = np.abs(yfft[0:int(N/2)]) * 2/N
	yfftabs[0] = 0 #ignore dc component
	#print(yfft)
	#print(yfftabs)
	yifft = yCnstr.real

	#datain.plot(actual_t,audstrm.T[0])
	ploty.plot(t,y)
	plotfft.plot(feqScale,yfftabs)
	plotyCnstr.plot(t,yifft)

	plt.show()


if __name__ == "__main__" :
	y = inputData()
	yfft = doFFT(y)
	yCnstr = doIFFT(yfft)
	doPlot(y,yfft,yCnstr)
