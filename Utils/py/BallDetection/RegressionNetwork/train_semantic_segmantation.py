from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Convolution2D, LeakyReLU, MaxPooling2D, Flatten, Dense, ReLU, Input, \
    PReLU, Softmax, Dropout, UpSampling2D, concatenate
from tensorflow.keras import Model
import cv2
import numpy as np




a = get_model()
a.summary()

# TODO load a blender folder correctly
base_path = "C:/RoboCup/NaoTH-2018/Utils/py/BallDetection/RegressionNetwork/data/TK-03/blender/20190303_143233_patchMask/"
img0 = cv2.imread(base_path + "ball_patch_bw/0_bottom.png", cv2.IMREAD_GRAYSCALE)
img1 = cv2.imread(base_path + "ball_patch_bw/1_top.png", cv2.IMREAD_GRAYSCALE)
img2 = cv2.imread(base_path + "ball_patch_bw/2_top.png", cv2.IMREAD_GRAYSCALE)

mask0 = cv2.imread(base_path + "ball_patch_mask/0_bottom.png", cv2.IMREAD_GRAYSCALE)
mask1 = cv2.imread(base_path + "ball_patch_mask/1_top.png", cv2.IMREAD_GRAYSCALE)
mask2 = cv2.imread(base_path + "ball_patch_mask/2_top.png", cv2.IMREAD_GRAYSCALE)

image_list = [img0, img1, img2]
mask_list = [mask0, mask1, mask2]

x = np.array(image_list)
x = x.reshape(*x.shape, 1)

y = np.array(image_list)
y= y.reshape(*y.shape, 1)
print(x.shape)
print(y.shape)

history = a.fit(x, y, batch_size=1, epochs=1, verbose=1)