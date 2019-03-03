#!/usr/bin/env python3

import argparse
import pickle
from utility_functions.stats import keras_infer
from utility_functions.loader import loadImage

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
print(keras_infer(img.reshape(1, res["x"], res["y"], 1), args.model_path))
