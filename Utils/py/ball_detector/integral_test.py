#!/usr/bin/python

import os, sys, getopt, math

import matplotlib.pyplot as plt
import numpy as np
from scipy import stats
from PIL import Image
from PIL import PngImagePlugin

def parseArguments(argv):
  inputfile = ''
  outputdir = None
  
  try:
    opts, args = getopt.getopt(argv,"hi:o:",["ifile=","outdir="])
  except getopt.GetoptError:
    print 'ExportTopImages.py -i <input file> -o <output directory>'
    sys.exit(2)
    
  for opt, arg in opts:
    if opt == '-h':
      print 'ExportTopImages.py -i <input file> -o <output directory>'
      sys.exit()
    elif opt in ("-i", "--ifile"):
      inputfile = arg
    elif opt in ("-o", "--ofile"):
      outputdir = arg
      
  return inputfile, outputdir

def integral_image(x):
    return x.cumsum(1).cumsum(0)


# simple color classification
class ColorClassifier:
  def __init__(self):
    self.brightnesConeRadiusWhite = 40
    self.brightnesConeRadiusBlack = 3
    self.brightnesConeOffset = 80
    self.colorAngleCenter = -2.4
    self.colorAngleWith = 0.6

  # return true if the (y,u,v) have a low color part, i.e., white, gray, black
  def noColor(self, y, u, v):
    brightnesAlpha = (self.brightnesConeRadiusWhite - self.brightnesConeRadiusBlack) / (255.0 - self.brightnesConeOffset)
    cromaThreshold = np.clip(self.brightnesConeRadiusBlack + brightnesAlpha * (y-self.brightnesConeOffset), self.brightnesConeRadiusBlack, 255)
    chroma = self.getChroma(y, u, v)
    return np.logical_and(np.less(chroma,cromaThreshold), np.greater(y,self.brightnesConeOffset)) #np.multiply(np.logical_and(np.less(chroma,cromaThreshold), np.greater(y,self.brightnesConeOffset)), y)

  def isCroma(self, y, u, v):
    chromaAngle = np.arctan2(img_u-128,img_v-128)
    return np.abs(chromaAngle-self.colorAngleCenter) < self.colorAngleWith

  def isColor(self, y, u, v):
    return np.logical_and(np.logical_not(self.noColor(y, u, v)),self.isCroma(y, u, v))

  def getChroma(self, y, u, v):
    chroma = np.hypot(img_u-128,img_v-128)
    brightnesAlpha = (self.brightnesConeRadiusWhite - self.brightnesConeRadiusBlack) / (255.0 - self.brightnesConeOffset)
    brightness = np.clip(self.brightnesConeRadiusBlack + brightnesAlpha * (y-self.brightnesConeOffset), self.brightnesConeRadiusBlack, 255.0)
    return brightness#255.0 - chroma

def pixelToAngle(x,y):
  x0 = 610.0 # focal length
  y0 = 320.0 - x
  z0 = 240.0 - y 
  pitch = np.arctan2(z0,x0) # atan(z0/x0)
  roll = np.arctan2(y0,x0)
  return pitch, roll

def pitchToPixel(pitch):
  f = 610.0 # focal length
  p = 240.0 - np.tan(pitch)*f
  #print str(p)
  return p
  
def ballSize(x,y, pitch, roll):
  px, rx = pixelToAngle(x,y) # angles inside the image
  #print "px = " + str(px)
  
  q = 500.0 / np.sin(pitch+px)
  d = np.tan(pitch+px)*500.0
  r = 50.0
  beta = np.arctan2(r,q)
  #print "q = " + str(q)
  #print "d = " + str(d)
  #print "beta = " + str(beta*180/3.14)
  
  xu = pitchToPixel(px - beta)
  xl = pitchToPixel(px + beta)
  #print "xu = " + str(xu), "xl = " + str(xl)
  return xu - xl
  
def region(img,x0,y0,x1,y1):
  return img[y1,x1]+img[y0-1,x0-1]-img[y0-1,x1]-img[y1,x0-1]

def density(img,x0,y0,x1,y1):
  return float(region(img,x0,y0,x1,y1)) / float((np.abs(x1-x0)+1)*(np.abs(y1-y0)+1))
  
def boxDiff(img, x,y, size, border):
  v = region(img, x,y, x+size, y+size)
  vo = region(img, x-border,y-border, x+size+border, y+size+border)
  return v - 2*(vo - v)
  
