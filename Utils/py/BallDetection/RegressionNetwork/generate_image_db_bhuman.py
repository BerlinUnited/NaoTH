"""
Converts the b-human 2019 dataset to the naoth format so we can run performance comparisons

TODO add option to make a balanced dataset
TODO add option to only get negative images
"""
import pickle
import numpy as np
import h5py
from pathlib import Path
from urllib.request import urlretrieve
from urllib.error import HTTPError, URLError
from utility_functions.loader import calculate_mean, subtract_mean


def download_bhuman2019(origin, target):
    def dl_progress(count, block_size, total_size):
        print('\r', 'Progress: {0:.2%}'.format(min((count * block_size) / total_size, 1.0)), sep='', end='', flush=True)

    if not Path(target).exists():
        target_folder = Path(target).parent
        target_folder.mkdir(parents=True, exist_ok=True)
    else:
        return

    error_msg = 'URL fetch failure on {} : {} -- {}'
    try:
        try:
            urlretrieve(origin, target, dl_progress)
            print('\nFinished')
        except HTTPError as e:
            raise Exception(error_msg.format(origin, e.code, e.reason))
        except URLError as e:
            raise Exception(error_msg.format(origin, e.errno, e.reason))
    except (Exception, KeyboardInterrupt):
        if Path(target).exists():
            Path(target).unlink()
        raise


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
