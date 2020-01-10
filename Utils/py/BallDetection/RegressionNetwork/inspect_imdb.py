#!/usr/bin/env python3

import argparse
import pickle
from tensorflow.keras.models import *
from tensorflow.keras.layers import *
from datetime import datetime
import numpy as np


def str2bool(v):
    if v.lower() in ('yes', 'true', 't', 'y', '1'):
        return True
    elif v.lower() in ('no', 'false', 'f', 'n', '0'):
        return False
    else:
        raise argparse.ArgumentTypeError('Boolean value expected.')


parser = argparse.ArgumentParser(description='Train the network given ')

parser.add_argument('-b', '--database-path', dest='imgdb_path',
                    help='Path to the image database to use for training. '
                         'Default is img.db in current folder.')

args = parser.parse_args()

imgdb_path = "img.db"

if args.imgdb_path is not None:
    imgdb_path = args.imgdb_path

with open(imgdb_path, "rb") as f:
    mb = pickle.load(f)
    x = pickle.load(f)
    y = pickle.load(f)
    p = pickle.load(f)
    real_images = pickle.load(f)

ball = np.count_nonzero(y[:, 0])
noball = len(x) - ball
# calculate balls with center outside the image:
outside = 0
for i in range(len(y)):
    if(y[i][3] != 0):
        if(y[i][1] < 0 or y[i][1] > 1 or y[i][2] < 0 or y[i][2] > 1):
            outside += 1

print("images: {}".format(len(x)))
print("ball: {} / noball: {}".format(ball, noball))
print("balls outside of the patch: {}".format(outside))
print("real balls: {} / real noballs: {}".format(real_images[0], real_images[1]))
print("mean brightness: {}".format(mb))
