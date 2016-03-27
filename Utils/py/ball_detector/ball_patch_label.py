#!/usr/bin/python

import os, sys, getopt, math
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as ptc
import matplotlib

import patchReader

lls = []
idx = 0

show_size = (10, 10) # width, height
patch_size = (12, 12) # width, height
fullscreen = False


def patch_pos(x,y):
  return (x*(patch_size[0]+1)-1,y*(patch_size[1]+1)-1)

  
selected = []
patches = []
for i in range(0,show_size[0]*show_size[1]):
  y = i // show_size[0]
  x = i % show_size[0]
  patches.append(ptc.Rectangle( patch_pos(x,y), width=patch_size[0]+1, height=patch_size[1]+1, alpha=0.3))
  selected.append(False)
  
  
def on_click(event):

  if event.xdata != None and event.ydata != None:
    y = int(event.ydata+0.5) / 13
    x = int(event.xdata+0.5) / 13
    i = show_size[0]*y + x
    ax = plt.gca()
    rect = patches[i]
    
    if not selected[i]:
      ax.add_patch(rect)
      selected[i] = True
    else:
      rect.remove()
      selected[i] = False
      
    plt.draw()
 
def key_pressed(event):
  
  if event.key == 'enter':
    plt.close()
  elif event.key == 'escape' or event.key == 'q':
    exit(0)

def load_from_csv(file):
  f = open(file, 'r')
  patches = []
  
  for line in f:
    s = line.split(';')
    a = np.array(s).astype(float)
    patches.append(a)
    
if __name__ == "__main__":
  #patches = load_from_csv("./patches.txt")
  patchdata = patchReader.readAllPatchesFromLog('./ball-move-around-patches.log')
  
  image = np.zeros(((patch_size[1]+1)*show_size[1], (patch_size[0]+1)*show_size[0]))

  i = 0
  for p in patchdata:
    a = np.array(p).astype(float)
    a = np.transpose(np.reshape(a, patch_size))

    if i == show_size[0]*show_size[1]:
      plt.connect('button_press_event', on_click)
      plt.connect("key_press_event", key_pressed)
      plt.imshow(image, cmap=plt.cm.gray, interpolation='nearest')
      plt.xticks(())
      plt.yticks(())
      
      if matplotlib.get_backend() == 'Qt4Agg':
        f_manager = plt.get_current_fig_manager()
        f_manager.window.move(0, 0)
        if fullscreen:
          f_manager.window.showMaximized()
      
      plt.show()
      
      for j in range(0,len(selected)):
        if selected[j]:
          lls.append(idx - show_size[0]*show_size[1] + j)
        selected[j] = False
        
      print lls
      i = 0
    
    y = i // show_size[0]
    x = i % show_size[0]
    image[y*13:y*13+12,x*13:x*13+12] = a

    i += 1
    idx += 1

    
