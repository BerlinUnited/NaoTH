#!/usr/bin/env python3
"""Convert a Keras model to frugally-deep format.
"""

import base64
import datetime
import hashlib
import json
import sys

import keras
import numpy as np
from keras import backend as K
from keras.layers import Input
from keras.models import Model, load_model

__author__ = "Tobias Hermann"
__copyright__ = "Copyright 2017, Tobias Hermann"
__license__ = "MIT"
__maintainer__ = "Tobias Hermann, https://github.com/Dobiasd/frugally-deep"
__email__ = "editgym@gmail.com"

STORE_FLOATS_HUMAN_READABLE = False


def write_text_file(path, text):
    """Write a string to a file"""
    with open(path, "w") as text_file:
        print(text, file=text_file)


def arr_as_arr5(arr):
    """Convert an n-tensor to a 5-tensor"""
    depth = len(arr.shape)
    if depth == 1:
        return arr.reshape(1, 1, 1, 1, arr.shape[0])
    if depth == 2:
        return arr.reshape(1, 1, 1, arr.shape[0], arr.shape[1])
    if depth == 3:
        return arr.reshape(1, 1, arr.shape[0], arr.shape[1], arr.shape[2])
    if depth == 4:
        return arr.reshape(1, arr.shape[0], arr.shape[1], arr.shape[2], arr.shape[3])
    if depth == 5:
        return arr
    if depth == 6 and arr.shape[0] in [None, 1]:  # todo: Is this still needed?
        return arr.reshape(arr.shape[1:])
    raise ValueError('invalid number of dimensions')


def get_layer_input_shape_shape5(layer):
    """Convert a keras shape to an fdeep shape"""
    shape = layer.input_shape[1:]
    depth = len(shape)
    if depth == 1:
        return (1, 1, 1, 1, shape[0])
    if depth == 2:
        return (1, 1, 1, shape[0], shape[1])
    if depth == 3:
        return (1, 1, shape[0], shape[1], shape[2])
    if depth == 4:
        return (1, shape[0], shape[1], shape[2], shape[3])
    if depth == 5:
        return shape
    raise ValueError('invalid number of dimensions')


def show_tensor5(tens):
    """Serialize 3-tensor to a dict"""
    values = tens.flatten()
    return {
        'shape': tens.shape,
        'values': encode_floats(values)
    }


def show_test_data_as_tensor5(arr):
    """Serialize model test data"""
    return show_tensor5(arr_as_arr5(arr))


def get_model_input_layers(model):
    """Works for different Keras version."""
    if hasattr(model, '_input_layers'):
        return model._input_layers
    if hasattr(model, 'input_layers'):
        return model.input_layers
    raise ValueError('can not get (_)input_layers from model')


def measure_predict(model, data_in):
    """Returns output and duration in seconds"""
    start_time = datetime.datetime.now()
    data_out = model.predict(data_in)
    end_time = datetime.datetime.now()
    duration = end_time - start_time
    print('Forward pass took {} s.'.format(duration.total_seconds()))
    return data_out, duration.total_seconds()


def replace_none_with(value, shape):
    """Replace every None with a fixed value."""
    return tuple(list(map(lambda x: x if x is not None else value, shape)))


