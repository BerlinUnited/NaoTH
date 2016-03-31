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

def shuffle_and_split(X, labels, ratio):

    # combine both to the same matrix so shuffling does not disconnect the samples and labels
    labelsAsMatrix = np.reshape(labels, (-1,1))

    Y = np.concatenate((X, labelsAsMatrix), axis=1)
    # do shuffling
    np.random.shuffle(Y)

    # split into two
    splitIdx = Y.shape[0] * ratio

    Y1 = Y[0:splitIdx,:]
    Y2 = Y[splitIdx+1:,:]

    X1, labels1 = np.hsplit(Y1, [X.shape[1]])
    X2, labels2 = np.hsplit(Y2, [X.shape[1]])

    return X1, labels1, X2, labels2

def calc_precision_recall(X, goldstd_response, actual_response):

    errorIdx = list()
    # count
    tp = 0.0
    fp = 0.0
    fn = 0.0
    for i in range(0,X.shape[0]):
        if actual_response[i] ==  1:
            if goldstd_response[i] == 1:
                tp += 1.0
            else:
                fp += 1.0
                errorIdx.append(i)
        elif actual_response[i] == 0:
            if goldstd_response[i] == 1:
                fn += 1.0
                errorIdx.append(i)

    precision = 0.0
    if tp+fp > 0:
        precision = tp/(tp+fp)

    recall = 0.0
    if tp+fn > 0:
        recall = tp/(tp+fn)

    return precision, recall, errorIdx

def img_from_patch(p):
    if len(p) == 4*12*12:
        a = np.array(p[0::4]).astype(float)
        a = np.transpose(np.reshape(a, patch_size))
    else:
        a = np.array(p).astype(float)
        a = np.transpose(np.reshape(a, patch_size))
    return a
    
def colors_from_patch(p):
    a = np.array(p[3::4]).astype(np.uint8)
    a = np.transpose(np.reshape(a, patch_size))
    return a

def show_evaluation(X, goldstd_response, actual_response):

    image = np.zeros(((patch_size[1]+1)*show_size[1], (patch_size[0]+1)*show_size[0]))

    precision, recall, errorIdx = calc_precision_recall(X, goldstd_response, actual_response)
    print("precision", precision)
    print("recall", recall)


    # show errors
    maxShownErrors = min(len(errorIdx), show_size[0]*show_size[1])
    shownErrorIdx = errorIdx[0:maxShownErrors]

    j = 0
    marker = []
    for i in shownErrorIdx:
        a = img_from_patch(X[i,:])
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

def load_multi_data(args, camera=-1, type=0):
    X = None
    labels = None

    for a in args:
        X_local, labels_local = load_data(a.strip(), camera, type)
        if X == None:
            X = X_local
        else:
            X = np.vstack([X, X_local])

        if labels == None:
            labels = labels_local
        else:
            labels = np.append(labels, labels_local)
    return X, labels

def load_data(file, camera=-1, type=0):
    
    patches, cameraIdx = patchReader.readAllPatchesFromLog(file+'.log', type)
   
    cameraIdx = np.asarray(cameraIdx)
    cameraIdx = np.reshape(cameraIdx, (len(cameraIdx), ))

    X = np.array(patches)
    labels = np.negative(np.ones((X.shape[0],)))

    label_file = file+'.json'
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
    validLabelIdx = labels[:] >= 0
    cameraBottomIdx = cameraIdx[:] == 0
    cameraTopIdx = cameraIdx[:] == 1

    validIdx = []
    if camera == 0:
        validIdx = np.logical_and(validLabelIdx, cameraBottomIdx)
    elif camera == 1:

        validIdx = np.logical_and(validLabelIdx, cameraTopIdx)
        #print("valid", validIdx)
        #print("validLabel", validLabelIdx)
        #print("camera", cameraTopIdx)
    else:
        validIdx = validLabelIdx

    X = X[validIdx]
    labels = labels[validIdx]

    return X, labels
