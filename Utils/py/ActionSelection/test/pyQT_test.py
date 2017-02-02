'''
Todo think about the final layout and what I want to with this whole think
For a start: do 2 column layout with left column a pen picker for changing the robots position and show best action and maybe
particle category distribution over time
'''

from __future__ import unicode_literals
import sys
import os
from PyQt5 import QtCore, QtWidgets
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
from matplotlib.patches import Circle
from matplotlib import pyplot as plt
import math
import field_info as field
import tools as tools
import action as a
import potential_field as pf
import math2d as m2d

program_name = os.path.basename(sys.argv[0])
program_version = "0.1"


class MyMplCanvas(FigureCanvas):
    """Ultimately, this is a QWidget (as well as a FigureCanvasAgg, etc.)."""

    def __init__(self, parent=None, width=5, height=4, dpi=100):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = fig.add_subplot(111)

        self.compute_initial_figure()

        FigureCanvas.__init__(self, fig)
        self.setParent(parent)

        FigureCanvas.setSizePolicy(self,
                                   QtWidgets.QSizePolicy.Expanding,
                                   QtWidgets.QSizePolicy.Expanding)
        FigureCanvas.updateGeometry(self)

    def compute_initial_figure(self):
        pass


class FieldViewer(MyMplCanvas):
    """A canvas that updates itself every second with a new plot."""

    def __init__(self, *args, **kwargs):
        MyMplCanvas.__init__(self, *args, **kwargs)
        timer = QtCore.QTimer(self)
        timer.timeout.connect(self.update_figure)
        timer.start(10)

    def compute_initial_figure(self):
        plot_field(self)

    def update_figure(self):
        # Todo: draw animation
        action_consequences = []  # results for all actions and particles
        cat_hist = []  # histogram of results for all actions

        # Simulate Consequences
        for action in action_list:
            categorized_ball_pos_list, hist = simulate_consequences(action, pose, ball_position)

            action_consequences.append(categorized_ball_pos_list)
            cat_hist.append(hist)  # Todo use that somewhere

        # Decide best action
        # best_action = decide_smart(action_consequences, cat_hist)
        # print action_list[best_action].name
        self.axes.cla()
        ax = self.axes
        plot_field(self)

        ax.add_artist(Circle(xy=(1000, 2000), radius=100, fill=False, edgecolor='white'))  # should be robot pos * ball pos
        plot_color = ['ro', 'go', 'bo']
        for idx, action in enumerate(action_consequences):
            for i in range(0, a.num_particles):
                ax.plot(action[i].ball_pos.x, action[i].ball_pos.y, plot_color[idx])

        self.draw()


class ApplicationWindow(QtWidgets.QMainWindow):
    def __init__(self):
        QtWidgets.QMainWindow.__init__(self)
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose)
        self.setWindowTitle("application main window")
        self.showMaximized()  # Todo set min size - looks weird if untoggled

        self.file_menu = QtWidgets.QMenu('&File', self)
        self.file_menu.addAction('&Quit', self.file_quit,
                                 QtCore.Qt.CTRL + QtCore.Qt.Key_Q)
        self.menuBar().addMenu(self.file_menu)

        self.help_menu = QtWidgets.QMenu('&Help', self)
        self.menuBar().addSeparator()
        self.menuBar().addMenu(self.help_menu)
        self.help_menu.addAction('&About', self.about)

        self.main_widget = QtWidgets.QWidget(self)
        self.horizontalGroupBox = QtWidgets.QGroupBox("Grid")
        self.init_ui()
        # self.init_ui2()

    def init_ui(self):
        layout1 = QtWidgets.QVBoxLayout(self.main_widget)

        dc = FieldViewer(self.main_widget, width=5, height=4, dpi=100)
        layout1.addWidget(dc)

        self.main_widget.setFocus()
        self.setCentralWidget(self.main_widget)

    def init_ui2(self):

        layout = QtWidgets.QGridLayout(self.horizontalGroupBox)
        layout.setColumnStretch(1, 4)
        layout.setColumnStretch(2, 4)

        layout.addWidget(QtWidgets.QPushButton('1'))
        layout.addWidget(QtWidgets.QPushButton('2'))
        layout.addWidget(QtWidgets.QPushButton('3'))
        layout.addWidget(QtWidgets.QPushButton('4'))
        layout.addWidget(QtWidgets.QPushButton('5'))
        layout.addWidget(QtWidgets.QPushButton('6'))
        layout.addWidget(QtWidgets.QPushButton('7'))
        layout.addWidget(QtWidgets.QPushButton('8'))
        layout.addWidget(QtWidgets.QPushButton('9'))

        self.horizontalGroupBox.setLayout(layout)
        self.horizontalGroupBox.setFocus()
        #self.setCentralWidget(self.horizontalGroupBox)

    def file_quit(self):
        self.close()

    def closeEvent(self, ce):
        self.file_quit()

    def about(self):
        QtWidgets.QMessageBox.about(self, "About",
                                    """Test for Simulating Kicks with Matplotlib and PyQt5"""
                                )


