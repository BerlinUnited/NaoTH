import pickle
import numpy as np

pickle_files = [
    '../data/fy_1500_0/history.pkl',
    '../data/fy_1500_1/history.pkl',
    '../data/fy_1500_2/history.pkl',
    '../data/fy_1500_3/history.pkl',
    '../data/fy_1500_4/history.pkl',
    '../data/fy_1500_5/history.pkl',
    '../data/fy_1500_6/history.pkl',
    '../data/fy_1500_7/history.pkl',
    '../data/fy_1500_8/history.pkl',
    '../data/fy_1500_9/history.pkl',
    '../data/fy_1500_10/history.pkl',
    '../data/fy_1500_11/history.pkl',
    '../data/fy_1500_12/history.pkl',
    '../data/fy_1500_13/history.pkl',
    '../data/fy_1500_14/history.pkl',
    '../data/fy_1500_15/history.pkl',
    '../data/fy_1500_16/history.pkl',
    '../data/fy_1500_17/history.pkl',
    '../data/fy_1500_18/history.pkl',
    '../data/fy_1500_19/history.pkl',
    '../data/fy_1500_20/history.pkl',
    '../data/fy_1500_21/history.pkl',
    '../data/fy_1500_22/history.pkl',
    '../data/fy_1500_23/history.pkl',
    '../data/fy_1500_24/history.pkl',
    '../data/fy_1500_25/history.pkl',
    '../data/fy_1500_26/history.pkl',
    '../data/fy_1500_27/history.pkl',
    '../data/fy_1500_28/history.pkl',
    '../data/fy_1500_29/history.pkl',
]
loss_runs = []
acc_runs = []
epoch_runs = []

for idx, filename in enumerate(pickle_files):
    with open(filename, 'rb') as f:
        history = pickle.load(f)

        loss = np.array(history['val_loss'])
        acc = np.array(history['val_acc'])

        min_pos = loss.argmin()
        min_loss_val = loss[min_pos]
        best_acc = acc[min_pos]

        print("Epoch: %d with loss %1.19f and accurray %1.19g" % (min_pos, min_loss_val, best_acc))
        loss_runs.append(min_loss_val)
        acc_runs.append(best_acc)
        epoch_runs.append(min_pos)

print()
best_run = np.array(loss_runs).argmin()
print("Best run: %d with loss %1.19f, acc %1.19f in epoch %d" % (best_run, loss_runs[best_run], acc_runs[best_run], epoch_runs[best_run]))
