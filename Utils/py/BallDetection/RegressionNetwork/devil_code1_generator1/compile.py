#!/usr/bin/env python3

import argparse
import pickle
from utility_functions.onbcg import keras_compile
from tensorflow.keras.models import load_model


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Compile keras network to c++')
    parser.add_argument('-b', '--database-path', dest='imgdb_path',
                        help='Path to the image database to use for training. '
                             'Default is imgdb.pkl in current folder.', default='imgdb.pkl')
    parser.add_argument('-m', '--model-path', dest='model_path',
                        help='Store the trained model using this path. Default is model.h5.', default='model.h5')
    parser.add_argument('-c', '--code-path', dest='code_path',
                        help='Store the c code in this file. Default is <model_name>.c.')

    args = parser.parse_args()

    if args.code_path is None:
        model = load_model(args.model_path)
        args.code_path = model.name + ".c"

    images = {}
    with open(args.imgdb_path, "rb") as f:
        images["mean"] = pickle.load(f)
        images["images"] = pickle.load(f)
        images["y"] = pickle.load(f)

    keras_compile(images, args.model_path, args.code_path, unroll_level=2, arch="sse3")
