#!/usr/bin/env python3

import argparse
import pickle
from utility_functions.stats import keras_plot_thresh_dist, keras_infer, save_false_positives

parser = argparse.ArgumentParser(description='Train the network given ')

parser.add_argument('-b', '--database-path', dest='imgdb_path',
                    help='Path to the image database to use for training. '
                         'Default is img.db in current folder.')
parser.add_argument('-m', '--model-path', dest='model_path',
                    help='Store the trained model using this path. Default is model.h5.')


args = parser.parse_args()

imgdb_path = "img.db"
model_path = "model.h5"

if args.imgdb_path is not None:
    imgdb_path = args.imgdb_path

if args.model_path is not None:
    model_path = args.model_path

with open(imgdb_path, "rb") as f:
    mean = pickle.load(f)
    x = pickle.load(f)
    y = pickle.load(f)
    p = pickle.load(f)

pred = keras_infer(x, model_path)
save_false_positives(pred, x, y, p, mean)
keras_plot_thresh_dist(pred, y)

