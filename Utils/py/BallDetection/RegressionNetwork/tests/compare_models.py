"""
    Compare two different implementations of the same architecture. They should output the same
"""
import pickle
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Convolution2D, LeakyReLU, MaxPooling2D, Flatten, Dense
import tensorflow as tf


def fy_1500_old():
    initializer = tf.keras.initializers.Ones()
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_1500_old"

    model.add(Convolution2D(4, (3, 3), input_shape=input_shape, padding='same', name="Conv2D_1",
                            kernel_initializer=initializer))
    model.add(LeakyReLU(alpha=0.0, name="activation_1"))  # alpha unknown, so default

    model.add(Convolution2D(4, (3, 3), padding='same', name="Conv2D_2", kernel_initializer=initializer))
    model.add(LeakyReLU(name="activation_2"))
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(8, (3, 3), padding='same', name="Conv2D_3", kernel_initializer=initializer))
    model.add(LeakyReLU(name="activation_3"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_1"))

    model.add(Convolution2D(8, (3, 3), padding='same', name="Conv2D_4", kernel_initializer=initializer))
    model.add(LeakyReLU(alpha=0.0, name="activation_4"))

    model.add(Convolution2D(8, (1, 1), padding='same', name="Conv2D_5", kernel_initializer=initializer))

    # classifier
    model.add(Flatten(name="flatten_1"))
    # output is radius, x, y, confidence
    model.add(Dense(4, activation='relu', name="dense_1", kernel_initializer=initializer))

    return model


def fy_1500_new():
    initializer = tf.keras.initializers.Ones()
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_1500_new"

    model.add(Convolution2D(4, (3, 3), input_shape=input_shape, padding='same', name="Conv2D_1", activation='relu',
                            kernel_initializer=initializer))

    model.add(
        Convolution2D(4, (3, 3), padding='same', name="Conv2D_2", activation='relu', kernel_initializer=initializer))
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(
        Convolution2D(8, (3, 3), padding='same', name="Conv2D_3", activation="relu", kernel_initializer=initializer))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_1"))

    model.add(
        Convolution2D(8, (3, 3), padding='same', name="Conv2D_4", activation='relu', kernel_initializer=initializer))

    model.add(Convolution2D(8, (1, 1), padding='same', name="Conv2D_5", kernel_initializer=initializer))

    # classifier
    model.add(Flatten(name="flatten_1"))
    # output is radius, x, y, confidence
    model.add(Dense(4, activation='relu', name="dense_1", kernel_initializer=initializer))

    return model


model_new = fy_1500_new()
model_old = fy_1500_old()

with open("../data/imgdb.pkl", "rb") as f:
    mean = pickle.load(f)
    print("mean=" + str(mean))
    x = pickle.load(f)
    y = pickle.load(f)

test_data = x[0].reshape((1, 16, 16, 1))

print("All the outputs should be the same!")
for i in range(10):
    a = model_new.predict(test_data)
    print(a)
print()
for i in range(10):
    a = model_old.predict(test_data)
    print(a)
