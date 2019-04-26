""" Olli's Code Generator (OCG) """

__author__ = "Oliver Urbann"
__copyright__ = "Copyright 2017-2018, Oliver Urbann"
__credits__ = ["Oliver Urbann"]
__license__ = "Commercial, free for educational usage."
__version__ = "1.0.0"
__email__ = "oliver.urbann@tu-dortmund.de"

import random
import os
import platform
import numpy as np
from keras import backend as K, Model
from keras.layers import Convolution2D, MaxPooling2D, Flatten, Dropout, BatchNormalization, LeakyReLU, Dense
from keras.models import load_model

if platform.system() != 'Darwin':
    compiler = 'g++ -mssse3 -std=c++11 -g -march=bonnell -DCNN_TEST '
    compiler_O3 = 'g++ -O3 -mssse3 -std=c++11 -march=bonnell -DCNN_TEST '
else:
    compiler = 'clang++ --target=i686-unknown-linux-gnu -msse3 -march=bonnell -std=c++11 -g -DCNN_TEST '
    compiler_O3 = 'clang++ -O3 --target=i686-unknown-linux-gnu -msse3 -march=bonnell -std=c++11 -g -DCNN_TEST '

footer_test = '''
#ifdef CNN_TEST
#include <stdio.h>
    
int main()
{{
    int i, j, res;
    FILE *f = fopen("img.bin", "r");
    float x[{x_dim}][{y_dim}][{z_dim}];
    float scores[2];
    for (j = 0; j < {x_dim}; j++)
        for (i = 0; i < {y_dim}; i++)
            for (int k = 0; k < {z_dim}; k++)
                fread(&x[j][i][k], sizeof(float), 1, f);
    fclose(f);
    res = 0;
    cnn(x, &res, scores);
    return res;
}}
#endif
'''

footer_no_test = '''
#ifdef CNN_TEST
#include <stdio.h>
int main()
{
    printf("Thx for using Olli's Code Generator (OCG)");
    return 0;
}
#endif
'''

footer_benchmark = '''
#ifdef CNN_TEST
#include <stdio.h>
#include <sys/time.h>

int main()
{{
    int i, j, res;
    struct timeval st, et;
    float x[{x_dim}][{y_dim}][{z_dim}];
    const int x_out_dim = {:d};
    const int y_out_dim = {:d};
    const int z_out_dim = {:d};
    float output_tensor alignas(16) [x_out_dim][y_out_dim][z_out_dim];
    
    // read test image
    FILE *f = fopen("img.bin", "r");
    for (j = 0; j < {x_dim}; j++)
        for (i = 0; i < {y_dim}; i++)
            for (int k = 0; k < {z_dim}; k++)
                fread(&x[j][i][k], sizeof(float), 1, f);
    res = 0;
    for (int i = 0; i < 1000; i++)
    {{
        gettimeofday(&st,NULL);
        cnn(x, output_tensor);
        gettimeofday(&et,NULL);
        int elapsed = ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec);
        printf("inference time: %d micro seconds\\n",elapsed);
    }}
    
    // write result to stdout
    for (j = 0; j < x_out_dim; j++)
        for (i = 0; i < y_out_dim; i++)
            for (int k = 0; k < z_out_dim; k++)
                printf("%f\\n", output_tensor[j][i][k]);
    return res;
}}
#endif
'''

sse_conv = '''
{indent}w = _mm_set_ps({w3}f, {w2}f, {w1}f, {w0}f);
{indent}x = _mm_load_ps1(&x{prev_layer}[{x_1}][{x_2}][{kw}]);
{indent}y = _mm_mul_ps(w, x);
{indent}x = _mm_load_ps((float*)&x{layer}[{x_out_1}][{x_out_2}][{lw}]);
{indent}x = _mm_add_ps(x, y);
{indent}_mm_store_ps((float*)&x{layer}[{x_out_1}][{x_out_2}][{lw}], x);
'''

sse_conv_2 = '''
{indent}w = _mm_set_ps({w3}f, {w2}f, {w1}f, {w0}f);
{indent}x = _mm_load_ps1(&x{prev_layer}[{x_1}][{x_2}][{kw}]);
{indent}t = _mm_mul_ps(w, x);
{indent}y = _mm_add_ps(y, t);
'''

sse_conv_even = '''
{indent}w = _mm_set_ps({w3}f, {w2}f, {w1}f, {w0}f);
{indent}x = _mm_load_ps1(&x{prev_layer}[{x_1}][{x_2}][{kw}]);
{indent}t = _mm_mul_ps(w, x);
{indent}y = _mm_add_ps(y, t);
'''

sse_conv_odd = '''
{indent}w = _mm_set_ps({w3}f, {w2}f, {w1}f, {w0}f);
{indent}x = _mm_load_ps1(&x{prev_layer}[{x_1}][{x_2}][{kw}]);
{indent}t2 = _mm_mul_ps(w, x);
{indent}y2 = _mm_add_ps(y2, t2);
'''

x86_conv = '''
{{
{indent}static const float g alignas(16) [4] = {{{w0}f, {w1}f, {w2}f, {w3}f}};
{indent}asm ("movaps %2, %%xmm0\\n"  
{indent}     "movss %1, %%xmm1\\n" 
{indent}     "shufps $0, %%xmm1, %%xmm1\\n"
{indent}     "mulps %%xmm0, %%xmm1\\n"
{indent}     "movaps %0, %%xmm2\\n"
{indent}     "addps %%xmm1, %%xmm2\\n" 
{indent}     "movaps %%xmm2, %0\\n"
{indent}     :"+m"(x{layer}[{x_out_1}][{x_out_2}][{lw}])
{indent}     :"m"(x{prev_layer}[{x_1}][{x_2}][{kw}]), "m"(g) 
{indent}     :"%xmm0", "%xmm1", "%xmm2"
{indent});
}}
'''

sse_relu = '''
{indent}x = _mm_load_ps((float*)&x{prev_layer}[{i}][{j}][{k}]);
{indent}x = _mm_max_ps(x, _mm_setzero_ps());
{indent}_mm_store_ps((float*)&x{prev_layer}[{i}][{j}][{k}], x);
'''

sse_leaky = '''
{indent}x = _mm_load_ps((float*)&x{prev_layer}[{i}][{j}][{k}]);
{indent}x = _mm_max_ps(x, _mm_mul_ps(y, x));
{indent}_mm_store_ps((float*)&x{prev_layer}[{i}][{j}][{k}], x);
'''


def write_naoth_header_file():
    fp = open("CNN_thomas_balls.h", "w")
    with fp:
        print("#ifndef _CNN_THOMAS_BALLS_H", file=fp)
        print("#define _CNN_THOMAS_BALLS_H", file=fp)
        print("", file=fp)
        print("# include <emmintrin.h>", file=fp)
        print("# include <math.h>", file=fp)
        print("", file=fp)
        print("#include \"AbstractCNNClassifier.h\"", file=fp)
        print("", file=fp)
        print("class CNN_THOMAS_BALLS : public AbstractCNNClassifier {", file=fp)
        print("", file=fp)
        print("public:", file=fp)
        print("\tint cnn(float x0[16][16][1],  double output_tensor[3][1][1]);", file=fp)
        print("", file=fp)
        print("};", file=fp)
        print("# endif", file=fp)


def compile(c_inf, optimize=False):
    c = compiler_O3 if optimize else compiler
    if os.system(c + c_inf["path"] + " -o " + c_inf["path"][:-2]) != 0:
        print("Error compiling file.")
        exit(-3)


