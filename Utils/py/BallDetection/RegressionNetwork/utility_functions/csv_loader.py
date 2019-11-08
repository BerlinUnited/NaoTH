import random
import os
import numpy as np
import cv2
import csv
import json
from pathlib import Path


def adjust_gamma(image, gamma=1.0):
    inv_gamma = 1.0 / gamma
    table = np.array([((i / 255.0) ** inv_gamma) * 255
                      for i in np.arange(0, 256)]).astype("uint8")
    return cv2.LUT(image, table)


def load_image_from_path(path, db_balls, db_noballs, res):
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


def load_images_from_csv(root_path, res, limit_noballs):
    print("Looking for csv files in: ", root_path)
    db_balls = []
    db_noballs = []

    # find csv files
    all_paths = list(Path(root_path).absolute().glob('**/*.csv'))
    print(all_paths)

    for path in all_paths:
        load_image_from_path(str(path), db_balls, db_noballs, res)

    if limit_noballs is True and len(db_balls) < len(db_noballs):
        db_noballs = np.random.choice(db_noballs, len(db_balls))

    db = db_balls + db_noballs
    random.shuffle(db)

    x, y, p = list(map(np.array, list(zip(*db))))
    mean = np.mean(x)
    x -= mean

    x = x.reshape(*x.shape, 1)

    print("Loading finished")
    print("images: " + str(len(x)) + " balls: " + str(len(db_balls)) + " no balls: " + str(
        len(db_noballs)))
    return x, y, mean, p
