#!/usr/bin/env python3

import argparse
import pickle
import tensorflow.keras as keras
import numpy as np
from pathlib import Path

DATA_DIR = Path(Path(__file__).parent.absolute() / "data").resolve()
MODEL_DIR = Path(Path(__file__).parent.absolute() / "models/best_models").resolve()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Evaluate the network given ')

    parser.add_argument('-b', '--database-path', dest='imgdb_path', default=str(DATA_DIR / 'imgdb.pkl'),
                        help='Path to the image database containing test data.'
                             'Default is imgdb.pkl in the data folder.')
    parser.add_argument('-m', '--model-path', dest='model_path', default=str(MODEL_DIR / 'fy1500_conf.h5'),
                        help='Store the trained model using this path. Default is fy1500_conf.h5.')

    args = parser.parse_args()

    res = {"x": 16, "y": 16}
    with open(args.imgdb_path, "rb") as f:
        mean = pickle.load(f)
        print("mean=" + str(mean))
        x = pickle.load(f)
        y = pickle.load(f)

    model = keras.models.load_model(args.model_path)

    print(model.summary())

    x = np.array(x)
    y = np.array(y)

    result = model.evaluate(x, y)

    print("Evaluation result")
    print("=================")

    for idx in range(0, len(result)):
        print(model.metrics_names[idx] + ":", result[idx])
