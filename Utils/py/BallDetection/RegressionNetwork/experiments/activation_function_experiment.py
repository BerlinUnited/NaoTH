"""
    iteratively set the activation functions of a model to one each function in the list and run the trainings
     procedure multiple times and compare the best run with each other
"""
from utility_functions.activation_functions import *
from utility_functions.model_zoo import *
import tensorflow as tf

from experiments.rerun_training_and_compare import start_training

test_model = model1()
test_model.summary()

function_list = list()
function_list.append('elu')
function_list.append('exponential')
function_list.append('hard_sigmoid')
function_list.append('linear')
function_list.append('relu')
function_list.append('selu')
function_list.append('sigmoid')
function_list.append('softplus')
function_list.append('softsign')
function_list.append('tanh')
function_list.append('softmax')
function_list.append('swish')
function_list.append('mish')

for activation_function in function_list:

    new_model = tf.keras.Sequential()
    for index, layer in enumerate(test_model.layers):
        if "activation" in layer.name:
            if activation_function in custom_keras_activations:
                new_model.add(
                    tf.keras.layers.Activation(custom_keras_activations[activation_function],
                                               name=layer.name + "_" + activation_function))
            else:
                new_model.add(
                    tf.keras.layers.Activation(activation_function, name=layer.name + "_" + activation_function))
        else:
            new_model.add(layer)

    # set name
    new_model._name = "model1" + "_" + activation_function

    new_model.summary()
    start_training(new_model)
