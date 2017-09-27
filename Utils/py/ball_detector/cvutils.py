#!/usr/bin/python

import cv2
import numpy as np


def learn(samples, responses, estimator):
            
    train_data = cv2.ml.TrainData_create(samples=samples, layout=cv2.ml.ROW_SAMPLE, responses=responses)
    
    estimator.train(train_data)
    
    return estimator


def classify(samples, responses, estimator):        
    classified = np.zeros(samples.shape[0], dtype=np.int)
    
    # classify
    for i in range(0, samples.shape[0]):
        ret, result = estimator.predict(np.asmatrix(samples[i, :]))
        classified[i] = result[0, 0]
        
    return classified