def gen_test_data(model):
    """Generate data for model verification test."""

    def set_shape_idx_0_to_1_if_none(shape):
        """Change first element in tuple to 1."""
        if shape[0] is not None:
            return shape
        shape_lst = list(shape)
        shape_lst[0] = 1
        shape = tuple(shape_lst)
        return shape

    def generate_input_data(input_layer):
        """Random data fitting the input shape of a layer."""
        if input_layer._outbound_nodes and isinstance(
                input_layer._outbound_nodes[0].outbound_layer, keras.layers.Embedding):
            random_fn = lambda size: np.random.randint(
                0, input_layer._outbound_nodes[0].outbound_layer.input_dim, size)
        else:
            random_fn = np.random.normal
        try:
            shape = input_layer.batch_input_shape
        except AttributeError:
            shape = input_layer.input_shape
        return random_fn(
            size=replace_none_with(32, set_shape_idx_0_to_1_if_none(shape))).astype(np.float32)

    data_in = list(map(generate_input_data, get_model_input_layers(model)))

    warm_up_runs = 3
    test_runs = 5
    data_out = None
    for _ in range(warm_up_runs):
        measure_predict(model, data_in)
    duration_sum = 0
    print('Starting performance measurements.')
    for _ in range(test_runs):
        data_out, duration = measure_predict(model, data_in)
        duration_sum = duration_sum + duration
    duration_avg = duration_sum / test_runs
    print('Forward pass took {} s on average.'.format(duration_avg))

    return {
        'inputs': list(map(show_test_data_as_tensor5, data_in)),
        'outputs': list(map(show_test_data_as_tensor5, data_out))
    }


def split_every(size, seq):
    """Split a sequence every seq elements."""
    return (seq[pos:pos + size] for pos in range(0, len(seq), size))


def encode_floats(arr):
    """Serialize a sequence of floats."""
    if STORE_FLOATS_HUMAN_READABLE:
        return arr.flatten().tolist()
    return list(split_every(1024, base64.b64encode(arr).decode('ascii')))


def prepare_filter_weights_conv_2d(weights):
    """Change dimension order of 2d filter weights to the one used in fdeep"""
    assert len(weights.shape) == 4
    return np.moveaxis(weights, [0, 1, 2, 3], [1, 2, 3, 0]).flatten()


def prepare_filter_weights_slice_conv_2d(weights):
    """Change dimension order of 2d filter weights to the one used in fdeep"""
    assert len(weights.shape) == 4
    return np.moveaxis(weights, [0, 1, 2, 3], [1, 2, 0, 3]).flatten()


def prepare_filter_weights_conv_1d(weights):
    """Change dimension order of 1d filter weights to the one used in fdeep"""
    assert len(weights.shape) == 3
    return np.moveaxis(weights, [0, 1, 2], [1, 2, 0]).flatten()


def show_conv_1d_layer(layer):
    """Serialize Conv1D layer to dict"""
    weights = layer.get_weights()
    assert len(weights) == 1 or len(weights) == 2
    assert len(weights[0].shape) == 3
    weights_flat = prepare_filter_weights_conv_1d(weights[0])
    assert layer.padding in ['valid', 'same']
    assert len(layer.input_shape) == 3
    assert layer.input_shape[0] is None
    result = {
        'weights': encode_floats(weights_flat)
    }
    if len(weights) == 2:
        bias = weights[1]
        result['bias'] = encode_floats(bias)
    return result


def show_conv_2d_layer(layer):
    """Serialize Conv2D layer to dict"""
    weights = layer.get_weights()
    assert len(weights) == 1 or len(weights) == 2
    assert len(weights[0].shape) == 4
    weights_flat = prepare_filter_weights_conv_2d(weights[0])
    assert layer.padding in ['valid', 'same']
    assert len(layer.input_shape) == 4
    assert layer.input_shape[0] is None
    result = {
        'weights': encode_floats(weights_flat)
    }
    if len(weights) == 2:
        bias = weights[1]
        result['bias'] = encode_floats(bias)
    return result


def show_separable_conv_2d_layer(layer):
    """Serialize SeparableConv2D layer to dict"""
    weights = layer.get_weights()
    assert layer.depth_multiplier == 1
    assert len(weights) == 2 or len(weights) == 3
    assert len(weights[0].shape) == 4
    assert len(weights[1].shape) == 4

    # probably incorrect for depth_multiplier > 1?
    slice_weights = prepare_filter_weights_slice_conv_2d(weights[0])
    stack_weights = prepare_filter_weights_conv_2d(weights[1])

    assert layer.padding in ['valid', 'same']
    assert len(layer.input_shape) == 4
    assert layer.input_shape[0] is None
    result = {
        'slice_weights': encode_floats(slice_weights),
        'stack_weights': encode_floats(stack_weights),
    }
    if len(weights) == 3:
        bias = weights[2]
        result['bias'] = encode_floats(bias)
    return result


