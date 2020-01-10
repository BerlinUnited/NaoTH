import numpy as np
from copy import copy, deepcopy


def generate(max_steps, max_episode_length, output_format=2):
    from env_0 import env
    from env_0.reward import simple_reward
    from env_0.features import no_features_normalized

    print("-" * 20 + "\nSimulation started\n" + "-" * 20)

    # -- initialize stuff --
    history = []

    env = env.Env(simple_reward, no_features_normalized)

    steps = 0
    episodes = 0
    added_episodes = 0

    while True:
        episode_history = []

        if steps > max_steps:
            break

        done = False
        episode_length = 0

        episodes += 1

        # if steps > 10:
        #    env.render(mode=1)

        state = env.reset()
        while not done and episode_length <= max_episode_length:

            if steps > max_steps:
                break

            state = copy(state)

            selected_action = np.random.randint(low=0, high=5)

            # make step in environment and get observation, reward, termination status
            next_state, reward, done, _ = env.step(selected_action)

            # save transition in history
            episode_history.append([state, selected_action, reward, next_state, done])

            # output format specifies what will be displayed in the console
            # 0 - nothing at all
            # 1 - Episodes and steps after each episode
            # 2 - Everything after each step
            if output_format == 2:
                print("Step: " + str(steps) + ";\t Episode: " + str(
                    episodes) + ";\t Step in Episode: " +
                      str(episode_length))

            episode_length += 1
            state = next_state
            # steps += 1

        if output_format == 1:
            print("Steps: " + str(steps) + ";\t Episode: " + str(episodes))

        if done:
            history += episode_history
            steps += len(episode_history)
            added_episodes += 1
            print("Added Simulation with " + str(len(episode_history)) + " steps.")

    print("-" * 20 + "\nSimulated " + str(episodes) + " Episodes in " + str(steps) +
          " steps " + "\n" + "-" * 20)
    print("Added " + str(added_episodes) + " Episodes")

    return np.array(history)


def save_stuff(list, filename):
    save_data = np.array(list)
    np.save(file=filename, arr=save_data)
    print("-" * 20 + "\nSaved " + filename)


if __name__ == "__main__":
    start_index = 20
    for i in range(20):
        filename = "save" + str(start_index + i) + ".npy"
        save_stuff(generate(max_steps=50000, max_episode_length=10, output_format=1),
                   filename=filename)
