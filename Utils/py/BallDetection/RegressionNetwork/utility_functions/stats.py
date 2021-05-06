import tensorflow.keras as keras
from keras.models import load_model
import numpy as np
import matplotlib.pyplot as plt
import sys
from collections import Counter
import cv2
from os import makedirs
import re


def keras_infer(x, model_path):
    model = load_model(model_path)
    return model.predict(x)


def get_stats(pred, y, thresh=0.5):
    pred_ball = np.array(pred[:, 1] >= thresh)
    gt_ball = np.array(y[:, 1] == 1)
    gt_noball = np.array(y[:, 1] == 0)
    accuracy = list(np.equal(pred_ball, gt_ball)).count(True) / y.shape[0]

    gt_ball_idx = list(np.where(y[:, 1] == 1)[0])
    gt_noball_idx = list(np.where(y[:, 1] == 0)[0])
    TruePositive = list(pred_ball[gt_ball_idx]).count(True) / list(gt_ball).count(True)
    FalsePositive = list(pred_ball[gt_noball_idx]).count(True) / list(gt_noball).count(True)
    TrueNegative = list(pred_ball[gt_noball_idx]).count(False) / list(gt_noball).count(True)
    FalseNegative = list(pred_ball[gt_ball_idx]).count(False) / list(gt_ball).count(True)
    return accuracy, TruePositive, FalsePositive, TrueNegative, FalseNegative


def keras_eval_thresh(x, y, model_path):
    model = load_model(model_path)
    print("Predicting...")
    pred = model.predict(x)
    TruePositive = []
    FalsePositive = []
    FalseNegative = []
    TrueNegative = []
    threshs = [float(x) / 1000 for x in range(990, 1000)]
    print("Plotting...", end="")
    for thresh in threshs:
        print(".", end="")
        sys.stdout.flush()
        acc, tp, fp, tn, fn = get_stats(pred, y, thresh)
        TruePositive.append(tp)
        FalsePositive.append(fp)
        FalseNegative.append(fn)
        TrueNegative.append(tn)
    fig, ax = plt.subplots()
    ax.plot(threshs, FalsePositive)
    fig.savefig("test.png")


def histogram(iterable, low, high, bins):
    step = (high - low + 0.0) / bins
    dist = Counter((float(x) - low) // step for x in iterable)
    return np.arange(min(iterable), max(iterable), step), [dist[b] for b in range(bins)]


def keras_plot_thresh_dist(pred, y):
    gt_ball_idx = list(np.where(y[:, 1] == 1)[0])
    gt_noball_idx = list(np.where(y[:, 1] == 0)[0])
    fp = pred[gt_noball_idx][:, 1][pred[gt_noball_idx][:, 1] > pred[gt_noball_idx][:, 0]]
    tp = pred[gt_ball_idx][:, 1][pred[gt_ball_idx][:, 1] > pred[gt_ball_idx][:, 0]]
    x_fp, hist_fp = histogram(fp, min(fp), max(fp), 1000)
    x_tp, hist_tp = histogram(tp, min(tp), max(tp), 1000)
    fig, ax = plt.subplots()
    ax.plot(x_fp, hist_fp)
    fig.savefig("fp.png")
    fig, ax = plt.subplots()
    ax.plot(x_tp, hist_tp)
    fig.savefig("tp.png")


def save_false_positives(pred, x, y, p, mean):
    try:
        makedirs('false_positives')
    except:
        pass
    gt_noball_idx = list(np.where(y[:, 1] == 0)[0])
    idxs = [gt_noball_idx[i] for i in np.where(pred[gt_noball_idx][:, 1] > pred[gt_noball_idx][:, 0])[0]]
    for img, path, _pred in zip(x[idxs], p[idxs], pred[idxs, 1]):
        cv2.imwrite('false_positives/' + "(" + str(_pred) + ")" + re.sub('[^\w\-_\. ]', '_', path), (img + mean) * 255)
