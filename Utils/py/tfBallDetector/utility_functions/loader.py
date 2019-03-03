import random
from glob import glob
import cv2
import numpy as np
from keras.utils.np_utils import to_categorical
import imutils
from os.path import relpath


def adjust_gamma(image, gamma=1.0):
    invGamma = 1.0 / gamma
    table = np.array([((i / 255.0) ** invGamma) * 255
                      for i in np.arange(0, 256)]).astype("uint8")
    return cv2.LUT(image, table)


def loadImage(path, res):
    img = cv2.imread(path, 0) / 255
    return cv2.resize(img, (res["x"], res["y"]))


def getDBPath(path, clss, filetype):
    return glob(path + "/**/" + str(clss) + "/*." + filetype) + glob(path + "/" + str(clss) + "/*." + filetype)


def loadImages(path, res, num_classes, rotate=True, flip=True, gain=True, color=False):
    db = []
    print("Loading imgs...")
    num = np.zeros(num_classes)
    for clss in range(num_classes):
        for f in getDBPath(path, clss, "png") + getDBPath(path, clss, "jpg"):
            num[clss] += 1
            p = relpath(f, path)
            print("\rLoading " + f + "... \t\t(" + str(len(db)) + ")", end="")
            if color:
              img = cv2.imread(f)
            else:
              img = cv2.imread(f, 0)
            img = cv2.resize(img, (res["x"], res["y"]))
            db.append((img / 255, to_categorical(clss, num_classes), p))
            if clss == 1:  # Sorry, hard coded for now
                if rotate:
                    for a in (90, 180, 270):
                        db.append((imutils.rotate(img / 255, a), to_categorical(clss, num_classes), p))
                if flip:
                    db.append((cv2.flip(img, 1) / 255, to_categorical(clss, num_classes), p))
                if gain:
                    for g in (0.4, 1.3):
                        db.append((adjust_gamma(img, g) / 255, to_categorical(clss, num_classes), p))
    random.shuffle(db)
    x, y, p = list(map(np.array, list(zip(*db))))
    mean = np.mean(x)
    x -= mean
    if color:
        x = x.reshape(*x.shape)
    else:
        x = x.reshape(*x.shape, 1)
    print("Loading finished")
    print("Ball images: " + str(num[1]))
    print("Other images: " + str(num[0]))
    return x, y, mean, p