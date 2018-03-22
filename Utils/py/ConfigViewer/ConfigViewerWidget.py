import glob, json, collections, re, zipfile
import os, configparser, tarfile

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
        self.scheme = None
        self.config = {}

        if os.path.isdir(config):
            self.config_file = DirectoryConfig(config + "/" if config[-1] != "/" else "")
        elif tarfile.is_tarfile(config):
            self.config_file = TargzConfig(config)
        elif zipfile.is_zipfile(config):
            self.config_file = ZipConfig(config)
        else:
            self.config_file = None

        self.__reset_ui()
        self.__read_scheme_file()
        self.__read_config_directory()
        self.__update_tree()

        self.ui.config_dir.setText(self.config_file.getName() if self.config_file else "ERROR: Invalid config file/directory")

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
        self.scheme = self.config_file.getScheme() if self.config_file else None

    def __read_config_directory(self):
        # read defined directories of config directory
        self.config = self.config_file.readConfig(self.dirs) if self.config_file else {}

        if self.config:
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
                if url.isLocalFile() and (
                       os.path.isdir(url.toLocalFile())
                    or tarfile.is_tarfile(url.toLocalFile())
                    or zipfile.is_zipfile(url.toLocalFile())
                ):
                    e.accept()
        else:
            e.ignore()

    def dropEvent(self, e):
        # sanity checks
        if e.mimeData().hasUrls() and len(e.mimeData().urls()) == 1:
            # update ui and read file
            self.__init_config(e.mimeData().urls()[0].toLocalFile())


class DirectoryConfig:
    def __init__(self, directory):
        self.directory = directory

    def getName(self):
        return self.directory

    def getScheme(self):
        if os.path.isfile(self.directory + "scheme.cfg"):
            with open(self.directory + "scheme.cfg", "r") as f:
                return f.readline().strip()
        return None

    def readConfig(self, config_dirs):
        config = {}
        for key in config_dirs:
            config[key] = {}
            # does directory exist
            if os.path.isdir(self.directory + config_dirs[key][0]):
                # read subdirectory
                if config_dirs[key][1]:
                    for d in os.scandir(self.directory + config_dirs[key][0]):
                        # read config files from subdirectories
                        if d.is_dir():
                            config[key][d.name] = self.__read_config_files(self.directory + config_dirs[key][0] + d.name)
                else:
                    # read config files without traversing subdirectories
                    config[key] = self.__read_config_files(self.directory + config_dirs[key][0])
        return config

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


class TargzConfig:
    def __init__(self, file):
        self.file = tarfile.open(file)

    def getName(self):
        return self.file.name

    def __getMember(self, name):
        try:
            return self.file.getmember(name)
        except:
            pass
        return None

    def getScheme(self):
        scheme_file = self.__getMember("Config/scheme.cfg")
        if scheme_file and scheme_file.isfile():
            # read scheme file
            return self.file.extractfile(scheme_file).readline().decode("utf-8").strip()
        return None

    def readConfig(self, config_dirs):
        config = {}
        for key in config_dirs:
            config[key] = {}
            directory = self.__getMember("Config/"+config_dirs[key][0].strip('/'))

            # does directory exist
            if directory and directory.isdir():
                # read subdirectory
                if config_dirs[key][1]:
                    for d in self.file.getmembers():
                        # read config files from subdirectories
                        if d.isdir() and re.match("Config/"+config_dirs[key][0]+"[^/]+$", d.name) is not None:
                            config[key][d.name.replace("Config/"+config_dirs[key][0],"")] = self.__read_config_files(d.name)
                else:
                    # read config files without traversing subdirectories
                    config[key] = self.__read_config_files(directory.name)

        return config

    def __read_config_files(self,  prefix):
        # only use '=' as assignment delimiter and be case-sensitive
        parser = configparser.ConfigParser(delimiters='=')
        parser.optionxform = str

        for m in self.file.getmembers():
            if m.name.startswith(prefix + '/') and m.isfile() and m.name.endswith('.cfg'):
                try:
                    parser.read_string(self.file.extractfile(m).read().decode("utf-8"), m.name)
                except configparser.DuplicateOptionError as e:
                    # TODO: hint on duplicate option in the ui
                    print(e)

        return parser._sections


class ZipConfig:
    def __init__(self, file):
        self.file = zipfile.ZipFile(file)

    def getName(self):
        return self.file.filename

    def __getMember(self, name):
        try:
            return self.file.getinfo(name)
        except:
            pass
        return None

    def getScheme(self):
        scheme_file = self.__getMember("Config/scheme.cfg")
        #print(dir(scheme_file))
        if scheme_file and not scheme_file.is_dir():
            # read scheme file
            return self.file.read(scheme_file).decode("utf-8").strip()
        return None

    def readConfig(self, config_dirs):
        config = {}
        for key in config_dirs:
            config[key] = {}
            directory = self.__getMember("Config/"+config_dirs[key][0])
            # does directory exist
            if directory and directory.is_dir():
                # read subdirectory
                if config_dirs[key][1]:
                    for d in self.file.infolist():
                        if d.is_dir():
                            sub = re.search("Config/"+config_dirs[key][0]+"([^/]+)/$", d.filename)
                            if sub and sub[1]:
                                # read config files from subdirectories
                                config[key][sub[1]] = self.__read_config_files(d.filename)

                else:
                    # read config files without traversing subdirectories
                    config[key] = self.__read_config_files(directory.filename)
        return config

    def __read_config_files(self,  prefix):
        # only use '=' as assignment delimiter and be case-sensitive
        parser = configparser.ConfigParser(delimiters='=')
        parser.optionxform = str

        for m in self.file.infolist():
            if not m.is_dir() and m.filename.startswith(prefix) and m.filename.endswith('.cfg'):
                try:
                    parser.read_string(self.file.read(m).decode("utf-8"), m.filename)
                    pass
                except configparser.DuplicateOptionError as e:
                    # TODO: hint on duplicate option in the ui
                    print(e)

        return parser._sections

