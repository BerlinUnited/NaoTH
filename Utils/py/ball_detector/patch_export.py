#!/usr/bin/python

import os, sys, getopt, math
from os.path import dirname
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as ptc
import matplotlib
import json
from PIL import Image

import cv2

import naoth.patchReader as patchReader

patch_size = (12, 12) # width, height

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
    
    # gray (for backgrounds)
    cv2.imwrite(file_path,a)

    # gray + set green to 0 (used for balls)
    #cv2.imwrite(file_path, np.multiply(np.not_equal(b, 7), a))

'''

USAGE:
  python ball_patch_label.py ./data/ball-move-around-patches.log

'''
if __name__ == "__main__":

  if len(sys.argv) > 1:
    logFilePath = sys.argv[1]

  # type: 0-'Y', 1-'YUV', 2-'YUVC'
  patchdata, _ = patchReader.readAllPatchesFromLog(logFilePath, type = 2)

  # load the label file
  base_file, file_extension = os.path.splitext(logFilePath)
  label_file = base_file+'.json'
  labels, label_names = load_labels(label_file)

  # create an export directory
  if not os.path.exists(base_file):
    os.makedirs(base_file)

  exportPatches(patchdata, labels, label_names, base_file)

