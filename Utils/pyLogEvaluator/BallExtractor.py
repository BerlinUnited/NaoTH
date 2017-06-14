#!/usr/bin/python

import os, sys, getopt

from LogReader import LogReader
from LogReader import Parser

import math

import matplotlib as mlp
from matplotlib import pyplot
import numpy
from PIL import Image


def imageFromProto(message):

    # read each channel of yuv422 separately
    yuv422 = numpy.fromstring(message.data, dtype=numpy.uint8)
    y = yuv422[0::2]
    u = yuv422[1::4]
    v = yuv422[3::4]

    # convert from yuv422 to yuv888
    yuv888 = numpy.zeros(message.height*message.width*3, dtype=numpy.uint8)
    yuv888[0::3] = y
    yuv888[1::6] = u
    yuv888[2::6] = v
    yuv888[4::6] = u
    yuv888[5::6] = v

    yuv888 = yuv888.reshape(message.height,message.width,3)

    return yuv888


def getImage(frame):
    # we are only interested in top images
    message = frame["Image"]
    return [frame.number, imageFromProto(message)]

def hsiSeparate(y,u,v):
    brightnessConeOffset = 80.0
    brightnessConeRadiusWhite = 40.0
    brightnessConeRadiusBlack = 3.0

    colorAngleCenter = -2.
    colorAngleWith = 1.3

    brightnessAlpha = (brightnessConeRadiusWhite - brightnessConeRadiusBlack) / (255.0 - brightnessConeOffset)

    dp = numpy.array([u-128.0, v-128.0])
    d = numpy.linalg.norm(dp)
    a = numpy.arctan2(dp[1], dp[0])

    cromaThreshold = max(brightnessConeRadiusBlack, brightnessConeRadiusBlack + brightnessAlpha * (y-brightnessConeOffset))

    return (d > cromaThreshold and abs(normalizeAngle(colorAngleCenter - a)) < colorAngleWith)

def normalizeAngle(data):
    PI2 = math.pi*2

    if (data < math.pi and data >= -math.pi):
        return data

    #ndata = data - ((data // PI2))*PI2;

    if data >= math.pi:
        data -= PI2;
    elif data < -math.pi:
        data += PI2;

    return data;

def findBox(region):
    width = len(region[0])
    height = len(region)

    xh = numpy.zeros(width, dtype=numpy.uint8)
    yh = numpy.zeros(height, dtype=numpy.uint8)

    box = [-1,-1,-1,-1]

    foundX = False
    foundY = False

    for y in range(0, height, 2):
        for x in range(0,width,2):
            if not hsiSeparate(region[y][x][0],region[y][x][1],region[y][x][2]):
                xh[x] = xh[x] + 1
                yh[y] = yh[y] + 1

            if not foundX and y==height-2:
                if xh[x] > 10 and box[0] == -1:
                    box[0] = x
                if (box[2]==-1 and box[0] > -1) and (xh[x] < 5 or x == width-2):
                    if abs(box[0] - x) < 10:
                        box[0] = -1
                        continue
                    foundX = True
                    box[2] = x
                    if foundY:
                        return True, box

        if not foundY:
            if yh[y] > 10 and box[1] == -1:
                box[1] = y
            if (box[1] > -1 and box[3] == -1) and (yh[y] < 5 or y == height-2):
                if abs(box[1] - y) < 10:
                    box[1] = -1
                    continue
                foundY = True
                box[3] = y
                if foundX:
                    return True, box

    return False, None


def extractBall(region):
    found, box = findBox(region)

    if found:
        width = len(region[0])
        height = len(region)
        img = Image.frombytes('YCbCr', (width, height), region.tostring())

        box = (box[0]-2, box[1]-2, box[2]+2, box[3]+2)
        print box
        img = img.crop(box)
        return img

    return None


if __name__ == "__main__":

    # initialize the parser
    myParser = Parser()
    # register the protobuf message name for the 'ImageTop'
    myParser.register("ImageTop", "Image")

    fileName = "game.log"
    log = LogReader(fileName, myParser)

    # initialize pyplot
    fig = pyplot.figure()
    ax = fig.gca()
    fig.show()

    # get all the images from the logfile
    images = [getImage(f) for f in log] #map(getImageTop, )
    # create picture directory
    if not os.path.isdir('logPics'):
        os.mkdir('logPics')

    for i in range(len(images)):
        print i,"of", len(images)
        img = images[i][1]
        img = extractBall(img)
        if img == None:
            continue
        #pyplot.clf()
        #pyplot.plot(xh)
        #pyplot.plot(yh)
        image = ax.imshow(img)
        fig.canvas.draw()
        img = img.convert('RGB')
        img.save(os.getcwd() + "/logPics/still-green-" + "frame" + str(i+1) + ".png")

        #pyplot.show()

