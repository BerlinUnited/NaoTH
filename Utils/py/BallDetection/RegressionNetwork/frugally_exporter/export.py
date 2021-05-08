"""
    load the model and export it again with include_optimizer=False

    guide: https://github.com/Dobiasd/frugally-deep/blob/master/keras_export/save_application_examples.py
"""
import argparse
from frugally_exporter import convert_model
from pathlib import Path

MODEL_DIR = Path(Path(__file__).parent.parent.absolute() / "models/best_models").resolve()


def save_model(keras_filename):
    """Save and convert Keras model"""
    fdeep_file = f'{Path(keras_filename).stem}.json'

    print(f'Converting {keras_filename} to {fdeep_file}.')
    convert_model.convert(keras_filename, fdeep_file)
    print(f'Conversion of model {keras_filename} to {fdeep_file} done.')


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Compile keras network to frugally deep format')
    parser.add_argument('-m', '--model-path', dest='model_path',
                        help='Store the trained model using this path. Default is fy1500_conf.h5.',
                        default=str(MODEL_DIR / 'fy1500_conf.h5'))

    args = parser.parse_args()

    save_model(args.model_path)
