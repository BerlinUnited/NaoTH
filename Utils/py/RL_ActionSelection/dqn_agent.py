import numpy as np
from copy import copy, deepcopy

import keras
from keras import backend as K

from ring_buffer import History

"""
class History(object):
    # save s,a,r,s',d
    def __init__(self, length_buffer, time_window=2):
        # make some kind of ring buffer with certain length
        self.length_buffer = length_buffer
        self.time_window = time_window
        self.buffer = [None]*self.length_buffer
        self.input = 0
        self.output = 0
        self.full = False

    def append(self, item):
        self.buffer[self.input] = deepcopy(item)
        self.input += 1
        if self.input == self.length_buffer:
            # give breaf note that buffer is full
            if not self.full:
                print("\t\t!!!Buffer is full, starting to overwrite entries!!!")
                self.full = True
            # set index to start
            self.input = 0
        if self.input > self.length_buffer:
            print("Input index to high!")
            raise IOError

    def sample_output(self):
        # TODO: at some point Non got returned!!
        output_list = []
        try:
            # check if there are None entries ( buffer not filled )
            max = self.buffer.index(None) - 1 # maybe this helps with respect to the to-do
            min = (self.time_window-1)
        except ValueError:
            # if filled set max to length min to zero
            max = self.length_buffer
            min = 0
        max_index = np.random.randint(low=min, high=max)
        # don't mix entries where we add new entries and delete old ones
        while self.input <= max_index < (self.input+self.time_window):
            max_index = np.random.randint(low=min, high=max)
        # output the corresponding frames
        if max_index < self.time_window:
            min_index = max_index - self.time_window
            output_list += self.buffer[min_index:]
            output_list += self.buffer[:max_index]
        else:
            output_list += self.buffer[(max_index-self.time_window):max_index]
        return output_list
"""


def loss_function(y_true, y_pred):
    # mean squard error
    return np.mean(np.square(y_true - y_pred))
    # https://github.com/keras-team/keras/blob/master/keras/losses.py


