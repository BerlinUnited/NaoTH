from keras.models import Sequential
from keras.layers import Dense

import numpy as np
from state import State


class agent():
    memory = []

    # hyper parameter
    epsilon = 1.0
    epsilon_decay = 0.9
    epsilon_min = 0.01

    gamma = 0.95

    learning_rate = 0.001



    # initialize keras model

    def __init__(self, feature, action_space_size, load_model=False):
        self.action_space_size = action_space_size

        self.gen_feature = feature #  function for generating the used features
        # has two outputs; the feature vector and the dimensions of the 'sub feature vectors'

        if not callable(self.gen_feature):
            raise TypeError

        self.feature_size = self.gen_feature(State())[0].shape[1]
        self._init_model()


    def _init_model(self, load_model=False):
        self.model = Sequential()

        self.model.add(Dense(units=128, input_dim=self.feature_size, activation='relu'))

        self.model.add(Dense(units=self.action_space_size, activation='softmax'))

        self.model.compile(loss='mse', optimizer='adam')


    def act(self, state):
        if np.random.rand() <= self.epsilon:
            return np.random.randint(0,self.action_space_size)

        try:
            features, _ = self.gen_feature(state)
            action_values = self.model.predict(features)
        except NameError:
            print "The model was not initialized ... \n\
                    or something else went wrong while predicting the next action."

        return np.argmax(action_values[0])

    def remember(self, state, action, reward, next_state, done):
        self.memory.append((state, action, reward, next_state, done))


if __name__ == "__main__":
    import features
    Agent = agent(features.feature_vec)
    Agent.init_model()
    for i in range(10):
        print(Agent.act(State()))
