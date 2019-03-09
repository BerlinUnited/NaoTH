import random
from glob import glob
import cv2
import numpy as np
import imutils
from os.path import relpath
import os
import numpy as np;
import cv2


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
    print("Loading images...")


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
            if os.path.exists(f_mask):
                img_mask = cv2.imread(f_mask, 0)
                img_mask = cv2.resize(img_mask, (res["x"], res["y"]))
            elif "noball" in mask_path:
                img_mask = np.zeros((res["x"], res["y"]))
            else:
                raise "Missing mask file for " + f

            img_mask = cv2.GaussianBlur(img_mask,(3,3), 0.4)
            
            #cv2.namedWindow('image',cv2.WINDOW_NORMAL)
            #cv2.resizeWindow('image', 600,600)
            #cv2.imshow("image", np.concatenate((img, img_mask), axis=1))
            #cv2.waitKey()

            #make the image mask a two channel
            img_mask_channel = np.zeros((res["x"], res["y"], 2))
            img_mask_channel[:,:,0] = 1-(img_mask/255)
            img_mask_channel[:,:,1] = img_mask/255

            db.append((img / 255, img_mask_channel, p))

    random.shuffle(db)
    x, y, p = list(map(np.array, list(zip(*db))))
    mean = np.mean(x)
    x -= mean

    x = x.reshape(*x.shape, 1)
    
    print("Loading finished")
    print("images: " + str(len(x)))
    return x, y, mean, p

if __name__ == "__main__":
    loadImages("/home/thomas/src/nao2018/Utils/py/Blender/training_set_patchMask", res={"x":16, "y":16})