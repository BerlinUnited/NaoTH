#!/usr/bin/env python3

import tensorflow as tf
from tensorflow import keras as keras

import argparse
import pickle

from datetime import datetime
import numpy as np
import model_zoo


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
        if max_idx >= 0 and max_idx < len(self.acc):
            print("Maximum is {:.4f}".format(self.acc[max_idx]))


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
parser.add_argument('-m', '--model-path', dest='model_path',
                    help='Store the trained model using this path. Default is model.h5.')
parser.add_argument("--proceed", type=str2bool, nargs='?', dest="proceed",
                    const=True,
                    help="Use the stored and pre-trained model base.")
parser.add_argument("--log", dest="log",
                    help="Tensorboard log location.")

args = parser.parse_args()

imgdb_path = "img.db"
model_path = "model.h5"
log_dir = None

if args.imgdb_path is not None:
    imgdb_path = args.imgdb_path

if args.model_path is not None:
    model_path = args.model_path

if args.log is not None:
    log_dir = args.log

with open(imgdb_path, "rb") as f:
    pickle.load(f)  # skip mean
    x = pickle.load(f)
    y = pickle.load(f)

# define the Keras network
if args.proceed is None or args.proceed == False:
    print("Creating new model")

    model = model_zoo.fy_1500()
else:
    print("Loading model " + model_path)
    model = load_model(model_path)

model.compile(loss='mean_squared_error',
              optimizer='adam',
              metrics=['accuracy'])

print(model.summary())

save_callback = tf.keras.callbacks.ModelCheckpoint(filepath=model_path, monitor='loss', verbose=1,
                                                save_best_only=True)

callbacks = [save_callback]

if log_dir is not None:
    log_callback = keras.callbacks.TensorBoard(
        log_dir='./logs/' + str(datetime.now()).replace(" ", "_"))
    callbacks.append(log_callback)

history = model.fit(x, y, batch_size=64, epochs=80, verbose=1, validation_split=0.1,
                    callbacks=callbacks)
model.save(model_path)
