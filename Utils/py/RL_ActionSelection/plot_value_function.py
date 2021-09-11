# deriving the value function from q values
import numpy as np
import matplotlib.pyplot as plt

from env_0.state import State
from env_0.env import Env

from naoth.math import Vector2 as Vec


def run_field_creation(model, env, seg_x, seg_y):
    x_dim = 9000 / seg_x + 1
    y_dim = 6000 / seg_y + 1

    field_matrix = np.zeros((x_dim, y_dim))
    position_matrix = np.zeros((2, x_dim, y_dim))

    # create coordinates
    for x, x_value in enumerate(range(-4500, 4500 + seg_x, seg_x)):
        for y, y_value in enumerate(range(-3000, 3000 + seg_y, seg_y)):
            position_matrix[:, x, y] = np.array([x_value, y_value])

    field_vector = field_matrix.reshape(x_dim * y_dim, )
    position_vector_x = position_matrix[0].reshape(x_dim * y_dim, )
    position_vector_y = position_matrix[1].reshape(x_dim * y_dim, )

    for entry in range(x_dim * y_dim):
        x_value = position_vector_x[entry]
        y_value = position_vector_y[entry]
        max_q = get_prediction(model, env, x_value, y_value)
        field_vector[entry] = max_q

    max_value = np.max(field_matrix)

    p_field_normalized = field_matrix / max_value

    return p_field_normalized

    # return field_matrix


def get_prediction(model, env, x_value, y_value):
    env.world.state = State(Vec(x_value, y_value))
    state = env.current_state()

    action_space = env.action_space

    # get features for every action (1,...,5)
    features = []
    for action in action_space:
        # add action to features
        feature = np.append(state, action)
        # make 2 dim array ( here matrix with 1 row )
        feature = np.expand_dims(feature, axis=0)
        # transpose to get vektor ( 1 column, multiple rows )
        feature = np.transpose(feature)
        # dimension adjustment for Keras
        feature = np.expand_dims(feature, axis=0)
        # append new feature to feature list
        features.append(feature)

    # make features ready for model predictions ( get (nb_of_actions, 1, 4 , 1) input form )
    features = np.array(features)

    # https://keras.io/models/sequential/
    # get predictions for the features
    predictions = model.predict(features)

    # select best action ( integer in {0,...,4} )
    value = np.max(predictions)

    return value


def start_agent_and_plot(weights, feature_function, reward_function, net_config, seg_x, seg_y):
    from keras.models import Sequential
    from keras.layers import Dense, Activation, Flatten
    from keras.optimizers import RMSprop

    from env_0 import env
    from env_0.state import State

    from ring_buffer import History
    from dqn_agent import huber_loss

    feature_shape = feature_function(State()).shape[0]

    model = Sequential()
    model.add(Flatten(input_shape=(1, feature_shape + 1, 1)))

    for neurons in net_config:
        model.add(Dense(neurons))
        model.add(Activation('relu'))

    model.add(Dense(1))
    # model.add(Activation('sigmoid'))

    model.summary()

    model.load_weights(weights)
    print("Model loaded\n" + "#" * 20)

    # model.compile(optimizer=RMSprop(lr=0.00025, rho=0.95, epsilon=0.01), loss='mean_squared_error')
    # model.compile(optimizer=RMSprop(lr=0.00025, rho=0.95, epsilon=0.01), loss=huber_loss)
    model.compile(optimizer=RMSprop(lr=0.00025, rho=0.95, epsilon=0.01), loss=huber_loss)

    # env = env.Env(simple_reward_and_termination, no_features)
    env = env.Env(reward_function, feature_function)

    p_field = run_field_creation(model=model, env=env, seg_x=seg_x, seg_y=seg_y)

    return p_field.transpose()


def plot_field(weights, net_config, reward_function, feature_function, seg_x=250, seg_y=250):
    potential_field = start_agent_and_plot(weights=weights, net_config=net_config,
                                           reward_function=reward_function,
                                           feature_function=feature_function,
                                           seg_x=seg_x, seg_y=seg_y)

    plt.imshow(potential_field, cmap='hot')
    plt.show()


def plot_field_by_model(model, env, seg_x=250, seg_y=250):
    p_field = run_field_creation(model=model, env=env, seg_x=seg_x, seg_y=seg_y)

    plt.subplot(411)
    plt.imshow(p_field.transpose(), cmap='hot')
    plt.pause(0.05)


if __name__ == "__main__":
    # _-- load model --_
    from keras.models import Sequential
    from keras.layers import Dense, Activation, Flatten
    from keras.optimizers import RMSprop

    from env_0.reward import simple_reward
    from env_0.features import feature_vec_nodirection

    potential_field = start_agent_and_plot(weights="test_weights_obstacle_2.hdf5",
                                           net_config=(100, 30),
                                           reward_function=simple_reward,
                                           feature_function=feature_vec_nodirection,
                                           seg_x=100, seg_y=100)

    plt.imshow(potential_field, cmap='hot')
    plt.show()
