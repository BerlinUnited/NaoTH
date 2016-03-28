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
  
  errorIdx = list()
  # count
  tp = 0.0
  fp = 0.0
  fn = 0.0
  for i in range(0,X.shape[0]):
    if actual_response[i] ==  1:
      if goldstd_response[i] == 1:
	tp = tp + 1.0
      else:
	fp = fp + 1.0
	errorIdx.append(i)
    else:
      if goldstd_response[i] == 1:
	fn = fn + 1.0
	errorIdx.append(i)
	
  print("precision", tp/(tp+fp))
  print("recall", tp/(tp+fn))
      
  # show errors
  maxShownErrors = min(len(errorIdx), show_size[0]*show_size[1])
  shownErrorIdx = errorIdx[0:maxShownErrors]
  
  j = 0
  marker = []
  for i in shownErrorIdx:
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
  labels = np.negative(np.ones((X.shape[0],)))
  
  label_file = './'+file+'.json'
  if os.path.isfile(label_file):
    with open(label_file, 'r') as data_file:
      ball_labels = json.load(data_file)
    
    if ball_labels.has_key("noball"):
      labels[ball_labels["noball"]] = 0
    else:
      # set all values to 0 since we have to assume everything unmarked is no ball
      labels = np.zeros((X.shape[0],))
      
    labels[ball_labels["ball"]] = 1
    
  else:
    print "ERROR: no label file ", label_file
  
  # filter out invalid labels
  validIDX = labels[:] >= 0
  X = X[validIDX]
  labels = labels[validIDX]
  return X, labels

