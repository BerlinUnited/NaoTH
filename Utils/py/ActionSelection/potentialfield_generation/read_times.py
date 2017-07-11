import csv
import matplotlib.pyplot as plt
from numpy import concatenate,array,ones,linalg,vstack, append


with open('data_sort_y.csv', 'rt') as csvfile:
    spamreader = csv.reader(csvfile, delimiter=',')

    x_vals = [[]]
    times = [[]]
    y_vals = []
    spamreader.next() #skipping header
    first_row = spamreader.next()
    y = first_row[3]

    x_vals[0].append(first_row[2])
    times[0].append(first_row[0])
    y_vals.append(y)
    count = 0
    for row in spamreader:
        if y == row[3]:
            if float(row[0]) < 100.:
                x_vals[count].append(row[2])
                times[count].append(row[0])
        else:
            count += 1
            y = row[3]
            y_vals.append(y)
            x_vals.append([])
            times.append([])
            if float(row[0]) < 100.:
                x_vals[count].append(row[2])
                times[count].append(row[0])

    for i in range(len(times)):
        plt.figure(1)
        plt.plot(x_vals[i], times[i], label = y_vals[i])
        plt.legend()


    for i in range(len(times)):
        for j in range(len(times[i])):
            times[i][j] = float(times[i][j])
            x_vals[i][j] = float(x_vals[i][j])
            y_vals[i] = float(y_vals[i])

    weights = [] # lineare regression durchfuehren
    for i in range(len(times)):
        x = array(x_vals[i])
        A = vstack([x, ones(len(x))]).T
        y = array(times[i])
        # linearly generated sequence
        w = linalg.lstsq(A, y)[0]  # obtaining the parameters
        # y = mx + c -- m first c second value
        weights.append(w.tolist())

    print weights

    for i in range(len(weights)):
        plt.figure(2)
        y = []
        for j in range(len(x_vals[i])):
            y.append(weights[i][0]*x_vals[i][j] + weights[i][1])
        plt.plot(x_vals[i], y, label=y_vals[i])
        plt.legend()

    plt.show()






