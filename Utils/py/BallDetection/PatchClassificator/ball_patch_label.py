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


def parse_arguments(argv):
    input_file = ''
    try:
        opts, args = getopt.getopt(argv, "hi:o:", ["ifile="])
    except getopt.GetoptError:
        print('ball_patch_label.py -i <input file>')
        sys.exit(2)
    if not opts:
        print('ball_patch_label.py -i <input file>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print('ball_patch_label.py -i <input file>')
            sys.exit()
        elif opt in ("-i", "--ifile"):
            input_file = arg

    return input_file


class PatchLabeling:
    def __init__(self):
        self.shift_is_held = False
        self.patchdata = None
        self.image_canvas = None
        self.fullscreen = False
        self.window_idx = 0
        self.labels = None
        self.patches = []
        self.selected = []
        self.invalid = []
        self.invalid_selected = []
        self.show_size = (10, 10)  # width, height
        self.patch_size = (24, 24)  # width, height
        for i in range(0, self.show_size[0] * self.show_size[1]):
            y = i // self.show_size[0]
            x = i % self.show_size[0]
            self.patches.append(ptc.Rectangle(self.patch_pos(x, y), width=self.patch_size[0] + 1,
                                              height=self.patch_size[1] + 1, alpha=0.3))
            self.invalid.append(ptc.Rectangle(self.patch_pos(x, y), width=self.patch_size[0] + 1,
                                              height=self.patch_size[1] + 1, color="Red",
                                              alpha=0.3))
            self.selected.append(False)
            self.invalid_selected.append(False)

    def patch_pos(self, x, y):
        return x * (self.patch_size[0] + 1) - 1, y * (self.patch_size[1] + 1) - 1

    def set_marker(self, i, v):
        if v == 0 and self.selected[i]:
            self.patches[i].remove()
            self.selected[i] = False
        elif v == 1 and not self.selected[i]:
            ax = plt.gca()
            ax.add_patch(self.patches[i])
            self.selected[i] = True
        elif v == 0 and self.invalid_selected[i]:
            self.invalid[i].remove()
            self.invalid_selected[i] = False
        elif v == 2 and not self.invalid_selected[i]:
            ax = plt.gca()
            ax.add_patch(self.invalid[i])
            self.invalid_selected[i] = True

    def on_click(self, event):

        if event.xdata is not None and event.ydata is not None:
            y = int(event.ydata + 0.5) / (self.patch_size[1] + 1)
            x = int(event.xdata + 0.5) / (self.patch_size[0] + 1)
            if 0 <= y < self.show_size[1] and 0 <= x < self.show_size[0]:
                i = self.show_size[0] * int(y) + int(x)
                if self.labels[self.window_idx + i] <= 0 and not self.shift_is_held:
                    self.labels[self.window_idx + i] = 1
                elif self.labels[self.window_idx + i] <= 0 and self.shift_is_held:
                    self.labels[self.window_idx + i] = 2
                else:
                    self.labels[self.window_idx + i] = 0

                self.set_marker(i, self.labels[self.window_idx + i])
                plt.draw()

    def key_pressed(self, event):
        for i in range(self.show_size[0] * self.show_size[1]):
            self.set_marker(i, 0)

        if event.key == 'enter' or event.key == ' ' \
                or event.key == 'w' or event.key == 'a' or event.key == 'd' \
                or event.key == 'c' or event.key == 'y' \
                or event.key == '+' or event.key == '-' \
                or event.key == 'shift':

            self.save_labels(label_file)

            idx_step = self.show_size[0] * self.show_size[1]

            if event.key == 'w':
                self.window_idx = 0
            elif event.key == 'a':
                if self.window_idx > 0:
                    self.window_idx -= idx_step
            elif event.key == 'c':
                self.window_idx += idx_step * 10
            elif event.key == 'y':
                if self.window_idx - (idx_step * 10) >= 0:
                    self.window_idx -= idx_step * 10
            elif event.key == '+':  # select all
                end_idx = min(idx_step, len(self.labels) - self.window_idx)
                self.labels[self.window_idx:self.window_idx + end_idx] = 1
            elif event.key == '-':  # deselect all
                end_idx = min(idx_step, len(self.labels) - self.window_idx)
                self.labels[self.window_idx:self.window_idx + end_idx] = 0
            elif event.key == 'shift':  # set invalid label
                self.shift_is_held = True
            else:
                self.window_idx += idx_step
            self.show_patches()
        elif event.key == 'escape' or event.key == 'q':
            exit(0)

    def key_release(self, event):
        if event.key == 'shift':
            self.shift_is_held = False

    def save_labels(self, json_file):
        ball = []
        noball = []
        inval = []
        for i, val in enumerate(self.labels):
            if val == 1:
                ball.append(i)
            elif val == 2:
                inval.append(i)
            elif val == 0:
                noball.append(i)

        with open(json_file, 'w') as outfile:
            json.dump({"ball": ball, "noball": noball, "invalid": inval}, outfile)

    def load_labels(self, json_file):
        # init with invalid label
        # TODO why init with -1 and later in the else branch set everything else to 0?
        tmp_labels = np.negative(np.ones((len(self.patchdata),)))

        if os.path.isfile(json_file):
            with open(json_file, 'r') as data_file:
                ball_labels = json.load(data_file)
            tmp_labels[ball_labels["ball"]] = 1
            if "noball" in ball_labels:
                tmp_labels[ball_labels["noball"]] = 0
            if "invalid" in ball_labels:
                tmp_labels[ball_labels["invalid"]] = 2
            else:
                # set all values to 0 since we have to assume everything unmarked is no ball
                tmp_labels = np.zeros((len(self.patchdata),))

            # TODO this seems unnecessary
            tmp_labels[ball_labels["ball"]] = 1

        return tmp_labels

    def show_patches(self):
        image = np.zeros(((self.patch_size[1] + 1) * self.show_size[1],
                          (self.patch_size[0] + 1) * self.show_size[0]))

        for i in range(self.show_size[0] * self.show_size[1]):
            if self.window_idx + i + 1 > len(self.patchdata):
                break

            p = self.patchdata[self.window_idx + i]
            if len(p) == 4 * self.patch_size[0] * self.patch_size[1]:
                a = np.array(p[0::4]).astype(float)
                a = np.transpose(np.reshape(a, self.patch_size))
            else:
                a = np.array(p).astype(float)
                a = np.transpose(np.reshape(a, self.patch_size))

            y = i // self.show_size[0]
            x = i % self.show_size[0]
            image[y * (self.patch_size[1] + 1):y * (self.patch_size[1] + 1) + self.patch_size[1],
            x * (self.patch_size[0] + 1):x * (self.patch_size[0] + 1) + self.patch_size[0]] = a

            if self.labels[self.window_idx + i] < 0:
                # remember this former invalid column as seen
                self.labels[self.window_idx + i] = 0
            self.set_marker(i, self.labels[self.window_idx + i])

        if self.image_canvas is None:
            self.image_canvas = fig.gca().imshow(image, cmap=plt.get_cmap('gray'),
                                                 interpolation='nearest')
        else:
            self.image_canvas.set_data(image)

        fig.suptitle(str(self.window_idx) + ' - ' + str(
            self.window_idx + self.show_size[0] * self.show_size[1]) + ' / ' + str(
            len(self.patchdata)), fontsize=20)
        fig.canvas.draw()


'''

USAGE:
    python ball_patch_label.py -i game.log

'''
if __name__ == "__main__":
    fig = plt.figure()

    bla = PatchLabeling()
    #logFilePath = "C:/Users/Benji/.naoth/datasets/demo_image/rc17_ball_far.log" #parse_arguments(sys.argv[1:])
    logFilePath = "game.log"  # parse_arguments(sys.argv[1:])

    # type : 0-'Y', 1-'YUV', 2-'YUVC'
    bla.patchdata, _ = patchReader.read_all_patches_from_log(logFilePath, type=2)
    print(len(bla.patchdata))
    print(bla.patchdata[0])

    for i in bla.patchdata:
        print(i.shape)
        a = np.array(i[0::4])
        print(a.shape)

    # load the label file
    label_file = os.path.join(os.path.dirname(logFilePath), 'ball_patch.json')

    bla.labels = bla.load_labels(label_file)

    bla.show_patches()
    plt.connect('button_press_event', bla.on_click)
    plt.connect("key_press_event", bla.key_pressed)
    plt.connect('key_release_event', bla.key_release)

    # fig.gca().xticks(())
    # fig.gca().yticks(())

    if matplotlib.get_backend() == 'Qt4Agg':
        f_manager = plt.get_current_fig_manager()
        f_manager.window.move(0, 0)
        if bla.fullscreen:
            f_manager.window.showMaximized()

    plt.show()