class DQNAgent(object):
    # reward and feature function are integrated in environment ( env )

    # _-- hyperparameters and spaces --_

    # -- action space --
    # action_space = range(1,13)   # discrete action space

    # -- exploration parameter --
    epsilon = 1.

    # -- learning parameter --
    gamma = 0.90

    max_episode_length = 10000

    training_history = []
    epsilon_history = []
    episodes_history = []

    steps_trained = 0

    epsilons = np.arange(1, 0.05, -0.05)

    def __init__(self, model, env, history, loss, update_step_size, epsilon_decay):
        self.loss = loss
        self.model = model
        self.env = env
        self.history = history

        self.action_space = env.world.action_space

        self.update_step_size = update_step_size

        self.epsilon_decay = epsilon_decay

    def save_weights(self, filepath):
        self.model.save_weights(filepath)

    def update_epsilon(self):
        index = (self.steps_trained / self.epsilon_decay)
        old_epsilon = copy(self.epsilon)
        if index >= len(self.epsilons):
            index = len(self.epsilons) - 1
        self.epsilon = self.epsilons[index]
        if old_epsilon != self.epsilon:
            print("New Epsilon: " + str(self.epsilon))

    def run(self, max_steps, output_format=2):

        print("-" * 20 + "\nSimulation started\n" + "-" * 20)

        print("Training with Epsilon: " + str(self.epsilon))

        self.epsilon_history.append(self.epsilon)

        steps = 1
        episodes = 0
        while True:

            if steps > max_steps:
                break

            done = False
            episode_length = 0

            episodes += 1

            self.state = self.env.reset()
            while not done and episode_length <= self.max_episode_length:

                if steps > max_steps:
                    break

                state = copy(self.state)

                # get features for every action (1,...,5)
                features = []
                for action in self.action_space:
                    # add action to features
                    feature = np.append(self.state, action)
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
                predictions = self.model.predict(features)

                # select best action with epsilon greedy strategy ( integer in {0,...,4} )

                if np.random.uniform(low=0., high=1.) < self.epsilon:
                    selected_action = np.random.randint(low=0, high=len(self.action_space))
                else:
                    selected_action = np.argmax(predictions)

                # make step in environment and get observation, reward, termination status
                next_state, reward, done, _ = self.env.step(selected_action)

                # save transition in history
                self.history.append((state, selected_action, reward, next_state, done))

                # output format specifies what will be displayed in the console
                # 0 - nothing at all
                # 1 - Episodes and steps after each episode
                # 2 - Everything after each step
                if output_format == 2:
                    print("Step: " + str(steps) + ";\t Episode: " + str(
                        episodes) + ";\t Step in Episode: " +
                          str(episode_length))

                episode_length += 1
                steps += 1

                self.state = next_state

            if output_format == 1:
                print("Steps: " + str(steps) + ";\t Episode: " + str(episodes))

        print("Simulated " + str(episodes) + " Episodes in " + str(steps) +
              " steps " + "\n" + "-" * 20)

        self.episodes_history.append(episodes)

    def run_filter(self, max_steps, output_format=2):

        print("-" * 20 + "\nSelective simulation started")

        steps = 0
        episodes = 0
        while True:

            if steps > max_steps:
                break

            done = False
            episode_length = 0

            self.state = self.env.reset()

            state = copy(self.state)

            short_term_history = []

            while not done and episode_length <= self.max_episode_length:

                if steps > max_steps:
                    break

                # get features for every action (1,...,5)
                features = []
                for action in self.action_space:
                    # add action to features
                    feature = np.append(self.state, action)
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
                predictions = self.model.predict(features)

                # select best action with epsilon greedy strategy ( integer in {0,...,4} )
                if np.random.uniform(low=0., high=1.) < self.epsilon:
                    selected_action = np.random.randint(low=0, high=len(self.action_space))
                else:
                    selected_action = np.argmax(predictions)

                # make step in environment and get observation, reward, termination status
                next_state, reward, done, _ = self.env.step(selected_action)

                # save transition in short term history
                short_term_history.append(
                    deepcopy((state, selected_action, reward, next_state, done)))

                self.state = next_state

                # output format specifies what will be displayed in the console
                # 0 - nothing at all
                # 1 - Episodes and steps after each episode
                # 2 - Everything after each step
                if output_format == 2:
                    print("Step: " + str(steps) + ";\t Episode: " + str(
                        episodes) + ";\t Step in Episode: " +
                          str(episode_length))

                episode_length += 1
                # steps += 1

            if done:
                # only add entries of actions led to goal

                episodes += 1
                steps += len(short_term_history)
                for element in short_term_history:
                    self.history.append(element)

                print("Episodes: " + str(episodes) + "Steps: " + str(steps))

            if output_format == 1:
                print("Steps: " + str(steps) + ";\t Episode: " + str(episodes))

            if steps > 10:
                self.env.render(mode=1)

        print("-" * 20 + "\nSimulated " + str(episodes) + " Episodes in " + str(steps) +
              " steps " + "\n" + "-" * 20)

    def learn(self, max_update_iterations, update_size=20, epochs=1, output_format=0):

        update_iterations = 1

        print("Update Properties:\n\n\tNumber of Iterations:\t" + str(
            max_update_iterations) + "\n\n\tBatch Size:\t\t" +
              str(update_size) + "\n\n\tWindow size:\t\t" + str(
            self.history.time_window) + "\n" + "-" * 20)

        while update_iterations <= max_update_iterations:
            # clone current model and update every #update_size steps

            if output_format == 1 or output_format == 2:
                print("\nUpdate Iteration: " + str(update_iterations))

            old_model = keras.models.clone_model(self.model)
            old_model.set_weights(self.model.get_weights())

            samples = []
            # get random sample from history
            for update in range(int(update_size / self.history.time_window)):
                for sample in self.history.sample_output():
                    samples.append(sample)

            x_data = []
            y_data = []
            for sample in samples:
                (state, action, reward, next_state, done) = sample

                # -- reconstruct agent input ( state, action ) for x_data --
                #  same as in run(...)
                x = np.append(state, action)
                # make 2 dim array ( here matrix with 1 row )
                x = np.expand_dims(x, axis=0)
                # transpose to get vektor ( 1 column, multiple rows )
                x = np.transpose(x)
                # dimension adjustment for Keras
                x = np.expand_dims(x, axis=0)
                # append new feature to feature list
                x_data.append(x)

                # -- obtain y_data approximation --
                if done:
                    y = np.array([reward])
                else:
                    features = []
                    for action in self.action_space:
                        # add action to features
                        feature = np.append(next_state, action)
                        # make 2 dim array ( here matrix with 1 row )
                        feature = np.expand_dims(feature, axis=0)
                        # transpose to get vektor ( 1 column, multiple rows )
                        feature = np.transpose(feature)
                        # dimension adjustment for Keras
                        feature = np.expand_dims(feature, axis=0)
                        # append new feature to feature list
                        features.append(feature)

                    # make features ready for model predictions ( get (nb_of_actions, 1, 4, 1) input form )
                    features = np.array(features)

                    # get predictions for the features
                    predictions = old_model.predict(features)

                    # get best best q_value from predictions ( best prediction )
                    max_q_value = np.max(predictions)

                    y = reward + self.gamma * max_q_value

                    y = np.array([y])

                y_data.append(y)

            x_data = np.array(x_data)

            y_data = np.array(y_data)

            #  -- update model --
            fit_data = self.model.fit(x=x_data, y=y_data, batch_size=5, epochs=epochs, verbose=1)

            self.training_history.append(fit_data)

            update_iterations += 1

            print("-" * 20)

        self.update_epsilon()
        self.steps_trained += update_size

        print("-" * 20 + "\nFinished training\n" + "-" * 20)

    def learn_on_data(self, data, epochs, batch_size=10, shuffle=False):
        old_model = keras.models.clone_model(self.model)
        old_model.set_weights(self.model.get_weights())

        x_data = []

        y_data = []

        for step in data:
            [state, action, reward, next_state, done] = step

            # -- reconstruct agent input ( state, action ) for x_data --
            #  same as in run(...)
            x = np.append(state, action)
            # make 2 dim array ( here matrix with 1 row )
            x = np.expand_dims(x, axis=0)
            # transpose to get vektor ( 1 column, multiple rows )
            x = np.transpose(x)
            # dimension adjustment for Keras
            x = np.expand_dims(x, axis=0)
            # append new feature to feature list
            x_data.append(x)

            if done:
                y = np.array([reward])
            else:
                features = []
                for action in self.action_space:
                    # add action to features
                    feature = np.append(next_state, action)
                    # make 2 dim array ( here matrix with 1 row )
                    feature = np.expand_dims(feature, axis=0)
                    # transpose to get vektor ( 1 column, multiple rows )
                    feature = np.transpose(feature)
                    # dimension adjustment for Keras
                    feature = np.expand_dims(feature, axis=0)
                    # append new feature to feature list
                    features.append(feature)

                # make features ready for model predictions ( get (nb_of_actions, 1, 4, 1) input form )
                features = np.array(features)

                # get predictions for the features
                predictions = old_model.predict(features)

                # get best best q_value from predictions ( best prediction )
                max_q_value = np.max(predictions)

                y = reward + self.gamma * max_q_value

                y = np.array([y])

            y_data.append(y)

        x_data = np.array(x_data)

        y_data = np.array(y_data)

        #  -- update model --
        fit_data = self.model.fit(x=x_data, y=y_data, batch_size=batch_size, epochs=epochs,
                                  verbose=1, shuffle=shuffle)

        self.training_history.append(fit_data)

        print("-" * 20)

    def test(self):
        print("-" * 20 + "\nSimulation started\n" + "-" * 20)

        while True:
            done = False
            episode_length = 0

            self.state = self.env.reset()
            while not done and episode_length <= self.max_episode_length:

                state = copy(self.state)

                # get features for every action (1,...,5)
                features = []
                for action in self.action_space:
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
                predictions = self.model.predict(features)

                # select best action ( integer in {0,...,4} )
                selected_action = np.argmax(predictions)

                # make step in environment and get observation, reward, termination status
                next_state, reward, done, _ = self.env.step(selected_action)

                # save transition in history
                self.history.append((state, selected_action, reward, next_state, done))

                episode_length += 1

                self.state = next_state

            self.env.render(mode=None)


