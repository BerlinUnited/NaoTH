import glob, json, collections, re, zipfile
import os, configparser, tarfile

from PyQt5.QtCore import Qt
from PyQt5.QtGui import QStandardItemModel, QStandardItem, QFont

import Config
import ConfigViewerUi
from PyQt5.QtWidgets import QWidget, QStyle, QHeaderView


def is_true(value):
    """Converts a string representation of a boolean to a python boolean."""
    return True if json.loads(value) else False


class Widget(QWidget):

    def __init__(self, config:str):
        super().__init__()

        self.ui = ConfigViewerUi.Ui_Form()
        self.ui.setupUi(self)

        # read all config files
        self.__init_config(config)

        # register callbacks
        self.ui.platforms.currentIndexChanged.connect(self.__update_tree)
        self.ui.schemes.currentIndexChanged.connect(self.__update_tree)
        self.ui.strategy.currentIndexChanged.connect(self.__update_tree)
        self.ui.robots.currentIndexChanged.connect(self.__update_tree)
        self.ui.bodies.currentIndexChanged.connect(self.__update_tree)
        self.ui.heads.currentIndexChanged.connect(self.__update_tree)

        self.setWindowTitle("Modules Config Viewer")
        self.setWindowIcon(self.style().standardIcon(QStyle.SP_FileDialogContentsView))
        self.setAcceptDrops(True)


    def __init_config(self, uri):
        # reset 'global' vars
        self.config = Config.readConfig(uri)

        # get the current scheme
        self.scheme = self.config.getScheme()
        self.strategy = self.config.getStrategy()

        self.__reset_ui()
        self.__init_ui()
        self.__update_tree()

        self.ui.config_dir.setText(self.config.getName())

    def __reset_ui(self):
        # enable everything
        self.ui.platforms.setEnabled(True)
        self.ui.schemes.setEnabled(True)
        self.ui.strategy.setEnabled(True)
        self.ui.robots.setEnabled(True)
        self.ui.bodies.setEnabled(True)
        self.ui.heads.setEnabled(True)
        self.ui.heads.setEnabled(True)
        self.ui.modules.setEnabled(True)
        # clear everything
        self.ui.platforms.clear()
        self.ui.schemes.clear()
        self.ui.strategy.clear()
        self.ui.robots.clear()
        self.ui.bodies.clear()
        self.ui.heads.clear()
        self.ui.config_dir.clear()
        # set defaults
        self.ui.platforms.addItem("Platform")
        self.ui.schemes.addItem("Scheme")
        self.ui.strategy.addItem("Strategy")
        self.ui.robots.addItem("Robots")
        self.ui.bodies.addItem("Bodies")
        self.ui.heads.addItem("Heads")

    def __init_ui(self):
        # update ui comboboxes
        self.ui.platforms.addItems(sorted(self.config.getPlatforms()))
        self.__setComboboxDisabled(self.ui.platforms)

        self.ui.schemes.addItems(sorted(self.config.getSchemes()))
        # enable defined scheme in combobox
        if self.scheme:
            self.ui.schemes.setCurrentIndex(self.ui.schemes.findText(self.scheme))
        self.__setComboboxDisabled(self.ui.schemes)

        self.ui.strategy.addItems(sorted(self.config.getStrategies()))
        # enable defined strategy in combobox
        if self.strategy:
            self.ui.strategy.setCurrentIndex(self.ui.strategy.findText(self.strategy))
        self.__setComboboxDisabled(self.ui.strategy)

        self.ui.robots.addItems(sorted(self.config.getRobots()))
        self.__setComboboxDisabled(self.ui.robots)

        self.ui.bodies.addItems(sorted(self.config.getRobots()))
        self.__setComboboxDisabled(self.ui.bodies)

        self.ui.heads.addItems(sorted(self.config.getHeads()))
        self.__setComboboxDisabled(self.ui.heads)

    def __setComboboxDisabled(self, box):
        if box.count() <= 1:
            box.setDisabled(True)

    def __update_tree(self):
        self.ui.tabWidget.setEnabled(False)

        current = self.config.getConfigFor(
            platform=self.ui.platforms.currentText(),
            scheme=self.ui.schemes.currentText(),
            strategy=self.ui.strategy.currentText(),
            robot=self.ui.robots.currentText(),
            body=self.ui.bodies.currentText(),
            head=self.ui.heads.currentText()
        )

        self.__update_tree_modules(current)
        self.__update_tree_parameters(current)

        self.ui.tabWidget.setEnabled(True)

    def __update_tree_modules(self, current):
        modules = current['modules'] if 'modules' in current else {}
        model = QStandardItemModel()
        parent_item = model.invisibleRootItem()  # type: QStandardItem

        for m in sorted(modules):
            text_item = QStandardItem(m)
            text_item.setEditable(False)
            text_item.setCheckState(Qt.Checked if is_true(modules[m][0].lower()) else Qt.Unchecked)

            # NOTE: some modules name their parameters differently than themselves
            if is_true(modules[m][0].lower()) and m in current:
                for i in sorted(current[m]):
                    item = QStandardItem(i + " = " + current[m][i][0])
                    item.setEditable(False)
                    font = QFont()
                    font.setItalic(True)
                    item.setFont(font)
                    text_item.appendRow([item, QStandardItem(current[m][i][1])])

            parent_item.appendRow([ text_item, QStandardItem(modules[m][1]) ])

        model.setHeaderData(0, Qt.Horizontal, "Modules")
        model.setHeaderData(1, Qt.Horizontal, "Origin")

        self.ui.modules.setModel(model)
        self.ui.modules.header().setSectionResizeMode(0, QHeaderView.Stretch)

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
                item = QStandardItem(i + " = " + current[m][i][0])
                item.setEditable(False)
                font = QFont()
                font.setItalic(True)
                item.setFont(font)
                text_item.appendRow([item, QStandardItem(current[m][i][1])])

            parent_item.appendRow([ text_item, QStandardItem() ])

        model.setHeaderData(0, Qt.Horizontal, "Parameters")
        model.setHeaderData(1, Qt.Horizontal, "Origin")

        self.ui.parameters.setModel(model)
        self.ui.parameters.header().setSectionResizeMode(0, QHeaderView.Stretch)

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

