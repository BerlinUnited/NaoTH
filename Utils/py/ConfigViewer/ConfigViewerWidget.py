import os, configparser

from PyQt5.QtCore import Qt
from PyQt5.QtGui import QStandardItemModel, QStandardItem

import ConfigViewerUi
from PyQt5.QtWidgets import QWidget, QStyle


class Widget(QWidget):

    def __init__(self, config:str):
        super().__init__()

        self.dirs = {
            'general':  'general/',
            'platform': 'platform/',
            'scheme':   'scheme/',
            'robots':   'robots/',
            'robots_bodies': 'robots_bodies/',
            'robot_heads':  'robot_heads/',
            'private':  'private/',
        }

        self.ui = ConfigViewerUi.Ui_Form()
        self.ui.setupUi(self)

        self.ui.platforms.currentIndexChanged.connect(self.__update_modules)
        self.ui.schemes.currentIndexChanged.connect(self.__update_modules)
        self.ui.robots.currentIndexChanged.connect(self.__update_modules)
        self.ui.bodies.currentIndexChanged.connect(self.__update_modules)
        self.ui.heads.currentIndexChanged.connect(self.__update_modules)

        self.setWindowTitle("Modules Config Viewer")
        self.setWindowIcon(self.style().standardIcon(QStyle.SP_FileDialogContentsView))
        self.setAcceptDrops(True)

        self.__init_config(config)

    def __init_config(self, config):

        self.config = config + "/" if config[-1] != "/" else ""
        self.scheme = None

        self.__reset_ui()
        self.__read_scheme_file()
        self.__read_config_directory()
        self.__update_modules()

        self.ui.config_dir.setText(self.config)

    def __reset_ui(self):
        # enable everything
        self.ui.platforms.setEnabled(True)
        self.ui.schemes.setEnabled(True)
        self.ui.robots.setEnabled(True)
        self.ui.bodies.setEnabled(True)
        self.ui.heads.setEnabled(True)
        self.ui.heads.setEnabled(True)
        self.ui.modules.setEnabled(True)
        # clear everything
        self.ui.platforms.clear()
        self.ui.schemes.clear()
        self.ui.robots.clear()
        self.ui.bodies.clear()
        self.ui.heads.clear()
        self.ui.config_dir.clear()
        # set defaults
        self.ui.platforms.addItem("Platform")
        self.ui.schemes.addItem("Scheme")
        self.ui.robots.addItem("Robots")
        self.ui.bodies.addItem("Bodies")
        self.ui.heads.addItem("Heads")

    def __read_scheme_file(self):
        if os.path.isfile(self.config + "scheme.cfg"):
            with open(self.config + "scheme.cfg", "r") as f:
                self.scheme = f.readline().strip()

    def __read_config_directory(self):
        if os.path.isdir(self.config + self.dirs['platform']):
            self.ui.platforms.addItems(sorted(os.listdir(self.config + self.dirs['platform'])))
        self.__setComboboxDisabled(self.ui.platforms)

        if os.path.isdir(self.config + self.dirs['scheme']):
            self.ui.schemes.addItems(sorted(os.listdir(self.config + self.dirs['scheme'])))
            if self.scheme:
                self.ui.schemes.setCurrentIndex(self.ui.schemes.findText(self.scheme))
        self.__setComboboxDisabled(self.ui.schemes)

        if os.path.isdir(self.config + self.dirs['robots']):
            self.ui.robots.addItems(sorted(os.listdir(self.config + self.dirs['robots'])))
        self.__setComboboxDisabled(self.ui.robots)

        if os.path.isdir(self.config + self.dirs['robots_bodies']):
            self.ui.bodies.addItems(sorted([e for e in os.scandir(self.config + self.dirs['robots_bodies']) if e.is_dir()]))
        self.__setComboboxDisabled(self.ui.bodies)

        if os.path.isdir(self.config + self.dirs['robot_heads']):
            self.ui.heads.addItems(sorted([ e for e in os.scandir(self.config + self.dirs['robot_heads']) if e.is_dir()]))
        self.__setComboboxDisabled(self.ui.heads)


    def __setComboboxDisabled(self, box):
        if box.count() <= 1:
            box.setDisabled(True)

    def __update_modules(self):
        self.ui.modules.setEnabled(False)

        config = configparser.ConfigParser()

        self.__update_modules_part(config, 'general')
        self.__update_modules_part(config, 'platform', self.ui.platforms)
        self.__update_modules_part(config, 'scheme', self.ui.schemes)
        self.__update_modules_part(config, 'robots', self.ui.robots)
        self.__update_modules_part(config, 'robots_bodies', self.ui.bodies)
        self.__update_modules_part(config, 'robot_heads', self.ui.heads)
        self.__update_modules_part(config, 'private')

        model = QStandardItemModel()
        parent_item = model.invisibleRootItem()  # type: QStandardItem
        if config.has_section('modules'):
            for m in sorted(config.options('modules')):
                text_item = QStandardItem(m)
                text_item.setEditable(False)
                text_item.setCheckState(Qt.Checked if config.getboolean('modules', m) else Qt.Unchecked)
                parent_item.appendRow(text_item)

        model.setHeaderData(0, Qt.Horizontal, "Modules")
        self.ui.modules.setModel(model)

        self.ui.modules.setEnabled(True)

    def __update_modules_part(self, config, dir, box = None):
        subdir = box.currentText() + os.sep if box else ""
        if os.path.isfile(self.config + self.dirs[dir] + subdir + "modules.cfg"):
            config.read(self.config + self.dirs[dir] + subdir + "modules.cfg")


    def dragEnterEvent(self, e):
        # only one item drop allowed
        if e.mimeData().hasUrls() and len(e.mimeData().urls()) == 1:
            # only directories are accepted
            for url in e.mimeData().urls():
                if url.isLocalFile() and os.path.isdir(url.toLocalFile()):
                    e.accept()
        else:
            e.ignore()

    def dropEvent(self, e):
        # sanity checks
        if e.mimeData().hasUrls() and len(e.mimeData().urls()) == 1:
            # update ui and read file
            self.__init_config(e.mimeData().urls()[0].toLocalFile())
