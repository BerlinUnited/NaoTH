#!/usr/bin/env python3

import argparse
import pickle
import sys
from datetime import datetime
from pathlib import Path
from sys import exit

import numpy as np
import tensorflow as tf
from tensorflow import keras as keras

import utility_functions.model_zoo as model_zoo

DATA_DIR = Path(Path(__file__).parent.absolute() / "data").resolve()


class AccHistory(keras.callbacks.Callback):
    def on_train_begin(self, logs={}):
        self.acc = []

    def on_epoch_end(self, batch, logs={}):
        self.acc.append(logs.get('acc'))
        prev = None
        max_idx = np.array(self.acc).argmax()
        offset = max(0, len(self.acc) - 10)
        print("Accuracy history:")
        for (idx, a) in enumerate(self.acc[-10:]):
            idx = idx + offset
            if prev is None:
                print("   {:.4f}".format(a))
            else:
                if max_idx == idx:
                    print("-> {:.4f} ({:+.4f}) [max]".format(a, (a - prev)))
                else:
                    print("-> {:.4f} ({:+.4f}) ".format(a, (a - prev)))
            prev = a
        if 0 <= max_idx < len(self.acc):
            print("Maximum is {:.4f}".format(self.acc[max_idx]))


def str2bool(v):
    if v.lower() in ('yes', 'true', 't', 'y', '1'):
        return True
    elif v.lower() in ('no', 'false', 'f', 'n', '0'):
        return False
    else:
        raise argparse.ArgumentTypeError('Boolean value expected.')


def main(raw_args=None, model=None):
    parser = argparse.ArgumentParser(description='Train the network given')

    parser.add_argument('-b', '--database-path', dest='imgdb_path', default=str(DATA_DIR / 'imgdb.pkl'),
                        help='Path to the image database to use for training. Default is imgdb.pkl in the data folder.')
    parser.add_argument("--output", dest="output", default="./", help="Folder where the trained models are saved")

    # for continuing training
    parser.add_argument('-m', '--model-path', dest='model_path', default="model.h5",
                        help='Store the trained model using this path. Default is model.h5.')
    parser.add_argument("--proceed", type=str2bool, nargs='?', dest="proceed",
                        const=True, help="Use the stored and pre-trained model base.")

    # hyperparameter
    parser.add_argument("--batch-size", dest="batch_size", default=256,
                        help="Batch size. Default is 256")
    parser.add_argument("--epochs", dest="epochs", default=200, type=int,
                        help="Number of epochs")

    args = parser.parse_args(raw_args)

    if model is not None:
        model.summary()

    with open(args.imgdb_path, "rb") as f:
        pickle.load(f)  # skip mean
        x = pickle.load(f)  # x are all input images
        y = pickle.load(f)  # y are the trainings target: [x,y,r,1]


    # define the Keras network
    if args.proceed is not None and args.proceed is True:
        print("Loading model " + args.model_path)
        model = tf.keras.models.load_model(args.model_path)

    elif model is not None:
        print("Creating new model")
    else:
        print("ERROR: No model specified")
        exit(1)
    # Define precision and recall for 0.5, 0.8 and 0.9 threshold
    # class_id=3 means use the third element of the output vector
    precision_class_05 = tf.keras.metrics.Precision(name="precision_classifcation_0.5", thresholds=0.5, class_id=3)
    recall_class_05 = tf.keras.metrics.Recall(name="recall_classifcation_0.5", thresholds=0.5, class_id=3)
    precision_class_08 = tf.keras.metrics.Precision(name="precision_classifcation_0.8", thresholds=0.8, class_id=3)
    recall_class_08 = tf.keras.metrics.Recall(name="recall_classifcation_0.8", thresholds=0.8, class_id=3)
    precision_class_09 = tf.keras.metrics.Precision(name="precision_classifcation_0.9", thresholds=0.9, class_id=3)
    recall_class_09 = tf.keras.metrics.Recall(name="recall_classifcation_0.9", thresholds=0.9, class_id=3)

    # For using custom loss import your loss function and use the name of the function as loss argument.
    # NOTE Tensorflow 2 does not allow precision metrics for regression tasks
    model.compile(loss='mean_squared_error',
                  optimizer='adam',
                  metrics=['accuracy', precision_class_05, recall_class_05,
                           precision_class_08, recall_class_08, precision_class_09, recall_class_09])

    """
        The save callback will overwrite the previous models if the new model is better then the last. Restarting the 
        training will always overwrite the models.
    """
    filepath = Path(args.output) / (model.name + ".h5")
    save_callback = tf.keras.callbacks.ModelCheckpoint(filepath=str(filepath), monitor='loss', verbose=1,
                                                       save_best_only=True)

    log_path = Path(args.output) / "logs" / (model.name + "_" + str(datetime.now()).replace(" ", "_").replace(":", "-"))
    log_callback = keras.callbacks.TensorBoard(log_dir=log_path, profile_batch=0)

    callbacks = [save_callback, log_callback]

    # TODO prepare an extra validation set, that is consistent over multiple runs
    # history = model.fit(x, y, batch_size=args.batch_size, epochs=args.epochs, verbose=1,
    # validation_data=(X_test, Y_test),callbacks=callbacks)
    history = model.fit(x, y, batch_size=args.batch_size, epochs=args.epochs, verbose=1, validation_split=0.1,
                        callbacks=callbacks)
    return history


if __name__ == '__main__':
    test_model = model_zoo.fy_1500_new()
    output_dir = "models"
    # forward commandline arguments to the argparser in the main function
    train_history = main(sys.argv[1:] + ['--output', output_dir], model=test_model)

    # save history in same folder as model
    history_filepath = Path(output_dir) / ("history_" + test_model.name + ".pkl")
    with open(str(history_filepath), "wb") as f:
        pickle.dump(train_history.history, f)
