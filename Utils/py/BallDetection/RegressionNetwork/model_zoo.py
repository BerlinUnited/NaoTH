import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Convolution2D, LeakyReLU, MaxPooling2D, Flatten, Dense, ReLU, \
    PReLU


def fy_1500():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_1500"

    model.add(Convolution2D(4, (3, 3), input_shape=input_shape, padding='same', name="Conv2D_1"))
    model.add(LeakyReLU(alpha=0.0, name="activation_1"))  # alpha unknown, so default

    model.add(Convolution2D(4, (3, 3), padding='same', name="Conv2D_2"))
    model.add(LeakyReLU(name="activation_2"))
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(8, (3, 3), padding='same', name="Conv2D_3"))
    model.add(LeakyReLU(name="activation_3"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_1"))

    model.add(Convolution2D(8, (3, 3), padding='same', name="Conv2D_4"))
    model.add(LeakyReLU(alpha=0.0, name="activation_4"))

    model.add(Convolution2D(8, (1, 1), padding='same', name="Conv2D_5"))

    # classifier
    model.add(Flatten(name="flatten_1"))
    # model.add(Dense(32))
    # radius, x, y
    # TODO order of values, also missing confidence
    model.add(Dense(4, activation="relu", name="dense_1"))

    return model


def model1():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "model1"

    model.add(Convolution2D(4, (3, 3), input_shape=input_shape, padding='same'))
    model.add(ReLU())

    model.add(Convolution2D(4, (3, 3), padding='same'))
    model.add(MaxPooling2D(pool_size=(2, 2)))
    model.add(ReLU())

    model.add(Convolution2D(8, (3, 3), padding='same'))
    model.add(MaxPooling2D(pool_size=(2, 2)))
    model.add(ReLU())

    model.add(Convolution2D(8, (3, 3), padding='same'))
    model.add(MaxPooling2D(pool_size=(2, 2)))
    model.add(ReLU())

    # model.add(Convolution2D(8, (1, 1), padding='same'))

    # classifier
    model.add(Flatten())
    #    model.add(Dense(32))
    # radius, x, y
    model.add(Dense(4, activation="relu"))

    return model


# Tests that where not used

def fy_4000():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_4000"

    model.add(Convolution2D(4, (3, 3), input_shape=input_shape, padding='same'))
    model.add(LeakyReLU(alpha=0.0))  # alpha unknown, so default

    model.add(Convolution2D(8, (3, 3), padding='same'))
    model.add(LeakyReLU())
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(12, (3, 3), padding='same'))
    model.add(LeakyReLU())
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(16, (3, 3), padding='same'))
    model.add(LeakyReLU(alpha=0.0))

    model.add(Convolution2D(16, (1, 1), padding='same'))

    # classifier
    model.add(Flatten())
    #    model.add(Dense(32))
    # radius, x, y
    model.add(Dense(4, activation="relu"))

    return model


def fy_1300():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_1300"

    model.add(Convolution2D(4, (3, 3), input_shape=input_shape, padding='same'))
    model.add(LeakyReLU(alpha=0.0))  # alpha unknown, so default

    model.add(Convolution2D(5, (3, 3), padding='same'))
    model.add(LeakyReLU())
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(6, (3, 3), padding='same'))
    model.add(LeakyReLU())
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(7, (3, 3), padding='same'))
    model.add(LeakyReLU(alpha=0.0))

    model.add(Convolution2D(7, (1, 1), padding='same'))

    # classifier
    model.add(Flatten())
    #    model.add(Dense(32))
    # radius, x, y
    model.add(Dense(4, activation="relu"))

    return model


def fy_1300_flexrelu():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_1300_flexrelu"

    model.add(Convolution2D(4, (3, 3), input_shape=input_shape, padding='same'))
    model.add(ReLU())

    model.add(Convolution2D(5, (3, 3), padding='same'))
    model.add(PReLU())
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(6, (3, 3), padding='same'))
    model.add(PReLU())
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(7, (3, 3), padding='same'))
    model.add(ReLU())

    model.add(Convolution2D(7, (1, 1), padding='same'))

    # classifier
    model.add(Flatten())
    #    model.add(Dense(32))
    # radius, x, y
    model.add(Dense(4, activation="relu"))

    return model


