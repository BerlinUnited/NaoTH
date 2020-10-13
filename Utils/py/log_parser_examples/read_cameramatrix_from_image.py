"""
    expects a image with added metadate like the ones exported with the image_example script and prints
    all the keys and values found in the meta data
"""
from argparse import ArgumentParser

from PIL import Image
from naoth.datasets import images

if __name__ == "__main__":
    parser = ArgumentParser(
        description='print the meta data saved in a png file')
    parser.add_argument("-i", "--input", help='path to png image')
    args = parser.parse_args()
    img = Image.open(args.input if args.input else images.load_data('image'))

    for k, v in img.info.items():
        print("%s: %s" % (k, v))
