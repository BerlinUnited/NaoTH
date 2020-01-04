import pickle
from pathlib import Path
import matplotlib.pyplot as plt
import numpy as np

loss_runs = []
acc_runs = []
epoch_loss_runs = []
epoch_acc_runs = []

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

        max_pos = acc.argmax()
        best_acc = acc[max_pos]

        loss_runs.append(min_loss_val)
        acc_runs.append(best_acc)
        epoch_loss_runs.append(min_pos)
        epoch_acc_runs.append(max_pos)
        # print("Best Epoch [Loss] is %d with loss %1.19f " % (min_pos, min_loss_val))
        # print("Best Epoch [ACC] is %d with accuracy %1.19g" % (max_pos, best_acc))

plt.show()
print()
best_run_loss = np.array(loss_runs).argmin()
best_acc_loss = np.array(acc_runs).argmax()
print("Best Loss: run: %d with loss %1.19f in epoch %d" % (
    best_run_loss, loss_runs[best_run_loss], epoch_loss_runs[best_run_loss]))
print("Best Acc run: %d with acc %1.19f in epoch %d" % (
    best_acc_loss, acc_runs[best_acc_loss], epoch_acc_runs[best_acc_loss]))
