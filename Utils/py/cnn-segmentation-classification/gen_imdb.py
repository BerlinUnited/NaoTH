#!/usr/bin/env python3

import argparse
from utility_functions.csv_loader import loadImages
import pickle


def str2bool(v):
    if v.lower() in ('yes', 'true', 't', 'y', '1'):
        return True
    elif v.lower() in ('no', 'false', 'f', 'n', '0'):
        return False
    else:
        raise argparse.ArgumentTypeError('Boolean value expected.')

parser = argparse.ArgumentParser(description='Generate the image database for training etc. '
                                              'using a folder with 0, 1 etc. subfolders with png images.')

parser.add_argument('-b', '--database-path', dest='imgdb_path',
                    help='Path to the image database to write. '
                         'Default is img.db in current folder.')
parser.add_argument('-i', '--image-folder', dest='img_path', nargs='+',
                    help='Path to the CSV file(s) with region annotation.')
parser.add_argument('-r', '--resolution', dest='res',
                    help='Images will be resized to this resolution. Default is 16x16')

parser.add_argument("-l","--limit-noball", type=str2bool, nargs='?', dest="limit_noball",
                        const=True,
                        help="Randomly select at most |balls| from no balls class")

args = parser.parse_args()

imgdb_path = "img.db"
img_path = "."
res = {"x": 16, "y": 16}

if args.imgdb_path is not None:
    imgdb_path = args.imgdb_path

if args.img_path is not None:
    img_path = args.img_path

if args.res is not None:
    res = {"x": int(args.res), "y": int(args.res)}
    
with open(imgdb_path, "wb") as f:
    x, y, mean, p = loadImages(img_path, res, args.limit_noball)

    pickle.dump(mean, f)
    pickle.dump(x, f)
    pickle.dump(y, f)
    pickle.dump(p, f)