#!/usr/bin/python

import os, sys, getopt, math
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as ptc
import matplotlib
import json

import naoth.patchReader as patchReader


# todo make class
patchdata = None
labels = None
window_idx = 0


fig = plt.figure()
image_canvas = None


show_size = (10, 10) # width, height
patch_size = (12, 12) # width, height
fullscreen = False


def patch_pos(x,y):
  return (x*(patch_size[0]+1)-1,y*(patch_size[1]+1)-1)

patches = []
selected = []
for i in range(0,show_size[0]*show_size[1]):
  y = i // show_size[0]
  x = i % show_size[0]
  patches.append(ptc.Rectangle( patch_pos(x,y), width=patch_size[0]+1, height=patch_size[1]+1, alpha=0.3))
  selected.append(False)
  
def setMarker(i, v):
  if v == 0 and selected[i]:
    patches[i].remove()
    selected[i] = False
  elif v == 1 and not selected[i]:
    ax = plt.gca()
    ax.add_patch(patches[i])
    selected[i] = True
  
  
def on_click(event):

  if event.xdata != None and event.ydata != None:
    y = int(event.ydata+0.5) / 13
    x = int(event.xdata+0.5) / 13
    i = show_size[0]*y + x
    
    if labels[window_idx+i] <= 0:
      labels[window_idx+i] = 1
    else:
      labels[window_idx+i] = 0
      
    setMarker(i, labels[window_idx+i])
    plt.draw()
    
 
def key_pressed(event):
  
  if event.key == 'enter' or event.key == ' ' \
    or event.key == 'w'  or event.key == 'a' or event.key == 'd':
    for i in range(show_size[0]*show_size[1]):
      setMarker(i, 0)
      
    save_labels(label_file)
    global window_idx
    
    idxStep = show_size[0]*show_size[1]
    
    if event.key == 'w':
      window_idx = 0
    elif event.key == 'a':
      if window_idx > 0:
        window_idx -= idxStep
    else:
      window_idx += idxStep
    showPatches()
  elif event.key == 'escape' or event.key == 'q':
    exit(0)

def save_labels(file):
  l = []
  noball = []
  for i, val in enumerate(labels):
    if val == 1:
      l.append(i)
    elif val == 0:
      noball.append(i)
    
  with open(file, 'w') as outfile:
        json.dump({"ball":l, "noball":noball}, outfile)    
    
def load_labels(file):
  # init with invalid label
  tmp_labels = np.negative(np.ones((len(patchdata),)))
  
  if os.path.isfile(file):
    with open(file, 'r') as data_file:
      ball_labels = json.load(data_file)
    tmp_labels[ball_labels["ball"]] = 1
    if ball_labels.has_key("noball"):
      tmp_labels[ball_labels["noball"]] = 0
    else:
      # set all values to 0 since we have to assume everything unmarked is no ball
      tmp_labels = np.zeros((len(patchdata),))
      
    tmp_labels[ball_labels["ball"]] = 1
    
  return tmp_labels
    
    
def showPatches():
  image = np.zeros(((patch_size[1]+1)*show_size[1], (patch_size[0]+1)*show_size[0]))
    
  for i in range(show_size[0]*show_size[1]):
    if window_idx+i+1 > len(patchdata):
      break
    
    p = patchdata[window_idx+i]
    if len(p) == 4*12*12:
      a = np.array(p[0::4]).astype(float)
      a = np.transpose(np.reshape(a, patch_size))
    else:
      a = np.array(p).astype(float)
      a = np.transpose(np.reshape(a, patch_size))

    y = i // show_size[0]
    x = i % show_size[0]
    image[y*13:y*13+12,x*13:x*13+12] = a
    if labels[window_idx+i] < 0:
      # remember this former invalid column as seen
      labels[window_idx+i]=0
    setMarker(i, labels[window_idx+i])
    
  global image_canvas
  if image_canvas is None:
    image_canvas = fig.gca().imshow(image, cmap=plt.cm.gray, interpolation='nearest')
  else:
    image_canvas.set_data(image)
  
  fig.suptitle(str(window_idx)+' - '+str(window_idx+show_size[0]*show_size[1])+' / '+str(len(patchdata)), fontsize=20)
  fig.canvas.draw()
    
    
if __name__ == "__main__":
  
  file = 'patches-approach-ball'
  file = 'patches-yuv-ball-move-around'
  if len(sys.argv) > 1:
    file = sys.argv[1]
  #file = 'patches-ball-sidecick'
  
  patchdata = patchReader.readAllPatchesFromLog('./'+file+'.log', type = 0)
  label_file = './'+file+'.json'
  labels = load_labels(label_file)
  
  
  window_idx = 0
  showPatches()
  plt.connect('button_press_event', on_click)
  plt.connect("key_press_event", key_pressed)
  
  #fig.gca().xticks(())
  #fig.gca().yticks(())
      
  if matplotlib.get_backend() == 'Qt4Agg':
    f_manager = plt.get_current_fig_manager()
    f_manager.window.move(0, 0)
    if fullscreen:
      f_manager.window.showMaximized()
  
  plt.show()
  

    
