from keras.models import Sequential
from keras.layers import Dense, Activation, Flatten
from keras.optimizers import Adam

from rl.agents.dqn import DQNAgent
from rl.memory import EpisodeParameterMemory
import rl.policy as policy

from env_0 import env
from env_0.reward import simple_reward_and_termination
from env_0.features import feature_vec


env = env.Env(simple_reward_and_termination, feature_vec)

nb_actions = env.action_space.n
obs_dim = env.observation_space.shape[0]


print env.observation_space.shape

print nb_actions

# initialize keras model

model = Sequential()
model.add(Flatten(input_shape=(1,872,1)))
model.add(Dense(128))
model.add(Activation('relu'))
model.add(Dense(nb_actions))
model.add(Activation('softmax'))

# simple model


print(model.summary())

memory = EpisodeParameterMemory(limit=1000, window_length=0)

policy = policy.LinearAnnealedPolicy(policy.EpsGreedyQPolicy(), attr='eps', value_max=1., value_min=.1, value_test=.05,
                                nb_steps=1000000)

dqn = DQNAgent(model=model, nb_actions=nb_actions, policy=policy, memory=memory,
               batch_size=50, nb_steps_warmup=10000, gamma=0.99, target_model_update=1000, train_interval=4, delta_clip=1.)

dqn.compile(Adam(lr=0.00025), metrics=['mae'])

dqn.fit(env, nb_steps=10000, visualize=False)


# initialize training



# initialize visualisation