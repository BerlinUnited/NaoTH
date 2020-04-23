"""
    expects a image with added metadate like the ones exported with the image_example script and prints
    all the keys and values found in the meta data
"""
from argparse import ArgumentParser

from PIL import Image
from naoth.log import get_demo_logfiles

if __name__ == "__main__":
    base_url = "https://www2.informatik.hu-berlin.de/~naoth/ressources/log/demo_image/"
    logfile_list = ["test.png"]
    get_demo_logfiles(base_url, logfile_list)

    parser = ArgumentParser(
        description='print the meta data saved in a png file')
    parser.add_argument("-i", "--input", help='path to png image', default="logs/test.png")

    args = parser.parse_args()
    img = Image.open(args.input)

    for k, v in img.info.items():
        print("%s: %s" % (k, v))
