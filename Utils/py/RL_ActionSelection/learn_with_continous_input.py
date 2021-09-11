from keras.models import Sequential
from keras.layers import Dense, Activation, Flatten
from keras.optimizers import Adam

from rl.agents.dqn import DQNAgent
from rl.memory import EpisodeParameterMemory, Memory, SequentialMemory
import rl.policy as policy

from env_0 import env
from env_0.reward import simple_reward_and_termination
from env_0.features import no_features

from env_0.state import State

env = env.Env(simple_reward_and_termination, no_features)

nb_actions = env.action_space.n

load_weights = True

save_weights = True

test_dqn = True

if test_dqn:
    load_weights = True

# initialize keras model

model = Sequential()
model.add(Flatten(input_shape=(1, 3, 1)))
model.add(Dense(128))
model.add(Activation('relu'))
model.add(Dense(128))
model.add(Activation('relu'))
model.add(Dense(128))
model.add(Activation('relu'))
model.add(Dense(128))
model.add(Activation('relu'))
model.add(Dense(nb_actions))
model.add(Activation('softmax'))

# simple model


print(model.summary())

memory = SequentialMemory(limit=10, window_length=1)

# memory = EpisodeParameterMemory(limit=80, window_length=0)

# policy = policy.LinearAnnealedPolicy(policy.EpsGreedyQPolicy(), attr='eps', value_max=10., value_min=-10., value_test=.05,
#                                nb_steps=300000)

policy = policy.EpsGreedyQPolicy()

dqn = DQNAgent(model=model, nb_actions=nb_actions, policy=policy, memory=memory,
               batch_size=80, nb_steps_warmup=10000, gamma=0.95, target_model_update=400,
               train_interval=400)

if load_weights:
    print("-----#-#-#-#-#-#-#-#----- loading weights -----#-#-#-#-#-#-#-#-----")
    dqn.model.load_weights('test_train.hdf5')
    print("-----#-#-#-#-#-#-#-#----- weights  loaded -----#-#-#-#-#-#-#-#-----")

dqn.compile(Adam(lr=0.01), metrics=['mae'])

if not test_dqn:
    # dqn.fit(env, nb_steps=500000, visualize=False, nb_max_episode_steps=10)
    dqn.fit(env, nb_steps=500000, visualize=False)
else:
    dqn.test(env, visualize=True, nb_max_episode_steps=20)

if save_weights:
    print("\n\t\tsaving weights\n")
    dqn.model.save_weights('test_train.hdf5', overwrite=True)

# initialize training


# initialize visualisation
