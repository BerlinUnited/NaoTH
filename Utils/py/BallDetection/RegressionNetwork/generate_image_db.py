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

from utility_functions.loader import load_images_from_csv_files, load_blender_images

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


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate the image database for training etc. '
                                                 'using a folder with 0, 1 etc. subfolders with png images.')
    parser.add_argument('-d', '--download', default=False, help='download dataset from kaggle')
    parser.add_argument('-b', '--database-path', dest='imgdb_path', default=str(DATA_DIR / 'imgdb.pkl'),
                        help='Path to the image database to write. Default is imgdb.pkl in the data folder.')
    parser.add_argument('-c', '--database-blender-path', dest='imgdb_blender_path', default=str(DATA_DIR / 'imgdb_blender.pkl'),
                        help='Path to the image database to write. Default is imgdb_blender_new.pkl in the data folder.')
    parser.add_argument('-i', '--image-folder', dest='img_path', default=str(DATA_DIR / 'TK-03'),
                        help='Path to the CSV file(s) with region annotation.')
    parser.add_argument('-r', '--resolution', dest='res',
                        help='Images will be resized to this resolution. Default is 16x16')
    parser.add_argument("-l", "--limit-noball", type=str2bool, nargs='?', dest="limit_noball",
                        const=True, help="Randomly select at most |balls| from no balls class")

    args = parser.parse_args()
    if args.download:
        download_from_kaggle()

    # set default values for resolution
    res = {"x": 16, "y": 16}
    if args.res is not None:
        res = {"x": int(args.res), "y": int(args.res)}

    # set default parameter for processing of images from blender folder (image + mask)
    IMG_MASK = True
    IMG = False

    if IMG:
        # if ignore_blender is set to false
        # images (with masks) from blender folder are processed together with other images
        # and images with masks are ignored if ignore_blender is set to true
        x, y, mean, p = load_images_from_csv_files(args.img_path, res, args.limit_noball, ignore_blender=False)
        with open(args.imgdb_path, "wb") as f:
            # save image db
            pickle.dump(mean, f)
            pickle.dump(x, f)
            pickle.dump(y, f)
            pickle.dump(p, f)

    if IMG_MASK:
        path = args.img_path + "/blender"
        x_b, y_b, mean_b, p_b = load_blender_images(path, res)
        with open(args.imgdb_blender_path, "wb") as f:
            pickle.dump(mean_b, f)
            pickle.dump(x_b, f)
            pickle.dump(y_b, f)
            pickle.dump(p_b, f)
