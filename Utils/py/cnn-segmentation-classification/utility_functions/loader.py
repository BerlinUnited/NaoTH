import random
from glob import glob
import cv2
import numpy as np
from keras.utils.np_utils import to_categorical
import imutils
from os.path import relpath
import os


def adjust_gamma(image, gamma=1.0):
    invGamma = 1.0 / gamma
    table = np.array([((i / 255.0) ** invGamma) * 255
                      for i in np.arange(0, 256)]).astype("uint8")
    return cv2.LUT(image, table)


def loadImage(path, res):
    img = cv2.imread(path, 0) / 255
    return cv2.resize(img, (res["x"], res["y"]))


def getPatchPaths(path):
    return glob(path + "/**/*_patch_bw/") + glob(path + "/*_patch_bw/")


def loadImages(path, res):
    db = []
    print("Loading imgs...")


    for patch_folder in getPatchPaths(path):
        patch_folder_rel = str(relpath(patch_folder, path))
        mask_path = path + "/" + patch_folder_rel[0:-len("_patch_bw")] + "_patch_mask" 
        for f in glob(patch_folder + "/*.png"):
            
            p = relpath(f, path)
#            print("\rLoading " + f + "... \t\t(" + str(len(db)) + ")", end="")
            img = cv2.imread(f, 0)
            img = cv2.resize(img, (res["x"], res["y"]))

            # load mask file
            f_mask = mask_path + "/" +  os.path.basename(p)
            img_mask = cv2.imread(f_mask, 0)
            img_mask = cv2.resize(img, (res["x"], res["y"]))
            db.append((img / 255, img_mask == 255, p))

    random.shuffle(db)
    x, y, p = list(map(np.array, list(zip(*db))))
    mean = np.mean(x)
    x -= mean
    
    x = x.reshape(*x.shape, 1)
    y = y.reshape(*y.shape, 1)
    y = to_categorical(y, num_classes=2)

    print("Loading finished")
    print("images: " + str(len(x)))
    return x, y, p

if __name__ == "__main__":
    loadImages("/home/thomas/src/nao2018/Utils/py/Blender/training_set_patchMask", res={"x":16, "y":16})