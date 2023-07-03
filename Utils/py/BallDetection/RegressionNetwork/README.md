# Tensorflow for Balldetection
The requirements are defined in requirements.txt. You can install them with pip with

```sh
pip install -r requirements.txt
``` 

NOTE: make sure you have a compatible python interpreter for the tensorflow version specified. especially Arch users!


## Input data
The 2019 dataset for balldetection can be automatically downloded from kaggle. See `generate_image_db.py`. The images are
then put in a pickle file which is used for training.

To download the dataset you have to setup kaggle api credentials first. 
 - create `$HOME/.kaggle/kaggle.json` with {"username":"your username","key":"your key"}. NaoTH Members
 can find the team credentials at https://scm.cms.hu-berlin.de/berlinunited/orga/-/wikis/team/Accounts
- run `python generate_image_db.py -d True`

## How to create a new dataset
TODO

## Training
The scripts are optimised for 16x16 single channel input images and output (4,1) vector representing
radius, x, y and confidence of the ball location relative to the image.

run `python train.py` to train the fy_1500 model. Modify the script to train other models. You can
pass trainings parameter as command line arguments like so:
`python train.py --epochs 1` 

The command line arguments are then passed to the main function and evaluated there. This is done 
so it can be used from other scripts as well. See `experiments/rerun_training_and_compare.py` for an
example.

## Export Neural Networks
Currently we have three methods for exporting the trained neural network. One exports a json file for 
use with the frugally deep learning lib. The other two export c++ code. The c++ exported versions are
designed to be used inside the NaoTH Framework. 

### C++ Version - devil code generator 1
run `python devil_code_generator1/compile.py`

### C++ Version - devil code generator 2
TODO