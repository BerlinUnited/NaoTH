import csv
import json
import os
import random
from glob import glob
from os.path import relpath
from pathlib import Path

import cv2
import numpy as np


def adjust_gamma(image, gamma=1.0):
    inv_gamma = 1.0 / gamma
    table = np.array([((i / 255.0) ** inv_gamma) * 255
                      for i in np.arange(0, 256)]).astype("uint8")
    return cv2.LUT(image, table)


def get_blender_patch_paths(path):
    patch_folders = glob(path + "/**/*_patch_bw") + glob(path + "/*_patch_bw")
    mask_folders = glob(path + "/**/*_patch_mask") + glob(path + "/*_patch_mask")

    return zip(patch_folders, mask_folders)


def load_images(path, res):
    db = []
    print("Loading images...")
    for patch_folder, mask_folder in get_blender_patch_paths(path):
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


def load_image_from_csv(path, db_balls, db_noballs, res):
    print("Loading images from " + path + "...")

    # parse csv file
    with open(path, newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        next(reader)
        for row in reader:
            f = os.path.join(os.path.dirname(path), row["filename"])
            p = row["filename"]

            # load image
            try:
                img = cv2.imread(f, cv2.IMREAD_GRAYSCALE)
                img = cv2.resize(img, (res["x"], res["y"]))
            except Exception as ex:
                print("Error loading image ", f)
                continue

            is_ball = False
            # load ball information
            num_regions = int(row["region_count"])
            if num_regions > 0:
                atts = json.loads(row["region_attributes"])
                if atts["type"] == "smudged_ball":
                    # ignore this image
                    continue
                elif atts["type"] == "ball":
                    shape = json.loads(row["region_shape_attributes"])
                    if shape["name"] == "circle":
                        x = int(shape["cx"])
                        y = int(shape["cy"])
                        radius = int(shape["r"])

                        if x < 0 or y < 0 or x > res["x"] or y > res["y"]:
                            continue

                        # draw detected circle into debug image
                        # cv2.circle(debug_img, (int(x),int(y)), int(radius), color=(0,0,255))

                        # normalize to resolution
                        x = (x / res["x"])
                        y = (y / res["y"])
                        radius = radius / max(res["x"], res["y"])

                        is_ball = True
                    else:
                        # we only support circles
                        continue
                else:
                    # unknown type
                    print("Unknown type \"" + atts["type"] + "\" in file " + f)
                    continue
            else:
                # no region means no ball
                radius = 0.0
                x = 0
                y = 0

            # for each row add the image and the prediction
            if is_ball:
                y = np.array([radius, x, y, 1.0])
            else:
                y = np.array([radius, x, y, 0.0])

            img_f = img.astype(float) / 255.0

            if is_ball:
                db_balls.append((img_f, y, p))
            else:
                db_noballs.append((img_f, y, p))

            avg_img_f = np.average(img_f)

            # augment: binarized image
            bin_img = cv2.adaptiveThreshold(img, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
                                            cv2.THRESH_BINARY, 11, 2).reshape((16, 16))

            if is_ball:
                db_balls.append((bin_img.astype(float) / 255.0, y, p))
            else:
                db_noballs.append((bin_img.astype(float) / 255.0, y, p))

            if 0.2 <= avg_img_f <= 0.8:
                # augment: gamma
                for g in (0.4, 1.3):
                    if is_ball:
                        db_balls.append((adjust_gamma(img, g).astype(float) / 255.0, y, p))
                    else:
                        db_noballs.append((adjust_gamma(img, g).astype(float) / 255.0, y, p))


def load_images_from_csv_files(root_path, res, limit_noballs):
    print("Looking for csv files in: ", root_path)
    db_balls = []
    db_noballs = []

    # find csv files
    all_paths = list(Path(root_path).absolute().glob('**/*.csv'))
    print(all_paths)

    for path in all_paths:
        load_image_from_csv(str(path), db_balls, db_noballs, res)

    if limit_noballs is True and len(db_balls) < len(db_noballs):
        db_noballs = np.random.choice(db_noballs, len(db_balls))

    db = db_balls + db_noballs
    random.shuffle(db)

    x, y, p = list(map(np.array, list(zip(*db))))
    mean = np.mean(x)
    x -= mean

    x = x.reshape(*x.shape, 1)

    print("Loading finished")
    print("images: " + str(len(x)) + " balls: " + str(len(db_balls)) +
          " no balls: " + str(len(db_noballs)))
    return x, y, mean, p


if __name__ == "__main__":
    load_images("../data/TK-03/blender/", res={"x": 16, "y": 16})