def softmax(x, c_inf):
    assert (x.shape[2] == 2)  # Sorry, only for depth 2 at the moment
    x_out = np.zeros(x.shape).astype('float32')
    writeC(c_inf, '\tstatic float x{:d}[{:d}][{:d}][{:d}] = {{0}};\n'.format(c_inf["layer"], x.shape[0], x.shape[1],
                                                                             x.shape[2]))

    for i in range(size(x, 1)):
        for j in range(size(x, 2)):
            writeC(c_inf,
                   '\tstatic float max{:d} = x{:d}[{:d}][{:d}][0] > x{:d}[{:d}][{:d}][1] ? x{:d}[{:d}][{:d}][0] : x{:d}[{:d}][{:d}][1];\n'.format(
                       c_inf["layer"],
                       c_inf["layer"] - 1, i, j,
                       c_inf["layer"] - 1, i, j,
                       c_inf["layer"] - 1, i, j,
                       c_inf["layer"] - 1, i, j))
            x_out[i, j] = 2.718281828459045 ** (x - np.max(x))
            writeC(c_inf,
                   '\tx{:d}[{:d}][{:d}][0] = (float)exp(x{:d}[{:d}][{:d}][0] - max{:d});\n'
                   '\tx{:d}[{:d}][{:d}][1] = (float)exp(x{:d}[{:d}][{:d}][1] - max{:d});\n'.format(
                       c_inf["layer"], i, j,
                       c_inf["layer"] - 1, i, j,
                       c_inf["layer"],
                       c_inf["layer"], i, j,
                       c_inf["layer"] - 1, i, j,
                       c_inf["layer"]))

            x_out[i, j] = x_out[i, j] / x_out[i, j].sum()
            writeC(c_inf, '\tstatic float sum{:d};\n'.format(c_inf["layer"]))
            writeC(c_inf, '\tsum{:d} = x{:d}[{:d}][{:d}][0] + x{:d}[{:d}][{:d}][1];\n'.format(
                c_inf["layer"],
                c_inf["layer"], i, j,
                c_inf["layer"], i, j
            ))
            writeC(c_inf, '\tx{:d}[{:d}][{:d}][0] /= sum{:d};\n'
                          '\tx{:d}[{:d}][{:d}][1] /= sum{:d};\n'.format(
                           c_inf["layer"], i, j,
                           c_inf["layer"],
                           c_inf["layer"], i, j,
                           c_inf["layer"]))

    return x_out, c_inf


def writeC(c_inf, str):
    if c_inf["f"] is not None:
        c_inf["f"].write(str)


def keras_compile(imdb, model_path, code_path, unroll_level=0, arch="general", conv_mode=0):
    write_naoth_header_file()

    test_im_index = 0
    im = imdb["images"][test_im_index]
    c_inf = {}
    c_inf["path"] = code_path
    c_inf["x_dim"] = im.shape[0]
    c_inf["y_dim"] = im.shape[1]
    if len(im.shape) > 2:
        c_inf["z_dim"] = im.shape[2]
    else:
        c_inf["z_dim"] = 1
    # intermediate_output = []

    c_inf = write_header(c_inf, arch)
    if unroll_level > 0:
        writeC(c_inf, '\tfor (int xi = 0; xi < {:d}; xi += 1)\n\t{{\n'.format(size(im, 1)))
        _xi = 'xi'
    for xi in range(size(im, 1)):
        if unroll_level == 0:
            _xi = xi
        for xj in range(size(im, 2)):
            for xk in range(size(im, 3)):
                if unroll_level == 0 or xi == 0:
                    writeC(c_inf, '\tx0[{xi}][{:d}][{:d}] = (x0[{xi}][{:d}][{:d}] - {:f}f);\n'.format(
                        xj, xk, xj, xk, imdb["mean"], xi=_xi))
    if unroll_level > 0:
        writeC(c_inf, '\t}\n')
    model = load_model(model_path)
    _x = imdb["images"][test_im_index]
    last_activation = 'none'
    for layer in model.layers:
        print("layer is: ", layer, c_inf['layer'])
        if type(layer) == Convolution2D:
            writeC(c_inf, '\n \t// Convolution Layer\n')
            w = K.eval(layer.weights[0])
            b = K.eval(layer.bias)
            if conv_mode == 0:
                _x, c_inf = convolution(_x, w, b, layer.strides, layer.padding, c_inf, unroll_level, arch)
            elif conv_mode == 1:
                _x, c_inf = convolution_2(_x, w, b, layer.strides, layer.padding, c_inf, unroll_level, arch)
            elif conv_mode == 2:
                _x, c_inf = convolution_3(_x, w, b, layer.strides, layer.padding, c_inf, unroll_level, arch)
            else:
                raise NotImplementedError
            if layer.activation.__name__ == 'relu':
                _x, c_inf = rectifiedLinearUnit(_x, c_inf, unroll_level, 0, arch)
            if layer.activation.__name__ == 'softmax':
                _x, c_inf = softmax(_x, c_inf)
            last_activation = layer.activation.__name__
        elif type(layer) == MaxPooling2D:
            writeC(c_inf, '\n \t// Maxpool Layer \n')
            _x, c_inf = max_pool(_x, layer.pool_size, layer.strides, c_inf, unroll_level, arch)
        elif type(layer) == LeakyReLU:
            writeC(c_inf, '\n \t// Leaky ReLu Layer\n')
            _x, c_inf = rectifiedLinearUnit(_x, c_inf, unroll_level, float(layer.alpha), arch)
            last_activation = 'leaky'
        elif type(layer) == Flatten:
            pass
        elif type(layer) == Dropout:
            pass
        elif type(layer) == BatchNormalization:
            print("Warning: BatchNormalization not implemented")
            pass
        elif type(layer) == Dense:
            writeC(c_inf, '\n \t// Dense Layer\n')
            print("Warning: Dense implementation not finished")
            w = K.eval(layer.weights[0])
            b = K.eval(layer.bias)
            _x, c_inf = dense(_x, w, b, c_inf)

        else:
            print("Unknown layer")
            exit(-1)
        """
        intermediate_layer_model = Model(inputs=model.input,
                                         outputs=layer.output)
        io = intermediate_layer_model.predict(im.reshape(1, *im.shape))
        intermediate_output.append(io)
        if not np.allclose(_x.reshape(1, *_x.shape), io, atol=0.001):
            diff = np.abs(_x.reshape(1, *_x.shape) - io)
            print(str(layer) + ": err, maximum err = ", diff.max())
        else:
            print(str(layer) + ": ok")
        """

    # last layer with softmax activition indicates a classification problem
    # write scores etc. only in this case
    """
    classification = False
    if last_activation == 'softmax':
        for ix in range(size(_x, 3)):
            writeC(c_inf, '\tscores[{:d}] = x{:d}[{:d}][{:d}][{:d}];\n'.format(
                ix, c_inf["layer"], 0, 0, ix))
        writeC(c_inf, '\t*res = 0;\n')
        for i in range(1, size(_x, 3)):
            writeC(c_inf, '\t*res = scores[{:d}] > scores[*res] ? {:d} : *res;\n'.format(i, i))
        classification = True
    """
    write_footer(c_inf, _x, classification=False)
    """
    if classification:
        print("Testing...")
        for i in range(10000):
            im_num = random.randrange(len(imdb["images"]))
            # im_num = 407910
            im = imdb["images"][im_num]
            im = im.reshape(1, *im.shape)
            (im + imdb["mean"]).astype("float32").tofile("img.bin")
            res = os.system("./" + c_inf["path"][:-2])
            p = np.argmax(model.predict(im))
            if res >> 8 == p:
                print("Image " + str(im_num) + " passed, is " + str(p))
            else:
                print("Error at image " + str(im_num))
                exit(-2)
    else:
        return
        print("Running...")
        im.tofile("img.bin")
        expected_result = model.predict(im.reshape(1, *im.shape))

        output = os.popen("./" + c_inf["path"][:-2]).read()

        values = [float(o) for o in output.split("\n") if o and not o.startswith("inference")]
        c_result = np.array(values).reshape(expected_result.shape)
        if not np.allclose(expected_result, c_result, atol=0.001):
            diff = np.abs(expected_result - c_result)
            print("Error in result of c code, maximum err = ", diff.max())
        else:
            print("Result of c code is correct")

        times = [float(o.split()[2]) for o in output.split("\n") if o.startswith("inference")]
        times = np.array(times)
        print("runs: {}, total: {} micro seconds, mean: {} micro seconds".format(times.size, times.sum(), times.mean()))
    """


