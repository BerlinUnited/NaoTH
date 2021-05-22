"""
TODO show images from dataset with annotations for different datasets (naoth, b-human, etc)
"""
import numpy as np
import h5py
import matplotlib.pyplot as plt
import pickle
from pathlib import Path

DATA_DIR = Path(Path(__file__).parent.parent.absolute() / "data").resolve()
MODEL_DIR = Path(Path(__file__).parent.parent.absolute() / "data/best_models").resolve()

# get data
def visualize_bhuman_dataset():
    f = h5py.File('../data/bhuman/b-alls-2019.hdf5', 'r')

    negative_data = np.array(f.get('negatives/data'))
    positive_data = np.array(f.get('positives/data'))
    negative_labels = np.array(f.get('negatives/labels'))
    positive_labels = np.array(f.get('positives/labels'))

    fig = plt.figure(figsize=(8, 8))
    columns = 4
    rows = 5
    for i in range(1, columns * rows + 1):
        img = positive_data[i]
        fig.add_subplot(rows, columns, i)
        current_label = positive_labels[i]
        print("current_label:", current_label)
        circle1 = plt.Circle((current_label[1], current_label[2]), current_label[3], color='r', alpha=0.7)
        ax = fig.gca()
        ax.add_patch(circle1)
        plt.imshow(img, cmap='gray')
    plt.show()


"""

"""


def visualize_tk3_classification():
    pass


def visualize_tk3_detection():
    imgdb_path = str(DATA_DIR / 'tk03_natural_detection.pkl')
    with open(imgdb_path, "rb") as f:
        mean = pickle.load(f)
        x = pickle.load(f)
        y = pickle.load(f)

    fig = plt.figure(figsize=(8, 8))
    columns = 4
    rows = 5
    for i in range(1, columns * rows + 1):
        img = x[i]
        fig.add_subplot(rows, columns, i)
        current_label = y[i]
        print("current_label:", current_label)
        # format for plotting is (xy), r
        circle1 = plt.Circle((current_label[1]*16, current_label[2]*16), current_label[0]*16, color='r', alpha=0.7)
        ax = fig.gca()
        ax.add_patch(circle1)
        plt.imshow(img, cmap='gray')
    plt.show()


def visualize_tk3_segmenation():
    pass


if __name__ == '__main__':
    visualize_tk3_detection()
