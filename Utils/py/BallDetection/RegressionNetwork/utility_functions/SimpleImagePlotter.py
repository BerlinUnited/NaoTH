import numpy as np
import matplotlib.pyplot as plt
import matplotlib


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

    def key_pressed(self, event):
        if event.key == 'a' or event.key == 'd':
            idx_step = self.show_size[0] * self.show_size[1]

            if event.key == 'a':
                if self.window_idx > 0:
                    self.window_idx -= idx_step
            elif event.key == 'd':
                self.window_idx += idx_step
            self.show_patches()
        elif event.key == 'escape' or event.key == 'q':
            exit(0)

    def show_patches(self):
        image = np.zeros(((self.patch_size[1] + 1) * self.show_size[1],
                          (self.patch_size[0] + 1) * self.show_size[0]))

        for i in range(self.show_size[0] * self.show_size[1]):
            if self.window_idx + i + 1 > 120:
                break

            a = np.random.rand(self.patch_size[0], self.patch_size[1])

            y = i // self.show_size[0]
            x = i % self.show_size[0]
            image[y * (self.patch_size[1] + 1):y * (self.patch_size[1] + 1) + self.patch_size[1],
            x * (self.patch_size[0] + 1):x * (self.patch_size[0] + 1) + self.patch_size[0]] = a

        if self.image_canvas is None:
            self.image_canvas = fig.gca().imshow(image, cmap=plt.get_cmap('gray'),
                                                 interpolation='nearest')
        else:
            self.image_canvas.set_data(image)

        fig.suptitle(str(self.window_idx) + ' - ' + str(
            self.window_idx + self.show_size[0] * self.show_size[1]) + ' / ' + str(
            120), fontsize=20)
        fig.canvas.draw()


if __name__ == "__main__":
    fig = plt.figure()
    bla = PatchLabeling()

    bla.show_patches()
    plt.connect("key_press_event", bla.key_pressed)

    if matplotlib.get_backend() == 'Qt4Agg':
        f_manager = plt.get_current_fig_manager()
        f_manager.window.move(0, 0)
        if bla.fullscreen:
            f_manager.window.showMaximized()

    plt.show()
