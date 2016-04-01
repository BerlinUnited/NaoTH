#!/usr/bin/python

import cv2
import numpy as np

def orb(img, n):
	dim = img.shape
	if dim[0] != dim[1]:
		return np.zeros((0, n), dtype=np.float32)
		
	size = dim[0]
	psize = (size/2)-1
	
	orb = cv2.ORB_create(nfeatures=n,nlevels=8,patchSize=psize, edgeThreshold=psize)
	
	# add dummy keypoint in the center
	kp = cv2.KeyPoint(x=size/2,y=size/2, _size=size/2, _response=100.0)
	
	kp, des = orb.compute(img, [kp])
	return des.astype(dtype=np.float32)
	
# BRISK
__brisk = cv2.BRISK_create(patternScale=0.35, octaves=0)
	
def brisk(img):
	dim = img.shape
	if dim[0] != dim[1]:
		return np.zeros((0, n), dtype=np.float32)
		
	size = dim[0]
	psize = (size/2)-1
	
	# add dummy keypoint in the center
	kp = cv2.KeyPoint(x=size/2,y=size/2, _size=7, _response=100.0)
	
	#kp = __brisk.detect(img)
	keypoints, des = __brisk.compute(img, [kp])
	return des.astype(dtype=np.float32)
	
def bin1(img):
	img = cv2.adaptiveThreshold(img, 255, cv2.ADAPTIVE_THRESH_MEAN_C,\
		cv2.THRESH_BINARY,11,2)
	return img.reshape(1,144)
	
def bin2(img):
	img = img.astype("uint8")
	_, img = cv2.threshold(img,0,255,cv2.THRESH_BINARY+cv2.THRESH_OTSU)
	return img.reshape(1,144)

def histo(img, size=32):
	# normalize by maximum value
	img = img.astype("float32")
	_, maxVal, _, _ = cv2.minMaxLoc(img)
	hist = cv2.calcHist([img],[0],None,[size],[0.0, maxVal+1])
	
	# also normalize the histogram to 0 to 1.0f
	_, maxHistVal, _, _ = cv2.minMaxLoc(hist)
	if maxHistVal > 0:
		hist = np.divide(hist, maxHistVal)
	
	return hist.T
	
def histoNonGreen(img, colors):
	
	mask = np.asmatrix(colors != 7, np.uint8)
	numOfGreen = (12*12) - np.count_nonzero(mask)
	
	# normalize by maximum value
	img = img.astype("float32")
	_, maxVal, _, _ = cv2.minMaxLoc(img)
	histNoGreen = cv2.calcHist([img],[0], mask, [32],[0.0, maxVal+1])
	
	# also normalize the histogram to 0 to 1.0f
	_, maxHistVal, _, _ = cv2.minMaxLoc(histNoGreen)
	if maxHistVal > 0:
		histNoGreen = np.divide(histNoGreen, maxHistVal)
		
	feat = np.vstack([histNoGreen, np.float32([[numOfGreen]])])
	
	return feat.T

def histoDiff(img, size=16, ksize=3):
	
	# get legacy
	imgFeat = histo(img)
	
	# normalize by maximum value
	img = img.astype("float32")
	
	edgeimgX = np.absolute(np.diff(img, axis=0))
	edgeimgY = np.absolute(np.diff(img, axis=1))
	
	minValX, maxValX, _, _ = cv2.minMaxLoc(edgeimgX)
	histX = cv2.calcHist([edgeimgX],[0],None,[size],[minValX, maxValX+1])
	
	minValY, maxValY, _, _ = cv2.minMaxLoc(edgeimgY)
	histY = cv2.calcHist([edgeimgY],[0],None,[size],[minValY, maxValY+1])
	
	# also normalize the histogram to 0 to 1.0f
	_, maxHistValX, _, _ = cv2.minMaxLoc(histX)
	if maxHistValX > 0:
		histX = np.divide(histX, maxHistValX)
		
	_, maxHistValY, _, _ = cv2.minMaxLoc(histY)
	if maxHistValY > 0:
		histY = np.divide(histY, maxHistValY)
		
	
	#return histX.T
	return np.hstack([imgFeat, histX.T, histY.T])
	#return np.hstack([histX.T, histY.T])
