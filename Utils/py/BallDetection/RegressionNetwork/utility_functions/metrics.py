import tensorflow.keras.backend as K
import tensorflow as tf
from tensorflow.keras.losses import Loss
import numpy as np


class ClassificationMetric(tf.keras.metrics.Metric):
    def __init__(self, name='my metric', **kwargs):
        super(ClassificationMetric, self).__init__(name=name, **kwargs)
        self.result_val = self.add_weight(name='tp', initializer='zeros')

    def update_state(self, y_true, y_pred, sample_weight=None):
        """
        Our detection network should predict a vector with (x, y, r, confidence of ball)
        This metric calculates the mean of absolute difference of the ball confidence over all images in a batch
        This is the mean "classification error". The metric returns 1 - mean classification error
        """
        result = 1 - tf.reduce_mean(tf.abs(y_true[:, -1] - y_pred[:, -1]))
        self.result_val.assign(result)

    def result(self):
        return self.result_val


class CircleIOU(tf.keras.metrics.Metric):
    def __init__(self, name='my metric', **kwargs):
        super(CircleIOU, self).__init__(name=name, **kwargs)
        self.result_val = self.add_weight(name='tp', initializer='zeros')

    def update_state(self, y_true, y_pred, sample_weight=None):
        """
        Our detection network should predict a vector with (x, y, r, confidence of ball)
        This metric calculates the mean of absolute difference of the ball confidence over all images in a batch
        This is the mean "classification error". The metric returns 1 - mean classification error
        """
        result = 1 - tf.reduce_mean(tf.abs(y_true[:, -1] - y_pred[:, -1]))
        self.result_val.assign(result)

    def result(self):
        return self.result_val