def dense(_x, weights, b, c_inf):
    # TODO get output size dynamically
    # TODO use bias
    x_out = np.zeros(shape=(3, 1, 1)).astype('float32')

    x_dim = _x.shape[0]
    y_dim = _x.shape[1]
    channels = _x.shape[2]

    i = 0

    for output in range(len(x_out)):
        c_inf["f"].write("\tx{:d}[{:d}][0][0] = ".format(c_inf["layer"], output))
        for c in range(channels):
            for x in range(x_dim):
                for y in range(y_dim):
                    idx = (c - 1) * (y_dim * x_dim) + (y - 1) * y_dim + x
                    if i % 4 is 0:
                        c_inf["f"].write('\n\t')

                    if weights[idx, output] >= 0:
                        c_inf["f"].write(' + ')
                    else:
                        c_inf["f"].write(' - ')

                    c_inf["f"].write('{:f} * x{:d}[{:d}][{:d}][{:d}]'.format(
                        abs(weights[idx, output]),
                        c_inf["layer"] - 1, x, y, c
                    ))

                    i += 1
        c_inf["f"].write(';\n')
        c_inf["f"].write('\n')

    c_inf["layer"] = c_inf["layer"] + 1
    return x_out, c_inf


def size(x, i=1):
    return x.shape[i - 1]


def write_header(c_inf, arch):
    c_inf["f"] = open(c_inf["path"], 'w')
    c_inf["layer"] = 1
    if arch == 'sse3':
        c_inf["f"].write('#include <emmintrin.h>\n')
        c_inf["f"].write('#include CNN_thomas_balls.h\n\n')

    c_inf["f"].write('#include <math.h>\nint cnn(float x0[{:d}][{:d}][{:d}], int *res, double *scores)\n'.format(
        c_inf["x_dim"],
        c_inf["y_dim"],
        c_inf["z_dim"]) + '{\n')

    if arch == 'sse3':
        # c_inf["f"].write('__m128 w, x, y, y2, t, t2;\n')
        c_inf["f"].write('\t__m128 w, x, y;\n')

    return c_inf


def write_footer(c_inf, _x, classification):
    if c_inf["f"] is not None:
        c_inf["f"].write('\treturn 0;\n}\n')

        # Dummy Stuff for Naoth - TODO is this necessary
        # c_inf["f"].write('bool classify(const BallCandidates::Patch& p){return true;}\n')
        # c_inf["f"].write('bool classify(const BallCandidates::PatchYUVClassified& p){return true;}\n')
        # c_inf["f"].write('float getBallConfidence(){return 0.0f;}\n')
        # c_inf["f"].write('float getNoballConfidence(){return 0.0f;}\n')
        """
        if classification:
            c_inf["f"].write(footer_test.format(**c_inf))
        else:
            c_inf["f"].write(footer_benchmark.format(x_dim=c_inf['x_dim'], y_dim=c_inf['y_dim'], z_dim=c_inf['z_dim'],
                                                     *_x.shape))
            c_inf["f"].close()
            with open(c_inf["path"], 'r') as file:
                data = file.read()
            data = data.replace("int *res, float *scores", ' float output_tensor[{:d}][{:d}][{:d}]'.format(
                size(_x, 1),
                size(_x, 2),
                size(_x, 3)))
            data = data.replace('x{:d}['.format(c_inf["layer"] - 1), 'output_tensor[')
            with open(c_inf["path"], 'w') as file:
                file.write(data)
        c_inf["f"].close()
        c_inf["f"] = None
        #print("Compiling...")
        #compile(c_inf, optimize=True)
        """
    return c_inf


