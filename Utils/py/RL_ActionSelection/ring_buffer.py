from copy import deepcopy
import numpy as np


class History(object):
    # save s,a,r,s',d
    def __init__(self, length_buffer, time_window=2):
        # make some kind of ring buffer with certain length
        self.length_buffer = length_buffer
        self.time_window = time_window
        self.buffer = [None] * self.length_buffer
        self.input = 0
        self.output = 0
        self.full = False

    def append(self, item):
        self.buffer[self.input] = deepcopy(item)
        self.input += 1
        if self.input == self.length_buffer:
            # give breaf note that buffer is full
            if not self.full:
                print("\t\t!!!Buffer is full, starting to overwrite entries!!!")
                self.full = True
            # set index to start
            self.input = 0
        if self.input > self.length_buffer:
            print("Input index to high!")
            raise IOError

    def sample_output(self):
        # TODO: at some point Non got returned!!
        output_list = []
        try:
            # check if there are None entries ( buffer not filled )
            max = self.buffer.index(None) - 1  # maybe this helps with respect to the to-do
            min = (self.time_window - 1)
        except ValueError:
            # if filled set max to length min to zero
            max = self.length_buffer
            min = 0
        max_index = np.random.randint(low=min, high=max)
        # don't mix entries where we add new entries and delete old ones
        while self.input <= max_index < (self.input + self.time_window):
            max_index = np.random.randint(low=min, high=max)
        # output the corresponding frames
        if max_index < self.time_window:
            min_index = max_index - self.time_window
            output_list += self.buffer[min_index:]
            output_list += self.buffer[:max_index]
        else:
            output_list += self.buffer[(max_index - self.time_window):max_index]
        return output_list
