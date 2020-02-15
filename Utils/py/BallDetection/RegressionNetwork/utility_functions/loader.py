import csv
import json
import os
import random
from collections import defaultdict
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


def load_blender_images(path, res):
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
    print("Loading images from " + path + " ...")
    
    # parse csv file
    with open(path, newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        for row in reader:
            f = os.path.join(os.path.dirname(path), row["filename"])
            p = row["filename"]

            # load image
            try:
                img = cv2.imread(f, cv2.IMREAD_GRAYSCALE)
                img = cv2.resize(img, (res["x"], res["y"]))
                img_normalized = img.astype(float) / 255.0
            except Exception as ex:
                print("Error loading image ", f)
                continue

            is_ball = False
            # load ball information
            region_count = int(row["region_count"])
            if region_count > 0:
                atts = json.loads(row["region_attributes"])
                if atts["type"] == "smudged_ball":
                    # ignore this image
                    continue
                elif atts["type"] == "ball":
                    shape = json.loads(row["region_shape_attributes"])
                    if shape["name"] == "circle":
                        x_coord = int(shape["cx"])
                        y_coord = int(shape["cy"])
                        radius = int(shape["r"])

                        if x_coord < 0 or y_coord < 0 or x_coord > res["x"] or y_coord > res["y"]:
                            print("Ignoring annotation because center is outside image")
                            # TODO: i dont think this makes sense
                            continue

                        # draw detected circle into debug image
                        # cv2.circle(debug_img, (int(x),int(y)), int(radius), color=(0,0,255))

                        # normalize to resolution
                        x_coord = (x_coord / res["x"])
                        y_coord = (y_coord / res["y"])
                        radius = radius / max(res["x"], res["y"])

                        is_ball = True
                    else:
                        # we only support circles
                        print("WARNING: Annotation is not a circle")
                        continue
                else:
                    # unknown type
                    print("Unknown type \"" + atts["type"] + "\" in file " + f)
                    continue
            else:
                # no region means no ball
                radius = 0.0
                x_coord = 0
                y_coord = 0

            # for each row add the image and the prediction
            if is_ball:
                target = np.array([radius, x_coord, y_coord, 1.0])
                db_balls.append((img_normalized, target, p))
            else:
                target = np.array([radius, x_coord, y_coord, 0.0])
                db_noballs.append((img_normalized, target, p))

            # augment: binarized image
            bin_img = cv2.adaptiveThreshold(img, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,
                                            cv2.THRESH_BINARY, 11, 2).reshape((16, 16))

            if is_ball:
                db_balls.append((bin_img.astype(float) / 255.0, target, p))
            else:
                db_noballs.append((bin_img.astype(float) / 255.0, target, p))

            # augment: gamma adjusted image
            avg_img_f = np.average(img_normalized)
            if 0.2 <= avg_img_f <= 0.8:
                # augment: gamma
                for g in (0.4, 1.3):
                    if is_ball:
                        db_balls.append((adjust_gamma(img, g).astype(float) / 255.0, target, p))
                    else:
                        db_noballs.append((adjust_gamma(img, g).astype(float) / 255.0, target, p))


def load_images_from_csv_files(root_path, res, limit_noballs):
    print("Looking for csv files in: ", root_path)
    db_ball_list = []
    db_noball_list = []

    # find csv files
    all_paths = list(Path(root_path).absolute().glob('**/*.csv'))
    print(all_paths)

    for path in all_paths:
        load_image_from_csv(str(path), db_ball_list, db_noball_list, res)

    if limit_noballs is True and len(db_ball_list) < len(db_noball_list):
        print("Limit negative images to ", len(db_ball_list))
        no_ball_mask = np.random.choice(len(db_noball_list), len(db_ball_list))
        db_noball_list = [db_noball_list[i] for i in no_ball_mask]

    db = db_ball_list + db_noball_list
    random.shuffle(db)

    input_images, targets, file_paths = list(map(np.array, list(zip(*db))))
    mean = np.mean(input_images)
    input_images -= mean

    # expand dimensions of the input images for use with tensorflow
    input_images = input_images.reshape(*input_images.shape, 1)

    print("Loading finished")
    print("\nStatistic:")
    print("number of images: " + str(len(input_images)) + " balls images: " + str(len(db_ball_list)) +
          " no ball images: " + str(len(db_noball_list)))

    return input_images, targets, mean, file_paths


if __name__ == "__main__":
    load_blender_images("../data/TK-03/blender/", res={"x": 16, "y": 16})
