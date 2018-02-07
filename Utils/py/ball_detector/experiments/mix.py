import naoth
import cv2
import os
import random
import sys
import numpy as np

random.seed()
ballPath = "./selection"
negativePath = "./noball"

outputPath = "./ball"

if __name__ == "__main__":
    numPics = 1
    
    if len(sys.argv) > 1:
        numPics = int(sys.argv[1])
        
    ballImgList = [x for x in os.listdir(ballPath) if os.path.isfile(os.path.join(ballPath, x))]
    bgImgPath = [x for x in os.listdir(negativePath) if os.path.isfile(os.path.join(negativePath, x))]
    
    f = open(os.path.join(outputPath, "positive.txt"), 'w')
    
    idx = 0
    for x in range(0, numPics):
        randomBall = random.choice(ballImgList)
        randomNegativ = random.choice(bgImgPath)

        # get the actual images
        ballImg = cv2.imread(os.path.join(ballPath, randomBall))
        negImg = cv2.imread(os.path.join(negativePath, randomNegativ))
        
        if ballImg is None or negImg is None:
            continue
        
        # check if it's possible to copy ball into the negative image
        if negImg.shape[0] < ballImg.shape[0] or negImg.shape[1] < ballImg.shape[1]:
            print "Error"

        # calculate maximal offsets for images
        xOffset = negImg.shape[0] - ballImg.shape[0]
        yOffset = negImg.shape[1] - ballImg.shape[1]

        # get random value between zero and maximal offsets
        xInsertPoint = random.randint(0, xOffset)
        yInsertPoint = random.randint(0, yOffset)

        # create new Imag
        bg = negImg[xInsertPoint:xInsertPoint+ballImg.shape[0], yInsertPoint:yInsertPoint+ballImg.shape[1]]
        
        ppt = np.multiply(np.equal(ballImg, 0), bg) + np.multiply(np.not_equal(ballImg, 0), ballImg)
        
        negImg[xInsertPoint:xInsertPoint+ballImg.shape[0], yInsertPoint:yInsertPoint+ballImg.shape[1]] = ppt

        name = str(idx)+".png"
        f.write('{0} {1} {2} {3} {4} {5}\n'.format(name, 1, yInsertPoint, xInsertPoint, ballImg.shape[1], ballImg.shape[0]))
        
        target = os.path.join(outputPath, name)
        cv2.imwrite(target, negImg)
        idx += 1
        # cv2.imshow('test', negImg)
        # cv2.waitKey(0)
        
    f.close()
