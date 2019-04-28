from keras.models import *
from keras.layers import *

def fy_1300():
    input_shape = (16, 16, 1)

    model = Sequential()
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
    model.add(Dense(3, activation="relu"))

    return model

def fy_1300_flexrelu():
    input_shape = (16, 16, 1)

    model = Sequential()
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
    model.add(Dense(3, activation="relu"))

    return model

### Tests that where not used

def fy_max():
    input_shape = (16, 16, 1)

    model = Sequential()
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
    model.add(Dense(3, activation="relu"))

    return model


def naodevils():
    model = Sequential()
    model.add(Convolution2D(8, (5, 5), input_shape=(x.shape[1], x.shape[2], 1),
                            activation='relu', strides=(2, 2), padding='same'))
    model.add(MaxPooling2D(pool_size=(2, 2)))
    model.add(Convolution2D(12, (3, 3), activation='relu'))
    model.add(Convolution2D(2, (2, 2), activation='softmax'))
    model.add(Flatten())

    # radius, x, y
    model.add(Dense(3))

    return model

def fy_lessleaky():
    input_shape = (16, 16, 1)

    model = Sequential()
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
    model.add(Dense(3, activation="relu"))

    return model


def fy_1500():
    input_shape = (16, 16, 1)

    model = Sequential()
    model.add(Convolution2D(4, (3, 3), input_shape=input_shape, padding='same'))
    model.add(LeakyReLU(alpha=0.0))  # alpha unknown, so default

    model.add(Convolution2D(4, (3, 3), padding='same'))
    model.add(LeakyReLU())
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(8, (3, 3), padding='same'))
    model.add(LeakyReLU())
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(8, (3, 3), padding='same'))
    model.add(LeakyReLU(alpha=0.0))

    model.add(Convolution2D(8, (1, 1), padding='same'))
   
    # classifier
    model.add(Flatten())
    #    model.add(Dense(32))
    # radius, x, y
    model.add(Dense(3, activation="relu"))

    return model

def fy_half():
    input_shape = (16, 16, 1)

    model = Sequential()
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
    model.add(Dense(3, activation="relu"))

    return model

def fy_step1():
    input_shape = (16, 16, 1)

    model = Sequential()
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
    model.add(Dense(3, activation="relu"))

    return model

def fy_lessfeat():
    input_shape = (16, 16, 1)

    model = Sequential()
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
    model.add(Dense(3, activation="relu"))

    return model
    