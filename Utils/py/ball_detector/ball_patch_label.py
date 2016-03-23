#!/usr/bin/python

import os, sys, getopt, math
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as ptc

lls = []
idx = 0

selected = []
patches = []
for i in range(0,100):
  y = i // 10
  x = i % 10
  patches.append(ptc.Rectangle( (x*13-1,y*13-1), width=13, height=13, alpha=0.3))
  selected.append(False)
  
  
def on_click(event):

  if event.xdata != None and event.ydata != None:
    y = int(event.ydata+0.5) / 13
    x = int(event.xdata+0.5) / 13
    i = 10*y + x
    ax = plt.gca()
    rect = patches[i]
    
    if not selected[i]:
      ax.add_patch(rect)
      selected[i] = True
    else:
      rect.remove()
      selected[i] = False
      
    plt.draw()

if __name__ == "__main__":
  f = open("./patches.txt", 'r')

  image = np.zeros((13*10, 13*10))
  
  i = 0
  for line in f:
    s = line.split(';')
    a = np.array(s).astype(float)
    a = np.transpose(np.reshape(a, (12,12)))

    if i == 100:
      plt.connect('button_press_event', on_click)
      plt.imshow(image, cmap=plt.cm.gray, interpolation='nearest')
      plt.xticks(())
      plt.yticks(())
      plt.show()
      
      for j in range(0,len(selected)):
        if selected[j]:
          lls.append(idx - 100 + j)
        selected[j] = False
        
      print lls
      i = 0
    
    x = i // 10
    y = i % 10
    image[x*13:x*13+12, y*13:y*13+12] = a

    i += 1
    idx += 1

    