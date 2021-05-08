import argparse
import pickle

import numpy as np

parser = argparse.ArgumentParser(description='Train the network given ')

parser.add_argument('-b', '--database-path', dest='imgdb_path', default="imgdb.pkl",
                    help='Path to the image database to use for training. '
                         'Default is imgdb.pkl in current folder.')

args = parser.parse_args()

with open(args.imgdb_path, "rb") as f:
    mb = pickle.load(f)
    x = pickle.load(f)
    y = pickle.load(f)
    p = pickle.load(f)

ball = np.count_nonzero(y[:, 0])
noball = len(x) - ball

# calculate balls with center outside the image. Note since those are discarded by the generation script,
# this is most likely zero
outside = 0
for i in range(len(y)):
    if y[i][3] != 0:
        if y[i][1] < 0 or y[i][1] > 1 or y[i][2] < 0 or y[i][2] > 1:
            outside += 1

print("images: {}".format(len(x)))
print("ball: {} / noball: {}".format(ball, noball))
print("balls outside of the patch: {}".format(outside))
print("mean brightness: {}".format(mb))
