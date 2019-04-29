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
import csv
import json

def adjust_gamma(image, gamma=1.0):
    invGamma = 1.0 / gamma
    table = np.array([((i / 255.0) ** invGamma) * 255
                      for i in np.arange(0, 256)]).astype("uint8")
    return cv2.LUT(image, table)

def load_image_from_path(path, db, res):
    print("Loading images from " + path + "...")

    num_balls=0
    num_noballs=0

    # parse csv file
    with open(path, newline='') as csvfile:
        reader = csv.DictReader(csvfile)
        next(reader)
        for row in reader:
            f = os.path.join(os.path.dirname(path), row["filename"])
            p  = row["filename"]

            # load image
            try:
#                print(f)

                img = cv2.imread(f, cv2.IMREAD_GRAYSCALE)
                img = cv2.resize(img, (res["x"], res["y"]))
            except Exception as ex:
                print("Error loading image ", f)
                continue
#                raise ex

            is_ball = False
            debug_img = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)

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
                        cv2.circle(debug_img, (int(x),int(y)), int(radius), color=(0,0,255))

                        # normalize to resolution
                        x = (x / res["x"])
                        y = (y / res["y"])
                        radius = radius / max(res["x"], res["y"])

                        num_balls += 1
                        is_ball = True

                        #cv2.namedWindow('image',cv2.WINDOW_NORMAL)
                        #cv2.resizeWindow('image', 600,600)
                        #cv2.imshow("image", debug_img)
                        #cv2.waitKey()
                         
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
                num_noballs += 1

            # for each row add the image and the prediction 
            y = np.array([radius, x, y])

            db.append((img.astype(float) / 255.0, y, p))

            # augment: gamma
            for g in (0.4, 1.3):
                db.append((adjust_gamma(img, g).astype(float) / 255.0, y, p))
                if is_ball:
                    num_balls += 1
                else:
                    num_noballs += 1


    return num_balls, num_noballs

def loadImages(all_paths, res):
    db = []

    total_balls=0
    total_noballs=0

    for path in all_paths:
        balls, noballs = load_image_from_path(path, db, res)
        
        total_balls += balls
        total_noballs += noballs
   
    
    random.shuffle(db)
    x, y, p = list(map(np.array, list(zip(*db))))
    mean = np.mean(x)
    x -= mean

    x = x.reshape(*x.shape, 1)

    print("Loading finished")
    print("images: " + str(len(x)) + " balls: " + str(total_balls) + " no balls: " + str(total_noballs))
    return x, y, mean, p

if __name__ == "__main__":
    loadImages("/home/thomas/Roboter/logs/2018-04-03-Testgame-Hulks/090917-0109-Nao0225/bottom/via_region_data.csv", res={"x":16, "y":16})