# Note: pass in_keras=False to use this function with raw numbers of numpy arrays for testing
def huber_loss(a, b, in_keras=True):
    error = a - b
    quadratic_term = error * error / 2
    linear_term = abs(error) - 1 / 2
    use_linear_term = (abs(error) > 1.0)
    if in_keras:
        # Keras won't let us multiply floats by booleans, so we explicitly cast the booleans to floats
        use_linear_term = K.cast(use_linear_term, 'float32')
        return use_linear_term * linear_term + (1 - use_linear_term) * quadratic_term


def run(reward_function, feature_function, net_config, load_file=None,
        save_file="weights_test.hdf5", start_step=0,
        epsilon_decay=10000):
    from keras.models import Sequential
    from keras.layers import Dense, Activation, Flatten
    from keras.optimizers import RMSprop

    from env_0.naoth.math2d import Vector2 as Vec
    from env_0 import env
    from env_0.state import State
    from env_0.objects import Rectangle

    from time import sleep

    from plot_value_function import plot_field_by_model

    save = True
    load = False
    training_steps = 5
    training_intervals = 400

    feature_shape = feature_function(State()).shape[0]

    history = History(3000, time_window=3)

    model = Sequential()
    model.add(Flatten(input_shape=(1, feature_shape + 1, 1)))  # add one for action input

    for neurons in net_config:
        model.add(Dense(neurons))
        model.add(Activation('relu'))

    model.add(Dense(1))

    model.summary()

    sleep(0.3)  # for better output format, no other reason to wait here

    if load_file is not None:
        print("#" * 20 + "\nLoadig Model: " + str(load_file))
        model.load_weights(load_file)
        print("Model loaded\n" + "#" * 20)

    # model.compile(optimizer=RMSprop(lr=0.00025, rho=0.95, epsilon=0.01), loss='mean_squared_error')
    # model.compile(optimizer=RMSprop(lr=0.00025, rho=0.95, epsilon=0.01), loss=huber_loss)
    model.compile(optimizer=RMSprop(lr=0.00025, rho=0.95, epsilon=0.01), loss=huber_loss)

    # env = env.Env(simple_reward_and_termination, no_features)
    env = env.Env(reward_function, feature_function)

    opp_player_1 = Rectangle(Vec(0, 0), Vec(500, 0), Vec(500, 500), Vec(0, 500))
    opp_player_2 = Rectangle(Vec(3000, -500), Vec(3500, -500), Vec(3500, 0), Vec(3000, 0))

    env.world.add_object(opp_player_1)
    env.world.add_object(opp_player_2)

    agent = DQNAgent(model=model, env=env, history=history, loss=loss_function,
                     update_step_size=30,
                     epsilon_decay=epsilon_decay)

    if start_step is not None:
        agent.steps_trained = start_step
        agent.update_epsilon()

    # plot preparation
    for interval in range(training_intervals):

        for iteration in range(training_steps):
            # some progress printing
            length = int(((iteration / float(training_steps) * 20)))
            output = "[" + "=" * length + ">" + "_" * (20 - (length + 1)) + "]"
            print
            "Iteration: " + str(iteration + 1) + " of " + str(
                training_steps) + ";\t" + "Progress: " + output

            agent.run(max_steps=3000, output_format=0)
            # agent.run_filter(max_steps=5000, output_format=0)

            agent.learn(max_update_iterations=1, update_size=3000, epochs=4, output_format=2)

        print("-" * 20 + "\nSaving Weights: " + save_file)
        agent.save_weights(save_file)
        print("Weights saved\n" + "-" * 20)

        loss_values = []
        for event in agent.training_history:
            loss = event.history['loss'][-1]
            # if loss > 0.01:
            loss_values.append(loss)

        plot_values(loss=loss_values, episodes=agent.episodes_history,
                    epsilon=agent.epsilon_history)

        plot_field_by_model(model=model, env=env, seg_x=250, seg_y=250)


