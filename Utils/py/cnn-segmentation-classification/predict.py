#!/usr/bin/env python3

import argparse
import pickle
from utility_functions.stats import keras_infer
from utility_functions.loader import loadImage
import numpy as np
import cv2

parser = argparse.ArgumentParser(description='Train the network given ')

parser.add_argument('img', metavar='img',
                    help='The image to be predicted.')
parser.add_argument('-b', '--database-path', dest='imgdb_path',
                    help='Path to the image database containing the required image mean.'
                         'Default is img.db in current folder.')
parser.add_argument('-m', '--model-path', dest='model_path',
                    help='Store the trained model using this path. Default is model.h5.')


args = parser.parse_args()

imgdb_path = "img.db"
model_path = "model.h5"
res = {"x": 16, "y": 16}

if args.model_path is not None:
    model_path = args.model_path

if args.imgdb_path is not None:
    imgdb_path = args.imgdb_path

with open(imgdb_path, "rb") as f:
    mean = pickle.load(f)
img = loadImage(args.img, res) - mean
prediction= keras_infer(img.reshape(1, res["x"], res["y"], 1), model_path)

prediction = prediction[0]

# color-code classification
predicted_image = np.zeros((16,16))
for row in range(0,16):
    for col in range(0,16):
        if prediction[row][col][0] > prediction[row][col][1]:
            predicted_image[row][col] = 0.0
        else:
            predicted_image[row][col] = 1.0

cv2.namedWindow('image',cv2.WINDOW_NORMAL)
cv2.resizeWindow('image', 600,600)
cv2.imshow("image", np.concatenate((img, predicted_image), axis=1))
cv2.waitKey()
#print(np.argmax(prediction, axis=3))