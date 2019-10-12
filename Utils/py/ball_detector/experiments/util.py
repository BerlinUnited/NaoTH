#!/usr/bin/python

import patchReader
import json
import os
import sys
import getopt
import math
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as ptc
import features as feat

show_size = (50, 30)  # width, height
patch_size = (12, 12)  # width, height


def get_marker(x, y, c):
    pos = (x*(patch_size[0]+1)-1, y*(patch_size[1]+1)-1)
    return ptc.Rectangle(pos, width=patch_size[0]+1, height=patch_size[1]+1, alpha=0.3, color=c)


def shuffle_and_split(x, labels, split_ratio=0.75, ball_non_ball_ratio=-1.0):

    # combine both to the same matrix so shuffling does not disconnect the samples and labels
    labels_as_matrix = np.reshape(labels, (-1, 1))

    y = np.concatenate((x, labels_as_matrix), axis=1)
    # do shuffling
    np.random.shuffle(y)

    # split into two
    split_idx = y.shape[0] * split_ratio

    y1 = y[0:split_idx, :]
    y2 = y[split_idx+1:, :]

    x1, labels1 = np.hsplit(y1, [x.shape[1]])
    x2, labels2 = np.hsplit(y2, [x.shape[1]])

    if ball_non_ball_ratio >= 1.0:
        non_ball_idx_train = [idx for idx in range(len(labels1)) if labels1[idx] == 0]
        ball_idx_train = [idx for idx in range(len(labels1)) if labels1[idx] == 1]
        number_of_ball_train = len(ball_idx_train)
        number_of_non_ball_train = len(non_ball_idx_train)

        if (number_of_ball_train*ball_non_ball_ratio) < number_of_non_ball_train:
            # limit the number of non-balls
            allowed_num_non_ball = int(number_of_ball_train * ball_non_ball_ratio)
            limited_non_balls = non_ball_idx_train[:allowed_num_non_ball]
            valid_train_idx = np.hstack([limited_non_balls, ball_idx_train])
            x1 = x1[valid_train_idx, :]
            labels1 = labels1[valid_train_idx, :]

    return x1, labels1, x2, labels2


def calc_precision_recall(x, goldstd_response, actual_response):

    error_idx = list()
    # count
    tp = 0.0
    fp = 0.0
    fn = 0.0
    for i in range(0, x.shape[0]):
        if actual_response[i] == 1:
            if goldstd_response[i] == 1:
                tp += 1.0
            else:
                fp += 1.0
                error_idx.append(i)
        elif actual_response[i] == 0:
            if goldstd_response[i] == 1:
                fn += 1.0
                error_idx.append(i)

    precision = 0.0
    if tp+fp > 0:
        precision = tp/(tp+fp)

    recall = 0.0
    if tp+fn > 0:
        recall = tp/(tp+fn)

    return precision, recall, error_idx


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


def show_evaluation(x, goldstd_response, actual_response):

    image = np.zeros(((patch_size[1]+1)*show_size[1], (patch_size[0]+1)*show_size[0]))

    precision, recall, error_idx = calc_precision_recall(x, goldstd_response, actual_response)
    print("precision", precision)
    print("recall", recall)

    # show errors
    max_shown_errors = min(len(error_idx), show_size[0]*show_size[1])
    shown_error_idx = error_idx[0:max_shown_errors]

    j = 0
    marker = []
    for i in shown_error_idx:
        a = img_from_patch(x[i, :])
        y = j // show_size[0]
        x = j % show_size[0]
        image[y*13:y*13+12, x*13:x*13+12] = a

        if goldstd_response[i] != actual_response[i]:
            if goldstd_response[i] == 0:
                marker.append(get_marker(x, y, 'red'))
            else:
                marker.append(get_marker(x, y, 'yellow'))
        else:
            if goldstd_response[i] == 1:
                marker.append(get_marker(x, y, 'green'))
        j += 1

    plt.imshow(image, cmap=plt.get_cmap('gray'), interpolation='nearest')
    for m in marker:
        plt.gca().add_patch(m)
    plt.xticks(())
    plt.yticks(())
    plt.show()


def show_errors_as_feat(x, goldstd_response, actual_response):
  
    image = np.zeros(((patch_size[1]+1)*show_size[1], (patch_size[0]+1)*show_size[0]))
  
    error_idx = list()
    # count
    for i in range(0, x.shape[0]):
        if actual_response[i] != goldstd_response[i]:
            error_idx.append(i)
      
    # show errors
    max_shown_errors = min(len(error_idx), show_size[0]*show_size[1])
    shown_error_idx = error_idx[0:max_shown_errors]
  
    j = 0
    for i in shown_error_idx:
        a = image_from_patch(x[i, :])
        y = j // show_size[0]
        x = j % show_size[0]

        # apply feature
        f = feat.histo(a)

        image[y*13:y*13+12, x*13:x*13+12] = np.reshape(f, (12, 12))
        j += 1
    
    plt.imshow(image, cmap=plt.get_cmap('gray'), interpolation='nearest')

    plt.xticks(())
    plt.yticks(())
    plt.show()


def load_data_from_folder(rootdir, camera=-1, type=0):
    locations = list()
    for subdir, dirs, files in os.walk(rootdir):
        for file in files:
            if file.endswith(".json"):
                locations.append(os.path.join(subdir, file[:-len(".json")]))
    return load_multi_data(locations, camera, type)


def load_multi_data(args, camera=-1, type=0):
    x = None
    labels = None

    for a in args:
        x_local, labels_local = load_data(a.strip(), camera, type)
        if x is None:
            x = x_local
        else:
            x = np.vstack([x, x_local])

        if labels is None:
            labels = labels_local
        else:
            labels = np.append(labels, labels_local)
    return x, labels


def load_data(file, camera=-1, type=0):
    
    patches, camera_idx = patchReader.read_all_patches_from_log(file + '.log', type)
   
    camera_idx = np.asarray(camera_idx)
    camera_idx = np.reshape(camera_idx, (len(camera_idx), ))

    x = np.array(patches)
    labels = np.negative(np.ones((x.shape[0],)))

    label_file = file+'.json'
    if os.path.isfile(label_file):
        with open(label_file, 'r') as data_file:
            ball_labels = json.load(data_file)

        if "noball" in ball_labels:
            labels[ball_labels["noball"]] = 0
        else:
            # set all values to 0 since we have to assume everything unmarked is no ball
            labels = np.zeros((x.shape[0],))

        labels[ball_labels["ball"]] = 1

    else:
        print "ERROR: no label file ", label_file

    # filter out invalid labels
    valid_label_idx = labels[:] >= 0
    camera_bottom_idx = camera_idx[:] == 0
    camera_top_idx = camera_idx[:] == 1

    if camera == 0:
        valid_idx = np.logical_and(valid_label_idx, camera_bottom_idx)
    elif camera == 1:

        valid_idx = np.logical_and(valid_label_idx, camera_top_idx)
        # print("valid", valid_idx)
        # print("validLabel", valid_label_idx)
        # print("camera", camera_top_idx)
    else:
        valid_idx = valid_label_idx

    x = x[valid_idx]
    labels = labels[valid_idx]

    return x, labels