def plot_values(loss, epsilon=None, episodes=None):
    import matplotlib.pyplot as plt

    # TODO: make fancy

    plots = 2
    values_for_ploting = [loss]
    if epsilon is not None:
        values_for_ploting.append(epsilon)
        plots += 1
    if episodes is not None:
        values_for_ploting.append(episodes)
        plots += 1

    for plot, values in enumerate(values_for_ploting):
        plot_number = int(str(plots) + str(1) + str(plot + 2))
        plt.subplot(plot_number)
        plt.plot(values, '-b')

    plt.pause(0.005)


def load_data_run():
    from keras.models import Sequential
    from keras.layers import Dense, Activation, Flatten
    from keras.optimizers import RMSprop

    from env_0 import env
    from env_0.reward import simple_reward
    from env_0.features import no_features, no_features_normalized

    import matplotlib.pyplot as plt

    from time import sleep

    save = True
    load = False

    history = History(30000, time_window=3)

    model = Sequential()
    model.add(Flatten(input_shape=(1, 4, 1)))
    model.add(Dense(800))
    model.add(Activation('relu'))
    model.add(Dense(100))
    model.add(Activation('relu'))
    model.add(Dense(1))
    # model.add(Activation('sigmoid'))

    model.summary()

    sleep(0.3)  # for better output format, no other reason to wait here

    if load:
        print("#" * 20 + "\nLoadig Model")
        model.load_weights('weights_test_pretrain.hdf5')
        print("Model loaded\n" + "#" * 20)

    # model.compile(optimizer=RMSprop(lr=0.00025, rho=0.95, epsilon=0.01), loss='mean_squared_error')
    # model.compile(optimizer=RMSprop(lr=0.00025, rho=0.95, epsilon=0.01), loss=huber_loss)
    model.compile(optimizer=RMSprop(lr=0.00025, rho=0.95, epsilon=0.01), loss=huber_loss)

    # env = env.Env(simple_reward_and_termination, no_features)
    env = env.Env(simple_reward, no_features)

    agent = DQNAgent(model=model, env=env, history=history, loss=loss_function,
                     update_step_size=30)

    # _-- hyperparams and settings --_

    # -- train on saved data --
    for i in range(39):
        print("Iteration: " + str(i + 1))
        filepath = "training_data/filtered_episodes/save" + str(i) + ".npy"

        data = np.load(filepath)
        agent.learn_on_data(data, epochs=4, batch_size=5, shuffle=True)

        loss_values = []
        for event in agent.training_history:
            loss = event.history['loss'][-1]
            # if loss > 0.01:
            loss_values.append(loss)

        plt.plot(loss_values)
        plt.pause(0.05)

        if save:
            # TODO: make ask for filename
            agent.save_weights('weights_test_pretrain_unnormalized.hdf5')
            print("Weights saved\n" + "-" * 20)

    plt.show()


