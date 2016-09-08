#!/usr/bin/python3
import scipy.fftpack
import numpy as np
import matplotlib.pyplot as plt

# Number of samplepoints -- FFT size
N = 512
# sampling frequency : fs
# maximum frequency that can be measured "fs/2" Nyquist's theorem
fs= 1000 #Hz 
# sampling time period = 1/fs 
T = 1/fs 
# length of the signal in time
L = N*T 
# Vector with sampling time-stamps
t = np.linspace(0.0,L,N)
# input signal
# test signal -- replace this --
y = 0.5 * np.sin(150 * 2.0 * np.pi * t) + np.sin(400 * 2.0 * np.pi * t)

fig, (datain,dataout) = plt.subplots(nrows=1,ncols=2,sharex=False,sharey=False)

yf = scipy.fftpack.fft(y)


# yfabs[0] gives the DC component of the input signal
# If the FFT is input is just real numbers (true for all real world apps.)
#   second half of fft-out is complex conjugate of first half of the fft-out
#<VVI> Why are we scaling the output ??
yfabs = np.abs(yf[0:int(N/2)]) * 2/N
fd = np.array(list(range(0,int(N/2)))).astype(float) * fs/N
print(y)
print(yfabs)

datain.plot(t,y)
dataout.plot(fd,yfabs)
#yinv = scipy.fftpack.ifft(yf)
#dataout.plot(t,yinv)

#xf = np.linspace(0.0, 1.0/(2.0*T), N/2)

#fig, ax = plt.subplots()
#ax.plot(xf[1:], 2.0/N * np.abs(yf[1:N/2]))
plt.show()
