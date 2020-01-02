import pickle
from pathlib import Path
import matplotlib.pyplot as plt
import numpy as np

loss_runs = []
acc_runs = []
epoch_runs = []

loss_figure = plt.figure(1)
acc_figure = plt.figure(2)
for filename in Path('../data/model1/').glob('**/*.pkl'):
    with open(str(filename), 'rb') as f:
        # load trainings history of a single run
        history = pickle.load(f)

        # get loss and acc
        plt.figure(1)
        loss = np.array(history['val_loss'])
        acc = np.array(history['val_acc'])

        # plot trainings progress
        plt.plot(history['val_loss'], label=str(filename.name))
        plt.ylabel('loss')
        plt.xlabel('epoch')
        plt.legend(loc='upper left')

        plt.figure(2)
        plt.plot(history['val_acc'], label=str(filename.name))
        plt.ylabel('accuracy')
        plt.xlabel('epoch')

        # get data from best epoch
        min_pos = loss.argmin()
        min_loss_val = loss[min_pos]
        best_acc = acc[min_pos]

        loss_runs.append(min_loss_val)
        acc_runs.append(best_acc)
        epoch_runs.append(min_pos)
        print("Epoch: %d with loss %1.19f and accurray %1.19g" % (min_pos, min_loss_val, best_acc))

plt.show()
print()
best_run = np.array(loss_runs).argmin()
print("Best run: %d with loss %1.19f, acc %1.19f in epoch %d" % (
    best_run, loss_runs[best_run], acc_runs[best_run], epoch_runs[best_run]))

# TODO make multiple plots for comparing all the metrics over multiple runs and then train vs validation of best run
quit()
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
