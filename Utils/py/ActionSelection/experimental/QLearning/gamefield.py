from tools import field_info as field

from Tkinter import *
import threading
import time


master = Tk()

# create functions to draw field + decisions made / q values ranging from green to red
# draw field + update field

square_size = 200 # relative to field
field_x = field.x_field_length + 200 # add a few pixels, for better devision
field_y = field.y_field_length + 200 # no consequences for the calculation since outside the gamefield

columns = field_x / square_size
rows = field_y / square_size

square_display_size = 10

board_x = columns * square_display_size
board_y = rows * square_display_size

actions = ["up","down","right","left"]

field = {} # general field i.e. cells and arrows

specials = {} # goals, outlying area

def fill_specials():
    for i in range(columns):
        for j in range(rows):

            # add own goal
            if (3000 <= j * square_size < 4600) and (0 <= i * square_size < 800):
                specials[(i, j)] = (1, "green")
            elif (3000 <= j * square_size < 4600) and (9800 <= i * square_size < 10600):
                specials[(i, j)] = (-1, "red")
            elif j * square_size < 800 or j * square_size >= 6800 or i * square_size < 800 or i * square_size >= 9800:
                specials[(i, j)] = (-0.5, "orange")

fill_specials()


agent_start_position = (5,5)

agent_position = agent_start_position

reset = False

score_min, score_max = -0.2, 0.2

score = 1

board = Canvas(master, width= board_x, height=board_y)

def create_world():
    # create world elements
    global square_display_size, rows, columns, field
    for j in range(rows):
        for i in range(columns):

            field[(i,j)] = []

            # create fields

            # add special fields
            if (i,j) in specials:
                field[(i, j)].append(board.create_rectangle(i * square_display_size, j * square_display_size,
                    (i + 1) * square_display_size, (j + 1) * square_display_size, fill=specials[(i,j)][1], width=1))
                print specials[(i,j)][1]
            # fill everything else white - denoted as neutral
            else:
                field[(i, j)].append(board.create_rectangle(i * square_display_size, j * square_display_size,
                    (i + 1) * square_display_size, (j + 1) * square_display_size, fill="white", width=1))

            # add arrows in fields

            field[(i,j)].append(board.create_line(i * square_display_size + 2, j * square_display_size + 0.5 * square_display_size,
                              (i + 1) * square_display_size - 2 , j * square_display_size + 0.5 * square_display_size,
                              fill="black"))


def update_arrow(coords, action):
    (i, j) = coords
    # update arrow(heads)
    if action == "up":
        board.coords(field[coords][1], i*square_display_size + 0.5*square_display_size, (j+1)*square_display_size,
                          i*square_display_size + 0.5*square_display_size, j*square_display_size)
        board.itemconfigure(field[coords][1],arrow="last")
    elif action == "down":
        board.coords(field[coords][1], i*square_display_size + 0.5*square_display_size, j*square_display_size,
                          i*square_display_size + 0.5*square_display_size, (j+1)*square_display_size)
        board.itemconfigure(field[coords][1],arrow="last")
    elif action == "right":
        board.coords(field[coords][1], i*square_display_size, j*square_display_size + 0.5*square_display_size,
                          (i+1)*square_display_size, j*square_display_size + 0.5*square_display_size)
        board.itemconfigure(field[coords][1],arrow="last")
    elif action == "left":
        board.coords(field[coords][1], (i+1)*square_display_size, j*square_display_size + 0.5*square_display_size,
                          i*square_display_size, j*square_display_size + 0.5*square_display_size)
        board.itemconfigure(field[coords][1],arrow="last")
    else:
        print "no arrow drawn for field: ", (i,j)


def set_cell_color(coords, val):
    global score_min, score_max
    cell = field[coords][0]
    # color setting
    green_dec = int(min(255, max(0, (val - score_min) * 255.0 / (score_max - score_min))))
    green = hex(green_dec)[2:]
    red = hex(255-green_dec)[2:]
    if len(red) == 1:
        red += "0"
    if len(green) == 1:
        green += "0"
    color = "#" + red + green + "00"
    board.itemconfigure(cell, fill=color)


def convert((x,y)):
    # converts field position to cell in the grid
    i = int( x / square_size )
    j = int( y / square_size )
    return (i,j)


def move_agent(absolute = None, relative = None):
    global agent_position
    # set new coords for the agent
    if absolute != None:
        agent_position = absolute
    if relative != None:
        agent_position = (agent_position[0] + relative[0], agent_position[1] + relative[1])
    # display agent at new place
    board.coords(agent_display, agent_position[0] * square_display_size, agent_position[1] * square_display_size,
                 (agent_position[0] + 1) * square_display_size, (agent_position[1] + 1) * square_display_size)