def convolution(x, w, b, stride, pad, c_inf, unroll_level, arch):
    assert x.shape[2] == w.shape[2]
    assert w.shape[3] == b.shape[0]
    H, W, C_IN = x.shape
    KH, KW, _, C_OUT = w.shape
    SH, SW = stride

    if pad == 'valid':
        H_OUT = int(np.ceil((H - KH + 1) / SH))
        W_OUT = int(np.ceil((W - KW + 1) / SW))
        pad_top = pad_bottom = pad_left = pad_right = 0
    elif pad == 'same':
        H_OUT = int(np.ceil(float(H) / float(SH)))
        W_OUT = int(np.ceil(float(W) / float(SW)))
        pad_along_height = max((H_OUT - 1) * SH + KH - H, 0)
        pad_along_width = max((W_OUT - 1) * SW + KW - W, 0)
        pad_top = int(pad_along_height // 2)
        pad_bottom = int(pad_along_height - pad_top)
        pad_left = int(pad_along_width // 2)
        pad_right = int(pad_along_width - pad_left)
    else:
        print("Unknown padding type")
        exit(-4)
    x_out = np.zeros((H_OUT, W_OUT, C_OUT)).astype('float32')
    str_data = {'prev_layer': c_inf["layer"] - 1, 'x_1': 'x_1', 'x_2': 'x_2',
                'x_out_1': 'x_out_1', 'x_out_2': 'x_out_2', 'layer': c_inf["layer"],
                'lw': 'lw', 'indent': '\t', 'x_res': H_OUT, 'y_res': W_OUT, 'z_res': C_OUT,
                'stride0': stride[0], 'stride1': stride[1], 'i': 'i', 'j': 'j',
                'pt': pad_top, 'pb': pad_bottom, 'pl': pad_left, 'pr': pad_right}

    writeC(c_inf, '{indent}static float x{layer} alignas(16) [{x_res}][{y_res}][{z_res}] = {{0}};\n'.format(**str_data))

    if unroll_level > 0:
        writeC(c_inf, '{indent}for (int i = 0; i < {:d}; i += 1)\n{indent}{{\n'.format(H_OUT, **str_data))
        str_data['indent'] = '\t\t'
    for i in range(H_OUT):
        if unroll_level > 1 and i == 0:
            writeC(c_inf, '{indent}for (int j = 0; j < {:d}; j += 1)\n{indent}{{\n'.format(W_OUT, **str_data))
            str_data['indent'] = '\t\t\t'
        if unroll_level == 0:
            str_data['i'] = i
        for j in range(W_OUT):
            if unroll_level < 2:
                str_data['j'] = j
            for kw in range(C_OUT):
                str_data['kw'] = kw
                str_data['b'] = b[kw]
                x_out[i, j, kw] = b[kw]
                if unroll_level == 0 or (unroll_level == 1 and i == 0) or (unroll_level == 2 and i == 0 and j == 0):
                    writeC(c_inf, '{indent}x{layer}[{i}][{j}][{kw}] = {b}f;\n'.format(**str_data))
        if unroll_level > 1 and i == 0:
            str_data['indent'] = '\t\t'
            writeC(c_inf, '{indent}}}\n'.format(**str_data))
    if unroll_level > 0:
        str_data['indent'] = '\t'
        writeC(c_inf, '{indent}}}\n'.format(**str_data))

    if unroll_level > 0:
        writeC(c_inf, '{indent}for (int ix = -{pt}; ix < {:d}; ix += {stride0})\n{indent}{{\n'.format(
            H - KH + 1 + pad_bottom, **str_data))
        str_data['indent'] = '\t\t'
        writeC(c_inf, '{indent}int x_1, x_out_1;\n'.format(**str_data))

    for ix in range(-pad_top, H - KH + 1 + pad_bottom, SH):
        assert (ix + pad_top) % SH == 0
        x_out_1 = (ix + pad_top) // SH

        if unroll_level > 0 and ix == -pad_top:
            writeC(c_inf, '{indent}x_out_1 = (ix + {pt}) / {stride0};\n'.format(**str_data))
        else:
            str_data['x_out_1'] = x_out_1

        if unroll_level > 1 and ix == -pad_top:
            writeC(c_inf, '{indent}for (int jx = -{pl}; jx < {:d}; jx += {stride1})\n{indent}{{\n'.format(
                W - KW + 1 + pad_right, **str_data))
            str_data['indent'] = '\t\t\t'
            writeC(c_inf, '{indent}int x_2, x_out_2;\n'.format(**str_data))

        for jx in range(-pad_left, W - KW + 1 + pad_right, SW):
            assert (jx + pad_left) % SW == 0
            x_out_2 = (jx + pad_left) // SW

            if unroll_level > 1 and -pad_left == jx and ix == -pad_top:
                writeC(c_inf, '{indent}x_out_2 = (jx + {pl}) / {stride1};\n'.format(**str_data))
            else:
                str_data['x_out_2'] = x_out_2

            for iw in range(KH):
                x_1 = ix + iw
                if (ix == -pad_top and unroll_level == 1) or (unroll_level == 2 and jx == -pad_left and ix == -pad_top):
                    writeC(c_inf, '{indent}x_1 = ix + {:d};\n'.format(iw, **str_data))
                    if pad == 'same':
                        writeC(c_inf, '{indent}if (x_1 >= 0 && x_1 < {:d})\n{indent}{{\n'.format(H, **str_data))
                        str_data['indent'] += '\t'
                else:
                    str_data['x_1'] = x_1
                for jw in range(KW):
                    x_2 = jx + jw
                    if unroll_level > 1 and jx == -pad_left and ix == -pad_top:
                        writeC(c_inf, '{indent}x_2 = jx + {:d};\n'.format(jw, **str_data))
                        if pad == 'same':
                            writeC(c_inf, '{indent}if (x_2 >= 0 && x_2 < {:d})\n{indent}{{\n'.format(W, **str_data))
                            str_data['indent'] += '\t'
                    else:
                        str_data['x_2'] = x_2
                    for kw in range(C_IN):
                        str_data['kw'] = kw
                        for lw in range(0, C_OUT):
                            str_data['lw'] = lw
                            if arch == "sse3" and lw % 4 == 0 and C_OUT % 4 == 0:
                                str_data['w0'] = w[iw, jw, kw, lw]
                                str_data['w1'] = w[iw, jw, kw, lw + 1]
                                str_data['w2'] = w[iw, jw, kw, lw + 2]
                                str_data['w3'] = w[iw, jw, kw, lw + 3]
                                if x_1 >= 0 and x_1 < H:
                                    if x_2 >= 0 and x_2 < W:
                                        x_out[x_out_1, x_out_2, lw] = (
                                                x_out[x_out_1, x_out_2, lw] + w[iw, jw, kw, lw] * x[
                                            x_1, x_2, kw]).astype(
                                            'float32')
                                        x_out[x_out_1, x_out_2, lw + 1] = (
                                                x_out[x_out_1, x_out_2, lw + 1] + w[iw, jw, kw, lw + 1] * x[
                                            x_1, x_2, kw]).astype('float32')
                                        x_out[x_out_1, x_out_2, lw + 2] = (
                                                x_out[x_out_1, x_out_2, lw + 2] + w[iw, jw, kw, lw + 2] * x[
                                            x_1, x_2, kw]).astype('float32')
                                        x_out[x_out_1, x_out_2, lw + 3] = (
                                                x_out[x_out_1, x_out_2, lw + 3] + w[iw, jw, kw, lw + 3] * x[
                                            x_1, x_2, kw]).astype('float32')
                                if (unroll_level == 0 and x_1 >= 0 and x_1 < H and x_2 >= 0 and x_2 < W) \
                                        or (ix == -pad_top and unroll_level == 1 and x_2 >= 0 and x_2 < W) or \
                                        (unroll_level == 2 and jx == -pad_left and ix == -pad_top):
                                    writeC(c_inf, sse_conv.format(**str_data))
                            elif arch == "general" or (arch == "sse3" and C_OUT % 4 != 0):
                                str_data['w0'] = w[iw, jw, kw, lw]
                                if x_1 >= 0 and x_1 < H:
                                    if x_2 >= 0 and x_2 < W:
                                        x_out[x_out_1, x_out_2, lw] = \
                                            (x_out[x_out_1, x_out_2, lw] + w[iw, jw, kw, lw] * x[x_1, x_2, kw]).astype(
                                                'float32')
                                if (unroll_level == 0 and x_1 >= 0 and x_1 < H and x_2 >= 0 and x_2 < W) \
                                        or (ix == -pad_top and unroll_level == 1 and x_2 >= 0 and x_2 < W) or \
                                        (unroll_level == 2 and jx == -pad_left and ix == -pad_top):
                                    writeC(c_inf,
                                           '{indent}x{layer}[{x_out_1}][{x_out_2}][{lw}] += '
                                           '{w0}f * x{prev_layer}[{x_1}][{x_2}][{kw}];\n'.format(**str_data))
                    if unroll_level > 1 and jx == -pad_left and ix == -pad_top and pad == 'same':
                        str_data['indent'] = str_data['indent'][:-1]
                        writeC(c_inf, '{indent}}}\n'.format(**str_data))
                if ((ix == -pad_top and unroll_level == 1) or \
                    (unroll_level == 2 and jx == -pad_left and ix == -pad_top)) and pad == 'same':
                    str_data['indent'] = str_data['indent'][:-1]
                    writeC(c_inf, '{indent}}}\n'.format(**str_data))
        if unroll_level > 1 and ix == -pad_top:
            str_data['indent'] = '\t\t'
            writeC(c_inf, '{indent}}}\n'.format(**str_data))

    if unroll_level > 0:
        str_data['indent'] = '\t'
        writeC(c_inf, '{indent}}}\n'.format(**str_data))

    c_inf["layer"] = c_inf["layer"] + 1
    return x_out, c_inf


def convolution_2(x, w, b, stride, pad, c_inf, unroll_level, arch):
    assert x.shape[2] == w.shape[2]
    assert w.shape[3] == b.shape[0]
    H, W, C_IN = x.shape
    KH, KW, _, C_OUT = w.shape
    SH, SW = stride

    if pad == 'valid':
        H_OUT = int(np.ceil((H - KH + 1) / SH))
        W_OUT = int(np.ceil((W - KW + 1) / SW))
        pad_top = pad_bottom = pad_left = pad_right = 0
    elif pad == 'same':
        H_OUT = int(np.ceil(float(H) / float(SH)))
        W_OUT = int(np.ceil(float(W) / float(SW)))
        pad_along_height = max((H_OUT - 1) * SH + KH - H, 0)
        pad_along_width = max((W_OUT - 1) * SW + KW - W, 0)
        pad_top = int(pad_along_height // 2)
        pad_bottom = int(pad_along_height - pad_top)
        pad_left = int(pad_along_width // 2)
        pad_right = int(pad_along_width - pad_left)
    else:
        print("Unknown padding type")
        exit(-4)
    x_out = np.zeros((H_OUT, W_OUT, C_OUT)).astype('float32')
    str_data = {'prev_layer': c_inf["layer"] - 1, 'x_1': 'x_1', 'x_2': 'x_2',
                'x_out_1': 'x_out_1', 'x_out_2': 'x_out_2', 'layer': c_inf["layer"],
                'lw': 'lw', 'indent': '\t', 'x_res': H_OUT, 'y_res': W_OUT, 'z_res': C_OUT,
                'stride0': SH, 'stride1': SW, 'i': 'i', 'j': 'j',
                'pt': pad_top, 'pb': pad_bottom, 'pl': pad_left, 'pr': pad_right}

    writeC(c_inf, '{indent}static float x{layer} alignas(16) [{x_res}][{y_res}][{z_res}] = {{0}};\n'.format(**str_data))

    if unroll_level > 0:
        writeC(c_inf, '{indent}for (int i = 0; i < {:d}; i += 1)\n{indent}{{\n'.format(H_OUT, **str_data))
        str_data['indent'] = '\t\t'
    for i in range(H_OUT):
        if unroll_level > 1 and i == 0:
            writeC(c_inf, '{indent}for (int j = 0; j < {:d}; j += 1)\n{indent}{{\n'.format(W_OUT, **str_data))
            str_data['indent'] = '\t\t\t'
        if unroll_level == 0:
            str_data['i'] = i
        for j in range(W_OUT):
            if unroll_level < 2:
                str_data['j'] = j
            for kw in range(C_OUT):
                str_data['kw'] = kw
                str_data['b'] = b[kw]
                x_out[i, j, kw] = b[kw]
                if unroll_level == 0 or (unroll_level == 1 and i == 0) or (unroll_level == 2 and i == 0 and j == 0):
                    writeC(c_inf, '{indent}x{layer}[{i}][{j}][{kw}] = {b}f;\n'.format(**str_data))
        if unroll_level > 1 and i == 0:
            str_data['indent'] = '\t\t'
            writeC(c_inf, '{indent}}}\n'.format(**str_data))
    if unroll_level > 0:
        str_data['indent'] = '\t'
        writeC(c_inf, '{indent}}}\n'.format(**str_data))

    if unroll_level > 0:
        writeC(c_inf, '{indent}for (int ix = -{pt}; ix < {:d}; ix += {stride0})\n{indent}{{\n'.format(
            H - KH + 1 + pad_bottom, **str_data))
        str_data['indent'] = '\t\t'
        writeC(c_inf, '{indent}int x_1, x_out_1;\n'.format(**str_data))

    for ix in range(-pad_top, H - KH + 1 + pad_bottom, SH):
        assert (ix + pad_top) % SH == 0
        x_out_1 = (ix + pad_top) // SH

        if unroll_level > 0 and ix == -pad_top:
            writeC(c_inf, '{indent}x_out_1 = (ix + {pt}) / {stride0};\n'.format(**str_data))
        else:
            str_data['x_out_1'] = x_out_1

        if unroll_level > 1 and ix == -pad_top:
            writeC(c_inf, '{indent}for (int jx = -{pl}; jx < {:d}; jx += {stride1})\n{indent}{{\n'.format(
                W - KW + 1 + pad_right, **str_data))
            str_data['indent'] = '\t\t\t'
            writeC(c_inf, '{indent}int x_2, x_out_2;\n'.format(**str_data))

        for jx in range(-pad_left, W - KW + 1 + pad_right, SW):
            assert (jx + pad_left) % SW == 0
            x_out_2 = (jx + pad_left) // SW

            if unroll_level > 1 and -pad_left == jx and ix == -pad_top:
                writeC(c_inf, '{indent}x_out_2 = (jx + {pl}) / {stride1};\n'.format(**str_data))
            else:
                str_data['x_out_2'] = x_out_2
            for lw in range(0, C_OUT):  # Loop over target/filter depth
                str_data['lw'] = lw
                write_sse = arch == "sse3" and lw % 4 == 0 and C_OUT % 4 == 0
                write_general = arch == "general" or (arch == "sse3" and size(w, 4) % 4 != 0)
                #   Load target pixel
                if (unroll_level == 0 or (ix == -pad_top and unroll_level == 1) or \
                    (unroll_level == 2 and jx == -pad_left and ix == -pad_top)) and write_sse:
                    writeC(c_inf, '{indent}y = _mm_load_ps((float*)&x{layer}[{x_out_1}][{x_out_2}][{lw}]);\n'.format(
                        **str_data))
                #  ------------------
                for iw in range(KH):  # Loop over filter x
                    x_1 = ix + iw
                    if ((ix == -pad_top and unroll_level == 1) or \
                        (unroll_level == 2 and jx == -pad_left and ix == -pad_top)) and \
                            (write_sse or write_general):
                        writeC(c_inf, '{indent}x_1 = ix + {:d};\n'.format(iw, **str_data))
                        if pad == 'same':
                            writeC(c_inf, '{indent}if (x_1 >= 0 && x_1 < {:d})\n{indent}{{\n'.format(H, **str_data))
                            str_data['indent'] += '\t'
                    elif unroll_level == 0:
                        str_data['x_1'] = x_1

                    for jw in range(KW):  # Loop over filter y
                        x_2 = jx + jw
                        if (unroll_level > 1 and jx == -pad_left and ix == -pad_top) and (write_sse or write_general):
                            writeC(c_inf, '{indent}x_2 = jx + {:d};\n'.format(jw, **str_data))
                            if pad == 'same':
                                writeC(c_inf, '{indent}if (x_2 >= 0 && x_2 < {:d})\n{indent}{{\n'.format(W, **str_data))
                                str_data['indent'] += '\t'
                        elif unroll_level < 2:
                            str_data['x_2'] = x_2
                        for kw in range(C_IN):
                            str_data['kw'] = kw
                            if arch == "sse3" and lw % 4 == 0 and C_OUT % 4 == 0:
                                str_data['w0'] = w[iw, jw, kw, lw]
                                str_data['w1'] = w[iw, jw, kw, lw + 1]
                                str_data['w2'] = w[iw, jw, kw, lw + 2]
                                str_data['w3'] = w[iw, jw, kw, lw + 3]
                                if x_1 >= 0 and x_1 < H:
                                    if x_2 >= 0 and x_2 < W:
                                        x_out[x_out_1, x_out_2, lw] = (
                                                x_out[x_out_1, x_out_2, lw] + w[iw, jw, kw, lw] * x[
                                            x_1, x_2, kw]).astype(
                                            'float32')
                                        x_out[x_out_1, x_out_2, lw + 1] = (
                                                x_out[x_out_1, x_out_2, lw + 1] + w[iw, jw, kw, lw + 1] * x[
                                            x_1, x_2, kw]).astype('float32')
                                        x_out[x_out_1, x_out_2, lw + 2] = (
                                                x_out[x_out_1, x_out_2, lw + 2] + w[iw, jw, kw, lw + 2] * x[
                                            x_1, x_2, kw]).astype('float32')
                                        x_out[x_out_1, x_out_2, lw + 3] = (
                                                x_out[x_out_1, x_out_2, lw + 3] + w[iw, jw, kw, lw + 3] * x[
                                            x_1, x_2, kw]).astype('float32')
                                if (unroll_level == 0 and x_1 >= 0 and x_1 < H and x_2 >= 0 and x_2 < W) \
                                        or (ix == -pad_top and unroll_level == 1 and x_2 >= 0 and x_2 < W) or \
                                        (unroll_level == 2 and jx == -pad_left and ix == -pad_top):
                                    writeC(c_inf, sse_conv_2.format(**str_data))
                            elif write_general:
                                str_data['w0'] = w[iw, jw, kw, lw]
                                if x_1 >= 0 and x_1 < H:
                                    if x_2 >= 0 and x_2 < W:
                                        x_out[x_out_1, x_out_2, lw] = \
                                            (x_out[x_out_1, x_out_2, lw] + w[iw, jw, kw, lw] * x[x_1, x_2, kw]).astype(
                                                'float32')
                                if (unroll_level == 0 and x_1 >= 0 and x_1 < H and x_2 >= 0 and x_2 < W) \
                                        or (ix == -pad_top and unroll_level == 1 and x_2 >= 0 and x_2 < W) or \
                                        (unroll_level == 2 and jx == -pad_left and ix == -pad_top):
                                    writeC(c_inf,
                                           '{indent}x{layer}[{x_out_1}][{x_out_2}][{lw}] += '
                                           '{w0}f * x{prev_layer}[{x_1}][{x_2}][{kw}];\n'.format(**str_data))
                        if (unroll_level > 1 and jx == -pad_left and ix == -pad_top) and pad == 'same' and \
                                (write_sse or write_general):
                            str_data['indent'] = str_data['indent'][:-1]
                            writeC(c_inf, '{indent}}}\n'.format(**str_data))
                    if ((ix == -pad_top and unroll_level == 1) or \
                        (unroll_level == 2 and jx == -pad_left and ix == -pad_top)) and \
                            pad == 'same' and (write_sse or write_general):
                        str_data['indent'] = str_data['indent'][:-1]
                        writeC(c_inf, '{indent}}}\n'.format(**str_data))
                #  Save target pixel
                if (unroll_level == 0 or (ix == -pad_top and unroll_level == 1) or \
                    (unroll_level == 2 and jx == -pad_left and ix == -pad_top)) and write_sse:
                    writeC(c_inf, '{indent}_mm_store_ps((float*)&x{layer}[{x_out_1}][{x_out_2}][{lw}], y);\n'.format(
                        **str_data))
                #  -------------------
        if unroll_level > 1 and ix == -pad_top:
            str_data['indent'] = '\t\t'
            writeC(c_inf, '{indent}}}\n'.format(**str_data))

    if unroll_level > 0:
        str_data['indent'] = '\t'
        writeC(c_inf, '{indent}}}\n'.format(**str_data))

    c_inf["layer"] = c_inf["layer"] + 1
    return x_out, c_inf


def convolution_3(x, w, b, stride, pad, c_inf, unroll_level, arch):
    assert x.shape[2] == w.shape[2]
    assert w.shape[3] == b.shape[0]
    H, W, C_IN = x.shape
    KH, KW, _, C_OUT = w.shape
    SH, SW = stride

    if pad == 'valid':
        H_OUT = int(np.ceil((H - KH + 1) / SH))
        W_OUT = int(np.ceil((W - KW + 1) / SW))
        pad_top = pad_bottom = pad_left = pad_right = 0
    elif pad == 'same':
        H_OUT = int(np.ceil(float(H) / float(SH)))
        W_OUT = int(np.ceil(float(W) / float(SW)))
        pad_along_height = max((H_OUT - 1) * SH + KH - H, 0)
        pad_along_width = max((W_OUT - 1) * SW + KW - W, 0)
        pad_top = int(pad_along_height // 2)
        pad_bottom = int(pad_along_height - pad_top)
        pad_left = int(pad_along_width // 2)
        pad_right = int(pad_along_width - pad_left)
    else:
        print("Unknown padding type")
        exit(-4)
    x_out = np.zeros((H_OUT, W_OUT, C_OUT)).astype('float32')
    str_data = {'prev_layer': c_inf["layer"] - 1, 'x_1': 'x_1', 'x_2': 'x_2',
                'x_out_1': 'x_out_1', 'x_out_2': 'x_out_2', 'layer': c_inf["layer"],
                'lw': 'lw', 'indent': '\t', 'x_res': H_OUT, 'y_res': W_OUT, 'z_res': C_OUT,
                'stride0': SH, 'stride1': SW, 'i': 'i', 'j': 'j',
                'pt': pad_top, 'pb': pad_bottom, 'pl': pad_left, 'pr': pad_right}

    writeC(c_inf, '{indent}static float x{layer} alignas(16) [{x_res}][{y_res}][{z_res}] = {{0}};\n'.format(**str_data))

    if unroll_level > 0:
        writeC(c_inf, '{indent}for (int i = 0; i < {:d}; i += 1)\n{indent}{{\n'.format(H_OUT, **str_data))
        str_data['indent'] = '\t\t'
    for i in range(H_OUT):
        if unroll_level > 1 and i == 0:
            writeC(c_inf, '{indent}for (int j = 0; j < {:d}; j += 1)\n{indent}{{\n'.format(W_OUT, **str_data))
            str_data['indent'] = '\t\t\t'
        if unroll_level == 0:
            str_data['i'] = i
        for j in range(W_OUT):
            if unroll_level < 2:
                str_data['j'] = j
            for kw in range(C_OUT):
                str_data['kw'] = kw
                str_data['b'] = b[kw]
                x_out[i, j, kw] = b[kw]
                if unroll_level == 0 or (unroll_level == 1 and i == 0) or (unroll_level == 2 and i == 0 and j == 0):
                    writeC(c_inf, '{indent}x{layer}[{i}][{j}][{kw}] = {b}f;\n'.format(**str_data))
        if unroll_level > 1 and i == 0:
            str_data['indent'] = '\t\t'
            writeC(c_inf, '{indent}}}\n'.format(**str_data))
    if unroll_level > 0:
        str_data['indent'] = '\t'
        writeC(c_inf, '{indent}}}\n'.format(**str_data))

    if unroll_level > 0:
        writeC(c_inf, '{indent}for (int ix = -{pt}; ix < {:d}; ix += {stride0})\n{indent}{{\n'.format(
            H - KH + 1 + pad_bottom, **str_data))
        str_data['indent'] = '\t\t'
        writeC(c_inf, '{indent}int x_1, x_out_1;\n'.format(**str_data))

    for ix in range(-pad_top, H - KH + 1 + pad_bottom, SH):
        assert (ix + pad_top) % SH == 0
        x_out_1 = (ix + pad_top) // SH

        if unroll_level > 0 and ix == -pad_top:
            writeC(c_inf, '{indent}x_out_1 = (ix + {pt}) / {stride0};\n'.format(**str_data))
        else:
            str_data['x_out_1'] = x_out_1

        if unroll_level > 1 and ix == -pad_top:
            writeC(c_inf, '{indent}for (int jx = -{pl}; jx < {:d}; jx += {stride1})\n{indent}{{\n'.format(
                W - KW + 1 + pad_right, **str_data))
            str_data['indent'] = '\t\t\t'
            writeC(c_inf, '{indent}int x_2, x_out_2;\n'.format(**str_data))

        for jx in range(-pad_left, W - KW + 1 + pad_right, SW):
            assert (jx + pad_left) % SW == 0
            x_out_2 = (jx + pad_left) // SW

            if unroll_level > 1 and -pad_left == jx and ix == -pad_top:
                writeC(c_inf, '{indent}x_out_2 = (jx + {pl}) / {stride1};\n'.format(**str_data))
            else:
                str_data['x_out_2'] = x_out_2
            for lw in range(0, C_OUT):  # Loop over target/filter depth
                str_data['lw'] = lw
                write_sse = arch == "sse3" and lw % 4 == 0 and C_OUT % 4 == 0
                write_general = arch == "general" or (arch == "sse3" and size(w, 4) % 4 != 0)
                #   Load target pixel
                if (unroll_level == 0 or (ix == -pad_top and unroll_level == 1) or \
                    (unroll_level == 2 and jx == -pad_left and ix == -pad_top)) and write_sse:
                    writeC(c_inf, '{indent}y = _mm_load_ps((float*)&x{layer}[{x_out_1}][{x_out_2}][{lw}]);\n'.format(
                        **str_data))
                    writeC(c_inf, '{indent}y2 = _mm_setzero_ps();\n'.format(**str_data))
                    even = True
                #  ------------------
                for iw in range(KH):  # Loop over filter x
                    x_1 = ix + iw
                    if ((ix == -pad_top and unroll_level == 1) or \
                        (unroll_level == 2 and jx == -pad_left and ix == -pad_top)) and \
                            (write_sse or write_general):
                        writeC(c_inf, '{indent}x_1 = ix + {:d};\n'.format(iw, **str_data))
                        if pad == 'same':
                            writeC(c_inf, '{indent}if (x_1 >= 0 && x_1 < {:d})\n{indent}{{\n'.format(H, **str_data))
                            str_data['indent'] += '\t'
                    elif unroll_level == 0:
                        str_data['x_1'] = x_1

                    for jw in range(KW):  # Loop over filter y
                        x_2 = jx + jw
                        if (unroll_level > 1 and jx == -pad_left and ix == -pad_top) and (write_sse or write_general):
                            writeC(c_inf, '{indent}x_2 = jx + {:d};\n'.format(jw, **str_data))
                            if pad == 'same':
                                writeC(c_inf, '{indent}if (x_2 >= 0 && x_2 < {:d})\n{indent}{{\n'.format(W, **str_data))
                                str_data['indent'] += '\t'
                        elif unroll_level < 2:
                            str_data['x_2'] = x_2
                        for kw in range(C_IN):
                            str_data['kw'] = kw
                            if arch == "sse3" and lw % 4 == 0 and C_OUT % 4 == 0:
                                str_data['w0'] = w[iw, jw, kw, lw]
                                str_data['w1'] = w[iw, jw, kw, lw + 1]
                                str_data['w2'] = w[iw, jw, kw, lw + 2]
                                str_data['w3'] = w[iw, jw, kw, lw + 3]
                                if x_1 >= 0 and x_1 < H:
                                    if x_2 >= 0 and x_2 < W:
                                        x_out[x_out_1, x_out_2, lw] = (
                                                x_out[x_out_1, x_out_2, lw] + w[iw, jw, kw, lw] * x[
                                            x_1, x_2, kw]).astype(
                                            'float32')
                                        x_out[x_out_1, x_out_2, lw + 1] = (
                                                x_out[x_out_1, x_out_2, lw + 1] + w[iw, jw, kw, lw + 1] * x[
                                            x_1, x_2, kw]).astype('float32')
                                        x_out[x_out_1, x_out_2, lw + 2] = (
                                                x_out[x_out_1, x_out_2, lw + 2] + w[iw, jw, kw, lw + 2] * x[
                                            x_1, x_2, kw]).astype('float32')
                                        x_out[x_out_1, x_out_2, lw + 3] = (
                                                x_out[x_out_1, x_out_2, lw + 3] + w[iw, jw, kw, lw + 3] * x[
                                            x_1, x_2, kw]).astype('float32')
                                if (unroll_level == 0 and x_1 >= 0 and x_1 < H and x_2 >= 0 and x_2 < W) \
                                        or (ix == -pad_top and unroll_level == 1 and x_2 >= 0 and x_2 < W) or \
                                        (unroll_level == 2 and jx == -pad_left and ix == -pad_top):
                                    if even:
                                        writeC(c_inf, sse_conv_even.format(**str_data))
                                    else:
                                        writeC(c_inf, sse_conv_odd.format(**str_data))
                                    even = not even
                            elif write_general:
                                str_data['w0'] = w[iw, jw, kw, lw]
                                if x_1 >= 0 and x_1 < H:
                                    if x_2 >= 0 and x_2 < W:
                                        x_out[x_out_1, x_out_2, lw] = \
                                            (x_out[x_out_1, x_out_2, lw] + w[iw, jw, kw, lw] * x[x_1, x_2, kw]).astype(
                                                'float32')
                                if (unroll_level == 0 and x_1 >= 0 and x_1 < H and x_2 >= 0 and x_2 < W) \
                                        or (ix == -pad_top and unroll_level == 1 and x_2 >= 0 and x_2 < W) or \
                                        (unroll_level == 2 and jx == -pad_left and ix == -pad_top):
                                    writeC(c_inf,
                                           '{indent}x{layer}[{x_out_1}][{x_out_2}][{lw}] += '
                                           '{w0}f * x{prev_layer}[{x_1}][{x_2}][{kw}];\n'.format(**str_data))
                        if (unroll_level > 1 and jx == -pad_left and ix == -pad_top) and pad == 'same' and \
                                (write_sse or write_general):
                            str_data['indent'] = str_data['indent'][:-1]
                            writeC(c_inf, '{indent}}}\n'.format(**str_data))
                    if ((ix == -pad_top and unroll_level == 1) or \
                        (unroll_level == 2 and jx == -pad_left and ix == -pad_top)) and \
                            pad == 'same' and (write_sse or write_general):
                        str_data['indent'] = str_data['indent'][:-1]
                        writeC(c_inf, '{indent}}}\n'.format(**str_data))
                #  Save target pixel
                if (unroll_level == 0 or (ix == -pad_top and unroll_level == 1) or \
                    (unroll_level == 2 and jx == -pad_left and ix == -pad_top)) and write_sse:
                    writeC(c_inf, '{indent}y = _mm_add_ps(y, y2);\n'.format(**str_data))
                    writeC(c_inf, '{indent}_mm_store_ps((float*)&x{layer}[{x_out_1}][{x_out_2}][{lw}], y);\n'.format(
                        **str_data))
                #  -------------------
        if unroll_level > 1 and ix == -pad_top:
            str_data['indent'] = '\t\t'
            writeC(c_inf, '{indent}}}\n'.format(**str_data))

    if unroll_level > 0:
        str_data['indent'] = '\t'
        writeC(c_inf, '{indent}}}\n'.format(**str_data))

    c_inf["layer"] = c_inf["layer"] + 1
    return x_out, c_inf


def rectifiedLinearUnit(x, c_inf, unroll_level, alpha=0.0, arch='general'):
    x_out = np.zeros(x.shape).astype('float32')
    str_data = {'prev_layer': c_inf["layer"] - 1, 'i': 'i', 'j': 'j', 'k': 'k',
                'below_str': '0', 'indent': '\t'}
    if arch == "sse3" and not alpha == 0.0:
        writeC(c_inf, "{indent}y = _mm_set_ps1({alpha}f);\n".format(alpha=alpha, **str_data))
    if unroll_level > 0:
        writeC(c_inf, '\tfor (int i = 0; i < {:d}; i += 1)\n\t{{\n'.format(size(x, 1)))
        str_data['indent'] += '\t'
    for i in range(size(x, 1)):
        if unroll_level > 1 and i == 0:
            writeC(c_inf, '\t\tfor (int j = 0; j < {:d}; j += 1)\n\t\t{{\n'.format(size(x, 2)))
            str_data['indent'] += '\t'
        elif unroll_level == 0:
            str_data['i'] = i
        for j in range(size(x, 2)):
            if unroll_level < 2:
                str_data['j'] = j
            for k in range(size(x, 3)):
                if arch == "sse3":
                    str_data['k'] = k
                    if x[i, j, k] < 0:
                        x_out[i, j, k] = alpha * x[i, j, k]
                    else:
                        x_out[i, j, k] = x[i, j, k]
                    if unroll_level == 0 or (unroll_level == 1 and i == 0) or (unroll_level == 2 and i == 0 and j == 0):
                        if k % 4 == 0:
                            if alpha == 0.0:
                                writeC(c_inf, sse_relu.format(**str_data))
                            else:
                                writeC(c_inf, sse_leaky.format(**str_data))
                else:
                    str_data['k'] = k
                    if alpha != 0.0:
                        str_data['below_str'] = '{:f}f * x{prev_layer}[{i}][{j}][{k}]'.format(alpha, **str_data)
                    if x[i, j, k] < 0:
                        x_out[i, j, k] = alpha * x[i, j, k]
                    else:
                        x_out[i, j, k] = x[i, j, k]
                    if unroll_level == 0 or (unroll_level == 1 and i == 0) or (unroll_level == 2 and i == 0 and j == 0):
                        writeC(c_inf,
                               '{indent}x{prev_layer}[{i}][{j}][{k}] = x{prev_layer}[{i}][{j}][{k}] < 0 ? '
                               '{below_str} : x{prev_layer}[{i}][{j}][{k}];\n'.format(**str_data))
        if unroll_level > 1 and i == 0:
            writeC(c_inf, '\t\t}\n')
    if unroll_level > 0:
        writeC(c_inf, '\t}\n')
    return x_out, c_inf


def max_pool(x, p, stride, c_inf, unroll_level, arch='general'):
    z_res = size(x, 3)
    x_res = int(np.ceil((size(x, 1) - p[0] + 1) / stride[0]))
    y_res = int(np.ceil((size(x, 2) - p[1] + 1) / stride[1]))
    x_out = np.zeros((x_res, y_res, size(x, 3))).astype('float32')
    str_data = {'prev_layer': c_inf["layer"] - 1, 'ix': 'ix', 'jx': 'jx', 'kx': 'kx', 'indent': '\t',
                'layer': c_inf["layer"], 'mi': 'mi', 'mj': 'mj', 'x_out_1': 'x_out_1', 'x_out_2': 'x_out_2',
                'stride0': stride[0], 'p': p[0], 'x_res': x_res, 'y_res': y_res, 'z_res': z_res, 'stride1': stride[1]}
    writeC(c_inf, '\tstatic float x{layer}[{x_res}][{y_res}][{z_res}] = {{0}};\n'.format(**str_data))
    if unroll_level > 0:
        writeC(c_inf,
               '\tfor (int ix = 0; ix < {:d}; ix += {stride0})\n\t{{\n'.format(size(x, 1) - p[0] + 1, **str_data))
        str_data['indent'] = '\t\t'
        # writeC(c_inf, '{indent}int x_1, x_out_1;\n'.format(**str_data))  # FIXME generates unsused variable warning
        writeC(c_inf, '{indent}int x_out_1;\n'.format(**str_data))
    for ix in range(0, size(x, 1) - p[0] + 1, stride[0]):
        x_out_1 = ix // stride[0]  # ix is a multiple of stride[0], so integer division is fine
        if unroll_level > 0 and ix == 0:
            writeC(c_inf, '{indent}x_out_1 = ix / {stride0};\n'.format(**str_data))
            if unroll_level == 2:
                writeC(c_inf, '\tfor (int jx = 0; jx < {:d}; jx += {stride1})\n\t{{\n'.format(size(x, 2) - p[1] + 1,
                                                                                              **str_data))
                str_data['indent'] = '\t\t'
                writeC(c_inf, '{indent}int x_out_2;\n'.format(**str_data))
        elif unroll_level == 0:
            str_data['x_out_1'] = x_out_1
            str_data['ix'] = ix
        for jx in range(0, size(x, 2) - p[1] + 1, stride[1]):
            x_out_2 = jx // stride[1]  # jx is a multiple of stride[1], so integer division is fine
            if unroll_level == 2 and ix == 0 and jx == 0:
                writeC(c_inf, '{indent}x_out_2 = jx / {stride1};\n'.format(**str_data))
            if unroll_level < 2:
                str_data['x_out_2'] = x_out_2
                str_data['jx'] = jx
            for kx in range(size(x, 3)):
                str_data['kx'] = kx
                x_out[x_out_1, x_out_2, kx] = np.max(np.max(x[ix:ix + p[0], jx:jx + p[1], kx]))
                if unroll_level == 0 or (unroll_level == 1 and ix == 0) or (unroll_level == 2 and ix == 0 and jx == 0):
                    if arch == "sse3":
                        if kx % 4 == 0:
                            writeC(c_inf, "{indent}x = _mm_load_ps((float*)&x{prev_layer}[{ix}][{jx}][{kx}]);\n".format(
                                **str_data))
                    else:
                        writeC(c_inf,
                               '{indent}x{layer}[{x_out_1}][{x_out_2}][{kx}] = x{prev_layer}[{ix}][{jx}][{kx}];\n'.format(
                                   **str_data))
                for mi in range(0, p[0]):
                    if unroll_level == 0:
                        str_data['mi'] = ix + mi
                    else:
                        str_data['mi'] = 'ix + ' + str(mi)
                    for mj in range(jx, jx + p[1]):
                        if unroll_level < 2:
                            str_data['mj'] = mj
                        else:
                            str_data['mj'] = 'jx + ' + str(mj)
                        if unroll_level == 0 or (unroll_level == 1 and ix == 0) or (
                                unroll_level == 2 and ix == 0 and jx == 0):
                            if arch == "sse3":
                                if kx % 4 == 0:
                                    writeC(c_inf,
                                           "{indent}y = _mm_load_ps((float*)&x{prev_layer}[{mi}][{mj}][{kx}]);\n".format(
                                               **str_data))
                                    writeC(c_inf, "{indent}x = _mm_max_ps(x, y);\n".format(**str_data))
                            else:
                                writeC(c_inf,
                                       '{indent}x{layer}[{x_out_1}][{x_out_2}][{kx}] = '
                                       'x{prev_layer}[{mi}][{mj}][{kx}] > x{layer}[{x_out_1}][{x_out_2}][{kx}] ?'
                                       ' x{prev_layer}[{mi}][{mj}][{kx}] : x{layer}[{x_out_1}][{x_out_2}][{kx}];\n'.format(
                                           **str_data))
                    if unroll_level == 0 or (unroll_level == 1 and ix == 0) or (
                            unroll_level == 2 and ix == 0 and jx == 0):
                        if arch == "sse3" and kx % 4 == 0:
                            writeC(c_inf,
                                   "{indent}_mm_store_ps((float*)&x{layer}[{x_out_1}][{x_out_2}][{kx}], x);\n".format(
                                       **str_data))
        if unroll_level > 1 and ix == 0:
            writeC(c_inf, '\t\t}\n')
    c_inf["layer"] = c_inf["layer"] + 1
    if unroll_level > 0:
        writeC(c_inf, '\t}\n')
    return x_out, c_inf