def test_agent(weights, feature_function, reward_function, net_config):
    from keras.models import Sequential
    from keras.layers import Dense, Activation, Flatten
    from keras.optimizers import RMSprop

    from env_0 import env
    from env_0.state import State

    history = History(3000, time_window=3)

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

    agent = DQNAgent(model=model, env=env, history=history, loss=loss_function,
                     update_step_size=30)

    agent.test()


if __name__ == "__main__":
    from env_0.reward import simple_reward, distance_motivated_reward, simple_reward_obstacle
    from env_0.features import no_features, no_features_normalized, feature_vec, \
        feature_vec_nodirection, \
        no_features_no_angle

    net_config = (100, 30)

    # load_data_run()
    # test_agent('test_weights.hdf5', reward_function=simple_reward, feature_function=feature_vec_nodirection,
    #           net_config=net_config)
    run(save_file="test_weights_obstacle_2.hdf5",
        # load_file='weights_100_30_features_simple_reward_trained_1m.hdf5',
        reward_function=simple_reward_obstacle,
        feature_function=feature_vec_nodirection, start_step=0, net_config=net_config,
        epsilon_decay=5000)

    # TODO: add subplots with epsilon and episodes per run iteration
    # TODO: add value function for grid field
    # TODO: add plot for value function

    """
    test_weights.hdf5 configuration:
        865 input
        100 dense
        30  dense
        1 output
        
        reward:  simple_reward
        feature: feature_vec_no_direction
    """