def show_depthwise_conv_2d_layer(layer):
    """Serialize DepthwiseConv2D layer to dict"""
    weights = layer.get_weights()
    assert layer.depth_multiplier == 1
    assert len(weights) in [1, 2]
    assert len(weights[0].shape) == 4

    # probably incorrect for depth_multiplier > 1?
    slice_weights = prepare_filter_weights_slice_conv_2d(weights[0])

    assert layer.padding in ['valid', 'same']
    assert len(layer.input_shape) == 4
    assert layer.input_shape[0] is None
    result = {
        'slice_weights': encode_floats(slice_weights),
    }
    if len(weights) == 2:
        bias = weights[1]
        result['bias'] = encode_floats(bias)
    return result


def show_batch_normalization_layer(layer):
    """Serialize batch normalization layer to dict"""
    layer_axis = None
    if isinstance(layer.axis, int):
        layer_axis = layer.axis
    else:
        assert len(layer.axis) == 1
        layer_axis = layer.axis[0]
    assert layer_axis == -1 or layer_axis + 1 == len(layer.input_shape)
    moving_mean = K.get_value(layer.moving_mean)
    moving_variance = K.get_value(layer.moving_variance)
    result = {}
    result['moving_mean'] = encode_floats(moving_mean)
    result['moving_variance'] = encode_floats(moving_variance)
    if layer.center:
        beta = K.get_value(layer.beta)
        result['beta'] = encode_floats(beta)
    if layer.scale:
        gamma = K.get_value(layer.gamma)
        result['gamma'] = encode_floats(gamma)
    return result


def show_dense_layer(layer):
    """Serialize dense layer to dict"""
    weights = layer.get_weights()
    assert len(weights) == 1 or len(weights) == 2
    assert len(weights[0].shape) == 2
    weights_flat = weights[0].flatten()
    result = {
        'weights': encode_floats(weights_flat)
    }
    if len(weights) == 2:
        bias = weights[1]
        result['bias'] = encode_floats(bias)
    return result


def show_prelu_layer(layer):
    """Serialize prelu layer to dict"""
    weights = layer.get_weights()
    assert len(weights) == 1
    weights_flat = weights[0].flatten()
    result = {
        'alpha': encode_floats(weights_flat)
    }
    return result


def show_embedding_layer(layer):
    """Serialize Embedding layer to dict"""
    weights = layer.get_weights()
    assert len(weights) == 1
    result = {
        'weights': encode_floats(weights[0])
    }
    return result


def show_lstm_layer(layer):
    """Serialize LSTM layer to dict"""
    weights = layer.get_weights()
    assert len(weights) == 2 or len(weights) == 3
    result = {'weights': encode_floats(weights[0]),
              'recurrent_weights': encode_floats(weights[1])}

    if len(weights) == 3:
        result['bias'] = encode_floats(weights[2])

    return result


def show_gru_layer(layer):
    """Serialize GRU layer to dict"""
    weights = layer.get_weights()
    assert len(weights) == 2 or len(weights) == 3
    result = {'weights': encode_floats(weights[0]),
              'recurrent_weights': encode_floats(weights[1])}

    if len(weights) == 3:
        result['bias'] = encode_floats(weights[2])

    return result


