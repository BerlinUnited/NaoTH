#!/usr/bin/env python3

import argparse
import pickle
import sys
from datetime import datetime
from pathlib import Path
from sys import exit

# TODO encode dataset into output model name
import tensorflow as tf
from tensorflow import keras as keras

import utility_functions.model_zoo as model_zoo

DATA_DIR = Path(Path(__file__).parent.absolute() / "data").resolve()


def str2bool(v):
    if v.lower() in ('yes', 'true', 't', 'y', '1'):
        return True
    elif v.lower() in ('no', 'false', 'f', 'n', '0'):
        return False
    else:
        raise argparse.ArgumentTypeError('Boolean value expected.')


def main(raw_args=None, model=None):
    parser = argparse.ArgumentParser(description='Train the network given')

    parser.add_argument('-b', '--database-path', dest='imgdb_path', default=str(DATA_DIR / 'tk03.pkl'),
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

    # For using custom loss import your loss function and use the name of the function as loss argument.
    model.compile(loss='mean_squared_error',
                  optimizer='adam',
                  metrics=['accuracy'])

    """
        The save callback will overwrite the previous models if the new model is better then the last. Restarting the 
        training will always overwrite the models.
    """
    filepath = Path(args.output) / (model.name + "_" + Path(args.imgdb_path).name + ".h5")
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
    history_filepath = Path(output_dir) / ("history_" + test_model.name + "_tk03.pkl")
    with open(str(history_filepath), "wb") as f:
        pickle.dump(train_history.history, f)
