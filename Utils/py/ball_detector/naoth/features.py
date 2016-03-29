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
