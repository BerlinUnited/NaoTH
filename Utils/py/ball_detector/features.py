#!/usr/bin/python
inmport naoth
import cv2
import numpy as np


def orb(img, n):
    dim = img.shape
    if dim[0] != dim[1]:
        return np.zeros((0, n), dtype=np.float32)
        
    size = dim[0]
    psize = (size/2)-1
    
    orb = cv2.ORB_create(nfeatures=n, nlevels=8, patchSize=psize, edgeThreshold=psize)
    
    # add dummy keypoint in the center
    kp = cv2.KeyPoint(x=size/2, y=size/2, _size=size/2, _response=100.0)
    
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
    kp = cv2.KeyPoint(x=size/2, y=size/2, _size=7, _response=100.0)
    
    # kp = __brisk.detect(img)
    keypoints, des = __brisk.compute(img, [kp])
    return des.astype(dtype=np.float32)


def bin1(img):
    img = cv2.adaptiveThreshold(img, 255, cv2.ADAPTIVE_THRESH_MEAN_C, cv2.THRESH_BINARY, 11, 2)
    return img.reshape(1, 144)


def bin2(img):
    img = img.astype("uint8")
    _, img = cv2.threshold(img, 0, 255, cv2.THRESH_BINARY+cv2.THRESH_OTSU)
    return img.reshape(1, 144)


def histo(img, size=32):
    # normalize by maximum value
    img = img.astype("float32")
    _, max_val, _, _ = cv2.minMaxLoc(img)
    hist = cv2.calcHist([img], [0], None, [size], [0.0, max_val+1])
    
    # also normalize the histogram to 0 to 1.0f
    _, max_hist_val, _, _ = cv2.minMaxLoc(hist)
    if max_hist_val > 0:
        hist = np.divide(hist, max_hist_val)
    
    return hist.T


def histo_non_green(img, colors):
    
    mask = np.asmatrix(colors != 7, np.uint8)
    num_of_green = (12*12) - np.count_nonzero(mask)
    
    # normalize by maximum value
    img = img.astype("float32")
    _, max_val, _, _ = cv2.minMaxLoc(img)
    hist_no_green = cv2.calcHist([img], [0], mask, [32], [0.0, max_val+1])
    
    # also normalize the histogram to 0 to 1.0f
    _, max_hist_val, _, _ = cv2.minMaxLoc(hist_no_green)
    if max_hist_val > 0:
        hist_no_green = np.divide(hist_no_green, max_hist_val)
        
    feat = np.vstack([hist_no_green, np.float32([[num_of_green]])])
    
    return feat.T
