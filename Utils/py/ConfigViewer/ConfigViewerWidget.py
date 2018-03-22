import glob, json, collections
import os, configparser

from PyQt5.QtCore import Qt
from PyQt5.QtGui import QStandardItemModel, QStandardItem, QFont

import ConfigViewerUi
from PyQt5.QtWidgets import QWidget, QStyle


class Widget(QWidget):

    def __init__(self, config:str):
        super().__init__()

        # directories to read and if they have subdirectories
        self.dirs = {
            'general':      ( 'general/',        False ),
            'platform':     ( 'platform/',       True  ),
            'scheme':       ( 'scheme/',         True  ),
            'robots':       ( 'robots/',         True  ),
            'robots_bodies':( 'robots_bodies/',  True  ),
            'robot_heads':  ( 'robot_heads/',    True  ),
            'private':      ( 'private/',        False ),
        }

        self.ui = ConfigViewerUi.Ui_Form()
        self.ui.setupUi(self)

        # read all config files
        self.__init_config(config)

        # register callbacks
        self.ui.platforms.currentIndexChanged.connect(self.__update_tree)
        self.ui.schemes.currentIndexChanged.connect(self.__update_tree)
        self.ui.robots.currentIndexChanged.connect(self.__update_tree)
        self.ui.bodies.currentIndexChanged.connect(self.__update_tree)
        self.ui.heads.currentIndexChanged.connect(self.__update_tree)

        self.setWindowTitle("Modules Config Viewer")
        self.setWindowIcon(self.style().standardIcon(QStyle.SP_FileDialogContentsView))
        self.setAcceptDrops(True)


    def __init_config(self, config):
        # reset 'global' vars
        self.config_file = config + "/" if config[-1] != "/" else ""
        self.scheme = None
        self.config = {}

        self.__reset_ui()
        self.__read_scheme_file()
        self.__read_config_directory()
        self.__update_tree()

        self.ui.config_dir.setText(self.config_file)

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
        if os.path.isfile(self.config_file + "scheme.cfg"):
            with open(self.config_file + "scheme.cfg", "r") as f:
                self.scheme = f.readline().strip()

    def __read_config_directory(self):
        # read defined directories of config directory
        for p in self.dirs:
            self.__read_config_part(p)

        # update ui comboboxes
        self.ui.platforms.addItems(sorted(self.config['platform'].keys()))
        self.__setComboboxDisabled(self.ui.platforms)

        self.ui.schemes.addItems(sorted(self.config['scheme'].keys()))
        # enable defined scheme in combobox
        if self.scheme:
            self.ui.schemes.setCurrentIndex(self.ui.schemes.findText(self.scheme))
        self.__setComboboxDisabled(self.ui.schemes)

        self.ui.robots.addItems(sorted(self.config['robots'].keys()))
        self.__setComboboxDisabled(self.ui.robots)

        self.ui.bodies.addItems(sorted(self.config['robots_bodies'].keys()))
        self.__setComboboxDisabled(self.ui.bodies)

        self.ui.heads.addItems(sorted(self.config['robot_heads'].keys()))
        self.__setComboboxDisabled(self.ui.heads)

    def __read_config_part(self, key):
        self.config[key] = {}
        # does directory exist
        if os.path.isdir(self.config_file + self.dirs[key][0]):
            # read subdirectory
            if self.dirs[key][1]:
                for d in os.scandir(self.config_file + self.dirs[key][0]):
                    # read config files from subdirectories
                    if d.is_dir():
                        self.config[key][d.name] = self.__read_config_files(self.config_file + self.dirs[key][0] + d.name)
            else:
                # read config files without traversing subdirectories
                self.config[key] = self.__read_config_files(self.config_file + self.dirs[key][0])

    def __read_config_files(self,  dir):
        # only use '=' as assignment delimiter and be case-sensitive
        parser = configparser.ConfigParser(delimiters='=')
        parser.optionxform = str
        # read all config files in this directory
        for cfg in glob.glob(dir + "/*.cfg", recursive=True):
            try:
                parser.read(cfg)
            except configparser.DuplicateOptionError as e:
                # TODO: hint on duplicate option in the ui
                print(e)
        return parser._sections

    def __setComboboxDisabled(self, box):
        if box.count() <= 1:
            box.setDisabled(True)

    def __update_tree(self):
        self.ui.tabWidget.setEnabled(False)

        current = {}
        self.__update_config_part(current, 'general')
        self.__update_config_part(current, 'platform', self.ui.platforms)
        self.__update_config_part(current, 'scheme', self.ui.schemes)
        self.__update_config_part(current, 'robots', self.ui.robots)
        self.__update_config_part(current, 'robots_bodies', self.ui.bodies)
        self.__update_config_part(current, 'robot_heads', self.ui.heads)
        self.__update_config_part(current, 'private')

        self.__update_tree_modules(current)
        self.__update_tree_parameters(current)

        self.ui.tabWidget.setEnabled(True)

    def __update_config_part(self, config, key, box = None):
        if key in self.config:
            if box is None:
                self.update_dict(config, self.config[key])
            elif box.currentText() in self.config[key]:
                self.update_dict(config, self.config[key][box.currentText()])

    def update_dict(self, d, u):
        # recursive update dict
        for k, v in u.items():
            if isinstance(v, collections.Mapping):
                d[k] = self.update_dict(d.get(k, {}), v)
            else:
                d[k] = v
        return d

    def __update_tree_modules(self, current):
        modules = current['modules'] if 'modules' in current else {}
        model = QStandardItemModel()
        parent_item = model.invisibleRootItem()  # type: QStandardItem

        for m in sorted(modules):
            text_item = QStandardItem(m)
            text_item.setEditable(False)
            text_item.setCheckState(Qt.Checked if json.loads(modules[m].lower()) else Qt.Unchecked)

            # NOTE: some modules name their parameters differently than themselves
            if json.loads(modules[m].lower()) and m in current:
                for i in sorted(current[m]):
                    item = QStandardItem(i + " = " + current[m][i])
                    item.setEditable(False)
                    font = QFont()
                    font.setItalic(True)
                    item.setFont(font)
                    text_item.appendRow(item)

            parent_item.appendRow(text_item)

        model.setHeaderData(0, Qt.Horizontal, "Modules")
        self.ui.modules.setModel(model)

    def __update_tree_parameters(self, current):
        model = QStandardItemModel()
        parent_item = model.invisibleRootItem()  # type: QStandardItem

        for m in sorted(current):
            # skip modules
            if m == 'modules':
                continue
            text_item = QStandardItem(m)
            text_item.setEditable(False)

            for i in sorted(current[m]):
                item = QStandardItem(i + " = " + current[m][i])
                item.setEditable(False)
                font = QFont()
                font.setItalic(True)
                item.setFont(font)
                text_item.appendRow(item)

            parent_item.appendRow(text_item)

        model.setHeaderData(0, Qt.Horizontal, "Parameters")
        self.ui.parameters.setModel(model)

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