def show_bidirectional_layer(layer):
    """Serialize Bidirectional layer to dict"""
    forward_weights = layer.forward_layer.get_weights()
    assert len(forward_weights) == 2 or len(forward_weights) == 3

    backward_weights = layer.backward_layer.get_weights()
    assert len(backward_weights) == 2 or len(backward_weights) == 3

    result = {'forward_weights': encode_floats(forward_weights[0]),
              'forward_recurrent_weights': encode_floats(forward_weights[1]),
              'backward_weights': encode_floats(backward_weights[0]),
              'backward_recurrent_weights': encode_floats(backward_weights[1])}

    if len(forward_weights) == 3:
        result['forward_bias'] = encode_floats(forward_weights[2])
    if len(backward_weights) == 3:
        result['backward_bias'] = encode_floats(backward_weights[2])

    return result


def get_layer_functions_dict():
    return {
        'Conv1D': show_conv_1d_layer,
        'Conv2D': show_conv_2d_layer,
        'SeparableConv2D': show_separable_conv_2d_layer,
        'DepthwiseConv2D': show_depthwise_conv_2d_layer,
        'BatchNormalization': show_batch_normalization_layer,
        'Dense': show_dense_layer,
        'PReLU': show_prelu_layer,
        'Embedding': show_embedding_layer,
        'LSTM': show_lstm_layer,
        'GRU': show_gru_layer,
        'Bidirectional': show_bidirectional_layer,
        'TimeDistributed': show_time_distributed_layer,
        'UpSampling2D': check_upsampling_2d_layer
    }


def check_upsampling_2d_layer(layer):
    print(layer.get_config())
    assert layer.get_config()['interpolation'] == 'nearest', \
        'Only interpolation nearest is currently supported by frugally-deep.'
    return None


def show_time_distributed_layer(layer):
    show_layer_functions = get_layer_functions_dict()
    config = layer.get_config()
    class_name = config['layer']['class_name']

    if class_name in show_layer_functions:

        if len(layer.input_shape) == 3:
            input_shape_new = (layer.input_shape[0], layer.input_shape[2])
        elif len(layer.input_shape) == 4:
            input_shape_new = (layer.input_shape[0], layer.input_shape[2], layer.input_shape[3])
        elif len(layer.input_shape) == 5:
            input_shape_new = (layer.input_shape[0], layer.input_shape[2], layer.input_shape[3], layer.input_shape[4])
        elif len(layer.input_shape) == 6:
            input_shape_new = (layer.input_shape[0], layer.input_shape[2], layer.input_shape[3], layer.input_shape[4],
                               layer.input_shape[5])
        else:
            raise Exception('Wrong input shape')

        layer_function = show_layer_functions[class_name]
        attributes = dir(layer.layer)

        class CopiedLayer:
            pass

        copied_layer = CopiedLayer()

        for attr in attributes:
            try:
                if attr not in ['input_shape', '__class__']:
                    setattr(copied_layer, attr, getattr(layer.layer, attr))
                elif attr == 'input_shape':
                    setattr(copied_layer, 'input_shape', input_shape_new)
            except Exception:
                continue

        setattr(copied_layer, "output_shape", getattr(layer, "output_shape"))

        return layer_function(copied_layer)

    else:
        return None


def get_dict_keys(d):
    """Return keys of a dictionary"""
    return [key for key in d]


def merge_two_disjunct_dicts(x, y):
    """Given two dicts, merge them into a new dict as a shallow copy.
    No Key is allowed to be present in both dictionaries.
    """
    assert set(get_dict_keys(x)).isdisjoint(get_dict_keys(y))
    z = x.copy()
    z.update(y)
    return z


def is_ascii(some_string):
    """Check if a string only contains ascii characters"""
    try:
        some_string.encode('ascii')
    except UnicodeEncodeError:
        return False
    else:
        return True


