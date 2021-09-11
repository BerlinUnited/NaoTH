import pickle
from pathlib import Path
import toml
from tensorflow.keras.models import load_model
import utility_functions.metrics as metrics_module
from onbcg import NaoTHCompiler  # can throw linter warnings, but python3 can handle imports like that
from inspect import isclass, isfunction
DATA_DIR = Path(Path(__file__).parent.parent.absolute() / "data").resolve()
CPP_DIR = Path(Path(__file__).parent.parent.absolute() / "cpp").resolve()
MODEL_DIR = Path(Path(__file__).parent.parent.absolute() / "models").resolve()


def main(config_name):
    with open('../config.toml', 'r') as f:
        config_dict = toml.load(f)
    my_config = config_dict[config_name]

    code_path = CPP_DIR / (Path(my_config['model_path']).stem + ".cpp")
    model_path = MODEL_DIR / Path(my_config['model_path'])

    # print status
    print(f"imgdb_path = {my_config['trainings_data']}")
    print(f"model_path = {model_path}")
    print(f"code_path  = {code_path}")

    images = {}
    data_file = str(DATA_DIR / my_config["trainings_data"])
    with open(data_file, "rb") as f:
        images["mean"] = pickle.load(f)
        images["images"] = pickle.load(f)
        images["y"] = pickle.load(f)

    toml_metrics = my_config["metrics"]
    metrics_dict = {}
    for metrics_name in toml_metrics:
        reference = getattr(metrics_module, metrics_name)
        if isfunction(reference):
            metrics_dict.update({metrics_name: reference})
        if isclass(reference):
            metrics_dict.update({metrics_name: reference()})

    model = load_model(model_path, custom_objects=metrics_dict)

    if "compile_debug" in my_config.keys() and my_config["compile_debug"] is True:
        print("debug")
        debug_compiler = NaoTHCompiler(images, model, code_path, unroll_level=2, arch="sse3",
                                       test_binary=True)
        debug_compiler.keras_compile()
    else:
        compiler = NaoTHCompiler(images, model, code_path, unroll_level=2, arch="sse3",
                                 test_binary=False)
        compiler.keras_compile()


if __name__ == '__main__':
    main("stella_config")
