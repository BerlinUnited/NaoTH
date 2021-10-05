import argparse
import pickle
from inspect import isclass, isfunction
from pathlib import Path

import tensorflow as tf
import toml
from tensorflow.keras.models import load_model

import utility_functions.metrics as metrics_module
from nncg.nncg import NNCG

tf.compat.v1.disable_eager_execution()  # TODO why was this necessary???

DATA_DIR = Path(Path(__file__).parent.parent.absolute() / "data").resolve()
CPP_DIR = Path(Path(__file__).parent.parent.absolute() / "cpp").resolve()
MODEL_DIR = Path(Path(__file__).parent.parent.absolute() / "models").resolve()


def main(config_name):
    with open('../config.toml', 'r') as f:
        config_dict = toml.load(f)
    my_config = config_dict[config_name]

    code_path = CPP_DIR / (Path(my_config['model_path']).stem + "_nncg.cpp")
    model_path = MODEL_DIR / Path(my_config['model_path'])

    # print status
    print(f"imgdb_path = {my_config['trainings_data']}")
    print(f"model_path = {model_path}")
    print(f"code_path  = {code_path}")

    parser = argparse.ArgumentParser(description='Compile keras network to c++')
    parser.add_argument('-b', '--database-path', dest='imgdb_path',
                        help='Path to the image database to use for training. '
                             'Default is imgdb.pkl in current folder.',
                        default=str(DATA_DIR / 'tk03_natural_detection.pkl'))

    images = {}
    data_file = str(DATA_DIR / my_config["trainings_data"])
    with open(data_file, "rb") as f:
        images["mean"] = pickle.load(f)
        images["images"] = pickle.load(f)
        images["y"] = pickle.load(f)

    if "metrics" in my_config.keys():
        toml_metrics = my_config["metrics"]
        metrics_dict = {}
        for metrics_name in toml_metrics:
            reference = getattr(metrics_module, metrics_name)
            if isfunction(reference):
                metrics_dict.update({metrics_name: reference})
            if isclass(reference):
                metrics_dict.update({metrics_name: reference()})

        model = load_model(model_path, custom_objects=metrics_dict)
    else:
        model = load_model(model_path)
    nncg = NNCG()
    nncg.keras_compile(imdb=images['images'], model=model, code_path=code_path, arch="sse3")


if __name__ == '__main__':
    main("config2")
