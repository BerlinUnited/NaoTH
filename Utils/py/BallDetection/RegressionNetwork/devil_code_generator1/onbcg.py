""" Olli's Code Generator (OCG) """

__author__ = "Oliver Urbann"
__copyright__ = "Copyright 2017-2018, Oliver Urbann"
__credits__ = ["Oliver Urbann"]
__license__ = "Commercial, free for educational usage."
__version__ = "1.0.0"
__email__ = "oliver.urbann@tu-dortmund.de"

import os
import platform
import numpy as np
from tensorflow.keras import backend as K
from tensorflow.keras.layers import Convolution2D, MaxPooling2D, Flatten, Dropout, BatchNormalization, LeakyReLU, Dense, ReLU
from tensorflow.keras.models import load_model

if platform.system() != 'Darwin':
    compiler = 'g++ -mssse3 -std=c++11 -g -march=bonnell -DCNN_TEST '
    compiler_O3 = 'g++ -O3 -mssse3 -std=c++11 -march=bonnell -DCNN_TEST '
else:
    compiler = 'clang++ --target=i686-unknown-linux-gnu -msse3 -march=bonnell -std=c++11 -g -DCNN_TEST '
    compiler_O3 = 'clang++ -O3 --target=i686-unknown-linux-gnu -msse3 -march=bonnell -std=c++11 -g -DCNN_TEST '


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
{indent}alignas(16) static const float g [4] = {{{w0}f, {w1}f, {w2}f, {w3}f}};
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

classify_yuv_patch = '''
\n
void {}::predict(const BallCandidates::PatchYUVClassified& patch, double meanBrightness)
{{
\tASSERT(patch.size() == 16);

\tfor(size_t x=0; x < patch.size(); x++) {{
\t\tfor(size_t y=0; y < patch.size(); y++) {{
\t\t\t// TODO: check
\t\t\tfloat value = (static_cast<float>((patch.data[patch.size() * x + y].pixel.y)) / 255.0f) - static_cast<float>(meanBrightness);
\t\t\tin_step[y][x][0] = value;
\t\t}}
\t}}

\tcnn(in_step);
\t//std::cout << "scores[0]=" << scores[0] << " scores[1]=" << scores[1] << " scores[2]=" << scores[2] << std::endl;
}}
'''


