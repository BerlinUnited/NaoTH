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


import naoth.features as feat
import cv2

show_size = (50, 30) # width, height
patch_size = (12, 12) # width, height
def getMarker(x,y,c):
  pos = (x*(patch_size[0]+1)-1,y*(patch_size[1]+1)-1)
  return ptc.Rectangle( pos, width=patch_size[0]+1, height=patch_size[1]+1, alpha=0.3, color=c)

def unroll_responses(responses, class_n):
        sample_n = len(responses)
        new_responses = np.zeros(sample_n*class_n, np.int32)
        resp_idx = np.int32( responses + np.arange(sample_n)*class_n )
        new_responses[resp_idx] = 1
        return new_responses

def makeTrainData(X, labels, unroll=False):
  n_feat = 144;
  samples = np.zeros((0, n_feat), dtype=np.float32)
  for s in X:
    img = s.reshape((12,12))
    f = feat.custom(img)
    samples = np.vstack([samples, f])
  
  if unroll:
    responses = np.float32(unroll_responses(labels, 2).reshape(-1, 2))
  else:
    responses = labels
    
  return n_feat, samples, responses
  
def learn(X, labels):
  
  n_feat, samples, responses = makeTrainData(X, labels, True)
   
  #estimator = cv2.ml.KNearest_create()
  #estimator = cv2.ml.SVM_create(
  #estimator = cv2.ml.DTrees_create()
  #estimator = cv2.ml.RTrees_create()
  estimator = cv2.ml.ANN_MLP_create()
  estimator.setLayerSizes(np.asarray([n_feat,10,5,2]))
  # must be set!!!
  estimator.setActivationFunction(cv2.ml.ANN_MLP_SIGMOID_SYM )
  estimator.setTermCriteria((cv2.TERM_CRITERIA_COUNT + cv2.TERM_CRITERIA_EPS, 20000, 0.0001))
  estimator.setTrainMethod(cv2.ml.ANN_MLP_BACKPROP)
  #estimator.setBackpropWeightScale(0.001)
  #estimator.setBackpropMomentumScale(0.0)
  
  trainData = cv2.ml.TrainData_create(samples=samples, 
    layout=cv2.ml.ROW_SAMPLE , responses=responses)
  
  estimator.train(trainData)
  
  return estimator
  
def evaluate(X, labels, estimator):  
  image = np.zeros(((patch_size[1]+1)*show_size[1], (patch_size[0]+1)*show_size[0]))
  
  
  n_feat, samples, responses = makeTrainData(X, labels, True)
  
  # classify
  j = 0
  marker = []
  for i in range(4400,4400+show_size[0]*show_size[1]):
    a = np.transpose(np.reshape(X[i,:], (12,12)))
    y = j // show_size[0]
    x = j % show_size[0]
    image[y*13:y*13+12,x*13:x*13+12] = a
    
    ret, result = estimator.predict(np.asmatrix(samples[i,:]))

    if ret > 0:
      #print(ret, result)
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
  
  print("Learning...")
  estimator = learn(X, labels)
  print("Saving...")
  estimator.save("ball_detector_model.dat")
  print("Evaluating...")
  evaluate(X, labels, estimator)
  

    
