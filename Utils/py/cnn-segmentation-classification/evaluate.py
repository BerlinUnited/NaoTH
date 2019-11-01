#!/usr/bin/env python3

import argparse
import pickle
import keras
import numpy as np
import sys
import cv2

parser = argparse.ArgumentParser(description='Train the network given ')

parser.add_argument('-b', '--database-path', dest='imgdb_path',
                    help='Path to the image database containing test data.'
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
    print("mean=" + str(mean))
    x = pickle.load(f)
    y = pickle.load(f)

model = keras.models.load_model(model_path)

x_blurred = []
for img in x:
    img_blurred = cv2.GaussianBlur(img, (3, 3), 0.5)
    x_blurred.append(img_blurred.reshape(16, 16, 1))

print(model.summary())

result = model.evaluate(np.array(x_blurred), y)
# result = model.evaluate(np.array(x), y)

print("Evaluation result")
print("=================")

print("loss: {} precision: {}".format(result[0], result[1]))
