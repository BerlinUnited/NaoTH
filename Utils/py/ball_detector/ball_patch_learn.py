#!/usr/bin/python

import json
import os, sys, getopt, math
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as ptc

from sklearn.linear_model import Perceptron
from sklearn.linear_model import SGDClassifier
from sklearn import svm
import sklearn.neural_network as nn

import patchReader

show_size = (50, 30) # width, height
patch_size = (12, 12) # width, height
def getMarker(x,y,c):
  pos = (x*(patch_size[0]+1)-1,y*(patch_size[1]+1)-1)
  return ptc.Rectangle( pos, width=patch_size[0]+1, height=patch_size[1]+1, alpha=0.3, color=c)

  
def learn(X, labels):
  #estimator = Perceptron(n_iter=1000, shuffle=True)
  #estimator = SGDClassifier()
  estimator = svm.SVC(kernel='linear', C=0.001, class_weight='balanced')
  #estimator = nn.MLPClassifier()
  
  samples = range(3000,10000)
  print len(labels)
  estimator.fit(X[samples,:], labels[samples])
  #print estimator.decision_function(X[15,:])
  
  c = np.reshape(estimator.coef_, (12, 12))
  print estimator.coef_
  print estimator.intercept_
  plt.imshow(c, cmap=plt.cm.gray, interpolation='nearest')
  plt.show()
  
  
  image = np.zeros(((patch_size[1]+1)*show_size[1], (patch_size[0]+1)*show_size[0]))
  
  # classify
  j = 0
  marker = []
  for i in range(4400,4400+show_size[0]*show_size[1]):
    a = np.transpose(np.reshape(X[i,:], (12,12)))
    y = j // show_size[0]
    x = j % show_size[0]
    image[y*13:y*13+12,x*13:x*13+12] = a
    
    if estimator.decision_function(X[i,:].reshape(1, -1)) > 0:
      if labels[i]:
        marker.append(getMarker(x,y,'green'))
      else:
        marker.append(getMarker(x,y,'red'))
    else:
      if labels[i]:
        marker.append(getMarker(x,y,'yellow'))
      
    j += 1
    
  plt.imshow(image, cmap=plt.cm.gray, interpolation='nearest')
  for m in marker:
    plt.gca().add_patch(m)
  plt.xticks(())
  plt.yticks(())
  plt.show()
  
  
def load_patches_from_csv(file):
  f = open(file, 'r')
  patches = []
  
  for line in f:
    s = line.split(';')
    a = np.array(s).astype(float)
    patches.append(a)

    
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
    
    
if __name__ == "__main__":
  
  # load patches from the file
  X, labels = load_data('patches-approach-ball')
  learn(X, labels)
  

    