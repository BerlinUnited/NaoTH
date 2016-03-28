#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as ptc

from sklearn.linear_model import Perceptron
from sklearn.linear_model import SGDClassifier
from sklearn import svm
import sklearn.neural_network as nn

from naoth.util import *
  
def learn(X, labels):
  #estimator = Perceptron(n_iter=1000, shuffle=True)
  #estimator = SGDClassifier()
  estimator = svm.SVC(kernel='linear', C=0.001, class_weight='auto')
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
  
  return estimator

def classify(X, labels, estimator):  
  
  classified = np.zeros(X.shape[0], dtype=np.int)
  
  # classify
  for i in range(0, X.shape[0]):
    if estimator.decision_function(X[i,:].reshape(1, -1)) > 0:
      classified[i] = 1;
      
  return classified
    
if __name__ == "__main__":
  
  # load patches from the file
  X_train, labels_train = load_data('patches-approach-ball')
  X_eval, labels_eval = load_data('patches-ball-sidecick')
  
  print("Learning...")
  estimator = learn(X_train, labels_train)
  print("Evaluating...")
  classfied = classify(X_eval, labels_eval, estimator)
  show_evaluation(X_eval, labels_eval, classfied)
  

    
