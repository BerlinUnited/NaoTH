#!/usr/bin/python
import os
import sys
import getopt
import numpy as np
import json
import naoth
import cv2
import patchReader as patchReader

patch_size = (24, 24)  # width, height


def parse_arguments(argv):
    input_file = ''
    all = False
    try:
        opts, args = getopt.getopt(argv, "hi:", ["ifile=", "all"])
    except getopt.GetoptError:
        print('python patch_export.py -i <logfile> [--all]')
        sys.exit(2)
    if opts == []:
        print('python patch_export.py -i <logfile> [--all]')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print('python patch_export.py -i <logfile> [--all]')
            sys.exit()
        elif opt == "--all":
            all = True
        elif opt in ("-i", "--ifile"):
            input_file = arg

    return input_file, all


def load_labels(file):
    # init with invalid label
    tmp_labels = np.negative(np.ones((len(patchdata),), dtype=np.int))
    
    if os.path.isfile(file):
        with open(file, 'r') as data_file:
            ball_labels = json.load(data_file)
        tmp_labels[ball_labels["ball"]] = 1
        if ball_labels.has_key("noball"):
            tmp_labels[ball_labels["noball"]] = 0
        else:
            # set all values to 0 since we have to assume everything unmarked is no ball
            tmp_labels = np.zeros((len(patchdata),))
            
        tmp_labels[ball_labels["ball"]] = 1
        
        return tmp_labels, ["noball", "ball"]
    else:
        print('Label file does not exist. To export the patches regardless run this file with the --all option')
        sys.exit(-1)


def exportPatches(patchdata, labels, label_names, target_path):

    # create the ourput directories for all labels
    export_path = []
    for label in label_names:
        path = os.path.join(target_path, label)
        export_path.append(path)
        if not os.path.exists(path):
            os.makedirs(path)

    # export the patches
    for i in range(len(patchdata)):
        p = patchdata[i]
        if len(p) == 4*patch_size[0]*patch_size[1]:
            a = np.array(p[0::4]).astype(float)
            a = np.transpose(np.reshape(a, patch_size))
            
            b = np.array(p[3::4]).astype(float)
            b = np.transpose(np.reshape(b, patch_size))
        else:
            a = np.array(p).astype(float)
            a = np.transpose(np.reshape(a, patch_size))

        file_path = os.path.join(export_path[labels[i]], str(i)+".png")

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
        b = b[4:20, 4:20]
        
        # remove green:
        # gray + set green to 0 (used for balls)
        # a = np.multiply(np.not_equal(b, 7), a)

        cv2.imwrite(file_path, a)


def exportPatchesAll(patchdata, target_path):

    # create the ourput directories for all labels
    export_path = os.path.join(target_path, 'patch')
    if not os.path.exists(export_path):
        os.makedirs(export_path)
    
    # export the patches
    for i in range(len(patchdata)):
        p = patchdata[i]
        if len(p) == 4*patch_size[0]*patch_size[1]:
            a = np.array(p[0::4]).astype(float)
            a = np.transpose(np.reshape(a, patch_size))
            
            b = np.array(p[3::4]).astype(float)
            b = np.transpose(np.reshape(b, patch_size))
        else:
            a = np.array(p).astype(float)
            a = np.transpose(np.reshape(a, patch_size))

        file_path = os.path.join(export_path, str(i)+".png")
        print(file_path)

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
        b = b[4:20, 4:20]
        
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

    # type: 0-'Y', 1-'YUV', 2-'YUVC'
    patchdata, _ = patchReader.read_all_patches_from_log(logFilePath, type=2)

    # load the label file
    base_file, file_extension = os.path.splitext(logFilePath)

    if flag:
        # create an export directory
        if not os.path.exists(base_file):
            os.makedirs(base_file)
        exportPatchesAll(patchdata, base_file)
    else:
        label_file = base_file + '.json'
        labels, label_names = load_labels(label_file)

        # create an export directory
        if not os.path.exists(base_file):
          os.makedirs(base_file)

        exportPatches(patchdata, labels, label_names, base_file)
