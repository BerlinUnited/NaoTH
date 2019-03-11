#!/usr/bin/env python3

import argparse
import pickle
import keras
from keras.models import *
from keras.layers import *
from datetime import datetime

def conv_block(inputs, filters, kernel_size):
    x = inputs
    if kernel_size is not None and (kernel_size[0] == 3 and kernel_size[1] == 3):
        x = ZeroPadding2D((1,1))(x)
    elif kernel_size is not None and (kernel_size[0] == 5 and kernel_size[1] == 5):
        x = ZeroPadding2D((2,2))(x)

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

def sweaty_position():
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

    # classifier
    x = Flatten()(x)
    # radius, x, y
    x = Dense(3)(x)
    

    return Model(inputs=image_16x16, outputs=x)

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

def custom():
    image_16x16 = Input((16,16,1))

    # image dimension: 16x16
    x = conv_block(image_16x16, 16, (3,3))

    # downscale to image dimension: 8x8
    image_8x8 = MaxPooling2D((2,2))(x)

    x = conv_block(image_8x8, 16, (1,1))
    x = conv_block(x, 32, (3,3))
    concat_8x8 = Concatenate()([image_8x8, x])

    # downscale to image dimension: 4x4
    image_4x4 = MaxPooling2D((2,2))(concat_8x8)
    x = conv_block(image_4x4, 32, (1,1))

    # classifier
    x = Flatten()(x)
    # radius, x, y
    x = Dense(3)(x)
    

    return Model(inputs=image_16x16, outputs=x)

def yolo():
    input_shape = (16, 16, 1)

    model = Sequential()
    model.add(Convolution2D(12, (3, 3), input_shape=input_shape, padding='same'))
    model.add(LeakyReLU(alpha=0.0))  # alpha unknown, so default
    # model.add(BatchNormalization())

    model.add(Convolution2D(16, (3, 3), padding='same'))
    model.add(LeakyReLU())
    # model.add(BatchNormalization())
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(24, (3, 3), padding='same'))
    model.add(LeakyReLU())
    # model.add(BatchNormalization())
    model.add(MaxPooling2D(pool_size=(2, 2)))

    model.add(Convolution2D(32, (3, 3), padding='same'))
    model.add(LeakyReLU(alpha=0.0))
    # model.add(BatchNormalization())
#    model.add(MaxPooling2D(pool_size=(2, 2)))

 #   model.add(Convolution2D(32, (3, 3), padding='same'))
#    model.add(LeakyReLU())
    # model.add(BatchNormalization())

    model.add(Convolution2D(32, (1, 1), padding='same'))

#    model.add(Dense(32))
   
    # classifier
    model.add(Flatten())
    # radius, x, y
    model.add(Dense(3))

    return model

def yolo2():
    image_16x16 = Input((16,16,1))
    x = Convolution2D(12, (3, 3), input_shape=(16,16,1), padding='same')(image_16x16)
    x = LeakyReLU(alpha=0.0)(x)  # alpha unknown, so default

    x = Convolution2D(16, (3, 3), padding='same')(x)
    x = LeakyReLU()(x)

    x = Concatenate()([x, image_16x16])
    
    image_8x8 = MaxPooling2D(pool_size=(2, 2))(x)

    x = Convolution2D(24, (3, 3), padding='same')(image_8x8)
    x = LeakyReLU()(x)
    
    x = Concatenate()([x, image_8x8])
    image_4x4 = MaxPooling2D(pool_size=(2, 2))(x)

    x = Convolution2D(64, (1,1))(image_4x4)
    x = LeakyReLU()(x)
    x = Convolution2D(128, (3,3))(x)
    x = LeakyReLU()(x)
    x = Convolution2D(64, (1,1))(x)

   
    # classifier
    x= Flatten()(x)
    # radius, x, y
    x = Dense(3)(x)

    return Model(inputs=image_16x16, outputs=x)
    
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

# define the Keras network
if args.proceed is None or args.proceed == False:
    print("Creating new model")
    #model = small_sweaty3()
    #model = sweaty_position()
    model = yolo()
else:
    print("Loading model " + model_path)
    model = load_model(model_path)

model.compile(loss='mean_squared_error',
              optimizer='adam',
              metrics=['accuracy'])

print(model.summary())

save_callback = keras.callbacks.ModelCheckpoint(filepath=model_path, monitor='loss', verbose=1, save_best_only=True)
log_callback = keras.callbacks.TensorBoard(log_dir='./logs/' + str(datetime.now()).replace(" ", "_"))

history = model.fit(x, y, batch_size=64, epochs=40, verbose=1, validation_split=0.1, callbacks=[save_callback, log_callback])
model.save(model_path)