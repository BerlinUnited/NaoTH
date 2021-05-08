"""
    write evaluation functions here which differentiate between classification and detection
"""
import argparse
import pickle
import tensorflow.keras as keras
import numpy as np
import cv2
from pathlib import Path

DATA_DIR = Path(Path(__file__).parent.absolute() / "data").resolve()
MODEL_DIR = Path(Path(__file__).parent.absolute() / "data/best_models").resolve()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Evaluate the network given ')

    parser.add_argument('-b', '--database-path', dest='imgdb_path', default=str(DATA_DIR / 'tk03_natural_detection.pkl'),
                        help='Path to the image database containing test data.'
                             'Default is imgdb.pkl in the data folder.')
    parser.add_argument('-m', '--model-path', dest='model_path', default=str(MODEL_DIR / 'fy1500_conf.h5'),
                        help='Store the trained model using this path. Default is fy1500_conf.h5.')

    args = parser.parse_args()

    with open(args.imgdb_path, "rb") as f:
        mean = pickle.load(f)
        print("mean=" + str(mean))
        x = pickle.load(f)
        y = pickle.load(f)

    model = keras.models.load_model(args.model_path)
    model.summary()

    # compare image size to network size and resize if necessary
    target_input_shape = model.layers[0].input_shape[1:]
    input_shape = x.shape[1:]

    if target_input_shape != input_shape:
        print("will resize input images to match the required input shape")

        resized_images = list()
        for image in x:
            temp = cv2.resize(image, dsize=(target_input_shape[0], target_input_shape[1]),
                              interpolation=cv2.INTER_CUBIC)
            temp = temp.reshape(*temp.shape, 1)
            resized_images.append(temp)

        x = np.array(resized_images)

        # TODO resize radius
        for i in range(len(y)):
            if y[i][0] is not 0:
                y[i][0] = y[i][0] / 2
                y[i][1] = y[i][1] / 2
                y[i][2] = y[i][2] / 2

    x = np.array(x)
    y = np.array(y)

    result = model.evaluate(x, y)

    print("Evaluation result")
    print("=================")

    for idx in range(0, len(result)):
        print(model.metrics_names[idx] + ":", result[idx])
