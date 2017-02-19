import numpy as np
import matplotlib.pyplot as plt


num_kicks = (33, 23, 76, 139, 151, 168, 132, 121, 121, 96, 31, 6, 2)
N = len(num_kicks)
ind = np.arange(N)  # the x locations for the groups
width = 0.35       # the width of the bars

fig, ax = plt.subplots()
rects1 = ax.bar(ind, num_kicks, width, color='r')


# add some text for labels, title and axes ticks
ax.set_ylabel('Count')
ax.set_title('Number of Kicks until goal')
ax.set_xticks(ind + width / 2)
ax.set_xticklabels(('Error', '1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12'))


def autolabel(rects):
    """
    Attach a text label above each bar displaying its height
    """
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x() + rect.get_width()/2., 1.05*height,
                '%d' % int(height),
                ha='center', va='bottom')

autolabel(rects1)

plt.show()