def reset_agent():
    global agent_position, score, agent_display, reset, agent_start_position
    agent_position = agent_start_position
    score = 1 # don't know yet what this is for, remains since it could be forgotten otherwise
    reset = False
    board.coords(agent_display, agent_position[0] * square_display_size, agent_position[1] * square_display_size,
                 (agent_position[0] + 1) * square_display_size, (agent_position[1] + 1) * square_display_size)


def check_infield(coords):
    (i,j) = coords
    if (800 <= j * square_size < 6800) and (800 <= i * square_size < 9800):
        return True
    else:
        return False

# TODO: def check_special for special events like goal or out

def check_special(coords):
    pass

"""
def render_grid():
    global square_display_size, rows, columns
    for i in range(rows):
        for j in range(columns):

            # own and opp goal as green and red fields
            if (3000 <= j*square_size < 4600) and (0 <= i*square_size < 800):
                board.create_rectangle(i * square_display_size, j * square_display_size, (i + 1) * square_display_size,
                                       (j + 1) * square_display_size, fill="red", width=1)
            elif (3000 <= j*square_size < 4600) and (9800 <= i*square_size < 10600):
                board.create_rectangle(i * square_display_size, j * square_display_size, (i + 1) * square_display_size,
                                       (j + 1) * square_display_size, fill="green", width=1)
            # outer field in orange
            elif j*square_size < 800 or j*square_size >= 6800 or i*square_size < 800 or i*square_size >= 9800:
                board.create_rectangle(i * square_display_size, j * square_display_size, (i + 1) * square_display_size,
                                       (j + 1) * square_display_size, fill="orange", width=1)
            # innerfield white
            else:
                board.create_rectangle(i*square_display_size, j*square_display_size, (i+1)*square_display_size,
                    (j+1)*square_display_size, fill="white", width=1)
"""

"""
def draw_arrow(coords,action):
    (i, j) = coords
    if action == "up":
        board.create_line(i*square_display_size + 0.5*square_display_size, (j+1)*square_display_size, i*square_display_size + 0.5*square_display_size, j*square_display_size, fill="black",  arrow="last")
    elif action == "down":
        board.create_line(i*square_display_size + 0.5*square_display_size, j*square_display_size, i*square_display_size + 0.5*square_display_size, (j+1)*square_display_size, fill="black",  arrow="last")
    elif action == "right":
        board.create_line(i*square_display_size, j*square_display_size + 0.5*square_display_size, (i+1)*square_display_size, j*square_display_size + 0.5*square_display_size, fill="black",  arrow="last")
    elif action == "left":
        board.create_line((i+1)*square_display_size, j*square_display_size + 0.5*square_display_size, i*square_display_size, j*square_display_size + 0.5*square_display_size, fill="black",  arrow="last")
    else:
        print "no arrow drawn for field: ", (i,j)
"""

"""
def create_arrows():
    global arrows
    for i in range(rows):
        for j in range(columns):
            if (800 <= j * square_size < 6800) and (800 <= i * square_size < 9800):
                arrows[(i,j)] = board.create_line(i * square_display_size + 2, j * square_display_size + 0.5 * square_display_size,
                                  (i + 1) * square_display_size - 2 , j * square_display_size + 0.5 * square_display_size,
                                  fill="black")
"""


# TODO: add special fields such as goals, opponents

# TODO: add try_move function

def try_move(relative_coords):
    global agent_position, reset
    if reset == True:
        reset_agent() # when should be reset_agent be used ??!!
    (dx ,dy) = relative_coords
    if check_special():
        pass
    if check_infield((agent_position[0] + dx, agent_position[1] + dy)):
        move_agent(relative=(agent_position[0] + dx, agent_position[1] + dy))
    else:
        print "unkown situation \n resetting agent..."
        reset_agent()


create_world()

board.grid(row=0, column=0)

agent_display = board.create_oval(agent_position[0]*square_display_size,agent_position[1]*square_display_size,
                            (agent_position[0]+1)*square_display_size,(agent_position[1]+1)*square_display_size,fill="orange", width=1, tag="agent")


def run():
    global agent_position
    # run routine
    some_number = 0
    while True:
        if check_infield(agent_position):
            move_agent(relative = (2,1*(-1)**some_number))
            update_arrow((agent_position),actions[some_number])
            time.sleep(0.005)
            set_cell_color(agent_position, some_number/10.*(-1)**some_number)
            some_number += 1
            some_number %= 4
        else:
            reset_agent()


t = threading.Thread(target=run)
t.daemon = True
t.start()
master.mainloop()

