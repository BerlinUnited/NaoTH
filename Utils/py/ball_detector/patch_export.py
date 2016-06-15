#!/usr/bin/python

import os, sys, getopt, math
from os.path import dirname
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as ptc
import matplotlib
import json
from PIL import Image

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
    if len(p) == 4*12*12:
      a = np.array(p[0::4]).astype(float)
      a = np.transpose(np.reshape(a, patch_size))
    else:
      a = np.array(p).astype(float)
      a = np.transpose(np.reshape(a, patch_size))

    yuv888 = np.zeros(patch_size[0]*patch_size[1]*3, dtype=np.uint8)
    yuv888[0::3] = np.reshape(a, patch_size[0]*patch_size[1])
    yuv888[1::3] = 128
    yuv888[2::3] = 128

    img = Image.fromstring('YCbCr', patch_size, yuv888.tostring())
    img = img.convert('RGB')
    file_path = os.path.join(export_path[labels[i]], str(i)+".png")
    img.save(file_path)

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

