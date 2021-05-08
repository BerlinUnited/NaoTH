"""
 run trainings procedure multiple times and compare the best run with each other
"""
import pickle
from pathlib import Path
from inspect import getmembers, isfunction
import utility_functions.model_zoo as model_zoo
from train import main

DATA_DIR = Path(Path(__file__).parent.parent.absolute() / "data").resolve()


def start_training(test_model, num_runs=30, num_epochs=100):
    for i in range(num_runs):
        output_path = Path(test_model.name + "_" + str(i))
        if not output_path.exists():
            Path.mkdir(output_path)

        history = main(
            ['-b', str(DATA_DIR / 'imgdb.pkl'), '--epochs', str(num_epochs), '--output', str(output_path)],
            test_model)

        # save trainings history to file
        with open(str(output_path) + "/history_" + str(i) + ".pkl", "wb") as f:
            pickle.dump(history.history, f)


if __name__ == '__main__':
    num_runs = 30
    num_epochs = 100

    # this code assumes only model functions are in model zoo
    model_list = getmembers(model_zoo, isfunction)
    for model in model_list:
        print("Train on model: ", model[0])
        # call the model function from model zoo
        current_model = model[1]()

        start_training(current_model, num_runs, num_epochs)
