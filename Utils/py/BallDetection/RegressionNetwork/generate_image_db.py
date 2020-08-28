"""
    This scripts creates a pickle file containing all the images used in training. The generated file is also used
    during export with the devil_code_generator1

    Downloading from kaggle only works if $HOME/.kaggle/kaggle.json with a working API Token exists. You can sign up to
    kaggle and create your own: https://www.kaggle.com/docs/api

    NaoTH Members can use the team credentials found in the accounts wiki page
"""
import argparse
import pickle
from pathlib import Path

from kaggle.api.kaggle_api_extended import KaggleApi

from utility_functions.loader import load_images_from_csv_files

DATA_DIR = Path(Path(__file__).parent.absolute() / "data").resolve()


def download_from_kaggle():
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


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate the image database for training etc. '
                                                 'using a folder with 0, 1 etc. subfolders with png images.')
    parser.add_argument('-d', '--download', default=True, help='download dataset from kaggle')
    parser.add_argument('-b', '--database-path', dest='imgdb_path',
                        help='Path to the image database to write. '
                             'Default is imgdb.pkl in current folder.')
    parser.add_argument('-i', '--image-folder', dest='img_path',
                        help='Path to the CSV file(s) with region annotation.')
    parser.add_argument('-r', '--resolution', dest='res',
                        help='Images will be resized to this resolution. Default is 16x16')
    parser.add_argument("-l", "--limit-noball", type=str2bool, nargs='?', dest="limit_noball",
                        const=True,
                        help="Randomly select at most |balls| from no balls class")

    args = parser.parse_args()
    if args.download:
        download_from_kaggle()

    # set default values for output path, input path and resolution
    imgdb_path = "data/imgdb.pkl"
    img_path = "data/TK-03/"
    res = {"x": 16, "y": 16}

    if args.imgdb_path is not None:
        imgdb_path = args.imgdb_path
    if args.img_path is not None:
        img_path = args.img_path
    if args.res is not None:
        res = {"x": int(args.res), "y": int(args.res)}

    x, y, mean, p = load_images_from_csv_files(img_path, res, args.limit_noball)
    # TODO load the blender images via load_blender_images. Currently that would load them twice since a csv file also exists
    with open(imgdb_path, "wb") as f:
        # save image db
        pickle.dump(mean, f)
        pickle.dump(x, f)
        pickle.dump(y, f)
        pickle.dump(p, f)
