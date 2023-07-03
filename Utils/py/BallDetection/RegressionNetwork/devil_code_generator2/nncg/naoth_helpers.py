import os


def write_naoth_header_file(input_shape, output_shape, class_name, output_folder="."):
    fp = open(os.path.join(output_folder, class_name + ".h"), "w")
    with fp:
        print("#ifndef {}_H".format(class_name.upper()), file=fp)
        print("#define {}_H".format(class_name.upper()), file=fp)
        print("", file=fp)
        print("#include <emmintrin.h>", file=fp)
        print("", file=fp)
        print("#include \"AbstractCNNClassifier.h\"", file=fp)
        print("", file=fp)
        print("class {} : public AbstractCNNFinder {{".format(class_name), file=fp)
        print("", file=fp)
        print("public:", file=fp)
        print("\tvoid cnn(float x0[{:d}][{:d}][{:d}]);".format(input_shape[0], input_shape[1],
                                                               input_shape[2]),
              file=fp)
        print("\tvoid predict(const BallCandidates::PatchYUVClassified& p,double meanBrightness);", file=fp)
        print("\tvirtual double getRadius();", file=fp)
        print("\tvirtual Vector2d getCenter();", file=fp)
        print("\tvirtual double getBallConfidence();", file=fp)
        print("", file=fp)
        print("private:", file=fp)
        print("\tfloat in_step[{:d}][{:d}][{:d}];".format(input_shape[0], input_shape[1],
                                                          input_shape[2]), file=fp)
        print("\tfloat scores[{:d}];".format(output_shape), file=fp)
        print("", file=fp)
        print("};", file=fp)
        print("# endif", file=fp)


def print_predict_function(class_name):
    return '''
void %s::predict(const BallCandidates::PatchYUVClassified& patch, double meanBrightnessOffset){

    ASSERT(patch.size() == 16);

    for(size_t x=0; x < patch.size(); x++) {
        for(size_t y=0; y < patch.size(); y++) {
            // TODO: check
            // .pixel.y accesses the brightness channel of the pixel
            // subtract the mean brightness calculated on the dataset and the offset from the module parameters
            float value = (static_cast<float>((patch.data[patch.size() * x + y].pixel.y)) / 255.0f) - -0.000000f - static_cast<float>(meanBrightnessOffset);
            in_step[y][x][0] = value;
        }
    }
    cnn(in_step);
}\n''' % class_name


def print_get_radius_function(class_name):
    return '''
double %s::getRadius() {
    return scores[0];
}
''' % class_name


def print_get_center_function(class_name):
    return '''
Vector2d %s::getCenter() {
    return Vector2d(scores[1], scores[2]);
}
''' % class_name


def print_get_confidence_function(class_name):
    return '''
double %s::getBallConfidence() {
    return scores[3];
}
''' % class_name
