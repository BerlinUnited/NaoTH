#!/usr/bin/python

import cv2
import numpy as np

from sklearn.linear_model import Perceptron
from sklearn.linear_model import SGDClassifier
from sklearn import svm
import sklearn.neural_network as nn

import naoth.features as feat
from naoth.util import *

def unroll_responses(responses, class_n):
        sample_n = len(responses)
        new_responses = np.zeros(sample_n*class_n, np.int32)
        resp_idx = np.int32( responses + np.arange(sample_n)*class_n )
        new_responses[resp_idx] = 1
        return new_responses

def makeTrainData(X, labels):
  
  testFeat = feat.histoNonGreen(np.zeros((12,12), dtype=np.float32), np.zeros((12,12), dtype=np.uint8))
  n_feat = testFeat.shape[1]
  
  samples = np.zeros((0, n_feat), dtype=np.float32)
  for s in X:
    img = img_from_patch(s)
    colors = colors_from_patch(s)
    
    f = feat.histoNonGreen(img, colors)
    samples = np.vstack([samples, f])
  
  responses = np.int32(labels)
    
  return n_feat, samples, responses
  
  
if __name__ == "__main__":
  
  # load patches from the file
  f = "data_test/eu16-ball-04"
  X, labels = load_data(f, type=2)
  
  n_feat, samples, responses = makeTrainData(X, labels)

  balls = [x for x in range(len(responses)) if responses[x] == 1]
  nonballs = [x for x in range(len(responses)) if responses[x] == 0]

  np.random.shuffle(balls)
  np.random.shuffle(nonballs)

  i = 3
  _, p = plt.subplots(4,i)
  j =0
  for x in balls[0:i]:
    
    img = img_from_patch(X[x,:])
    out = feat.histoSobel(img)

    p[0,j].plot(feat.histoSobel(img,size=16).T)
    p[1,j].imshow(img, cmap=plt.cm.gray)
    j += 1
  j=0
  
  for x in nonballs[0:i]:
    img = img_from_patch(X[x,:])
    out = feat.histoSobel(img)

    p[2,j].plot(feat.histoSobel(img,size=16).T)
    p[3,j].imshow(img, cmap=plt.cm.gray)
    j += 1

  plt.show()
