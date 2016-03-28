#!/usr/bin/python

import patchReader	

import json
import os, sys, getopt, math
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as ptc


show_size = (50, 30) # width, height
patch_size = (12, 12) # width, height
def getMarker(x,y,c):
  pos = (x*(patch_size[0]+1)-1,y*(patch_size[1]+1)-1)
  return ptc.Rectangle( pos, width=patch_size[0]+1, height=patch_size[1]+1, alpha=0.3, color=c)


def show_evaluation(X, goldstd_response, actual_response):  
  
  image = np.zeros(((patch_size[1]+1)*show_size[1], (patch_size[0]+1)*show_size[0]))
  
  # classify
  j = 0
  marker = []
  for i in range(4400,4400+show_size[0]*show_size[1]):
	a = np.transpose(np.reshape(X[i,:], (12,12)))
	y = j // show_size[0]
	x = j % show_size[0]
	image[y*13:y*13+12,x*13:x*13+12] = a
    
	if(goldstd_response[i] != actual_response[i]):
		if goldstd_response[i] == 0:
			marker.append(getMarker(x,y,'red'))
		else:
			marker.append(getMarker(x,y,'yellow'))
	else:
		if goldstd_response[i] == 1:
			marker.append(getMarker(x,y,'green'))    
	j += 1
    
  plt.imshow(image, cmap=plt.cm.gray, interpolation='nearest')
  for m in marker:
	plt.gca().add_patch(m)
  plt.xticks(())
  plt.yticks(())
  plt.show()

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

