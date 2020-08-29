# Tensorflow for Balldetection
The requirements are defined in requirements.txt. You can install them with pip with 
`pip install -r requirements.txt`. Conda users can use the `environment.yml`. To create
a conda environment use `conda env export -n conda-env -f environment.yml`

## Input data
Data can be automatically downloded from kaggle. See `generate_image_db.py`. The images are
then put in a pickle file which is used for training.

## Training
The scripts are optimised for 16x16 single channel input images and output (4,1) vector representing
radius, x, y and confidence of the ball location relative to the image.

## Export Neurocal Networks
Currently we have two methods for exporting the trained neural network. One exports a json file for 
use with the frugally deep learning lib. The other exports c++ code. The c++ exported version is
designed to be used inside the NaoTH Framework. 