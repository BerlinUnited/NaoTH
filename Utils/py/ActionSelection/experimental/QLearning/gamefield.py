from tools import field_info as field

from Tkinter import *
import threading
import time




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

actions = ["up","down","right","left"]

arrows = {}

agent_position = (5,5)

board = Canvas(master, width= board_x, height=board_y)

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

def draw_arrow(i,j,action):
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

def create_arrows():
    global arrows
    for i in range(rows):
        for j in range(columns):
            if (800 <= j * square_size < 6800) and (800 <= i * square_size < 9800):
                arrows[(i,j)] = board.create_line(i * square_display_size, j * square_display_size + 0.5 * square_display_size,
                                  (i + 1) * square_display_size, j * square_display_size + 0.5 * square_display_size,
                                  fill="black", arrow="last")

def update_arrow(coords, action):
    global arrows
    i = coords[0]
    j = coords[1]
    if action == "up":
        board.coords(arrows[coords], i*square_display_size + 0.5*square_display_size, (j+1)*square_display_size,
                          i*square_display_size + 0.5*square_display_size, j*square_display_size)
    elif action == "down":
        board.coords(arrows[coords], i*square_display_size + 0.5*square_display_size, j*square_display_size,
                          i*square_display_size + 0.5*square_display_size, (j+1)*square_display_size)
    elif action == "right":
        board.coords(arrows[coords], i*square_display_size, j*square_display_size + 0.5*square_display_size,
                          (i+1)*square_display_size, j*square_display_size + 0.5*square_display_size)
    elif action == "left":
        board.coords(arrows[coords], (i+1)*square_display_size, j*square_display_size + 0.5*square_display_size,
                          i*square_display_size, j*square_display_size + 0.5*square_display_size)
    else:
        print "no arrow drawn for field: ", (i,j)



# add color updater

render_grid()

board.grid(row=0, column=0)

create_arrows()
agent_display = board.create_oval(agent_position[0]*square_display_size,agent_position[1]*square_display_size,
                            (agent_position[0]+1)*square_display_size,(agent_position[1]+1)*square_display_size,fill="orange", width=1, tag="agent")

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

def run():
    global agent_position
    while True:
        if agent_position[0] < 50:
            move_agent(relative = (1,0))
            update_arrow((agent_position),"up")
            time.sleep(0.1)
        else:
            move_agent(absolute = (5,5))



t = threading.Thread(target=run)
t.daemon = True
t.start()
master.mainloop()