def fy_max():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_max"

    model.add(Convolution2D(12, (3, 3), input_shape=input_shape, padding='same'))
    model.add(LeakyReLU(alpha=0.0))  # alpha unknown, so default

    model.add(Convolution2D(16, (3, 3), padding='same'))
    model.add(LeakyReLU())
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(24, (3, 3), padding='same'))
    model.add(LeakyReLU())
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(32, (3, 3), padding='same'))
    model.add(LeakyReLU(alpha=0.0))

    model.add(Convolution2D(32, (1, 1), padding='same'))

    # classifier
    model.add(Flatten())
    #    model.add(Dense(32))
    # radius, x, y
    model.add(Dense(4, activation="relu"))

    return model


def naodevils():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "naodevils"

    model.add(Convolution2D(8, (5, 5), input_shape=input_shape,
                            activation='relu', strides=(2, 2), padding='same'))
    model.add(MaxPooling2D(pool_size=(2, 2)))
    model.add(Convolution2D(12, (3, 3), activation='relu'))
    model.add(Convolution2D(2, (2, 2), activation='softmax'))
    model.add(Flatten())

    # radius, x, y
    model.add(Dense(4))

    return model


def fy_lessleaky():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_lessleaky"

    model.add(Convolution2D(4, (3, 3), input_shape=input_shape, padding='same'))
    model.add(LeakyReLU())

    model.add(Convolution2D(5, (3, 3), padding='same'))
    model.add(LeakyReLU())
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(6, (3, 3), padding='same'))
    model.add(LeakyReLU())
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(7, (3, 3), padding='same', activation="relu"))
    model.add(Convolution2D(7, (1, 1), padding='same', activation="relu"))

    # classifier
    model.add(Flatten())
    #    model.add(Dense(32))
    # radius, x, y
    model.add(Dense(4, activation="relu"))

    return model


def fy_half():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_half"

    model.add(Convolution2D(6, (3, 3), input_shape=input_shape, padding='same'))
    model.add(LeakyReLU(alpha=0.0))  # alpha unknown, so default
    # model.add(BatchNormalization())

    model.add(Convolution2D(8, (3, 3), padding='same'))
    model.add(LeakyReLU())
    # model.add(BatchNormalization())
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(16, (3, 3), padding='same'))
    model.add(LeakyReLU())
    # model.add(BatchNormalization())
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(32, (3, 3), padding='same'))
    model.add(LeakyReLU(alpha=0.0))
    # model.add(BatchNormalization())
    #    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(16, (1, 1), padding='same'))

    # classifier
    model.add(Flatten())
    #    model.add(Dense(32))
    # radius, x, y
    model.add(Dense(4, activation="relu"))

    return model


def fy_step1():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_step1"

    model.add(Convolution2D(12, (3, 3), input_shape=input_shape, padding='same'))
    model.add(LeakyReLU(alpha=0.0))  # alpha unknown, so default

    model.add(Convolution2D(13, (3, 3), padding='same'))
    model.add(LeakyReLU())
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(14, (3, 3), padding='same'))
    model.add(LeakyReLU())
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(15, (3, 3), padding='same'))
    model.add(LeakyReLU(alpha=0.0))

    model.add(Convolution2D(15, (1, 1), padding='same'))

    # classifier
    model.add(Flatten())
    # radius, x, y
    model.add(Dense(4, activation="relu"))

    return model


def fy_lessfeat():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_lessfeat"

    model.add(Convolution2D(4, (3, 3), input_shape=input_shape, padding='same'))
    model.add(LeakyReLU(alpha=0.0))  # alpha unknown, so default
    # model.add(BatchNormalization())

    model.add(Convolution2D(10, (3, 3), padding='same'))
    model.add(LeakyReLU())
    # model.add(BatchNormalization())
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(20, (3, 3), padding='same'))
    model.add(LeakyReLU())
    # model.add(BatchNormalization())
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(22, (3, 3), padding='same'))
    model.add(LeakyReLU(alpha=0.0))
    # model.add(BatchNormalization())
    #    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(22, (1, 1), padding='same'))

    # classifier
    model.add(Flatten())
    #    model.add(Dense(32))
    # radius, x, y
    model.add(Dense(4, activation="relu"))

    return model
