from __future__ import division
import pickle

# Dummy List for appending all pickle files
dumped_decisions = []

# one pickle file encodes a decision histogram(size 100) for each position
dumped_decisions_1 = pickle.load(open("../data/decision-simulate_every_pos-1-100.pickle", "rb"))
dumped_decisions_2 = pickle.load(open("../data/decision-simulate_every_pos-2-100.pickle", "rb"))
dumped_decisions_3 = pickle.load(open("../data/decision-simulate_every_pos-3-100.pickle", "rb"))
dumped_decisions_4 = pickle.load(open("../data/decision-simulate_every_pos-4-100.pickle", "rb"))
dumped_decisions_5 = pickle.load(open("../data/decision-simulate_every_pos-5-100.pickle", "rb"))
dumped_decisions_6 = pickle.load(open("../data/decision-simulate_every_pos-6-100.pickle", "rb"))
dumped_decisions_7 = pickle.load(open("../data/decision-simulate_every_pos-7-100.pickle", "rb"))
dumped_decisions_8 = pickle.load(open("../data/decision-simulate_every_pos-8-100.pickle", "rb"))
dumped_decisions_9 = pickle.load(open("../data/decision-simulate_every_pos-9-100.pickle", "rb"))
dumped_decisions_10 = pickle.load(open("../data/decision-simulate_every_pos-10-100.pickle", "rb"))
dumped_decisions_11 = pickle.load(open("../data/decision-simulate_every_pos-11-100.pickle", "rb"))
dumped_decisions_20 = pickle.load(open("../data/decision-simulate_every_pos-20-100.pickle", "rb"))
dumped_decisions_30 = pickle.load(open("../data/decision-simulate_every_pos-30-100.pickle", "rb"))
dumped_decisions_40 = pickle.load(open("../data/decision-simulate_every_pos-40-100.pickle", "rb"))
dumped_decisions_50 = pickle.load(open("../data/decision-simulate_every_pos-50-100.pickle", "rb"))
dumped_decisions_60 = pickle.load(open("../data/decision-simulate_every_pos-60-100.pickle", "rb"))
dumped_decisions_70 = pickle.load(open("../data/decision-simulate_every_pos-70-100.pickle", "rb"))
dumped_decisions_80 = pickle.load(open("../data/decision-simulate_every_pos-80-100.pickle", "rb"))
dumped_decisions_90 = pickle.load(open("../data/decision-simulate_every_pos-90-100.pickle", "rb"))


dumped_decisions.append(dumped_decisions_1)
dumped_decisions.append(dumped_decisions_2)
dumped_decisions.append(dumped_decisions_3)
dumped_decisions.append(dumped_decisions_4)
dumped_decisions.append(dumped_decisions_5)
dumped_decisions.append(dumped_decisions_6)
dumped_decisions.append(dumped_decisions_7)
dumped_decisions.append(dumped_decisions_8)
dumped_decisions.append(dumped_decisions_9)
dumped_decisions.append(dumped_decisions_10)
dumped_decisions.append(dumped_decisions_11)
dumped_decisions.append(dumped_decisions_20)
dumped_decisions.append(dumped_decisions_30)
dumped_decisions.append(dumped_decisions_40)
dumped_decisions.append(dumped_decisions_50)
dumped_decisions.append(dumped_decisions_60)
dumped_decisions.append(dumped_decisions_70)
dumped_decisions.append(dumped_decisions_80)
dumped_decisions.append(dumped_decisions_90)

for decisions in dumped_decisions:
    confidence_vector = []

    for pos_dump in decisions:
        new_x, new_y, new_rot, new_decision_histogram = pos_dump
        # print(new_x, new_y, new_rot, new_decision_histogram)

        # if not a totally clear decision
        confidence_vector.append(max(new_decision_histogram) / 100)

        # Evaluate the histogram here - percentage not in main column

    x = [i for i in confidence_vector if i <= 0.5]
    print(len(x))


"""
 when using enough samples in the simulation step, there should'nt be ambiguity in the decision histogram
  for Paper: count the positions on the field that have less or equal than 50% confidence in the decision
"""
