import logging
import os

from PyQt5 import QtCore
from PyQt5.QtWidgets import QTreeWidget, QTreeWidgetItem, QLabel

logger = logging.getLogger('__name__')


class FileTreeWidgetItem(QTreeWidgetItem):
    def __init__(self, url):
        super().__init__()
        self.url = url


class LogFileTreeWidget(QTreeWidget):
    def __init__(self, parent=None):
        super().__init__(parent)

    def dragEnterEvent(self, event):
        if event.mimeData().hasUrls():
            event.accept()
        else:
            event.ignore()

    def dragMoveEvent(self, event):
        if event.mimeData().hasUrls():
            # Do we need this?
            event.setDropAction(QtCore.Qt.CopyAction)
            event.accept()
        else:
            event.ignore()

    def dropEvent(self, event):
        if event.mimeData().hasUrls():
            event.setDropAction(QtCore.Qt.CopyAction)
            event.accept()

            children = self.list_logs(str(url.toLocalFile()) for url in event.mimeData().urls())
            for child in children:
                self.addTopLevelItem(child)
                self.expand_selected(child)
            if children:
                self.setHeaderLabels(['Select log files:'])
        else:
            event.ignore()

    def _selected_leaves(self, parent: QTreeWidgetItem):
        """
        Recusivly iterate though a parents leaves
        """
        if parent.checkState(0) != QtCore.Qt.Unchecked:
            if parent.childCount() > 0:
                for i in range(parent.childCount()):
                    for selected in self._selected_leaves(parent.child(i)):
                        yield selected
            else:
                yield parent

    def selected_files(self):
        """
        Iterate over selected files - (path, relative path) tuples
        """
        for i in range(self.topLevelItemCount()):
            parent = self.topLevelItem(i)
            base = parent.text(0)

            for leaf in self._selected_leaves(parent):
                # extract relative path
                relative_path = os.path.relpath(leaf.url, start=os.path.commonpath([parent.url, leaf.url]))
                yield leaf.url, os.path.join(base, relative_path)

    def expand_selected(self, parent: QTreeWidgetItem):
        """
        Recursively expand tree items if children are selected
        :param parent: to start from
        """
        if parent.checkState(0) != QtCore.Qt.Unchecked:
            parent.setExpanded(True)
            for i in range(parent.childCount()):
                self.expand_selected(parent.child(i))

    def list_logs(self, urls):
        """
        Recursively create file structure and show logs to be selected
        :param urls: path to folders or files which contain naoth logs
        :return: created parent items
        """
        parents = []
        for url in urls:
            base = os.path.basename(os.path.normpath(url))

            if os.path.isdir(url):
                # add node
                children = self.list_logs(os.path.join(url, sub_url) for sub_url in os.listdir(url))

                parent = FileTreeWidgetItem(url)
                parent.setExpanded(True)
                parent.setText(0, base)
                parent.setFlags(parent.flags() | QtCore.Qt.ItemIsTristate | QtCore.Qt.ItemIsUserCheckable)
                for child in children:
                    parent.addChild(child)
                if children:
                    parents.append(parent)
            elif os.path.isfile(url):
                # add leave
                parent = FileTreeWidgetItem(url)
                parent.setText(0, base)
                parent.setFlags(parent.flags() | QtCore.Qt.ItemIsUserCheckable)

                # set checked if log file
                if base.endswith('.log'):
                    # TODO should be a parameter
                    if base == 'game.log':
                        parent.setCheckState(0, QtCore.Qt.Checked)
                    else:
                        parent.setCheckState(0, QtCore.Qt.Unchecked)
                    parents.append(parent)
            else:
                logger.warning(f'Unrecognized URL {url} .')

        return parents


class ItemContainer(QLabel):
    def __init__(self, name, item):
        super().__init__(name)
        self.item = item
