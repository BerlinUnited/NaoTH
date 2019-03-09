#!/usr/bin/env python3

import argparse
import pickle
import keras;
from utility_functions.loader import loadImage
import numpy as np
import cv2

parser = argparse.ArgumentParser(description='Train the network given ')

parser.add_argument('img', metavar='img',
                    nargs='+',
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

model = keras.models.load_model(model_path)

cv2.namedWindow('image',cv2.WINDOW_NORMAL)
cv2.resizeWindow('image', 600,600)

out_images = list()

for img_path in args.img:
    img_orig = loadImage(img_path, res)
    img = img_orig - mean
    prediction = model.predict(img.reshape(1, res["x"], res["y"], 1))[0]

    classified = np.argmax(prediction, axis=2)
    noball_prop = prediction[:, :, 0]
    ball_prop = prediction[:, :, 1]
    out_images.append(np.concatenate((img_orig, noball_prop, ball_prop, classified), axis=0))

cv2.imshow("image", np.concatenate(out_images, axis=1))
cv2.waitKey()
#print(np.argmax(prediction, axis=3))