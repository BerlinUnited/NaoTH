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


def calculate_iou(y_true, y_pred):
    """
    Input:
    Keras provides the input as numpy arrays with shape (batch_size, num_columns).

    Arguments:
    y_true -- first box, numpy array with format [x, y, width, height, conf_score]
    y_pred -- second box, numpy array with format [x, y, width, height, conf_score]
    x any y are the coordinates of the top left corner of each box.

    Output: IoU of type float32. (This is a ratio. Max is 1. Min is 0.)

    """

    results = []

    for i in range(0, y_true.shape[0]):
        # set the types so we are sure what type we are using
        y_true = y_true.astype(np.float32)
        y_pred = y_pred.astype(np.float32)

        # boxTrue
        x_boxTrue_tleft = y_true[0, 0]  # numpy index selection
        y_boxTrue_tleft = y_true[0, 1]
        boxTrue_width = y_true[0, 2]
        boxTrue_height = y_true[0, 2]
        area_boxTrue = (boxTrue_width * boxTrue_height)

        # boxPred
        x_boxPred_tleft = y_pred[0, 0]
        y_boxPred_tleft = y_pred[0, 1]
        boxPred_width = y_pred[0, 2]
        boxPred_height = y_pred[0, 2]
        area_boxPred = (boxPred_width * boxPred_height)

        # calculate the bottom right coordinates for boxTrue and boxPred

        # boxTrue
        x_boxTrue_br = x_boxTrue_tleft + boxTrue_width
        y_boxTrue_br = y_boxTrue_tleft + boxTrue_height  # Version 2 revision

        # boxPred
        x_boxPred_br = x_boxPred_tleft + boxPred_width
        y_boxPred_br = y_boxPred_tleft + boxPred_height  # Version 2 revision

        # calculate the top left and bottom right coordinates for the intersection box, boxInt

        # boxInt - top left coords
        x_boxInt_tleft = np.max([x_boxTrue_tleft, x_boxPred_tleft])
        y_boxInt_tleft = np.max([y_boxTrue_tleft, y_boxPred_tleft])  # Version 2 revision

        # boxInt - bottom right coords
        x_boxInt_br = np.min([x_boxTrue_br, x_boxPred_br])
        y_boxInt_br = np.min([y_boxTrue_br, y_boxPred_br])

        # Calculate the area of boxInt, i.e. the area of the intersection
        # between boxTrue and boxPred.
        # The np.max() function forces the intersection area to 0 if the boxes don't overlap.

        # Version 2 revision
        area_of_intersection = \
            np.max([0, (x_boxInt_br - x_boxInt_tleft)]) * np.max([0, (y_boxInt_br - y_boxInt_tleft)])

        iou = area_of_intersection / ((area_boxTrue + area_boxPred) - area_of_intersection)

        # This must match the type used in py_func
        iou = iou.astype(np.float32)

        # append the result to a list at the end of each loop
        results.append(iou)

    # return the mean IoU score for the batch
    return np.mean(results)


def IoU(y_true, y_pred):
    # Note: the type float32 is very important. It must be the same type as the output from
    # the python function above or you too may spend many late night hours
    # trying to debug and almost give up.

    iou = tf.compat.v1.py_func(calculate_iou, [y_true, y_pred], tf.float32)

    return iou