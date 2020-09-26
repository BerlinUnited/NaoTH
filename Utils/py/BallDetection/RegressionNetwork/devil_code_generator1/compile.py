#!/usr/bin/env python3

import argparse
import pickle
from pathlib import Path

from tensorflow.keras.models import load_model

from onbcg import NaoTHCompiler   # can throw linter warnings, but python3 can handle imports like that

DATA_DIR = Path(Path(__file__).parent.parent.absolute() / "data").resolve()
CPP_DIR = Path(Path(__file__).parent.parent.absolute() / "cpp").resolve()
MODEL_DIR = Path(Path(__file__).parent.parent.absolute() / "data/best_models").resolve()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Compile keras network to c++')
    parser.add_argument('-b', '--database-path', dest='imgdb_path',
                        help='Path to the image database to use for training. '
                             'Default is imgdb.pkl in current folder.', default=str(DATA_DIR / 'imgdb.pkl'))
    parser.add_argument('-m', '--model-path', dest='model_path',
                        help='Store the trained model using this path. Default is fy1500.h5.',
                        default=str(MODEL_DIR / 'fy1500.h5'))
    parser.add_argument('-c', '--code-path', dest='code_path',
                        help='Store the c code in this file. Default is <model_name>.c.')

    args = parser.parse_args()

    if args.code_path is None:
        # load the model to get the name
        model = load_model(args.model_path)
        print(model.name)
        args.code_path = CPP_DIR / (model.name + ".cpp")

    images = {}
    with open(args.imgdb_path, "rb") as f:
        images["mean"] = pickle.load(f)
        images["images"] = pickle.load(f)
        images["y"] = pickle.load(f)

    #compiler = NaoTHCompiler(images, args.model_path, args.code_path, unroll_level=2, arch="sse3", test_binary=False)
    #compiler.keras_compile()

    debug_compiler = NaoTHCompiler(images, args.model_path, args.code_path, unroll_level=2, arch="sse3", test_binary=True)
    debug_compiler.keras_compile()
