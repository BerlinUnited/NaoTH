from model_zoo import fy_1500
import tensorflow as tf


test_model = fy_1500()
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

current_activation_fct = function_list[1]

new_model = tf.keras.Sequential()
for index, layer in enumerate(test_model.layers):
    if "activation" in layer.name:
        new_model.add(tf.keras.layers.Activation(current_activation_fct, name=layer.name + "_" + current_activation_fct))
    else:
        new_model.add(layer)

new_model.summary()
