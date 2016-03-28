#!/usr/bin/python

import patchReader	

import json
import os, sys, getopt, math
import numpy as np

def load_data(file):
  patches = patchReader.readAllPatchesFromLog('./'+file+'.log')
 
  X = np.array(patches)
  labels = np.zeros((X.shape[0],))
  
  label_file = './'+file+'.json'
  if os.path.isfile(label_file):
    with open(label_file, 'r') as data_file:
      ball_labels = json.load(data_file)
    labels[ball_labels["ball"]] = 1
  else:
    print "ERROR: no label file ", labels_file
    
  return X, labels

