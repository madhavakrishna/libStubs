#!/usr/bin/python3
import sys
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image

def imageCompressionSVD(imggray,k):
	# Read image as matrix
	imgmat = np.array(list(imggray.getdata(band=0)),float)
	img_width  = imggray.size[0]
	img_height = imggray.size[1]
	imgmat.shape = (img_height, img_width)
	imgmat = np.matrix(imgmat)

	U, sigma, V = np.linalg.svd(imgmat)
	if(k > sigma.shape[0]):
		k = len(sigma)
	compImg = np.matrix(U[:,:k]) * np.diag(sigma[:k]) * np.matrix(V[:k,:])
	
	#plot image
	fig, (inImg,outImg) = plt.subplots(nrows=1,ncols=2,sharex=False,sharey=False)
	inImg.imshow(imgmat, cmap='gray')
	outImg.imshow(compImg, cmap='gray')
	plt.show()



if __name__ == "__main__" :
	argc = len(sys.argv)
	if(argc != 3 or sys.argv[1] == "help") :
		print("usage: ./imgcomp <image-file> <int>")
		sys.exit(0)

	inputFile = sys.argv[1]
	k = int(sys.argv[2])			
	img = Image.open(inputFile)
	imggray = img.convert('L') #convert to gray-scale
	imageCompressionSVD(imggray,k)
