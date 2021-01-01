"""
    Converts the annotations in .txt format to image
    TODO format the output such that the generated masks can be uploaded to cvat
"""
from optparse import OptionParser
import sys
from pathlib import Path
import cv2
import re
import numpy as np


def create_segmentation_masks(image_folder, gt_folder):
    if not image_folder.exists() or not gt_folder.exists():
        print("data folders don't exits")
        sys.exit()

    numbers = re.compile(r'(\d+)')

    for file in image_folder.iterdir():
        image = cv2.imread(str(file))
        image_shape = image.shape
        segmentation_mask = np.ones((image_shape[0], image_shape[1], 3), np.uint8)

        # Debug stuff
        # cv2.imshow("image", image)
        # cv2.waitKey(0)

        # get the corresponding mask file
        image_number = int(numbers.split(file.name)[-2])
        mask_file = gt_folder / ("mask" + str(image_number) + ".txt")
        if mask_file.exists():
            file1 = open(str(mask_file), 'r')
            lines = file1.readlines()

            # count represents the y axis of the image
            for count, line in enumerate(lines):
                line = line.strip()
                line_array = line.split(' ')
                line_array = [int(x) for x in line_array]

                for idx, value in enumerate(line_array):
                    if value == 0:
                        # field + background
                        segmentation_mask[count, idx] = [0, 255, 0]
                    elif value is 1:
                        # ball
                        segmentation_mask[count, idx] = [0, 0, 0]
                    elif value is 3:
                        # robot
                        segmentation_mask[count, idx] = [255, 0, 0]
                    elif value is 4:
                        # robot
                        segmentation_mask[count, idx] = [255, 0, 0]
                    elif value is 5:
                        # robot
                        segmentation_mask[count, idx] = [255, 0, 0]
                    elif value is 6:
                        # robot
                        segmentation_mask[count, idx] = [255, 0, 0]
                    elif value is 7:
                        # robot
                        segmentation_mask[count, idx] = [255, 0, 0]
                    elif value is 9:
                        # robot
                        segmentation_mask[count, idx] = [255, 0, 0]
                    elif value is 10:
                        # robot
                        segmentation_mask[count, idx] = [255, 0, 0]
                    elif value is 11:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 250]
                    elif value is 12:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 250]
                    elif value is 13:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 14:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 16:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 17:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 18:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 19:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 20:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 21:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 22:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 23:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 27:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 28:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 29:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 30:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 31:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 32:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 33:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 34:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 35:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 36:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 38:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 39:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 40:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 41:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 42:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 43:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 45:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 46:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 47:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 48:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 49:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 50:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 52:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 53:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 54:
                        # lines
                        segmentation_mask[count, idx] = [127, 0, 255]
                    elif value is 55:
                        # lines
                        segmentation_mask[count, idx] = [0, 0, 255]
                    elif value is 57:
                        # goalpost
                        segmentation_mask[count, idx] = [255, 255, 255]
                    elif value is 58:
                        # goalpost
                        segmentation_mask[count, idx] = [255, 255, 255]
                    elif value is 59:
                        # goalpost
                        segmentation_mask[count, idx] = [255, 255, 255]
                    else:
                        print(value)
                        segmentation_mask[count, value] = 255

            # for debug
            # cv2.imshow("image", segmentation_mask)
            # cv2.waitKey(0)

            outputfile = gt_folder / file.name
            cv2.imwrite(str(outputfile), segmentation_mask)


if __name__ == "__main__":
    argv = sys.argv

    parser = OptionParser()
    parser.add_option("-i", "--imgData", action="store", type="string", dest="pathToImages",
                      default="data/Screenshots/")
    parser.add_option("-g", "--groundTruth", action="store", type="string", dest="pathToGroundTruth",
                      default="data/ScreenshotMasks/")
    (options, args) = parser.parse_args(sys.argv)

    IMAGE_SOURCE = options.pathToImages
    GT_SOURCE = options.pathToGroundTruth

    # create and save segmentation masks in ... format
    create_segmentation_masks(Path(IMAGE_SOURCE), Path(GT_SOURCE))
