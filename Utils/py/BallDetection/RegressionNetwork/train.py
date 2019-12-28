#!/usr/bin/env python3

import argparse
import pickle
from datetime import datetime
from pathlib import Path

import numpy as np
import tensorflow as tf
from tensorflow import keras as keras

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
        if 0 <= max_idx < len(self.acc):
            print("Maximum is {:.4f}".format(self.acc[max_idx]))


def str2bool(v):
    if v.lower() in ('yes', 'true', 't', 'y', '1'):
        return True
    elif v.lower() in ('no', 'false', 'f', 'n', '0'):
        return False
    else:
        raise argparse.ArgumentTypeError('Boolean value expected.')


parser = argparse.ArgumentParser(description='Train the network given ')

parser.add_argument('-b', '--database-path', dest='imgdb_path', default="img.db",
                    help='Path to the image database to use for training. '
                         'Default is img.db in current folder.')
parser.add_argument('-m', '--model-path', dest='model_path', default="model.h5",
                    help='Store the trained model using this path. Default is model.h5.')
parser.add_argument("--proceed", type=str2bool, nargs='?', dest="proceed",
                    const=True,
                    help="Use the stored and pre-trained model base.")
parser.add_argument("--log", dest="log", default="./logs/",
                    help="Tensorboard log location.")
parser.add_argument("--batch-size", dest="batch_size", default=256,
                    help="Batch size. Default is 256")

args = parser.parse_args()

if not Path(args.log).exists():
    Path.mkdir(Path(args.log))

with open(args.imgdb_path, "rb") as f:
    pickle.load(f)  # skip mean
    x = pickle.load(f)
    y = pickle.load(f)

# define the Keras network
if args.proceed is None or args.proceed is False:
    print("Creating new model")

    # FIXME make it possible to select a diffenrent model when calling train
    model = model_zoo.fy_1500()
else:
    print("Loading model " + args.model_path)
    model = tf.keras.models.load_model(args.model_path)

# Define precision and recall for 0.5, 0.8 and 0.9 threshold
# class_id=3 means use the third element of the output vector
precision_class_05 = tf.keras.metrics.Precision(name="precision_classifcation_0.5", thresholds=0.5, class_id=3)
recall_class_05 = tf.keras.metrics.Recall(name="recall_classifcation_0.5", thresholds=0.5, class_id=3)
precision_class_08 = tf.keras.metrics.Precision(name="precision_classifcation_0.8", thresholds=0.8, class_id=3)
recall_class_08 = tf.keras.metrics.Recall(name="recall_classifcation_0.8", thresholds=0.8, class_id=3)
precision_class_09 = tf.keras.metrics.Precision(name="precision_classifcation_0.9", thresholds=0.9, class_id=3)
recall_class_09 = tf.keras.metrics.Recall(name="recall_classifcation_0.9", thresholds=0.9, class_id=3)

model.compile(loss='mean_squared_error',
              optimizer='adam',
              metrics=['accuracy', precision_class_05, recall_class_05,
                       precision_class_08, recall_class_08, precision_class_09, recall_class_09])

model.summary()

save_callback = tf.keras.callbacks.ModelCheckpoint(filepath=args.model_path, monitor='loss', verbose=1,
                                                   save_best_only=True)

callbacks = [save_callback]

log_path = Path(args.log) / str(datetime.now()).replace(" ", "_").replace(":", "-")
log_callback = keras.callbacks.TensorBoard(log_dir=log_path, profile_batch=0)
callbacks.append(log_callback)

history = model.fit(x, y, batch_size=args.batch_size, epochs=200, verbose=1, validation_split=0.1,
                    callbacks=callbacks)
model.save(args.model_path)
