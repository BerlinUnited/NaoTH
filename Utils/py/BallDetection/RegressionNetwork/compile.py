#!/usr/bin/env python3

import argparse
import pickle
from utility_functions.onbcg import keras_compile

parser = argparse.ArgumentParser(description='Train the network given ')

parser.add_argument('-b', '--database-path', dest='imgdb_path',
                    help='Path to the image database to use for training. '
                         'Default is img.db in current folder.')
parser.add_argument('-m', '--model-path', dest='model_path',
                    help='Store the trained model using this path. Default is model.h5.')
parser.add_argument('-c', '--code-path', dest='code_path',
                    help='Store the c code in this file. Default is ./cnn.c.')

args = parser.parse_args()

imgdb_path = "imgdb.pkl"
model_path = "model.h5"
code_path = "cnn.c"

if args.imgdb_path is not None:
    imgdb_path = args.imgdb_path

if args.model_path is not None:
    model_path = args.model_path

if args.code_path is not None:
    code_path = args.code_path

images = {}
with open(imgdb_path, "rb") as f:
    images["mean"] = pickle.load(f)
    images["images"] = pickle.load(f)
    images["y"] = pickle.load(f)
keras_compile(images, model_path, code_path, unroll_level=2, arch="sse3")
