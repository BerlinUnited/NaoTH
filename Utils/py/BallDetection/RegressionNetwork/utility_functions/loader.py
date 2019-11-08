import random
from glob import glob
from os.path import relpath
import os
import numpy as np
import cv2
from pathlib import Path


def adjust_gamma(image, gamma=1.0):
    inv_gamma = 1.0 / gamma
    table = np.array([((i / 255.0) ** inv_gamma) * 255
                      for i in np.arange(0, 256)]).astype("uint8")
    return cv2.LUT(image, table)


def get_patch_paths(path):
    patch_folders = glob(path + "/**/*_patch_bw") + glob(path + "/*_patch_bw")
    mask_folders = glob(path + "/**/*_patch_mask") + glob(path + "/*_patch_mask")

    return zip(patch_folders, mask_folders)


def load_images(path, res):
    db = []
    print("Loading images...")
    for patch_folder, mask_folder in get_patch_paths(path):
        print("patch_folder: ", patch_folder)
        print("mask_folders: ", mask_folder)

        patch_images = list(Path(patch_folder).glob('**/*.png'))
        for patch_image in patch_images:
            relativ_patch_path = relpath(str(patch_image), path)
            print("P: ", relativ_patch_path)
            img = cv2.imread(str(patch_image), cv2.IMREAD_GRAYSCALE)
            img = cv2.resize(img, (res["x"], res["y"]))

            # load mask file
            f_mask = Path(mask_folder) / patch_image.name

            if f_mask.exists():
                img_mask = cv2.imread(str(f_mask), cv2.IMREAD_GRAYSCALE)
                img_mask = cv2.resize(img_mask, (res["x"], res["y"]))
            elif "noball" in mask_folder:
                # TODO check if the substring search actually works
                img_mask = np.zeros((res["x"], res["y"])).astype(np.uint8)
            else:
                raise "Missing mask file for " + str(patch_image)

            # fit circle on img_mask
            contours, _ = cv2.findContours(img_mask, 1, 2)

            debug_img = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)

            radius = 0
            x = res["x"] / 2
            y = res["y"] / 2
            # find largest circle (if existing)
            for cnt in contours:
                (nx, ny), nradius = cv2.minEnclosingCircle(cnt)
                if nradius >= radius:
                    x = nx
                    y = ny
                    radius = nradius + 1

            if radius > 0:
                # draw detected circle into debug image
                cv2.circle(debug_img, (int(x), int(y)), int(radius), color=(0, 0, 255))

                # normalize to resolution
                x = x / res["x"]
                y = y / res["y"]
                radius = radius / max(res["x"], res["y"])

            else:
                radius = 0
                x = 0.5
                y = 0.5

            y = np.array([radius, x, y])

            # cv2.namedWindow('image',cv2.WINDOW_NORMAL)
            # cv2.resizeWindow('image', 600,600)
            # cv2.imshow("image", debug_img)
            # cv2.waitKey()

            img = img.astype(float) / 255.0

            db.append((img, y, relativ_patch_path))
            # augment: flip
            #            db.append((cv2.flip(img, 1), y, p))
            #            # augment: blur
            #            db.append((cv2.GaussianBlur(img, (3,3), 1.2), y, p))
            # augment: reduce contrast
            db.append((np.clip((0.75 * img) + 0.25, 0, 1), y, relativ_patch_path))

    random.shuffle(db)
    x, y, p = list(map(np.array, list(zip(*db))))
    mean = np.mean(x)
    x -= mean

    x = x.reshape(*x.shape, 1)

    print("Loading finished")
    print("images: " + str(len(x)))
    return x, y, mean, p


if __name__ == "__main__":
    load_images("../data/TK-03/blender/", res={"x": 16, "y": 16})
