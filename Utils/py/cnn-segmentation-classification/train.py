#!/usr/bin/env python3

import argparse
import pickle
import keras
from keras.models import *
from keras.layers import *

def conv_block(inputs, filters, kernel_size):
    x = inputs
    if kernel_size is not None and (kernel_size[0] == 3 and kernel_size[1] == 3):
        x = ZeroPadding2D((1,1))(x)
    x = Conv2D(filters, kernel_size, use_bias=False)(x)
    x = BatchNormalization()(x)
    x = Activation("relu")(x)
    return x

def small_sweaty3(num_classes=2):
    image_16x16 = Input((16,16,1))

    # image dimension: 16x16
    x = conv_block(image_16x16, 16, (1,1))
    x = conv_block(x, 32, (3,3))

    concat_16x16 = Concatenate()([image_16x16, x])

    # downscale to image dimension: 8x8
    image_8x8 = MaxPooling2D((2,2))(concat_16x16)

    x = conv_block(image_8x8, 32, (1,1))
    x = conv_block(x, 64, (3,3))
    x = conv_block(x, 32, (1,1))
    x = conv_block(x, 64, (3,3))

    concat_8x8 = Concatenate()([image_8x8, x])

    # downscale to image dimension: 4x4
    image_4x4 = MaxPooling2D((2,2))(concat_8x8)

    x = conv_block(image_4x4, 64, (1,1))
    x = conv_block(x, 128, (3,3))
    x = conv_block(x, 64, (1,1))
    x = conv_block(x, 128, (3,3))
    x = conv_block(x, 64, (3,3))

    # upscale to image dimension 8x8
    x = UpSampling2D((2,2))(x)

    x = Concatenate()([x, concat_8x8])

    x = conv_block(x, 64, (1,1))
    x = conv_block(x, 32, (3,3))
    x = conv_block(x, 32, (3,3))

    # upscale to image dimension 16x6
    x = UpSampling2D((2,2))(x)

    x = Concatenate()([x, concat_16x16])

    x = conv_block(x, 16,(1,1))
    x = conv_block(x, 16,(3,3))
    x = conv_block(x, num_classes, (3,3))



    return Model(inputs=image_16x16, outputs=x)

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

# define the keras network
model = small_sweaty3()
model.compile(loss='mean_squared_error',
              optimizer='adam',
              metrics=['accuracy'])

print(model.summary())

save_callback = keras.callbacks.ModelCheckpoint(filepath='model.h5', monitor='loss', verbose=1, save_best_only=True)

model.fit(x, y, batch_size=4, epochs=200, verbose=1, validation_split=0.1, callbacks=[save_callback])
model.save(model_path)