import tensorflow as tf
from utility_functions.iou_helper import calculate_iou_tf


def iou_loss(y_true, y_pred):
    # TODO there are probably better ways than using mse as basis
    mse = tf.losses.mean_squared_error(y_true, y_pred)
    iou = calculate_iou_tf(y_true[..., 0], y_true[..., 1], y_true[..., 2],
                           y_pred[..., 0], y_pred[..., 1], y_pred[..., 2])
    return mse + (1 - iou)


def iou_metric(y_true, y_pred):
    return calculate_iou_tf(y_true[..., 0], y_true[..., 1], y_true[..., 2],
                            y_pred[..., 0], y_pred[..., 1], y_pred[..., 2])
