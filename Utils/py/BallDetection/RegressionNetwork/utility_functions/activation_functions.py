from tensorflow.keras import backend as K


# Custom activation functions:
def swish(x):
    return x / (1 + K.exp(-x))


def mish(x):
    return x * K.tanh(K.softplus(x))


custom_keras_activations = {'swish': swish, 'mish': mish}
