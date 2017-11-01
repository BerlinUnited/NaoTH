from tools import field_info as field

from Tkinter import *
import threading



master = Tk()


# create gridworld out of field data

# create functions to draw field + decisions made / q values ranging from green to red
# draw field + update field

square_size = 200 # relative to field
field_x = field.x_field_length + 200
field_y = field.y_field_length + 200

rows = field_x / square_size
columns = field_y / square_size

square_display_size = 10

board_x = rows * square_display_size
board_y = columns * square_display_size

board = Canvas(master, width= board_x, height=board_y)

def render_grid():
    global square_display_size, rows, columns
    for i in range(rows):
        for j in range(columns):
            if j*square_size < 800 or j*square_size >= 6800 or i*square_size < 800 or i*square_size >= 9800:
                board.create_rectangle(i * square_display_size, j * square_display_size, (i + 1) * square_display_size,
                                       (j + 1) * square_display_size, fill="orange", width=1)
            else:
                board.create_rectangle(i*square_display_size, j*square_display_size, (i+1)*square_display_size,
                    (j+1)*square_display_size, fill="white", width=1)

    # add own and opponent goal

# add color updater

render_grid()

board.grid(row=0, column=0)


master.mainloop()

