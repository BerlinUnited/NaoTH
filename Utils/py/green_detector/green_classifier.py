#!/usr/bin/python

import sys
import matplotlib.pyplot as plt
import matplotlib.lines as lines

import numpy as np
from PIL import Image
import cv2

from scipy import signal

# calculate th difference between angles a and b
def angle_diff(a,b):
  return np.arctan2(np.sin(a)-np.sin(b), np.cos(a)-np.cos(b))

# simple color classification
class ColorClassifier:
    def __init__(self, bW, bB, bO, cC, cW):
        self.brightnessConeRadiusWhite = bW #70
        self.brightnessConeRadiusBlack = bB #15
        self.brightnessConeOffset = bO #20
        self.colorAngleCenter = cC #-1.25 # yellow
        self.colorAngleWith = cW #0.1

    # return true if the (y,u,v) have a low color part, i.e., white, gray, black
    def no_color(self, y, u, v):
        brightness_alpha = (self.brightnessConeRadiusWhite - self.brightnessConeRadiusBlack) / (255.0 - self.brightnessConeOffset)
        chroma_threshold = np.clip(self.brightnessConeRadiusBlack + brightness_alpha * (y - self.brightnessConeOffset), self.brightnessConeRadiusBlack, 255)
        chroma = np.hypot(u-128, v-128)
        return np.less(chroma, chroma_threshold)

    def is_chroma(self, y, u, v):
        color_angle = np.arctan2(u-128, v-128)
        diff = angle_diff(color_angle, self.colorAngleCenter)
        return np.abs(diff) < self.colorAngleWith

    def is_color(self, y, u, v):
        return np.logical_and(np.logical_not(self.no_color(y, u, v)), self.is_chroma(y, u, v))

    '''
    def get_chroma(self, y, u, v):
        chroma = np.hypot(u-128, v-128)
        brightnesAlpha = (self.brightnessConeRadiusWhite - self.brightnessConeRadiusBlack) / (255.0 - self.brightnessConeOffset)
        brightness = np.clip(self.brightnessConeRadiusBlack + brightnesAlpha * (y - self.brightnessConeOffset), self.brightnessConeRadiusBlack, 255.0)
        return brightness  # 255.0 - chroma
    '''

def loadImage(path):
  img = Image.open(path)
  ycbcr = img.convert('YCbCr')

  width = ycbcr.size[0]
  height = ycbcr.size[1]
  size = (height, width)
  print ("Image size: {}".format(size))

  # separate chanels
  img_y = np.array(list(ycbcr.getdata(band=0)))
  img_u = np.array(list(ycbcr.getdata(band=1)))
  img_v = np.array(list(ycbcr.getdata(band=2)))

  img_y = np.reshape(img_y, size)
  img_u = np.reshape(img_u, size)
  img_v = np.reshape(img_v, size)
        
  return (img, img_y, img_u, img_v)
  
  
def myhist(a,b,f):
  hh = np.zeros((256/f,256/f))
  for i in range(0,a.shape[0]):
    for j in range(0,a.shape[1]):
      hh[a[i,j]/f, b[i,j]/f] += 1
  
  return hh
  
      
if __name__ == "__main__":

    fileName = "./11.png"
    
    (img, img_y, img_u, img_v) = loadImage(fileName)

    # classify the whole image
    classifierGreen = ColorClassifier(70, 5, 60, np.radians(30.5), np.radians(15)) # green
    img_green = classifierGreen.is_color(img_y, img_u, img_v)
    
    hist_uv, _, _ = np.histogram2d(np.reshape(img_u, (img_u.size,)), np.reshape(img_v, (img_v.size,)), bins = 256, range=((0,255),(0,255)))
    print hist_uv.shape
    
    
    hist_uv = myhist(img_u, img_v, 4)
    
    t = np.max(hist_uv)*0.01
    print t
    hist_uv[np.where(hist_uv > t)] = t
    
    print img_v[64,553]
    
    (u,v) = np.unravel_index(np.argmax(hist_uv, axis=None), hist_uv.shape)
    print (u,v)

    idx = np.where(np.logical_and(img_u == v, img_v == u))
    print np.array(idx).shape 
    
    
    nimg = np.zeros(img_u.shape)
    nimg[0,0] = 2
    nimg[np.where(img_u == u)] = 1
    nimg[np.where(img_v == v)] += 1
    
    f, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2) # sharex='col', sharey='row'
    ax1.imshow(img)
    
    ax2.imshow(nimg, cmap=plt.cm.gray)
    ax3.imshow(hist_uv, interpolation = "nearest")
    
    a = np.radians(30.5)
    x = 31-np.cos(a)*32
    y = 31+np.sin(a)*32
    
    #ax3.add_line(lines.Line2D([31,31], [100,100]))
    ax3.plot([31,x], [31,y])
    
    ax4.imshow(img_green)
    plt.show()
    

