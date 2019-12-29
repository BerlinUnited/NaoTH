"""
 run trainings procedure multiple times and compare the best run with each other
"""
from pathlib import Path
from train import main
import model_zoo
import matplotlib.pyplot as plt
import pickle

num_runs = 30
num_epochs = 100
test_model = model_zoo.fy_1500()

for i in range(num_runs):
    output_path = './fy_1500' + "_" + str(i)
    if not Path(output_path).exists():
        Path.mkdir(Path(output_path))

    history = main(['-b', '../img.db', '--log', '../logs/', '--epochs', str(num_epochs), '--output', output_path], test_model)

    # save trainings history to file
    with open(output_path + "/history.pkl", "wb") as f:
        pickle.dump(history.history, f)

    # TODO get epoch with best loss
    # TODO make plots for all metrics
    print(history.history.keys())
    plt.plot(history.history['loss'])
    plt.plot(history.history['val_loss'])
    plt.title('model loss')
    plt.ylabel('loss')
    plt.xlabel('epoch')
    plt.legend(['train', 'test'], loc='upper left')
    plt.show()

    plt.plot(history.history['acc'])
    plt.plot(history.history['val_acc'])
    plt.title('model accuracy')
    plt.ylabel('accuracy')
    plt.xlabel('epoch')
    plt.legend(['train', 'test'], loc='upper left')
    plt.show()
