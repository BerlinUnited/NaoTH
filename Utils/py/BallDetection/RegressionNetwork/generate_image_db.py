"""
    This scripts creates a pickle file containing all the images used in training. The generated file is also used
    during export with the devil_code_generator1

    Downloading from kaggle only works if $HOME/.kaggle/kaggle.json with a working API Token exists. You can sign up to
    kaggle and create your own: https://www.kaggle.com/docs/api

    NaoTH Members can use the team credentials found in the accounts wiki page
"""
import argparse
import os
import pickle
from pathlib import Path
import numpy as np

from utility_functions.loader import create_natural_dataset, create_blender_detection_dataset, calculate_mean, subtract_mean, \
    create_blender_segmentation_dataset, create_blender_classification_dataset

DATA_DIR = Path(Path(__file__).parent.absolute() / "data").resolve()


def download_from_kaggle():
    os.environ['KAGGLE_USERNAME'] = 'hubnao'
    os.environ['KAGGLE_KEY'] = '734455fc37853e123ff4a2a7a7f2ed2a'
    from kaggle.api.kaggle_api_extended import KaggleApi
    api = KaggleApi()
    api.authenticate()

    api.dataset_download_files('berlinunitednaoth/tk3balldetectionrobocup2019sydney', path=str(DATA_DIR / "TK-03"),
                               quiet=False,
                               unzip=True)


def str2bool(v):
    if v.lower() in ('yes', 'true', 't', 'y', '1'):
        return True
    elif v.lower() in ('no', 'false', 'f', 'n', '0'):
        return False
    else:
        raise argparse.ArgumentTypeError('Boolean value expected.')


def store_output(output_file, mean, x, y, p=None):
    with open(output_file, "wb") as f:
        pickle.dump(mean, f)
        pickle.dump(x, f)
        pickle.dump(y, f)
        pickle.dump(p, f)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate the image database for training etc. '
                                                 'using a folder with 0, 1 etc. subfolders with png images.')
    parser.add_argument('-d', '--download', default=False, help='download dataset from kaggle')
    parser.add_argument('-i', '--image-folder', dest='img_path', default=str(DATA_DIR / 'TK-03'),
                        help='Path to the CSV file(s) with region annotation.')
    parser.add_argument('-r', '--resolution', dest='res',
                        help='Images will be resized to this resolution. Default is 16x16')
    parser.add_argument("-l", "--limit-noball", type=str2bool, nargs='?', dest="limit_noball",
                        const=True, help="Randomly select at most |balls| from no balls class")
    parser.add_argument("--data_type", dest="data_type",
                        choices=["classification", "detection", "semantic_segmentation"], default="detection")

    args = parser.parse_args()
    if args.download:
        download_from_kaggle()

    # set default values for resolution
    res = {"x": 16, "y": 16}
    if args.res is not None:
        res = {"x": int(args.res), "y": int(args.res)}

    if args.data_type == "classification":
        x, y, p = create_natural_dataset(args.img_path, res, args.limit_noball, "classification")
        mean = calculate_mean(x)
        x = subtract_mean(x, mean)

        print("save classification dataset with natural images")
        output_name = str(DATA_DIR / 'tk03_natural_classification.pkl')
        store_output(output_name, mean, x, y, p)

        path = args.img_path + "/blender"
        x_syn,y_syn = create_blender_classification_dataset(path, res)
        mean_b = calculate_mean(x_syn)
        x_syn = subtract_mean(x_syn, mean_b)

        print("save classification dataset with synthetic images")
        output_name = str(DATA_DIR / 'tk03_synthetic_classification.pkl')
        store_output(output_name, mean_b, x_syn, y_syn)

        # merge the two datasets
        X = np.concatenate((x, x_syn))
        Y = np.concatenate((y, y_syn))
        mean = calculate_mean(X)

        print("save classification dataset with combined images")
        output_name = str(DATA_DIR / 'tk03_combined_classification.pkl')
        store_output(output_name, mean, X, Y)

    if args.data_type == "detection":
        x, y, p = create_natural_dataset(args.img_path, res, args.limit_noball, "detection")
        print("sum:", np.sum(y))
        mean = calculate_mean(x)
        x = subtract_mean(x, mean)

        print("save detection dataset with natural images")
        output_name = str(DATA_DIR / 'tk03_natural_detection.pkl')
        store_output(output_name, mean, x, y, p)

        path = args.img_path + "/blender"
        x_syn, y_syn, p_syn = create_blender_detection_dataset(path, res)
        mean_b = calculate_mean(x_syn)
        x_syn = subtract_mean(x_syn, mean_b)
    
        print("save detection dataset with synthetic images")
        output_name = str(DATA_DIR / 'tk03_synthetic_detection.pkl')
        store_output(output_name, mean_b, x_syn, y_syn, p_syn)

        # merge the two datasets
        X = np.concatenate((x, x_syn))
        Y = np.concatenate((y, y_syn))
        P = np.concatenate((p, p_syn))
        mean = calculate_mean(X)

        print("save detection dataset with combined images")
        output_name = str(DATA_DIR / 'tk03_combined_detection.pkl')
        store_output(output_name, mean, X, Y, P)

    if args.data_type == "semantic_segmentation":
        x, y, p = create_natural_dataset(args.img_path, res, args.limit_noball, "segmentation")
        mean = calculate_mean(x)
        x = subtract_mean(x, mean)

        print("save segmentation dataset with natural images")
        output_name = str(DATA_DIR / 'tk03_natural_segmentation.pkl')
        store_output(output_name, mean, x, y, p)

        path = args.img_path + "/blender"
        x_syn, y_syn = create_blender_segmentation_dataset(path, res)

        mean_b = calculate_mean(x_syn)
        x_syn = subtract_mean(x_syn, mean_b)
        output_name = str(DATA_DIR / 'tk03_synthetic_segmentation.pkl')
        store_output(output_name, mean_b, x_syn, y_syn)

        # merge the two datasets
        X = np.concatenate((x, x_syn))
        Y = np.concatenate((y, y_syn))
        mean = calculate_mean(X)

        print("save detection dataset with combined images")
        output_name = str(DATA_DIR / 'tk03_combined_segmentation.pkl')
        store_output(output_name, mean, X, Y)
