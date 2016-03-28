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
    responses = np.int32(labels)
    
  return n_feat, samples, responses
  
def learn(X, labels):
  
  n_feat, samples, responses = makeTrainData(X, labels, True)
   
  #estimator = cv2.ml.KNearest_create()
  #estimator = cv2.ml.SVM_create()
  #estimator = cv2.ml.DTrees_create()
  #estimator = cv2.ml.RTrees_create()
  estimator = cv2.ml.ANN_MLP_create()
  estimator.setLayerSizes(np.asarray([n_feat,10,5,2]))
  # must be set!!!
  estimator.setActivationFunction(cv2.ml.ANN_MLP_SIGMOID_SYM )
  #estimator.setTermCriteria((cv2.TERM_CRITERIA_COUNT + cv2.TERM_CRITERIA_EPS, 20000, 0.001))
  #estimator.setTrainMethod(cv2.ml.ANN_MLP_BACKPROP)
  #estimator.setBackpropWeightScale(0.001)
  #estimator.setBackpropMomentumScale(0.0)
  
  trainData = cv2.ml.TrainData_create(samples=samples, 
    layout=cv2.ml.ROW_SAMPLE , responses=responses)
  
  estimator.train(trainData)
  
  return estimator
  
def classify(X, labels, estimator):  
  n_feat, samples, responses = makeTrainData(X, labels, True)
  
  classified = np.zeros(samples.shape[0], dtype=np.int)
  
  # classify
  for i in range(0, samples.shape[0]):
    ret, result = estimator.predict(np.asmatrix(samples[i,:]))
    classified[i] = ret;
      
  return classified
  
if __name__ == "__main__":
  
  # load patches from the file
  X_train, labels_train = load_data('patches-approach-ball')
  X_eval, labels_eval = load_data('patches-ball-sidecick')
  
  print("Learning...")
  estimator = learn(X_train, labels_train)
  print("Saving...")
  estimator.save("ball_detector_model.dat")
  print("Evaluating...")
  classfied = classify(X_eval, labels_eval, estimator)
 
  show_evaluation(X_eval, labels_eval, classfied)
  

    