def centralCrossDiff(img, x,y, size, border):
  innerH  = region(img, x         ,y+border         , x+size         , y+size-border) # inner
  innerV  = region(img, x+border         ,y         , x+size-border         , y+size)
  
  outer  = region(img, x-border,y-border, x+size+border, y+size+border) # outer
  return (innerH+innerV) - outer#a - b + c
  #return inner
  
def centralGaussDiff(img, x,y, size, border):
  inner  = region(img, x         ,y         , x+size         , y+size) # inner
  #middle = region(img, x-border  ,y-border  , x+size+border  , y+size+border) # middle
  outer  = region(img, x-border,y-border, x+size+border, y+size+border) # outer
  return float(inner - (outer - inner)) / float((size+border+1)*(size+border+1))
  
  
def verticalGausDiff(img,x,y,width, height):
  a = region(img, x,y, x+width, y+height)
  b = region(img, x,y+height, x+width, y+height*2)
  c = region(img, x,y+height*2, x+width, y+height*3)
  return a - 2*b + c
  
def horizontalGausDiff(img,x,y,width, height):
  a = region(img, x,y, x+width, y+height)
  b = region(img, x+width,y, x+width*2, y+height)
  c = region(img, x+width*2,y, x+width*3, y+height)
  return a - 2*b + c

def xyGausDiff(img,x,y, width):
  ul = region(img, x        ,y, x+width    , y+width)
  ur = region(img, x+width+1,y, x+width*2+1, y+width)
  
  ll = region(img, x,y+width+1, x+width, y+width*2+1)
  lr = region(img, x+width+1,y+width+1, x+width*2+1, y+width*2+1)
  return ul - ur - ll + lr


def calculateKeyPoints(int_img,int_img_green,width,height,ball_size):
    result = np.zeros((height, width))
    maxValue = -1e+30
    maxIndex = (0,0)

    for x in range(1,width,1):
      for y in range(1,height,1):

        size = int(ball_size*1.0)
        border = int(ball_size*0.6)

        if x <= border or y <= border or x+size+border >= width or y+size+border >= height:
          continue

        inner = region(int_img, x, y, x+size, y+size)
        outer = region(int_img, x-border, y+size, x+size+border, y+size+border)
        greenBelow = density(int_img_green, x, y+size, x+size, y+size+border)

        if inner*2 > size*size and greenBelow > 0.3:
          value = float(inner - (outer - inner)) / float((size+border+1)*(size+border+1))
          result[y+size/2,x+size/2] = value

          if value > maxValue:
            maxIndex = (x+size/2, y+size/2)
            maxValue = value

    return result, maxIndex


if __name__ == "__main__":

  fileName, targetdir = parseArguments(sys.argv[1:])
  fileName = "../otsubinary/bottom2.png"

  img = Image.open(fileName)
  ycbcr = img.convert('YCbCr')

  width = ycbcr.size[0]
  height = ycbcr.size[1]
  size = (height, width)
  print "Image size: ", size

  # separate chanels
  img_y = np.array(list(ycbcr.getdata(band=0)))
  img_u = np.array(list(ycbcr.getdata(band=1)))
  img_v = np.array(list(ycbcr.getdata(band=2)))

  img_y = np.reshape(img_y, size)
  img_u = np.reshape(img_u, size)
  img_v = np.reshape(img_v, size)

  # classify the image
  classifier = ColorClassifier()
  img_no_color = classifier.getChroma(img_y, img_u, img_v)#classifier.noColor(img_y, img_u, img_v)
  img_green = classifier.isColor(img_y, img_u, img_v)

  # create the integral image
  int_img = integral_image(img_no_color)
  int_img_green = integral_image(img_green)

  ball_size = 100
  result, maxValue = calculateKeyPoints(int_img=int_img, int_img_green=int_img_green, height=height, width=width, ball_size=ball_size)
  #maxValue = np.unravel_index(np.argmax(result), result.shape)
  print maxValue
  circle = plt.Circle(maxValue,ball_size/2,color='r',fill=False)

  f, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2,2, sharex='col', sharey='row')
  # show the image
  #if targetdir is None:
  ax1.imshow(img)
  ax1.add_artist(circle)
  ax2.imshow(img_no_color)

  ax3.imshow(result)
  ax4.imshow(img_green)
  plt.show()
  

  