def get_all_weights(model):
    """Serialize all weights of the models layers"""
    show_layer_functions = get_layer_functions_dict()
    result = {}
    layers = model.layers
    assert K.image_data_format() == 'channels_last'
    for layer in layers:
        layer_type = type(layer).__name__
        if layer_type in ['Model', 'Sequential']:
            result = merge_two_disjunct_dicts(result, get_all_weights(layer))
        else:
            if hasattr(layer, 'data_format'):
                if layer_type in ['AveragePooling1D', 'MaxPooling1D', 'AveragePooling2D', 'MaxPooling2D',
                                  'GlobalAveragePooling1D', 'GlobalMaxPooling1D', 'GlobalAveragePooling2D',
                                  'GlobalMaxPooling2D']:
                    assert layer.data_format == 'channels_last' or layer.data_format == 'channels_first'
                else:
                    assert layer.data_format == 'channels_last'

            show_func = show_layer_functions.get(layer_type, None)
            name = layer.name
            assert is_ascii(name)
            if name in result:
                raise ValueError('duplicate layer name ' + name)
            shown_layer = None
            if show_func:
                shown_layer = show_func(layer)
            if shown_layer:
                result[name] = shown_layer
            if show_func and layer_type == 'TimeDistributed':
                if name not in result:
                    result[name] = {}

                result[name]['td_input_len'] = encode_floats(np.array([len(layer.input_shape) - 1], dtype=np.float32))
                result[name]['td_output_len'] = encode_floats(np.array([len(layer.output_shape) - 1], dtype=np.float32))
    return result


def get_model_name(model):
    """Return .name or ._name or 'dummy_model_name'"""
    if hasattr(model, 'name'):
        return model.name
    if hasattr(model, '_name'):
        return model._name
    return 'dummy_model_name'


def set_model_name(model, name):
    """Overwrite .name or ._name'"""
    if hasattr(model, 'name'):
        model.name = name
    elif hasattr(model, '_name'):
        model._name = name
    else:
        pass  # Model has no name property.


def convert_sequential_to_model(model):
    """Convert a sequential model to the underlying functional format"""
    if type(model).__name__ == 'Sequential':
        name = get_model_name(model)
        if hasattr(model, '_inbound_nodes'):
            inbound_nodes = model._inbound_nodes
        elif hasattr(model, 'inbound_nodes'):
            inbound_nodes = model.inbound_nodes
        else:
            raise ValueError('can not get (_)inbound_nodes from model')
        # Since Keras 2.2.0
        if model.model == model:
            input_layer = Input(batch_shape=model.layers[0].input_shape)
            prev_layer = input_layer
            for layer in model.layers:
                prev_layer = layer(prev_layer)
            funcmodel = Model([input_layer], [prev_layer])
            model = funcmodel
        else:
            model = model.model
        set_model_name(model, name)
        if hasattr(model, '_inbound_nodes'):
            model._inbound_nodes = inbound_nodes
        elif hasattr(model, 'inbound_nodes'):
            model.inbound_nodes = inbound_nodes
    assert model.layers
    for i in range(len(model.layers)):
        if type(model.layers[i]).__name__ in ['Model', 'Sequential']:
            model.layers[i] = convert_sequential_to_model(model.layers[i])
    return model


def offset_conv2d_eval(depth, padding, x):
    """Perform a conv2d on x with a given padding"""
    kernel = K.variable(value=np.array([[[[1]] + [[0]] * (depth - 1)]]),
                        dtype='float32')
    return K.conv2d(x, kernel, strides=(3, 3), padding=padding)


def offset_sep_conv2d_eval(depth, padding, x):
    """Perform a separable conv2d on x with a given padding"""
    depthwise_kernel = K.variable(value=np.array([[[[1]] * depth]]),
                                  dtype='float32')
    pointwise_kernel = K.variable(value=np.array([[[[1]] + [[0]] * (depth - 1)]]),
                                  dtype='float32')
    return K.separable_conv2d(x, depthwise_kernel,
                              pointwise_kernel, strides=(3, 3), padding=padding)


def conv2d_offset_max_pool_eval(_, padding, x):
    """Perform a max pooling operation on x"""
    return K.pool2d(x, (1, 1), strides=(3, 3), padding=padding, pool_mode='max')


