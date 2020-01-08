"""
    Implementation of helper functions for iou based losses and metrics

    Three different implementations are provided.
    1. naive numpy implementation
    2. vectorized numpy implementation
    3. tensorflow implementation
"""
import silence_tensorflow
import numpy as np
from matplotlib import pyplot as plt
import tensorflow as tf
import math as m


# tf.compat.v1.enable_eager_execution()

def calculate_iou_naive(x1, y1, r1, x2, y2, r2):
    distance = get_distance(x1, y1, x2, y2)
    intersection = intersection_area_circle(2, 2, 1)
    union = union_area_circle(distance, r1, r2)
    return intersection / union


def calculate_iou_vectorized(x1, y1, r1, x2, y2, r2):
    distance = get_distance(x1, y1, x2, y2)
    intersection = intersection_area_circle_vectorized(distance, r1, r2)
    union = union_area_circle_vectorized(distance, r1, r2)
    return intersection / union


def calculate_iou_tf(x1, y1, r1, x2, y2, r2):
    distance = get_distance_tf(x1, y1, x2, y2)
    intersection = intersection_area_circle_tf(distance, r1, r2)
    union = union_area_circle_tf(distance, r1, r2)
    return intersection / union


def get_distance(x1, y1, x2, y2):
    # this is already vectorized
    return np.sqrt((x1 - x2) ** 2 + (y1 - y2) ** 2)


def get_distance_tf(x1, y1, x2, y2):
    return tf.math.sqrt(tf.math.pow(x1 - x2, 2) + tf.math.pow(y1 - y2, 2))


def intersection_area_circle(d, radius1, radius2):
    """Return the area of intersection of two circles.

    The circles have radii radius1 and radius2, and their centres are separated by distance.

    """
    if d <= abs(radius1 - radius2):
        # One circle is entirely enclosed in the other.
        return get_area_circle(np.minimum(radius1, radius2))
    if d >= radius1 + radius2:
        # The circles don't overlap at all.
        return 0
    radius1_sq, radius2_sq, distance2 = radius1 ** 2, radius2 ** 2, d ** 2

    alpha = np.arccos((distance2 + radius2_sq - radius1_sq) / (2 * d * radius2))
    beta = np.arccos((distance2 + radius1_sq - radius2_sq) / (2 * d * radius1))

    return (radius2_sq * alpha + radius1_sq * beta -
            0.5 * (radius2_sq * np.sin(2 * alpha) + radius1_sq * np.sin(2 * beta))
            )


def intersection_area_circle_vectorized(d, radius1, radius2):
    """
    Return the area of intersection of a batch of circle pairs
    The circles each have radii radius1 and radius2, and their centres are separated by distance d.
    """
    test_diff = abs(radius1 - radius2)
    test_sum = radius1 + radius2

    radius1_sq, radius2_sq, distance2 = radius2 ** 2, radius1 ** 2, d ** 2

    # HACK if alpha dividend would be zero set it to 1
    alpha_dividend = np.where(d * radius2 == 0, 1, (2 * d * radius2))
    alpha = np.arccos((distance2 + radius1_sq - radius2_sq) / alpha_dividend)

    # HACK if beta dividend would be zero set it to 1
    beta_dividend = np.where(d * radius1 == 0, 1, (2 * d * radius1))
    beta = np.arccos((distance2 + radius2_sq - radius1_sq) / beta_dividend)

    else_result = radius1_sq * alpha + radius2_sq * beta - 0.5 * (
            radius1_sq * np.sin(2 * alpha) + radius2_sq * np.sin(2 * beta))

    result = np.where(d <= test_diff, get_area_circle(np.minimum(radius1, radius2)),
                      np.where(d >= test_sum, 0, else_result))

    return result


def intersection_area_circle_tf(d, radius1, radius2):
    d = tf.cast(d, tf.float64)
    radius1 = tf.cast(radius1, tf.float64)
    radius2 = tf.cast(radius2, tf.float64)
    batch_shape = d.shape

    radius1_sq = tf.math.pow(radius2, 2)
    radius2_sq = tf.math.pow(radius1, 2)
    distance2 = tf.math.pow(d, 2)

    c1 = tf.equal(d * radius2, tf.constant(0.0, dtype=tf.float64))
    alpha_dividend = tf.where(c1, tf.constant(1.0, dtype=tf.float64, shape=batch_shape), (2 * d * radius2))
    alpha = tf.math.acos((distance2 + radius1_sq - radius2_sq) / alpha_dividend)

    beta_dividend = tf.where(c1, tf.constant(1.0, dtype=tf.float64, shape=batch_shape), (2 * d * radius1))
    beta = tf.math.acos((distance2 + radius2_sq - radius1_sq) / beta_dividend)

    # One circle is entirely enclosed in the other.
    f1 = lambda: get_area_circle_tf(tf.math.minimum(radius1, radius2))
    # The circles don't overlap at all.

    f2 = lambda: tf.constant(0.0, shape=batch_shape, dtype=tf.float64)
    f3 = lambda: radius1_sq * alpha + radius2_sq * beta - 0.5 * (
            radius1_sq * tf.math.sin(2 * alpha) + radius2_sq * tf.math.sin(2 * beta))

    r = tf.where(tf.less_equal(d, tf.abs(radius1 - radius2)), f1(),
                 tf.where(tf.greater_equal(d, radius2 + radius1), f2(), f3())
                 )
    return r


