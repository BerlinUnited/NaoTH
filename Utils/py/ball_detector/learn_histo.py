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
  
  testFeat = feat.histo(np.zeros((12,12),dtype=np.float32))
  n_feat = testFeat.shape[1]
  
  samples = np.zeros((X.shape[0], n_feat), dtype=np.float32)
  i = 0
  for s in X:
    img = s.reshape((12,12))
    samples[i,:] = feat.histo(img)
    i += 1
  
  responses = np.int32(labels)
    
  return n_feat, samples, responses
  
def learn(X, labels):
  
  n_feat, samples, responses = makeTrainData(X, labels)
      
  estimator = cv2.ml.SVM_create()
  estimator.setType(cv2.ml.SVM_C_SVC)
  estimator.setKernel(cv2.ml.SVM_RBF)
#  estimator.setTermCriteria((cv2.TERM_CRITERIA_COUNT + cv2.TERM_CRITERIA_EPS, 10000, 1.19209e-07))
  ##estimator.setC(XXX)
  ##estimator.setGamma(XXX)
  
  trainData = cv2.ml.TrainData_create(samples=samples, \
    layout=cv2.ml.ROW_SAMPLE , responses=responses)
  
  estimator.train(trainData)
  
  return estimator
  
def classify(X, labels, estimator):  
  n_feat, samples, responses = makeTrainData(X, labels)
  
  classified = np.zeros(samples.shape[0], dtype=np.int)
  
  # classify
  for i in range(0, samples.shape[0]):
    ret, result = estimator.predict(np.asmatrix(samples[i,:]))
    classified[i] = result[0,0];
    
  return classified

def show_errors_asfeat(X, goldstd_response, actual_response):  
  
  image = np.zeros(((patch_size[1]+1)*show_size[1], (patch_size[0]+1)*show_size[0]))
  
  errorIdx = list()
  # count
  for i in range(0,X.shape[0]):
    if actual_response[i] != goldstd_response[i]:
        errorIdx.append(i)	
      
  # show errors
  maxShownErrors = min(len(errorIdx), show_size[0]*show_size[1])
  shownErrorIdx = errorIdx[0:maxShownErrors]
  
  j = 0
  for i in shownErrorIdx:
    a = np.transpose(np.reshape(X[i,:], (12,12)))
    y = j // show_size[0]
    x = j % show_size[0]
    
    # apply feature
    f = feat.histo(a)
    
    image[y*13:y*13+12,x*13:x*13+12] = np.reshape(f, (12,12))
    j += 1
    
  plt.imshow(image, cmap=plt.cm.gray, interpolation='nearest')

  plt.xticks(())
  plt.yticks(())
  plt.show()
  
if __name__ == "__main__":
  
  
  splitRatio = 0.75
  
  X_all, labels_all = load_multi_data(sys.argv[1:], -1)
  if X_all.shape[0] == 0:
    print "Nothing to learn"
    exit(0)

  X_train, labels_train, X_eval, labels_eval = shuffle_and_split(X_all, labels_all, splitRatio)
  
  # workaround for https://github.com/Itseez/opencv/issues/5054
  # add another (3rd) class
  dummyPatch = np.ones(X_train.shape[1])*255
  X_train = np.vstack([X_train, dummyPatch])
  labels_train = np.vstack([labels_train, [2]])
    
  print("Train size", X_train.shape[0])
  print("Eval size", X_eval.shape[0])
  
  numberOfBallTrain = len(labels_train[np.where(labels_train == 1)])
  numberOfBallEval = len(labels_eval[np.where(labels_eval == 1)])
  print("numballs-train", numberOfBallTrain)
  print("numballs-eval", numberOfBallEval)
      
  print("Learning...")
  estimator = learn(X_train, labels_train)
  print("Saving...")
  estimator.save("model_histo.dat")
  print("Evaluating...")
  classfied = classify(X_eval, labels_eval, estimator)
  
  show_evaluation(X_eval, labels_eval, classfied)
#  show_errors_asfeat(X_eval, labels_eval, classfied)

    