def conv2d_offset_average_pool_eval(_, padding, x):
    """Perform an average pooling operation on x"""
    return K.pool2d(x, (1, 1), strides=(3, 3), padding=padding, pool_mode='avg')


def check_operation_offset(depth, eval_f, padding):
    """Check if backend used an offset while placing the filter
    e.g. during a convolution.
    TensorFlow is inconsistent in doing so depending
    on the type of operation, the used device (CPU/GPU) and the input depth.
    """
    in_arr = np.array([[[[i] * depth for i in range(6)]]])
    input_data = K.variable(value=in_arr, dtype='float32')
    output = eval_f(depth, padding, input_data)
    result = K.eval(output).flatten().tolist()
    assert result in [[0, 3], [1, 4]]
    return result == [1, 4]


def get_shapes(tensor5s):
    """Return shapes of a list of tensors"""
    return [t['shape'] for t in tensor5s]


def calculate_hash(model):
    layers = model.layers
    hash_m = hashlib.sha256()
    for layer in layers:
        for weights in layer.get_weights():
            assert isinstance(weights, np.ndarray)
            hash_m.update(weights.tobytes())
        hash_m.update(layer.name.encode('ascii'))
    return hash_m.hexdigest()


def convert(in_path, out_path):
    """Convert any Keras model to the frugally-deep model format."""

    assert K.backend() == "tensorflow"
    assert K.floatx() == "float32"
    assert K.image_data_format() == 'channels_last'

    print('loading {}'.format(in_path))
    model = load_model(in_path)

    # Force creation of underlying functional model.
    # see: https://github.com/fchollet/keras/issues/8136
    # Loss and optimizer type do not matter, since we do not train the model.
    model.compile(loss='mse', optimizer='sgd')

    model = convert_sequential_to_model(model)

    test_data = gen_test_data(model)

    json_output = {}
    json_output['architecture'] = json.loads(model.to_json())

    json_output['image_data_format'] = K.image_data_format()
    for depth in range(1, 3, 1):
        json_output['conv2d_valid_offset_depth_' + str(depth)] = \
            check_operation_offset(depth, offset_conv2d_eval, 'valid')
        json_output['conv2d_same_offset_depth_' + str(depth)] = \
            check_operation_offset(depth, offset_conv2d_eval, 'same')
        json_output['separable_conv2d_valid_offset_depth_' + str(depth)] = \
            check_operation_offset(depth, offset_sep_conv2d_eval, 'valid')
        json_output['separable_conv2d_same_offset_depth_' + str(depth)] = \
            check_operation_offset(depth, offset_sep_conv2d_eval, 'same')
    json_output['max_pooling_2d_valid_offset'] = \
        check_operation_offset(1, conv2d_offset_max_pool_eval, 'valid')
    json_output['max_pooling_2d_same_offset'] = \
        check_operation_offset(1, conv2d_offset_max_pool_eval, 'same')
    json_output['average_pooling_2d_valid_offset'] = \
        check_operation_offset(1, conv2d_offset_average_pool_eval, 'valid')
    json_output['average_pooling_2d_same_offset'] = \
        check_operation_offset(1, conv2d_offset_average_pool_eval, 'same')
    json_output['input_shapes'] = list(map(get_layer_input_shape_shape5, get_model_input_layers(model)))
    json_output['tests'] = [test_data]
    json_output['trainable_params'] = get_all_weights(model)
    json_output['hash'] = calculate_hash(model)

    print('writing {}'.format(out_path))
    write_text_file(out_path, json.dumps(
        json_output, allow_nan=False, indent=2, sort_keys=True))


def main():
    """Parse command line and convert model."""

    usage = 'usage: [Keras model in HDF5 format] [output path]'

    if len(sys.argv) != 3:
        print(usage)
        sys.exit(1)

    in_path = sys.argv[1]
    out_path = sys.argv[2]

    convert(in_path, out_path)


if __name__ == "__main__":
    main()
