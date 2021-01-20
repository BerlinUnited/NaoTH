import tensorflow as tf


def custom_loss(y_true, y_pred):
    mse = tf.losses.mean_squared_error(y_true, y_pred)

    return 100 * mse


def huber_loss(y_true, y_pred):
    """
    for reqression tasks
    """
    threshold = 1
    error = y_true - y_pred
    is_small_error = tf.abs(error) <= threshold
    small_error_loss = tf.square(error) / 2
    big_error_loss = threshold * (tf.abs(error) - (0.5 * threshold))
    return tf.where(is_small_error, small_error_loss, big_error_loss)
