#!/usr/bin/env python3

import argparse
import pickle
from pathlib import Path

from tensorflow.keras.models import load_model

from onbcg import NaoTHCompiler  # can throw linter warnings, but python3 can handle imports like that

DATA_DIR = Path(Path(__file__).parent.parent.absolute() / "data").resolve()
CPP_DIR = Path(Path(__file__).parent.parent.absolute() / "cpp").resolve()
MODEL_DIR = Path(Path(__file__).parent.parent.absolute() / "data/best_models").resolve()

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Compile keras network to c++')
    parser.add_argument('-b', '--database-path', dest='imgdb_path',
                        help='Path to the image database to use for training. '
                             'Default is imgdb.pkl in current folder.', default=str(DATA_DIR / 'imgdb.pkl'))
    parser.add_argument('-m', '--model-path', dest='model_path',
                        help='Store the trained model using this path. Default is fy1500_conf.h5.',
                        default=str(MODEL_DIR / 'fy1500_conf.h5'))
    parser.add_argument('-c', '--code-path', dest='code_path',
                        help='Store the c code in this file. Default is <model_name>.c.')
    parser.add_argument('-d', '--debug', dest='debug_flag', default=False,
                        help='Switch between debug and production build. The debug build creates code for running the '
                             'network multiple times and provides timing measurements in the end.')

    args = parser.parse_args()

    if args.code_path is None:
        args.code_path = CPP_DIR / (Path(args.model_path).stem + ".cpp")

    # print status
    print(f"imgdb_path = {args.imgdb_path}")
    print(f"model_path = {args.model_path}")
    print(f"code_path  = {args.code_path}")

    images = {}
    with open(args.imgdb_path, "rb") as f:
        images["mean"] = pickle.load(f)
        images["images"] = pickle.load(f)
        images["y"] = pickle.load(f)

    if args.debug_flag:
        print("debug")
        debug_compiler = NaoTHCompiler(images, args.model_path, args.code_path, unroll_level=2, arch="sse3",
                                       test_binary=True)
        debug_compiler.keras_compile()
    else:
        compiler = NaoTHCompiler(images, args.model_path, args.code_path, unroll_level=2, arch="sse3", test_binary=False)
        compiler.keras_compile()


