"""
 run trainings procedure multiple times and compare the best run with each other
"""
import pickle
from pathlib import Path
import utility_functions.model_zoo as model_zoo
from train import main

DATA_DIR = Path(Path(__file__).parent.parent.absolute() / "data").resolve()


def start_training(test_model, num_runs=30, num_epochs=100):
    for i in range(num_runs):
        output_path = Path(test_model.name + "_" + str(i))
        if not output_path.exists():
            Path.mkdir(output_path)

        # TODO make custom config for each model, handle output path changes in each iteration somehow
        history = main(
            ['-b', str(DATA_DIR / 'tk03_combined_detection.pkl'), '--epochs', str(num_epochs), '--output', str(output_path)],
            test_model)

        # save trainings history to file
        with open(str(output_path) + "/history_" + str(i) + ".pkl", "wb") as f:
            pickle.dump(history.history, f)


if __name__ == '__main__':
    num_runs = 30
    num_epochs = 100

    # this code assumes only model functions are in model zoo
    model_list = ["fy_1500_new", "model1"]
    for modelname in model_list:
        print("Train on model: ", modelname)
        method_to_call = getattr(model_zoo, modelname)

        # call the model function from model zoo
        current_model = method_to_call()

        start_training(current_model, num_runs, num_epochs)
