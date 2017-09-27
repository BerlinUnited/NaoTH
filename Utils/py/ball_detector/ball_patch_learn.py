#!/usr/bin/python

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as ptc

from sklearn.linear_model import Perceptron
from sklearn.linear_model import SGDClassifier
from sklearn import svm
import sklearn.neural_network as nn

from util import *

mybins = np.linspace(0, 1, 33)


def learn(x, labels):
    # estimator = Perceptron(n_iter=1000, shuffle=True)
    # estimator = SGDClassifier()
    # estimator = svm.SVC(kernel='rbf')
    estimator = svm.SVC(C=1.0, cache_size=200, class_weight=None, coef0=0.0,
                        decision_function_shape=None, degree=3, gamma='auto', kernel='rbf',
                        max_iter=-1, probability=False, random_state=None, shrinking=True,
                        tol=0.001, verbose=False)
    # estimator = nn.MLPClassifier()
    
    # samples = range(3000,10000)
    # print len(labels)
    # estimator.fit(X[samples,:], labels[samples])
    estimator.fit(x, labels)
    # print estimator.decision_function(X[15,:])
    
    # c = np.reshape(estimator.coef_, (12, 12))
    # print estimator.coef_
    # print estimator.intercept_
    # plt.imshow(c, cmap=plt.cm.gray, interpolation='nearest')
    # plt.show()
    
    return estimator


def classify(x, labels, estimator):
    
    classified = np.zeros(x.shape[0], dtype=np.int)
    
    # classify
    for i in range(0, x.shape[0]):
        if estimator.decision_function(x[i, :].reshape(1, -1)) > 0:
            classified[i] = 1
            
    return classified
        
        
def histnp(img):
    global mybins
    
    img = img.astype("float32")
    max_val = np.max(img)
    if max_val > 0:
        img = np.divide(img, max_val)
        
    hist = np.histogram(img, mybins)[0]
    
    hist = hist.astype("float32")
    max_val = np.max(hist)
    if max_val > 0:
        hist = np.divide(hist, max_val)
    
    return hist


def calculate_hist_features(X):
    
    test_feat = histnp(np.zeros((12*12,), dtype=np.float32))
    n_feat = test_feat.shape[0]
    print n_feat
    
    samples = np.zeros((X.shape[0], n_feat), dtype=np.float32)
    i = 0
    for s in X:
        samples[i, :] = histnp(s)
        i += 1
    
    return samples
        
        
if __name__ == "__main__":

    # Todo give those files as a parameter
    # This needs the logfile already labeled with ball_patch_label.py
    # load patches from the file
    X_train, labels_train = load_data('eu16-patches-striker-ball')
    X_eval, labels_eval = load_data('eu16-patches-striker-ball')
    
    print("Calculate features...")
    X_features_train = calculate_hist_features(X_train)
    print(X_features_train.shape)
    X_features_eval = calculate_hist_features(X_eval)
    print("Learning...")
    estimator = learn(X_features_train, labels_train)
    print("Evaluating...")
    classfied = classify(X_features_eval, labels_eval, estimator)
    show_evaluation(X_eval, labels_eval, classfied)
