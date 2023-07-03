"""
Converts the b-human 2019 dataset to the naoth format so we can run performance comparisons

TODO add option to make a balanced dataset
TODO add option to only get negative images
"""
import pickle
import numpy as np
import h5py

from utility_functions.loader import calculate_mean, subtract_mean
from utility_functions.bhuman_helper import download_bhuman2019


def create_classification_dataset(negative_data, positive_data):
    images = np.append(negative_data, positive_data, axis=0)
    mean = calculate_mean(images)
    mean_images = subtract_mean(images, mean)

    negative_labels = [0] * len(negative_data)
    positive_labels = [1] * len(positive_data)
    labels = negative_labels + positive_labels

    with open("data/bhuman_classification.pkl", "wb") as f:
        pickle.dump(mean, f)
        pickle.dump(mean_images, f)
        pickle.dump(np.array(labels), f)


def create_detection_dataset(negative_data, positive_data, negative_labels, positive_labels):
    labels = np.append(negative_labels, positive_labels, axis=0)

    # swap dimensions to convert b-human format to berlin-united format
    new_labels = np.copy(labels)
    radii = labels[:, 0]
    classes = labels[:, -1]
    new_labels[:, 0] = classes
    new_labels[:, -1] = radii

    images = np.append(negative_data, positive_data, axis=0)
    mean = calculate_mean(images)

    mean_images = subtract_mean(images, mean)
    with open("data/bhuman_detection.pkl", "wb") as f:
        pickle.dump(mean, f)
        pickle.dump(mean_images, f)
        pickle.dump(new_labels, f)


if __name__ == '__main__':
    # TODO make a backup on our own servers
    download_bhuman2019("https://sibylle.informatik.uni-bremen.de/public/datasets/b-alls-2019/b-alls-2019.hdf5",
                        "data/bhuman/b-alls-2019.hdf5")
    download_bhuman2019("https://sibylle.informatik.uni-bremen.de/public/datasets/b-alls-2019/readme.txt",
                        "data/bhuman/readme.txt")

    # get data
    f = h5py.File('data/bhuman/b-alls-2019.hdf5', 'r')

    negative_data = np.array(f.get('negatives/data'))
    positive_data = np.array(f.get('positives/data'))
    negative_labels = np.array(f.get('negatives/labels'))
    positive_labels = np.array(f.get('positives/labels'))

    create_detection_dataset(negative_data, positive_data, negative_labels, positive_labels)
    create_classification_dataset(negative_data, positive_data)