def get_area_circle(r):
    # this is already in vectorized form
    return np.pi * r ** 2


def get_area_circle_tf(r):
    pi = tf.constant(m.pi, dtype=tf.float64)
    return pi * tf.math.pow(r, 2)


def union_area_circle(d, radius1, radius2):
    """Return the area of the union of two circles.

    The circles have radii R and r, and their centres are separated by d.

    """
    if d <= abs(radius1 - radius2):
        # One circle is entirely enclosed in the other.
        return np.pi * max(radius1, radius2) ** 2
    if d >= radius1 + radius2:
        # The circles don't overlap at all.
        return get_area_circle(radius1) + get_area_circle(radius2)

    return (get_area_circle(radius1) + get_area_circle(radius2)) - intersection_area_circle(d, radius1, radius2)


def union_area_circle_vectorized(d, radius1, radius2):
    test_diff = abs(radius1 - radius2)
    test_sum = radius2 + radius1

    result = np.where(d <= test_diff, get_area_circle(np.maximum(radius1, radius2)),
                      np.where(d >= test_sum, get_area_circle(radius1) + get_area_circle(radius2),
                               (get_area_circle(radius1) + get_area_circle(radius2)) - intersection_area_circle_vectorized(d, radius1, radius2)))

    return result


def union_area_circle_tf(d, radius1, radius2):
    # One circle is entirely enclosed in the other.
    f1 = lambda: tf.multiply(tf.constant(np.pi, dtype=tf.float64), tf.math.pow(tf.math.maximum(radius1, radius2), 2))
    # The circles don't overlap at all.
    f2 = lambda: get_area_circle(radius1) + get_area_circle(radius2)
    f3 = lambda: (get_area_circle_tf(radius1) + get_area_circle_tf(radius2)) \
                 - intersection_area_circle_tf(d, radius1, radius2)

    r = tf.where(tf.less_equal(d, tf.abs(radius1 - radius2)), f1(),
                 tf.where(tf.greater_equal(d, radius2 + radius1), f2(), f3()))
    return r


if __name__ == '__main__':
    """
        Naive Numpy Tests
    """
    y_true1 = np.array([2.0, 0.0, 2.0])
    y_pred1 = np.array([0.0, 0.0, 1.0])

    iou_naive = calculate_iou_naive(y_true1[0], y_true1[1], y_true1[2], y_pred1[0], y_pred1[1], y_pred1[2])

    print("Naive IOU: ", iou_naive)
    """
        Vectorized Numpy Tests
    """
    y_true = np.array([[2.0, 0.0, 2.0],
                       [2.0, 0.0, 2.0],
                       [2.0, 0.0, 2.0]])

    y_pred = np.array([[0.0, 0.0, 1.0],
                       [0.0, 0.0, 1.0],
                       [0.0, 0.0, 1.0]])

    iou_vectorized = calculate_iou_vectorized(y_true[..., 0], y_true[..., 1], y_true[..., 2],
                                              y_pred[..., 0], y_pred[..., 1], y_pred[..., 2])
    print("Vectorized IOU: ", iou_vectorized)
    """
        Tensorflow Tests
    """
    with tf.compat.v1.Session() as sess:
        iou = calculate_iou_tf(y_true[..., 0], y_true[..., 1], y_true[..., 2],
                               y_pred[..., 0], y_pred[..., 1], y_pred[..., 2])
        iou_v = sess.run(iou)
        print(iou_v)

    """
        Plot Circles
    """
    fig = plt.figure()
    ax = plt.axes(xlim=(-10, 10), ylim=(-5, 5))
    ax.set_aspect('equal')
    a = plt.Circle(xy=(y_true1[0], y_true1[1]), radius=y_true1[2], fill=False, color='blue')
    b = plt.Circle(xy=(y_pred1[0], y_pred1[1]), radius=y_pred1[2], fill=False, color='red')

    ax.add_patch(a)
    ax.add_patch(b)

    # plt.show()
