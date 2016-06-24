import cv2
import os
import random
import sys

random.seed()
ballPath = "C:\\Users\\Benji\\Desktop\\Python_Mixing\\balls"
negativePath = "C:\\Users\\Benji\\Desktop\\Python_Mixing\\negativeTest"

outputPath = "C:\\Users\\Benji\\Desktop\\Python_Mixing\\output"

if __name__ == "__main__":
    numPics = 1
    
    if len(sys.argv) > 1:
        numPics =  int(sys.argv[1])
        
    for x in range(0, numPics):
        randomBall = random.choice([x for x in os.listdir(ballPath) if os.path.isfile(os.path.join(ballPath, x))])
        randomNegativ = random.choice([x for x in os.listdir(negativePath) if os.path.isfile(os.path.join(negativePath, x))])

        #get the actual images
        ballImg = cv2.imread(ballPath + "\\"+ randomBall)
        negImg = cv2.imread(negativePath + "\\"+ randomNegativ)

        #check if it's possible to copy ball into the negative image
        if(negImg.shape[0] < ballImg.shape[0] or negImg.shape[1] < ballImg.shape[1]):
            print "Error"

        #calculate maximal offsets for images
        xOffset = negImg.shape[0] - ballImg.shape[0]
        yOffset = negImg.shape[1] - ballImg.shape[1]

        #get random value between zero and maximal offsets
        xInsertPoint = random.randint(0,xOffset)
        yInsertPoint = random.randint(0,yOffset)

        #create new Imag
        newImg = negImg.copy()
        newImg[xInsertPoint:xInsertPoint+ballImg.shape[0],yInsertPoint:yInsertPoint+ballImg.shape[1]] = ballImg

        cv2.imwrite(outputPath + "\\" + "new_" + randomBall,newImg)
        #cv2.imshow('test', newImg)
        #cv2.waitKey(0)