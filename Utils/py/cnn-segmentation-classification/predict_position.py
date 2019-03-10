#!/usr/bin/env python3

import argparse
import pickle
import keras;
from utility_functions.csv_loader import loadImage
import numpy as np
import cv2
import sys

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
print("Predicting image...", end="")
i = 0
for img_path in args.img:
    img_orig = loadImage(img_path, res)
    debug_img = cv2.cvtColor((img_orig*255).astype(np.uint8), cv2.COLOR_GRAY2BGR)

    img = img_orig - mean
    
    prediction = model.predict(img.reshape(1, res["x"], res["y"], 1))[0]

    radius = prediction[0]
    x = prediction[1]
    y = prediction[2]

    if radius > 0.125:
        radius = radius * max(res["x"], res["y"])
        x = x * res["x"]
        y = y * res["y"]
        cv2.circle(debug_img, (int(x),int(y)), int(radius), color=(0,0,255))
    else:
        cv2.rectangle(debug_img, (0,0), (res["x"]-1, res["y"]-1), color=(255,0,0))

    out_images.append(debug_img)

    if i % 10 == 0:
        print(".", end='')
        sys.stdout.flush()
print()
print("Showing matrix")
i = 0
image_row = list()
all_image_rows = list()
for img in out_images:
    image_row.append(img)
    if len(image_row) == 25:
        all_image_rows.append(np.concatenate(image_row, axis=1))
        image_row = list()


if len(image_row) > 0:
    all_image_rows.append(np.concatenate(image_row, axis=1))
    image_row = list()


cv2.imshow("image", np.concatenate(all_image_rows, axis=0))
cv2.waitKey()
#print(np.argmax(prediction, axis=3))