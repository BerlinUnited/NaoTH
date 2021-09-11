import pickle
from datetime import datetime
from inspect import isclass, isfunction
from pathlib import Path
from sys import exit

# TODO encode dataset into output model name
import tensorflow as tf
import toml
from tensorflow import keras as keras

import utility_functions.metrics as metrics_module
import utility_functions.model_zoo as model_zoo

DATA_DIR = Path(Path(__file__).parent.absolute() / "data").resolve()


def load_model(my_config):
    if "model_name" in my_config.keys():
        method_to_call = getattr(model_zoo, my_config["model_name"])
        model = method_to_call()
        # TODO how to handle execptions of getattr?
        return model
    elif "proceed" in my_config.keys() and my_config["proceed"] is True:
        # if you use proceed then you have to give in a model
        print("Loading model " + my_config.model_path)

        toml_metrics = my_config["metrics"]
        metrics_dict = {}
        for metrics_name in toml_metrics:
            reference = getattr(metrics_module, metrics_name)
            if isfunction(reference):
                metrics_dict.update({metrics_name: reference})
            if isclass(reference):
                metrics_dict.update({metrics_name: reference()})

        model = tf.keras.models.load_model(my_config.model_path, custom_objects=metrics_dict)
        return model
    else:
        print(
            "ERROR: No model specified, you have to either specify model_name in the config or set proceed to true and set model_path")
        exit(1)


def main(config_name):
    with open('config.toml', 'r') as f:
        config_dict = toml.load(f)

    my_config = config_dict[config_name]
    model = load_model(my_config)

    data_file = str(DATA_DIR / my_config["trainings_data"])
    with open(data_file, "rb") as f:
        pickle.load(f)  # skip mean
        x = pickle.load(f)  # x are all input images
        y = pickle.load(f)  # y are the trainings target: [r, x,y,1]

    """ 
        The save callback will overwrite the previous models if the new model is better then the last. Restarting the 
        training will always overwrite the models.
    """
    filepath = Path(my_config["output_path"]) / (model.name + "_" + Path(my_config["trainings_data"]).stem + ".h5")
    save_callback = tf.keras.callbacks.ModelCheckpoint(filepath=str(filepath), monitor='loss', verbose=1,
                                                       save_best_only=True, mode='max')

    log_path = Path(my_config["output_path"]) / "logs" / (
            model.name + "_" + str(datetime.now()).replace(" ", "_").replace(":", "-"))
    log_callback = keras.callbacks.TensorBoard(log_dir=log_path, profile_batch=0)

    callbacks = [save_callback, log_callback]

    # TODO prepare an extra validation set, that is consistent over multiple runs
    # history = model.fit(x, y, batch_size=args.batch_size, epochs=args.epochs, verbose=1,
    # validation_data=(X_test, Y_test),callbacks=callbacks)

    history = model.fit(x, y, batch_size=my_config["batch_size"], epochs=my_config["epochs"], verbose=1,
                        validation_split=0.1,
                        callbacks=callbacks)
    history_filename = "history_" + model.name + "_" + Path(my_config["trainings_data"]).stem + ".pkl"

    # save history in same folder as model
    history_filepath = Path(my_config["output_path"]) / history_filename
    with open(str(history_filepath), "wb") as f:
        pickle.dump(history.history, f)

    return history, history_filename


if __name__ == '__main__':
    main("stella_config")

