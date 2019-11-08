from glob import glob
import imutils
from os.path import relpath
import os
import numpy as np
import math
import cv2
import csv
import json
import sys


def getPatchPaths(path):
    return glob(path + "/**/*_patch_bw/") + glob(path + "/*_patch_bw/")


def loadImages(path, res):
    db = []
    print("Loading images...")

    with open(os.path.join(path, 'blender.csv'), 'w', newline='') as csvfile:
        fieldnames = ["filename", "file_size", "file_attributes", "region_count",
                      "region_id", "region_shape_attributes", "region_attributes"]
        writer = csv.DictWriter(
            csvfile, fieldnames=fieldnames, dialect='excel')
        writer.writeheader()

        for patch_folder in getPatchPaths(path):
            patch_folder_rel = str(relpath(patch_folder, path))
            mask_path = path + "/" + \
                        patch_folder_rel[0:-len("_patch_bw")] + "_patch_mask"
            for f in glob(patch_folder + "/*.png"):

                p = relpath(f, path)
                img = cv2.imread(f, cv2.IMREAD_GRAYSCALE)
                img = cv2.resize(img, (res["x"], res["y"]))

                # load mask file
                f_mask = mask_path + "/" + os.path.basename(p)
                if os.path.exists(f_mask):
                    img_mask = cv2.imread(f_mask, cv2.IMREAD_GRAYSCALE)
                    img_mask = cv2.resize(img_mask, (res["x"], res["y"]))
                elif "noball" in mask_path:
                    img_mask = np.zeros((res["x"], res["y"])).astype(np.uint8)
                else:
                    raise "Missing mask file for " + f

                # fit circle on img_mask
                _, contours, _ = cv2.findContours(img_mask, 1, 2)

                debug_img = cv2.cvtColor(img, cv2.COLOR_GRAY2BGR)

                radius = 0
                x = res["x"] / 2
                y = res["y"] / 2
                region_count = 0
                # find largest circle (if existing)
                for cnt in contours:
                    (nx, ny), nradius = cv2.minEnclosingCircle(cnt)
                    if nradius >= radius:
                        x = nx
                        y = ny
                        radius = nradius + 1

                if radius > 0:
                    # draw detected circle into debug image
                    cv2.circle(debug_img, (int(x), int(y)),
                               int(radius), color=(0, 0, 255))

                    region = {"name": "circle", "cx": x, "cy": y, "r": radius}
                    atts = {"type": "ball"}
                    region_count = 1

                else:
                    radius = 0
                    x = 0.5
                    y = 0.5

                    region = {}
                    atts = {}

                writer.writerow(
                    {"filename": p, "file_attributes": "{}", "region_count": region_count,
                     "region_id": 0,
                     "region_shape_attributes": json.dumps(region),
                     "region_attributes": json.dumps(atts)})

                # cv2.namedWindow('image', cv2.WINDOW_NORMAL)
                # cv2.resizeWindow('image', 600, 600)
                # cv2.imshow("image", debug_img)
                # cv2.waitKey()


if __name__ == "__main__":
    if len(sys.argv) > 0:
        loadImages(sys.argv[1], res={"x": 16, "y": 16})
