#!/usr/bin/env python3

from keras.layers import Flatten, MaxPooling2D, Convolution2D, Dropout, LeakyReLU, BatchNormalization
from keras.models import Sequential
from utility_functions.ocg import keras_compile
import numpy as np

input_shape = (16, 12, 3)

model_same = Sequential()
model_same.add(Convolution2D(4, (3, 3), input_shape=input_shape, padding='same'))
model_same.add(Convolution2D(4, (5, 3), strides=(2, 3), input_shape=input_shape, padding='same'))

model_same.compile(loss='categorical_crossentropy',
                   optimizer='adam',
                   metrics=['accuracy'])

model_same.summary()
model_same.save("conv_test_same.h5")

model_valid = Sequential()
model_valid.add(Convolution2D(4, (3, 3), input_shape=input_shape, padding='valid'))
model_valid.add(Convolution2D(4, (5, 3), strides=(2, 3), input_shape=input_shape, padding='valid'))

model_valid.compile(loss='categorical_crossentropy',
                   optimizer='adam',
                   metrics=['accuracy'])

model_valid.summary()
model_valid.save("conv_test_valid.h5")

np.random.seed(42)
array = np.random.randint(0, 255+1, size=input_shape).astype(np.float32)

test_img_db = {'images': [array], 'mean': 0.0}  # Replace this by real images

# same padding
print("Test 1")
keras_compile(test_img_db, "conv_test_same.h5", "conv_test.c", unroll_level=0, arch="sse3")
print("Test 2")
keras_compile(test_img_db, "conv_test_same.h5", "conv_test.c", unroll_level=1, arch="sse3")
print("Test 3")
keras_compile(test_img_db, "conv_test_same.h5", "conv_test.c", unroll_level=2, arch="sse3")

print("Test 4")
keras_compile(test_img_db, "conv_test_same.h5", "conv_test_conv_mode_1.c", unroll_level=0, arch="sse3", conv_mode=1)
print("Test 5")
keras_compile(test_img_db, "conv_test_same.h5", "conv_test_conv_mode_1.c", unroll_level=1, arch="sse3", conv_mode=1)
print("Test 6")
keras_compile(test_img_db, "conv_test_same.h5", "conv_test_conv_mode_1.c", unroll_level=2, arch="sse3", conv_mode=1)

print("Test 7")
keras_compile(test_img_db, "conv_test_same.h5", "conv_test_conv_mode_2.c", unroll_level=0, arch="sse3", conv_mode=2)
print("Test 8")
keras_compile(test_img_db, "conv_test_same.h5", "conv_test_conv_mode_2.c", unroll_level=1, arch="sse3", conv_mode=2)
print("Test 9")
keras_compile(test_img_db, "conv_test_same.h5", "conv_test_conv_mode_2.c", unroll_level=2, arch="sse3", conv_mode=2)

# valid padding
print("Test 10")
keras_compile(test_img_db, "conv_test_valid.h5", "conv_test.c", unroll_level=0, arch="sse3")
print("Test 11")
keras_compile(test_img_db, "conv_test_valid.h5", "conv_test.c", unroll_level=1, arch="sse3")
print("Test 12")
keras_compile(test_img_db, "conv_test_valid.h5", "conv_test.c", unroll_level=2, arch="sse3")

print("Test 13")
keras_compile(test_img_db, "conv_test_valid.h5", "conv_test_conv_mode_1.c", unroll_level=0, arch="sse3", conv_mode=1)
print("Test 14")
keras_compile(test_img_db, "conv_test_valid.h5", "conv_test_conv_mode_1.c", unroll_level=1, arch="sse3", conv_mode=1)
print("Test 15")
keras_compile(test_img_db, "conv_test_valid.h5", "conv_test_conv_mode_1.c", unroll_level=2, arch="sse3", conv_mode=1)

print("Test 16")
keras_compile(test_img_db, "conv_test_valid.h5", "conv_test_conv_mode_2.c", unroll_level=0, arch="sse3", conv_mode=2)
print("Test 17")
keras_compile(test_img_db, "conv_test_valid.h5", "conv_test_conv_mode_2.c", unroll_level=1, arch="sse3", conv_mode=2)
print("Test 18")
keras_compile(test_img_db, "conv_test_valid.h5", "conv_test_conv_mode_2.c", unroll_level=2, arch="sse3", conv_mode=2)
