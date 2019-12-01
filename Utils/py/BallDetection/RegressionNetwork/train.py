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
parser.add_argument("--batch-size", dest="batch_size",
                    help="Batch size. Default is 256")

args = parser.parse_args()

imgdb_path = "img.db"
model_path = "model.h5"
log_dir = None
batch_size = 256

if args.imgdb_path is not None:
    imgdb_path = args.imgdb_path

if args.model_path is not None:
    model_path = args.model_path

if args.log is not None:
    log_dir = args.log

if args.batch_size is not None:
    batch_size = args.batch_size

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

# Define precision and recall for 0.5, 0.8 and 0.9 threshold
# class_id=3 means use the third element of the output vector
precision_class_05 = tf.keras.metrics.Precision(name="precision_classifcation_0.5", thresholds=0.5, class_id=3)
recall_class_05 = tf.keras.metrics.Recall(name="recall_classifcation_0.5", thresholds=0.5,class_id=3)
precision_class_08 = tf.keras.metrics.Precision(name="precision_classifcation_0.8", thresholds=0.8, class_id=3)
recall_class_08 = tf.keras.metrics.Recall(name="recall_classifcation_0.8", thresholds=0.8, class_id=3)
precision_class_09 = tf.keras.metrics.Precision(name="precision_classifcation_0.9", thresholds=0.9, class_id=3)
recall_class_09 = tf.keras.metrics.Recall(name="recall_classifcation_0.9", thresholds=0.9, class_id=3)

model.compile(loss='mean_squared_error',
              optimizer='adam',
              metrics=['accuracy', precision_class_05, recall_class_05,
                       precision_class_08, recall_class_08, precision_class_09, recall_class_09])

print(model.summary())

save_callback = tf.keras.callbacks.ModelCheckpoint(filepath=model_path, monitor='loss', verbose=1,
                                                   save_best_only=True)

callbacks = [save_callback]

if log_dir is not None:
    log_callback = keras.callbacks.TensorBoard(
        log_dir='./logs/' + str(datetime.now()).replace(" ", "_"), profile_batch=0)
    callbacks.append(log_callback)

history = model.fit(x, y, batch_size=batch_size, epochs=200, verbose=1, validation_split=0.1,
                    callbacks=callbacks)
model.save(model_path)
