import numpy as np
from keras.models import load_model
from utility_functions.onbcg import keras_compile
import os

model_path = "models/fy1500_conf.h5"
#model_path = "models/extra-step.h5"
model = load_model(model_path)
model.summary()


input_shape = (16, 16, 1)

np.random.seed(42)
array = np.random.randint(0, 255+1, size=input_shape).astype(np.float32)

test_img_db = {'images': [array], 'mean': 0.0}  # Replace this by real images

model_name = os.path.splitext(os.path.basename(model_path))[0]

keras_compile(test_img_db, model_path, "cpp/{}.cpp".format(model_name.title()), unroll_level=2, arch="sse3")
