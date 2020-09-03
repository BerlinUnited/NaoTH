import tensorflow as tf


def custom_loss(y_true, y_pred):
    mse = tf.losses.mean_squared_error(y_true, y_pred)

    return 100 * mse
