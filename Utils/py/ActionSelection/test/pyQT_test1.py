import sys
from PyQt5.QtWidgets import QApplication, QMainWindow, QMenu, QVBoxLayout, QSizePolicy, QMessageBox, QWidget, QPushButton
from PyQt5.QtGui import QIcon
from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.figure import Figure
import matplotlib.pyplot as plt
from matplotlib.patches import Circle
import random


class App(QMainWindow):

    def __init__(self):
        super(App, self).__init__()
        self.left = 10
        self.top = 10
        self.title = 'PyQt5 matplotlib example - pythonspot.com'
        self.width = 640
        self.height = 400
        self.initUI()

    def initUI(self):
        self.setWindowTitle(self.title)
        self.setGeometry(self.left, self.top, self.width, self.height)

        m = PlotCanvas(self, width=5, height=4)
        m.move(0, 0)

        button = QPushButton('PyQt5 button', self)
        button.setToolTip('This s an example button')
        button.move(500, 0)
        button.resize(140, 100)

        self.show()


class PlotCanvas(FigureCanvas):

    def __init__(self, parent=None, width=5, height=4, dpi=100):
        fig = Figure(figsize=(width, height), dpi=dpi)
        self.axes = fig.add_subplot(111)

        FigureCanvas.__init__(self, fig)
        self.setParent(parent)

        FigureCanvas.setSizePolicy(self, QSizePolicy.Expanding, QSizePolicy.Expanding)
        FigureCanvas.updateGeometry(self)

        self.plot_field()

    def plot_field(self):
        ax = self.figure.add_subplot(111)

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


if __name__ == '__main__':
    app = QApplication(sys.argv)
    ex = App()
    sys.exit(app.exec_())
