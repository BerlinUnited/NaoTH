#!/usr/bin/env python3

import argparse
import pickle
from keras.layers import Flatten, MaxPooling2D, Convolution2D, Dropout, LeakyReLU, UpSampling2D, Dense
from keras.models import Sequential

parser = argparse.ArgumentParser(description='Train the network given ')

parser.add_argument('-b', '--database-path', dest='imgdb_path',
                    help='Path to the image database to use for training. '
                         'Default is img.db in current folder.')
parser.add_argument('-m', '--model-path', dest='model_path',
                    help='Store the trained model using this path. Default is model.h5.')

args = parser.parse_args()

imgdb_path = "img.db"
model_path = "model.h5"

if args.imgdb_path is not None:
    imgdb_path = args.imgdb_path

if args.model_path is not None:
    model_path = args.model_path

with open(imgdb_path, "rb") as f:
    pickle.load(f) # skip mean
    x = pickle.load(f)
    y = pickle.load(f)

# The keras network. Adapt to your needs.

model = Sequential()
model.add(Convolution2D(8, (5, 5), input_shape=(x.shape[1], x.shape[2], 1),
                        activation='relu', strides=(2, 2), padding='same'))
model.add(MaxPooling2D(pool_size=(2, 2)))
model.add(Convolution2D(12, (3, 3), activation='relu'))
#model.add(LeakyReLU(alpha=0.2))
#model.add(MaxPooling2D(pool_size=(2, 2)))
#model.add(Dropout(0.4))
#model.add(Convolution2D(2, (2, 2), activation='softmax'))
model.add(Dense(1))
# begin upscaling
model.add(UpSampling2D(size=(8, 8)))

# Now define how to train the net.

model.compile(loss='mean_squared_error',
              optimizer='adam',
              metrics=['accuracy'])

print(model.summary())
model.fit(x, y, batch_size=1000, epochs=200, verbose=1, validation_split=0.1)
model.save(model_path)