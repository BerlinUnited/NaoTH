"""
    expects a image with added metadate like the ones exported with the image_example script and prints
    all the keys and values found in the meta data
"""
from argparse import ArgumentParser
from PIL import Image
from pathlib import Path
from pywget import wget


def get_demo_image():
    base_url = "https://www2.informatik.hu-berlin.de/~naoth/ressources/log/demo_image/"
    logfile_list = ["test.png"]

    target_dir = Path("logs")
    Path.mkdir(target_dir, exist_ok=True)

    for logfile in logfile_list:
        if not Path(target_dir / logfile).is_file():
            wget.download(base_url + logfile, target_dir)


if __name__ == "__main__":
    get_demo_image()

    parser = ArgumentParser(
        description='print the meta data saved in a png file')
    parser.add_argument("-i", "--input", help='path to png image', default="logs/test.png")

    args = parser.parse_args()
    img = Image.open(args.input)

    for k, v in img.info.items():
        print("%s: %s" % (k, v))