def plot_field(self):

    ax = self.axes
    ax.plot([0, 0], [-3000, 3000], 'white')  # Middleline
    ax.plot([4500, 4500], [-3000, 3000], 'white')
    ax.plot([-4500, -4500], [-3000, 3000], 'white')

    ax.plot([-4500, 4500], [3000, 3000], 'white')
    ax.plot([-4500, 4500], [-3000, -3000], 'white')

    ax.plot([3900, 3900], [-1100, 1100], 'white')  # opppenalty
    ax.plot([4500, 3900], [1100, 1100], 'white')  # opppenalty
    ax.plot([4500, 3900], [-1100, -1100], 'white')  # opppenalty

    ax.plot([-3900, -3900], [-1100, 1100], 'white')  # ownpenalty
    ax.plot([-4500, -3900], [1100, 1100], 'white')  # ownpenalty
    ax.plot([-4500, -3900], [-1100, -1100], 'white')  # ownpenalty

    # Middle Circle
    ax.add_artist(Circle(xy=(0, 0), radius=750, fill=False, edgecolor='white'))
    # Penalty Marks
    ax.add_artist(Circle(xy=(4500-1300, 0), radius=50, color='white'))
    ax.add_artist(Circle(xy=(-4500+1300, 0), radius=50, color='white'))

    # OwnGoalBox not quite correct - goalpostradius is not considered
    ax.plot([-4500, -5000], [-750, -750], 'white')  # ownpenalty
    ax.plot([-4500, -5000], [750, 750], 'white')  # ownpenalty
    ax.plot([-5000, -5000], [-750, 750], 'white')  # ownpenalty

    # OppGoalBox not quite correct - goalpostradius is not considered
    ax.plot([4500, 5000], [-750, -750], 'white')  # ownpenalty
    ax.plot([4500, 5000], [750, 750], 'white')  # ownpenalty
    ax.plot([5000, 5000], [-750, 750], 'white')  # ownpenalty

    ax.set_xlim([-5200, 5200])  # Todo use FieldInfo Stuff
    ax.set_ylim([-3700, 3700])
    ax.set_axis_bgcolor('green')
    ax.set_aspect("equal")


def simulate_consequences(action, pose, ball_position):  # Todo Check for Collisions with opp goal and if ball is out

    categorized_ball_pos_list = []
    cat_hist = [0]*len(a.Categories)

    for i in range(0, a.num_particles):

        new_ball_pos = action.predict(ball_position)
        new_ball_pos_global = pose * new_ball_pos
        global_ball_pos = pose * ball_position
        shootline = m2d.LineSegment(global_ball_pos, new_ball_pos_global)
        #shootline = sympy.Segment(sympy.Point(global_ball_pos.x, global_ball_pos.y), sympy.Point(new_ball_pos_global.x, new_ball_pos_global.y))

        # if ball is not in field check for collisions with oppGoal and shorten the ball Todo test it
        if shootline.intersection(field.goal_back_side):
            intersection = shootline.intersection(field.goal_back_side)
            print intersection
            #shootline = sympy.Segment(sympy.Point(global_ball_pos.x, global_ball_pos.y), sympy.Point(intersection[0],intersection[1]))
        #elif shootline.intersection(field.goal_left_side):
            #intersection = shootline.intersection(field.goal_left_side)[0]
            #shootline = sympy.Segment(sympy.Point(global_ball_pos.x, global_ball_pos.y), sympy.Point(intersection[0],intersection[1]))
        #elif shootline.intersection(field.goal_right_side):
            #intersection = shootline.intersection(field.goal_right_side)[0]
            #shootline = sympy.Segment(sympy.Point(global_ball_pos.x, global_ball_pos.y), sympy.Point(intersection[0],intersection[1]))

        if shootline.intersection(field.opponent_goal_line):
            category = "OPPGOAL"
        elif -4500 < new_ball_pos_global.x < 4500 and -3000 < new_ball_pos_global.y < 3000:
            category = "INFIELD"
        elif shootline.intersection(field.own_goal_line):
            category = "OWNGOAL"
        else:
            category = "OUT"
        cat_hist[a.Categories.index(category)] += 1
        categorized_ball_pos_list.append(a.CategorizedBallPosition(new_ball_pos_global, category))

    return categorized_ball_pos_list, cat_hist


def decide_smart(action_consequences, cat_hist):  # does not behave like cpp function

    number_infield_pos = 0
    acceptable_actions = []
    goal_actions = []
    # score = 0
    for action in action_consequences:
        for i in range(0, a.num_particles):
            if action[i].category == "OWNGOAL":
                continue
            elif action[i].category == "INFIELD" or action[i].category == "OPPGOAL":
                number_infield_pos += 1

        score = number_infield_pos/30  # 30=number particles
        if score <= max(0.0, a.good_threshold_percentage):
            continue
        acceptable_actions.append(action)

    for action in acceptable_actions:
        if not goal_actions:
            goal_actions.append(action)
            continue

    # Evaluate by potential field
    sum_potential_list = []
    for action in action_consequences:
        sum_potential = 0
        for i in range(0, a.num_particles):
            sum_potential += pf.evaluate_action(action[i].ball_pos.x, action[i].ball_pos.y)

        sum_potential_list.append(sum_potential)

    min_potential = 10000
    min_index = 0
    for i in range(0, len(sum_potential_list)):
        if sum_potential_list[i] < min_potential:
            min_potential = sum_potential_list[i]
            min_index = i
    return min_index

if __name__ == '__main__':
    # Robot Position
    pose = m2d.Pose2D()
    pose.translation = m2d.Vector2(1000, 2000)
    pose.rotation = math.radians(0)
    # Ball Position
    ball_position = m2d.Vector2()
    ball_position.x = 0.0
    ball_position.y = 0.0

    sidekick_right = a.Action("sidekick_right", 750, 150, -89.657943335302260, 10.553726275058064)
    sidekick_left = a.Action("sidekick_left", 750, 150, 86.170795364136380, 10.669170653645670)
    kick_short = a.Action("kick_short", 780, 150, 8.454482265522328, 6.992268841997358)

    action_list = [sidekick_right, sidekick_left, kick_short]

    qApp = QtWidgets.QApplication(sys.argv)

    aw = ApplicationWindow()
    aw.setWindowTitle("%s" % program_name)
    aw.show()
    sys.exit(qApp.exec_())
    # qApp.exec_()
