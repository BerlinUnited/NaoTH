#!/usr/bin/python

import os
import sys
import getopt
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as ptc
import matplotlib
import json
import patchReader as patchReader

# todo make class
patchdata = None
labels = None
window_idx = 0

fig = plt.figure()
image_canvas = None

show_size = (10, 10)    # width, height
patch_size = (24, 24)    # width, height
fullscreen = False


def parse_arguments(argv):
    input_file = ''
    try:
        opts, args = getopt.getopt(argv, "hi:o:", ["ifile="])
    except getopt.GetoptError:
        print('ball_patch_label.py -i <input file>')
        sys.exit(2)
    if opts == []:
        print('ball_patch_label.py -i <input file>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print('ball_patch_label.py -i <input file>')
            sys.exit()
        elif opt in ("-i", "--ifile"):
            input_file = arg

    return input_file


def patch_pos(x, y):
    return x*(patch_size[0]+1)-1, y*(patch_size[1]+1)-1

patches = []
selected = []
for i in range(0, show_size[0]*show_size[1]):
    y = i // show_size[0]
    x = i % show_size[0]
    patches.append(ptc.Rectangle(patch_pos(x, y), width=patch_size[0]+1, height=patch_size[1]+1, alpha=0.3))
    selected.append(False)


def set_marker(i, v):
    if v == 0 and selected[i]:
        patches[i].remove()
        selected[i] = False
    elif v == 1 and not selected[i]:
        ax = plt.gca()
        ax.add_patch(patches[i])
        selected[i] = True
    
    
def on_click(event):

    if event.xdata is not None and event.ydata is not None:
        y = int(event.ydata+0.5) / (patch_size[1]+1)
        x = int(event.xdata+0.5) / (patch_size[0]+1)
        
        if 0 <= y < show_size[1] and 0 <= x < show_size[0]:
            i = show_size[0]*y + x
            
            if labels[window_idx+i] <= 0:
                labels[window_idx+i] = 1
            else:
                labels[window_idx+i] = 0
                
            set_marker(i, labels[window_idx + i])
            plt.draw()
        
 
def key_pressed(event):
    
    for i in range(show_size[0]*show_size[1]):
            set_marker(i, 0)
            
    if event.key == 'enter' or event.key == ' ' \
            or event.key == 'w' or event.key == 'a' or event.key == 'd' \
            or event.key == 'c' or event.key == 'y' \
            or event.key == '+' or event.key == '-':
            
        save_labels(label_file)
        global window_idx
        
        idx_step = show_size[0]*show_size[1]
        
        if event.key == 'w':
            window_idx = 0
        elif event.key == 'a':
            if window_idx > 0:
                window_idx -= idx_step
        elif event.key == 'c':
            window_idx += idx_step*10
        elif event.key == 'y':
            if window_idx - (idx_step*10) >= 0:
                window_idx -= idx_step*10
        elif event.key == '+':  # select all
            end_idx = min(idx_step, len(labels) - window_idx)
            labels[window_idx:window_idx+end_idx] = 1
        elif event.key == '-':  # deselect all
            end_idx = min(idx_step, len(labels) - window_idx)
            labels[window_idx:window_idx+end_idx] = 0
        else:
            window_idx += idx_step
        show_patches()
    elif event.key == 'escape' or event.key == 'q':
        exit(0)


def save_labels(json_file):
    l = []
    noball = []
    for i, val in enumerate(labels):
        if val == 1:
            l.append(i)
        elif val == 0:
            noball.append(i)
        
    with open(json_file, 'w') as outfile:
                json.dump({"ball": l, "noball": noball}, outfile)


def load_labels(json_file):
    # init with invalid label
    tmp_labels = np.negative(np.ones((len(patchdata),)))
    
    if os.path.isfile(json_file):
        with open(json_file, 'r') as data_file:
            ball_labels = json.load(data_file)
        tmp_labels[ball_labels["ball"]] = 1
        if "noball" in ball_labels:
            tmp_labels[ball_labels["noball"]] = 0
        else:
            # set all values to 0 since we have to assume everything unmarked is no ball
            tmp_labels = np.zeros((len(patchdata),))
            
        tmp_labels[ball_labels["ball"]] = 1
        
    return tmp_labels
    
        
def show_patches():
    image = np.zeros(((patch_size[1]+1)*show_size[1], (patch_size[0]+1)*show_size[0]))
        
    for i in range(show_size[0]*show_size[1]):
        if window_idx+i+1 > len(patchdata):
            break
        
        p = patchdata[window_idx+i]
        if len(p) == 4*patch_size[0]*patch_size[1]:
            a = np.array(p[0::4]).astype(float)
            a = np.transpose(np.reshape(a, patch_size))
        else:
            a = np.array(p).astype(float)
            a = np.transpose(np.reshape(a, patch_size))

        y = i // show_size[0]
        x = i % show_size[0]
        image[y*(patch_size[1]+1):y*(patch_size[1]+1)+patch_size[1], x*(patch_size[0]+1):x*(patch_size[0]+1)+patch_size[0]] = a
        if labels[window_idx+i] < 0:
            # remember this former invalid column as seen
            labels[window_idx+i] = 0
        set_marker(i, labels[window_idx + i])
        
    global image_canvas
    if image_canvas is None:
        image_canvas = fig.gca().imshow(image, cmap=plt.get_cmap('gray'), interpolation='nearest')
    else:
        image_canvas.set_data(image)
    
    fig.suptitle(str(window_idx)+' - '+str(window_idx+show_size[0]*show_size[1])+' / '+str(len(patchdata)), fontsize=20)
    fig.canvas.draw()
        

'''

USAGE:
    python ball_patch_label.py -i ball-move-around-patches.log

'''
if __name__ == "__main__":
    logFilePath = parse_arguments(sys.argv[1:])

    # type: 0-'Y', 1-'YUV', 2-'YUVC'
    patchdata, _ = patchReader.read_all_patches_from_log(logFilePath, type=2)

    # load the label file
    base_file, file_extension = os.path.splitext(logFilePath)
    label_file = base_file+'.json'
    labels = load_labels(label_file)

    window_idx = 0
    show_patches()
    plt.connect('button_press_event', on_click)
    plt.connect("key_press_event", key_pressed)

    # fig.gca().xticks(())
    # fig.gca().yticks(())

    if matplotlib.get_backend() == 'Qt4Agg':
        f_manager = plt.get_current_fig_manager()
        f_manager.window.move(0, 0)
        if fullscreen:
            f_manager.window.showMaximized()

    plt.show()