def write_naoth_header_file(c_inf, class_name, output_folder="."):
    fp = open(os.path.join(output_folder, class_name + ".h"), "w")
    with fp:
        print("#ifndef _{}_H".format(class_name.upper()), file=fp)
        print("#define _{}_H".format(class_name.upper()), file=fp)
        print("", file=fp)
        print("# include <emmintrin.h>", file=fp)
        print("", file=fp)
        print("#include \"AbstractCNNClassifier.h\"", file=fp)
        print("", file=fp)
        print("class {} : public AbstractCNNFinder {{".format(class_name), file=fp)
        print("", file=fp)
        print("public:", file=fp)
        print("\tvoid cnn(float x0[{:d}][{:d}][{:d}]);".format(c_inf["x_dim"], c_inf["y_dim"], c_inf["z_dim"]), file=fp)
        print("\tvoid predict(const BallCandidates::PatchYUVClassified& p,double meanBrightness);", file=fp)
        print("\tvirtual double getRadius();", file=fp)
        print("\tvirtual Vector2d getCenter();", file=fp)
        print("\tvirtual double getBallConfidence();", file=fp)
        print("", file=fp)
        print("private:", file=fp)
        print("\tfloat in_step[{:d}][{:d}][{:d}];".format(c_inf["x_dim"], c_inf["y_dim"], c_inf["z_dim"]), file=fp)
        print("\tfloat scores[{:d}];".format(c_inf["output_dim"]), file=fp)
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
    writeC(c_inf, '\tstatic float x{:d}[{:d}][{:d}][{:d}] = {{}};\n'.format(c_inf["layer"], x.shape[0], x.shape[1],
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

    class_name = os.path.splitext(os.path.basename(code_path))[0]
    print("Creating class", class_name)

    output_folder = os.path.dirname(os.path.abspath(code_path))

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

    c_inf = write_header(c_inf, arch, class_name)
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
        elif type(layer) == ReLU:
            writeC(c_inf, '\n \t// ReLu Layer\n')
            _x, c_inf = rectifiedLinearUnit(_x, c_inf, unroll_level, 0, arch)
        elif type(layer) == Flatten:
            pass
        elif type(layer) == Dropout:
            pass
        elif type(layer) == BatchNormalization:
            print("Warning: BatchNormalization not implemented")
            pass
        elif type(layer) == Dense:
            writeC(c_inf, '\n \t// Dense Layer\n')
            w = K.eval(layer.weights[0])
            b = K.eval(layer.bias)

            if layer.activation.__name__ == 'relu':
                print("use dense layer argument for activation")
                _x, c_inf = dense(_x, w, b, c_inf, relu=True)
            else:
                _x, c_inf = dense(_x, w, b, c_inf, relu=False)

        else:
            print("ERROR: Unknown layer: {}".format(type(layer)))
            print("ERROR: aborting generation. cpp file unfinished!")
            exit(-1)

    # NOTE: this creates a header file
    #       at this point we should know the size of the output layer
    write_naoth_header_file(c_inf, class_name, output_folder)

    write_footer(c_inf, _x, class_name=class_name)


def dense(_x, weights, b, c_inf, relu):

    x_dim = _x.shape[0]
    y_dim = _x.shape[1]
    channels = _x.shape[2]
    
    output_dim = weights.shape[1]

    i = 0
    for output in range(output_dim):
        c_inf["f"].write("\tscores[{:d}] = {:f}f".format(output, b[output]))
        for x in range(x_dim):
            for y in range(y_dim):
                for c in range(channels):
                    idx = (x * (y_dim * channels)) + (y * channels) + c
                    # print("x: {}, y: {}, c: {} -> {}".format(x,y,c, idx))
                    if i % 4 is 0:
                        c_inf["f"].write('\n\t')

                    if weights[idx, output] >= 0:
                        c_inf["f"].write(' + ')
                    else:
                        c_inf["f"].write(' - ')

                    c_inf["f"].write('{:f}f * x{:d}[{:d}][{:d}][{:d}]'.format(
                        abs(weights[idx, output]),
                        c_inf["layer"] - 1, x, y, c
                    ))

                    i += 1

        # ReLU
        if relu:
            c_inf["f"].write(';\n\n')
            c_inf["f"].write("\t// Apply ReLU\n")
            c_inf["f"].write("\tscores[{:d}] = scores[{:d}] > 0.0f ? scores[{:d}] : 0.0f;\n".format(output, output, output))

        c_inf["f"].write('\n')
        
    c_inf["layer"] = c_inf["layer"] + 1
    c_inf["output_dim"] = output_dim
    
    x_out = np.zeros(shape=(output_dim, 1, 1)).astype('float32')
    
    return x_out, c_inf


def size(x, i=1):
    return x.shape[i - 1]


def write_header(c_inf, arch, class_name):
    c_inf["f"] = open(c_inf["path"], 'w')
    c_inf["layer"] = 1
    c_inf["f"].write('#include \"{}.h\"\n\n'.format(class_name))
    c_inf["f"].write("""#if WIN32\n\t#define alignas(x) __declspec(align(x))\n#endif\n\n""")

    if arch == 'sse3':
        c_inf["f"].write('#include <emmintrin.h>\n')

    c_inf["f"].write('void {}::cnn(float x0[{:d}][{:d}][{:d}])\n'.format(
        class_name,
        c_inf["x_dim"],
        c_inf["y_dim"],
        c_inf["z_dim"]) + '{\n')

    if arch == 'sse3':
        # c_inf["f"].write('__m128 w, x, y, y2, t, t2;\n')
        c_inf["f"].write('\t__m128 w, x, y;\n')

    return c_inf


def write_footer(c_inf, _x, class_name):
    if c_inf["f"] is not None:
        c_inf["f"].write('}\n')

        c_inf["f"].write(classify_yuv_patch.format(class_name))
        c_inf["f"].write('double {}::getRadius() {{return scores[0];}}\n'.format(class_name))
        c_inf["f"].write('Vector2d {}::getCenter() {{return Vector2d(scores[1], scores[2]);}}\n'.format(class_name))
        c_inf["f"].write('double {}::getBallConfidence() {{return scores[3];}}\n'.format(class_name))
        c_inf["f"].close()
        c_inf["f"] = None

        # replace the name of the output layer
        with open(c_inf["path"], 'r') as file:
            data = file.read()

        data = data.replace("int *res, double *scores", ' double output_tensor[{:d}][{:d}][{:d}]'.format(
            size(_x, 1),
            size(_x, 2),
            size(_x, 3)))
        data = data.replace('x{:d}['.format(c_inf["layer"] - 1), 'output_tensor[')

        with open(c_inf["path"], 'w') as file:
            file.write(data)

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

    writeC(c_inf, '{indent}alignas(16) static float x{layer} [{x_res}][{y_res}][{z_res}] = {{}};\n'.format(**str_data))

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
                if ((ix == -pad_top and unroll_level == 1) or
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

    writeC(c_inf, '{indent}alignas(16) static float x{layer} [{x_res}][{y_res}][{z_res}] = {{}};\n'.format(**str_data))

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

    writeC(c_inf, '{indent}alignas(16) static float x{layer} [{x_res}][{y_res}][{z_res}] = {{}};\n'.format(**str_data))

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
    writeC(c_inf, '\tstatic float x{layer}[{x_res}][{y_res}][{z_res}] = {{}};\n'.format(**str_data))
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