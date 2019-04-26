import numpy as np
from keras.models import load_model
from utility_functions.onbcg import keras_compile

model_path = "models/bottom.h5"
model = load_model(model_path)
model.summary()


input_shape = (16, 16, 1)

np.random.seed(42)
array = np.random.randint(0, 255+1, size=input_shape).astype(np.float32)

test_img_db = {'images': [array], 'mean': 0.0}  # Replace this by real images

keras_compile(test_img_db, model_path, "yolo.c", unroll_level=2, arch="sse3")
