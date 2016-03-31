#!/usr/bin/python

import cv2
import numpy as np
import naoth.features as feat
from naoth.util import *
from naoth.cvutils import *


def makeTrainData(X, labels):
  
 # testFeat = feat.histoNonGreen(np.zeros((12,12), dtype=np.float32), np.zeros((12,12), dtype=np.uint8))
  testFeat = feat.histo(np.zeros((12,12), dtype=np.float32))
  n_feat = testFeat.shape[1]
  
  samples = np.zeros((0, n_feat), dtype=np.float32)
  for s in X:
    img = img_from_patch(s)
    colors = colors_from_patch(s)
    
    #f = feat.histoNonGreen(img, colors)
    f = feat.histo(img)
    samples = np.vstack([samples, f])
  
  responses = np.int32(labels)
    
  return n_feat, samples, responses
  

if __name__ == "__main__":
  
  ballNonBallRatio = 10.0
  splitRatio = 0.75
  cam = 1
  outfile = "model_histo_top.dat"
  
  estimator = cv2.ml.SVM_create()
  estimator.setType(cv2.ml.SVM_C_SVC)
  estimator.setKernel(cv2.ml.SVM_CHI2)
#  estimator.setTermCriteria((cv2.TERM_CRITERIA_COUNT + cv2.TERM_CRITERIA_EPS, 10000, 1.19209e-07))
  ##estimator.setC(XXX)
  ##estimator.setGamma(XXX)
  
  X_all, labels_all = load_data_from_folder(sys.argv[1], camera=cam, type=2)
  if X_all.shape[0] == 0:
    print "Nothing to learn"
    exit(0)

  X_train, labels_train, X_eval, labels_eval = shuffle_and_split(X_all, labels_all, splitRatio, ballNonBallRatio)
    
  print("numballs-train", len(labels_train[np.where(labels_train == 1)]))
  print("numballs-eval", len(labels_eval[np.where(labels_eval == 1)]))
  
  # workaround for https://github.com/Itseez/opencv/issues/5054
  # add another (3rd) class
  dummyPatch = np.ones(X_train.shape[1])*255
  X_train = np.vstack([X_train, dummyPatch])
  labels_train = np.vstack([labels_train, [2]])
  
  print("Train size", X_train.shape[0])
  print("Eval size", X_eval.shape[0])
      
  print("Learning...")
  n_feat, samples, responses = makeTrainData(X_train, labels_train)
  learn(samples, responses, estimator)
  print("Saving...")
  estimator.save(outfile)
  print("Evaluating...")
  n_feat, samples, responses = makeTrainData(X_eval, labels_eval)
  classfied = classify(samples, responses, estimator)
  
  show_evaluation(X_eval, labels_eval, classfied)

    
