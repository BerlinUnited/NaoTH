import random
from glob import glob
import cv2
import numpy as np
import imutils
from os.path import relpath
import os
import numpy as np
import math
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
            img = cv2.imread(f, cv2.IMREAD_GRAYSCALE)
            img = cv2.resize(img, (res["x"], res["y"]))
            
            # load mask file
            f_mask = mask_path + "/" +  os.path.basename(p)
            if os.path.exists(f_mask):
                img_mask = cv2.imread(f_mask, cv2.IMREAD_GRAYSCALE)
                img_mask = cv2.resize(img_mask, (res["x"], res["y"]))
            elif "noball" in mask_path:
                img_mask = np.zeros((res["x"], res["y"])).astype(np.uint8)
            else:
                raise "Missing mask file for " + f

            # fit circle on img_mask
            _,contours,_ = cv2.findContours(img_mask, 1, 2)
            
            debug_img = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)
            
            radius = 0
            x = res["x"]/2
            y = res["y"]/2
            # find largest circle (if existing)
            for cnt in contours:
                (nx,ny), nradius = cv2.minEnclosingCircle(cnt)
                if nradius >= radius:
                    x = nx
                    y = ny
                    radius = nradius + 1

            if radius > 0:
                # draw detected circle into debug image
                cv2.circle(debug_img, (int(x),int(y)), int(radius), color=(0,0,255))

                # normalize to resolution
                x = x / res["x"]
                y = y / res["y"]
                radius = radius / max(res["x"], res["y"])
            
            else:
                radius = 0
                x = 0.5
                y = 0.5
            
            y = np.array([radius, x, y])

            #cv2.namedWindow('image',cv2.WINDOW_NORMAL)
            #cv2.resizeWindow('image', 600,600)
            #cv2.imshow("image", debug_img)
            #cv2.waitKey()

            img = img.astype(float) / 255.0
            
            db.append((img, y, p))
            # augment: flip
#            db.append((cv2.flip(img, 1), y, p))
#            # augment: blur
#            db.append((cv2.GaussianBlur(img, (3,3), 1.2), y, p))
             
    random.shuffle(db)
    x, y, p = list(map(np.array, list(zip(*db))))
    mean = np.mean(x)
    x -= mean

    x = x.reshape(*x.shape, 1)
    
    print("Loading finished")
    print("images: " + str(len(x)))
    return x, y, mean, p

if __name__ == "__main__":
    loadImages("/home/thomas/src/nao2018/Utils/py/Blender/20190304_060526_patchMask", res={"x":16, "y":16})