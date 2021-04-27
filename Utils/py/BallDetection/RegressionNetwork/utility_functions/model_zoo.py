"""
    use activation argument in layers instead of defining activation functions as extra layers. The result is the same
    (https://stackoverflow.com/questions/59476564/relu-as-a-parameter-in-dense-or-any-other-layer-vs-relu-as-a-layer-in-keras)
    but the code generators support it better
"""
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Convolution2D, LeakyReLU, MaxPooling2D, Flatten, Dense, ReLU, Input, \
    PReLU, Softmax, concatenate, Dropout, UpSampling2D
from tensorflow.keras import Model


# TODO maybe rewrite every model with functional api for more flexibility

def fy_1500_old():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_1500_old"

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
    # output is radius, x, y, confidence
    model.add(Dense(4, activation="relu", name="dense_1"))

    return model


def model1():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "model1"

    model.add(Convolution2D(4, (3, 3), input_shape=input_shape, padding='same', name="Conv2D_1"))
    model.add(ReLU(name="activation_1"))

    model.add(Convolution2D(4, (3, 3), padding='same', name="Conv2D_2"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_1"))
    model.add(ReLU(name="activation_2"))

    model.add(Convolution2D(8, (3, 3), padding='same', name="Conv2D_3"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_2"))
    model.add(ReLU(name="activation_3"))

    model.add(Convolution2D(8, (3, 3), padding='same', name="Conv2D_4"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_3"))
    model.add(ReLU(name="activation_4"))

    # classifier
    model.add(Flatten(name="flatten_1"))

    # output is radius, x, y, confidence
    model.add(Dense(4, activation="relu", name="dense_1"))

    return model


def bhuman_base():
    """
    architecture taken from the bhuman code release 2019
    :return:
    """
    input_shape = (32, 32, 1)
    model = Sequential()
    model._name = "bhuman_classificator2019"

    # we don't know the kernel size b-human used
    model.add(Convolution2D(8, (3, 3), input_shape=input_shape, padding='same', name="Conv2D_1"))
    # Batch Norm
    model.add(ReLU(name="activation_1"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_1"))

    # we don't know the kernel size b-human used
    model.add(Convolution2D(16, (3, 3), padding='same', name="Conv2D_2"))
    # Batch Norm
    model.add(ReLU(name="activation_2"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_2"))

    # we don't know the kernel size b-human used
    model.add(Convolution2D(16, (3, 3), padding='same', name="Conv2D_3"))
    # Batch Norm
    model.add(ReLU(name="activation_3"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_3"))

    # we don't know the kernel size b-human used
    model.add(Convolution2D(32, (3, 3), padding='same', name="Conv2D_4"))
    # Batch Norm
    model.add(ReLU(name="activation_4"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_4"))

    return model


def bhuman_classificator():
    """
    architecture taken from the bhuman code release 2019
    :return:
    """
    # TODO batch norm is missing
    model = bhuman_base()
    model.add(Flatten(name="flatten_1"))
    model.add(Dense(32, activation="relu", name="dense_1"))
    model.add(Dense(64, activation="relu", name="dense_2"))
    model.add(Dense(16, activation="relu", name="dense_3"))
    model.add(Dense(1, activation="relu", name="dense_4"))

    return model


def bhuman_detector():
    """
    architecture taken from the bhuman code release 2019
    :return:
    """
    # TODO batch norm is missing
    model = bhuman_base()
    model.add(Flatten(name="flatten_1"))
    model.add(Dense(32, activation="relu", name="dense_1"))
    model.add(Dense(64, activation="relu", name="dense_2"))
    model.add(Dense(3, activation="relu", name="dense_3"))

    return model


def semantic_segmentation01():
    in1 = Input(shape=(16, 16, 1))

    conv1 = Convolution2D(32, (3, 3), activation='relu', kernel_initializer='he_normal', padding='same')(in1)
    conv1 = Dropout(0.2)(conv1)
    conv1 = Convolution2D(32, (3, 3), activation='relu', kernel_initializer='he_normal', padding='same')(conv1)
    pool1 = MaxPooling2D((2, 2))(conv1)

    conv2 = Convolution2D(64, (3, 3), activation='relu', kernel_initializer='he_normal', padding='same')(pool1)
    conv2 = Dropout(0.2)(conv2)
    conv2 = Convolution2D(64, (3, 3), activation='relu', kernel_initializer='he_normal', padding='same')(conv2)
    pool2 = MaxPooling2D((2, 2))(conv2)

    conv3 = Convolution2D(128, (3, 3), activation='relu', kernel_initializer='he_normal', padding='same')(pool2)
    conv3 = Dropout(0.2)(conv3)
    conv3 = Convolution2D(128, (3, 3), activation='relu', kernel_initializer='he_normal', padding='same')(conv3)
    pool3 = MaxPooling2D((2, 2))(conv3)

    conv4 = Convolution2D(128, (3, 3), activation='relu', kernel_initializer='he_normal', padding='same')(pool3)
    conv4 = Dropout(0.2)(conv4)
    conv4 = Convolution2D(128, (3, 3), activation='relu', kernel_initializer='he_normal', padding='same')(conv4)

    up1 = concatenate([UpSampling2D((2, 2))(conv4), conv3], axis=-1)
    conv5 = Convolution2D(64, (3, 3), activation='relu', kernel_initializer='he_normal', padding='same')(up1)
    conv5 = Dropout(0.2)(conv5)
    conv5 = Convolution2D(64, (3, 3), activation='relu', kernel_initializer='he_normal', padding='same')(conv5)

    up2 = concatenate([UpSampling2D((2, 2))(conv5), conv2], axis=-1)
    conv6 = Convolution2D(64, (3, 3), activation='relu', kernel_initializer='he_normal', padding='same')(up2)
    conv6 = Dropout(0.2)(conv6)
    conv6 = Convolution2D(64, (3, 3), activation='relu', kernel_initializer='he_normal', padding='same')(conv6)

    up2 = concatenate([UpSampling2D((2, 2))(conv6), conv1], axis=-1)
    conv7 = Convolution2D(32, (3, 3), activation='relu', kernel_initializer='he_normal', padding='same')(up2)
    conv7 = Dropout(0.2)(conv7)
    conv7 = Convolution2D(32, (3, 3), activation='relu', kernel_initializer='he_normal', padding='same')(conv7)
    segmentation = Convolution2D(1, (1, 1), activation='sigmoid', name='seg')(conv7)

    model = Model(inputs=[in1], outputs=[segmentation])



    return model


# Tests that where not used

def fy_4000():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_4000"

    model.add(Convolution2D(4, (3, 3), input_shape=input_shape, padding='same', name="Conv2D_1"))
    model.add(LeakyReLU(alpha=0.0, name="activation_1"))  # alpha unknown, so default

    model.add(Convolution2D(8, (3, 3), padding='same', name="Conv2D_2"))
    model.add(LeakyReLU(name="activation_2"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_1"))

    model.add(Convolution2D(12, (3, 3), padding='same', name="Conv2D_3"))
    model.add(LeakyReLU(name="activation_3"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_2"))

    model.add(Convolution2D(16, (3, 3), padding='same', name="Conv2D_4"))
    model.add(LeakyReLU(alpha=0.0, name="activation_4"))

    model.add(Convolution2D(16, (1, 1), padding='same', name="Conv2D_5"))

    # classifier
    model.add(Flatten(name="flatten_1"))

    # output is radius, x, y, confidence
    model.add(Dense(4, activation="relu", name="dense_1"))

    return model


def fy_1300():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_1300"

    model.add(Convolution2D(4, (3, 3), input_shape=input_shape, padding='same', name="Conv2D_1"))
    model.add(LeakyReLU(alpha=0.0, name="activation_1"))  # alpha unknown, so default

    model.add(Convolution2D(5, (3, 3), padding='same', name="Conv2D_2"))
    model.add(LeakyReLU(name="activation_2"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_1"))

    model.add(Convolution2D(6, (3, 3), padding='same', name="Conv2D_3"))
    model.add(LeakyReLU(name="activation_3"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_2"))

    model.add(Convolution2D(7, (3, 3), padding='same', name="Conv2D_4"))
    model.add(LeakyReLU(alpha=0.0, name="activation_4"))

    model.add(Convolution2D(7, (1, 1), padding='same', name="Conv2D_5"))

    # classifier
    model.add(Flatten(name="flatten_1"))

    # output is radius, x, y, confidence
    model.add(Dense(4, activation="relu", name="dense_1"))

    return model


def fy_1300_flexrelu():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_1300_flexrelu"

    model.add(Convolution2D(4, (3, 3), input_shape=input_shape, padding='same', name="Conv2D_1"))
    model.add(ReLU(name="activation_1"))

    model.add(Convolution2D(5, (3, 3), padding='same', name="Conv2D_2"))
    model.add(PReLU(name="activation_2"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_1"))

    model.add(Convolution2D(6, (3, 3), padding='same', name="Conv2D_3"))
    model.add(PReLU(name="activation_3"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_2"))

    model.add(Convolution2D(7, (3, 3), padding='same', name="Conv2D_4"))
    model.add(ReLU(name="activation_4"))

    model.add(Convolution2D(7, (1, 1), padding='same', name="Conv2D_5"))

    # classifier
    model.add(Flatten(name="flatten_1"))

    # output is radius, x, y, confidence
    model.add(Dense(4, activation="relu", name="dense_1"))

    return model


def fy_max():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_max"

    model.add(Convolution2D(12, (3, 3), input_shape=input_shape, padding='same', name="Conv2D_1"))
    model.add(LeakyReLU(alpha=0.0, name="activation_1"))  # alpha unknown, so default

    model.add(Convolution2D(16, (3, 3), padding='same', name="Conv2D_2"))
    model.add(LeakyReLU(name="activation_2"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_1"))

    model.add(Convolution2D(24, (3, 3), padding='same', name="Conv2D_3"))
    model.add(LeakyReLU(name="activation_3"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_2"))

    model.add(Convolution2D(32, (3, 3), padding='same', name="Conv2D_4"))
    model.add(LeakyReLU(alpha=0.0, name="activation_4"))

    model.add(Convolution2D(32, (1, 1), padding='same', name="Conv2D_5"))

    # classifier
    model.add(Flatten(name="flatten_1"))

    # output is radius, x, y, confidence
    model.add(Dense(4, activation="relu", name="dense_1"))

    return model


def naodevils():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "naodevils"

    model.add(Convolution2D(8, (5, 5), input_shape=input_shape,
                            strides=(2, 2), padding='same', name="Conv2D_1"))
    model.add(ReLU(name="activation_1"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_1"))
    model.add(Convolution2D(12, (3, 3), name="Conv2D_2"))
    model.add(ReLU(name="activation_2"))
    model.add(Convolution2D(2, (2, 2), name="Conv2D_3"))
    model.add(Softmax(name="activation_3"))
    model.add(Flatten(name="flatten_1"))

    # output is radius, x, y, confidence
    model.add(Dense(4, name="dense_1"))

    return model


def fy_lessleaky():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_lessleaky"

    model.add(Convolution2D(4, (3, 3), input_shape=input_shape, padding='same', name="Conv2D_1"))
    model.add(LeakyReLU(name="activation_1"))

    model.add(Convolution2D(5, (3, 3), padding='same', name="Conv2D_2"))
    model.add(LeakyReLU(name="activation_2"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_1"))

    model.add(Convolution2D(6, (3, 3), padding='same', name="Conv2D_3"))
    model.add(LeakyReLU(name="activation_3"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_2"))

    model.add(Convolution2D(7, (3, 3), padding='same', activation="relu", name="Conv2D_4"))
    model.add(Convolution2D(7, (1, 1), padding='same', activation="relu", name="Conv2D_5"))

    # classifier
    model.add(Flatten(name="flatten_1"))

    # output is radius, x, y, confidence
    model.add(Dense(4, activation="relu", name="dense_1"))

    return model


def fy_half():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_half"

    model.add(Convolution2D(6, (3, 3), input_shape=input_shape, padding='same', name="Conv2D_1"))
    model.add(LeakyReLU(alpha=0.0, name="activation_1"))  # alpha unknown, so default

    model.add(Convolution2D(8, (3, 3), padding='same', name="Conv2D_2"))
    model.add(LeakyReLU(name="activation_2"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_1"))

    model.add(Convolution2D(16, (3, 3), padding='same', name="Conv2D_3"))
    model.add(LeakyReLU(name="activation_3"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_2"))

    model.add(Convolution2D(32, (3, 3), padding='same', name="Conv2D_4"))
    model.add(LeakyReLU(alpha=0.0, name="activation_4"))

    model.add(Convolution2D(16, (1, 1), padding='same', name="Conv2D_5"))

    # classifier
    model.add(Flatten(name="flatten_1"))
    # output is radius, x, y, confidence
    model.add(Dense(4, activation="relu", name="dense_1"))

    return model


def fy_step1():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_step1"

    model.add(Convolution2D(12, (3, 3), input_shape=input_shape, padding='same', name="Conv2D_1"))
    model.add(LeakyReLU(alpha=0.0, name="activation_1"))  # alpha unknown, so default

    model.add(Convolution2D(13, (3, 3), padding='same', name="Conv2D_2"))
    model.add(LeakyReLU(name="activation_2"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_1"))

    model.add(Convolution2D(14, (3, 3), padding='same', name="Conv2D_3"))
    model.add(LeakyReLU(name="activation_3"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_2"))

    model.add(Convolution2D(15, (3, 3), padding='same', name="Conv2D_4"))
    model.add(LeakyReLU(alpha=0.0, name="activation_4"))

    model.add(Convolution2D(15, (1, 1), padding='same', name="Conv2D_5"))

    # classifier
    model.add(Flatten(name="flatten_1"))
    # output is radius, x, y, confidence
    model.add(Dense(4, activation="relu", name="dense_1"))

    return model


def fy_lessfeat():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_lessfeat"

    model.add(Convolution2D(4, (3, 3), input_shape=input_shape, padding='same', name="Conv2D_1"))
    model.add(LeakyReLU(alpha=0.0, name="activation_1"))  # alpha unknown, so default

    model.add(Convolution2D(10, (3, 3), padding='same', name="Conv2D_2"))
    model.add(LeakyReLU(name="activation_2"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_1"))

    model.add(Convolution2D(20, (3, 3), padding='same', name="Conv2D_3"))
    model.add(LeakyReLU(name="activation_3"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_2"))

    model.add(Convolution2D(22, (3, 3), padding='same', name="Conv2D_4"))
    model.add(LeakyReLU(alpha=0.0, name="activation_4"))

    model.add(Convolution2D(22, (1, 1), padding='same', name="Conv2D_5"))

    # classifier
    model.add(Flatten(name="flatten_1"))
    # output is radius, x, y, confidence
    model.add(Dense(4, activation="relu", name="dense_1"))

    return model


def fy_1500_new():
    input_shape = (16, 16, 1)

    model = Sequential()
    model._name = "fy_1500_new"

    model.add(Convolution2D(4, (3, 3), input_shape=input_shape, padding='same', name="Conv2D_1", activation='relu'))
    # model.add(LeakyReLU(alpha=0.0, name="activation_1"))  # alpha unknown, so default

    model.add(Convolution2D(4, (3, 3), padding='same', name="Conv2D_2", activation='relu'))
    # model.add(LeakyReLU(name="activation_2"))
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(8, (3, 3), padding='same', name="Conv2D_3", activation="relu"))
    # model.add(LeakyReLU(name="activation_3"))
    model.add(MaxPooling2D(pool_size=(2, 2), name="pooling_1"))

    model.add(Convolution2D(8, (3, 3), padding='same', name="Conv2D_4", activation='relu'))
    # model.add(LeakyReLU(alpha=0.0, name="activation_4"))

    model.add(Convolution2D(8, (1, 1), padding='same', name="Conv2D_5"))

    # classifier
    model.add(Flatten(name="flatten_1"))
    # output is radius, x, y, confidence
    model.add(Dense(4, activation='relu', name="dense_1"))

    return model


def fy_1500_new2():
    # TODO fix the compiler for that
    input_layer = Input(shape=(16, 16, 1))
    x = Convolution2D(4, (3, 3), padding='same', name="Conv2D_1", activation='relu')(input_layer)
    x = Convolution2D(4, (3, 3), padding='same', name="Conv2D_2", activation='relu')(x)
    x = MaxPooling2D(pool_size=(2, 2))(x)
    x = Convolution2D(8, (3, 3), padding='same', name="Conv2D_3", activation="relu")(x)
    x = MaxPooling2D(pool_size=(2, 2), name="pooling_1")(x)
    x = Convolution2D(8, (3, 3), padding='same', name="Conv2D_4", activation='relu')(x)
    x = Convolution2D(8, (1, 1), padding='same', name="Conv2D_5")(x)
    x = Flatten(name="flatten_1")(x)
    # output is radius, x, y, confidence
    predictions = Dense(4, activation='relu', name="dense_1")(x)

    return Model(inputs=input_layer, outputs=predictions)
