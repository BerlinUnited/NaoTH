#!/usr/bin/python
import os
import sys
import getopt
import numpy as np
import json
import cv2
import patchReader as patchReader

patch_size = (24, 24)  # width, height


def parse_arguments(argv):
    input_file = ''
    export_all = False
    try:
        opts, args = getopt.getopt(argv, "hi:", ["ifile=", "all"])
    except getopt.GetoptError:
        print('python patch_export.py -i <logfile> [--all]')
        sys.exit(2)
    if not opts:
        print('python patch_export.py -i <logfile> [--all]')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print('python patch_export.py -i <logfile> [--all]')
            sys.exit()
        elif opt == "--all":
            export_all = True
        elif opt in ("-i", "--ifile"):
            input_file = arg

    return input_file, export_all


def load_labels(file):
    # init with invalid label
    tmp_labels = np.negative(np.ones((len(patchdata),), dtype=np.int))

    if os.path.isfile(file):
        with open(file, 'r') as data_file:
            ball_labels = json.load(data_file)
        tmp_labels[ball_labels["ball"]] = 1
        if "noball" in ball_labels:
            tmp_labels[ball_labels["noball"]] = 0
        else:
            # set all values to 0 since we have to assume everything unmarked is no ball
            tmp_labels = np.zeros((len(patchdata),))

        tmp_labels[ball_labels["ball"]] = 1

        return tmp_labels, ["noball", "ball"]
    else:
        print(
            'Label file does not exist. To export the patches regardless run this file with the --all option')
        sys.exit(-1)


def unpack_y_channel(p):
    # pixel after 2019 have 4 bytes: [padding, u, y, v] + color class 1 byte
    if len(p) == (3 + 1 + 1) * patch_size[0] * patch_size[1]:
        a = np.array(p[2::5]).astype(float)  ## read the 2nd channel to get the y value
        a = np.transpose(np.reshape(a, patch_size))
    # pixel 3 bytes + color class 1 byte
    elif len(p) == (3 + 1) * patch_size[0] * patch_size[1]:
        a = np.array(p[0::4]).astype(float)
        a = np.transpose(np.reshape(a, patch_size))

        # b = np.array(p[3::4]).astype(float)
        # b = np.transpose(np.reshape(b, patch_size))
    else:
        a = np.array(p).astype(float)
        a = np.transpose(np.reshape(a, patch_size))

    return a


def export_patches(patches, camera, labels, label_names, target_path):
    # create the output directories for all labels
    export_path_top = []
    export_path_bottom = []
    for label in label_names:

        # create output path for top images
        path = os.path.join(target_path, 'top', label)
        export_path_top.append(os.path.join(path))

        if not os.path.exists(path):
            os.makedirs(path)

        # create output path for bottom images
        path = os.path.join(target_path, 'bottom', label)
        export_path_bottom.append(path)
        if not os.path.exists(path):
            os.makedirs(path)

    # export the patches
    for i in range(len(patches)):
        a = unpack_y_channel(patches[i])

        if camera[i][0] == 0:
            file_path = os.path.join(export_path_bottom[labels[i]], str(i) + ".png")
        else:
            file_path = os.path.join(export_path_top[labels[i]], str(i) + ".png")

        # rgba
        '''
        rgba = np.zeros((patch_size[0],patch_size[1],4), dtype=np.uint8)
        rgba[:,:,0] = a
        rgba[:,:,1] = a
        rgba[:,:,2] = a
        rgba[:,:,3] = np.not_equal(b, 7)*255
        cv2.imwrite(file_path, rgba)
        '''

        # grayscale
        '''
        yuv888 = np.zeros(patch_size[0]*patch_size[1], dtype=np.uint8)
        yuv888 = np.reshape(a, patch_size[0]*patch_size[1])
        gray_image = cv2.cvtColor(yuv888, cv2.COLOR_BGR2GRAY)
        '''

        # only the center part
        a = a[4:20, 4:20]
        # b = b[4:20, 4:20]

        # remove green:
        # gray + set green to 0 (used for balls)
        # a = np.multiply(np.not_equal(b, 7), a)

        cv2.imwrite(file_path, a)


def export_patches_all(patches, camera, target_path):
    # create an export directory
    path = os.path.join(target_path, 'top')
    if not os.path.exists(path):
        os.makedirs(path)

    path = os.path.join(target_path, 'bottom')
    if not os.path.exists(path):
        os.makedirs(path)

    # export the patches
    for i in range(len(patches)):
        a = unpack_y_channel(patches[i])

        if camera[i][0] == 0:
            file_path = os.path.join(target_path, 'bottom', str(i) + ".png")
        else:
            file_path = os.path.join(target_path, 'top', str(i) + ".png")

        # rgba
        '''
        rgba = np.zeros((patch_size[0],patch_size[1],4), dtype=np.uint8)
        rgba[:,:,0] = a
        rgba[:,:,1] = a
        rgba[:,:,2] = a
        rgba[:,:,3] = np.not_equal(b, 7)*255
        cv2.imwrite(file_path, rgba)
        '''

        # grayscale
        '''
        yuv888 = np.zeros(patch_size[0]*patch_size[1], dtype=np.uint8)
        yuv888 = np.reshape(a, patch_size[0]*patch_size[1])
        gray_image = cv2.cvtColor(yuv888, cv2.COLOR_BGR2GRAY)
        '''

        # only the center part
        a = a[4:20, 4:20]
        # b = b[4:20, 4:20]

        # remove green:
        # gray + set green to 0 (used for balls)
        # a = np.multiply(np.not_equal(b, 7), a)

        cv2.imwrite(file_path, a)


'''
USAGE:
    python patch_export.py -i <logfile> [--all]
'''

if __name__ == "__main__":
    logFilePath, flag = parse_arguments(sys.argv[1:])

    # load the label file
    current_folder_full = os.path.dirname(
        logFilePath)  # this path to current folder TODO rename variable

    # Check if file is located inside the log structure
    parent_folder = os.path.split(current_folder_full)[0]
    extracted_folder = os.path.join(os.path.dirname(parent_folder), 'extracted')

    if os.path.isdir(extracted_folder):
        export_folder = os.path.join(extracted_folder, os.path.split(current_folder_full)[-1])
    else:
        print("WARNING: Log is not in expected folder structure")
        export_folder = current_folder_full

    print("Patches will be exported to %s" % (export_folder))

    """ type: 0-'Y', 1-'YUV', 2-'YUVC' """
    patchdata, camera_index = patchReader.read_all_patches_from_log(logFilePath, type=2)

    if flag:
        export_patches_all(patchdata, camera_index, export_folder)

    else:
        label_file = os.path.join(os.path.dirname(logFilePath), 'ball_patch.json')
        labels, label_names = load_labels(label_file)

        export_patches(patchdata, camera_index, labels, label_names, export_folder)

    print("Finished exporting the patches from the logs")
