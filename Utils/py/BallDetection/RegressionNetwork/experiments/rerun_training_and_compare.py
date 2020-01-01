"""
 run trainings procedure multiple times and compare the best run with each other
"""
import pickle
from pathlib import Path

import model_zoo
from train import main

num_runs = 30
num_epochs = 100
test_model = model_zoo.fy_1500()

for i in range(num_runs):
    output_path = Path(test_model.name + "_" + str(i))
    if not output_path.exists():
        Path.mkdir(output_path)

    history = main(['-b', '../img.db', '--log', '../logs/', '--epochs', str(num_epochs), '--output', str(output_path)],
                   test_model)

    # save trainings history to file
    with open(output_path + "/history.pkl", "wb") as f:
        pickle.dump(history.history, f)
