# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'log_extractor.ui'
#
# Created by: PyQt5 UI code generator 5.12.2
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_log_file_extractor_widget(object):
    def setupUi(self, log_file_extractor_widget):
        log_file_extractor_widget.setObjectName("log_file_extractor_widget")
        log_file_extractor_widget.resize(1015, 534)
        self.main_layout = QtWidgets.QVBoxLayout(log_file_extractor_widget)
        self.main_layout.setObjectName("main_layout")
        self.job_creation_layout = QtWidgets.QHBoxLayout()
        self.job_creation_layout.setObjectName("job_creation_layout")
        self.file_list_layout = QtWidgets.QVBoxLayout()
        self.file_list_layout.setObjectName("file_list_layout")
        self.files_label = QtWidgets.QLabel(log_file_extractor_widget)
        font = QtGui.QFont()
        font.setPointSize(12)
        font.setBold(True)
        font.setWeight(75)
        self.files_label.setFont(font)
        self.files_label.setObjectName("files_label")
        self.file_list_layout.addWidget(self.files_label)
        self.file_tree = LogFileTreeWidget(log_file_extractor_widget)
        self.file_tree.setAcceptDrops(True)
        self.file_tree.setObjectName("file_tree")
        self.file_list_layout.addWidget(self.file_tree)
        self.clear_file_tree_layout = QtWidgets.QHBoxLayout()
        self.clear_file_tree_layout.setObjectName("clear_file_tree_layout")
        spacerItem = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.clear_file_tree_layout.addItem(spacerItem)
        self.clear_file_tree_button = QtWidgets.QPushButton(log_file_extractor_widget)
        self.clear_file_tree_button.setObjectName("clear_file_tree_button")
        self.clear_file_tree_layout.addWidget(self.clear_file_tree_button)
        self.file_list_layout.addLayout(self.clear_file_tree_layout)
        self.job_creation_layout.addLayout(self.file_list_layout)
        self.extractor_layout = QtWidgets.QVBoxLayout()
        self.extractor_layout.setObjectName("extractor_layout")
        self.output_label = QtWidgets.QLabel(log_file_extractor_widget)
        font = QtGui.QFont()
        font.setPointSize(12)
        font.setBold(True)
        font.setWeight(75)
        self.output_label.setFont(font)
        self.output_label.setObjectName("output_label")
        self.extractor_layout.addWidget(self.output_label)
        self.output_layout = QtWidgets.QHBoxLayout()
        self.output_layout.setObjectName("output_layout")
        self.output_selection = QtWidgets.QLineEdit(log_file_extractor_widget)
        self.output_selection.setObjectName("output_selection")
        self.output_layout.addWidget(self.output_selection)
        self.browse_button = QtWidgets.QPushButton(log_file_extractor_widget)
        self.browse_button.setObjectName("browse_button")
        self.output_layout.addWidget(self.browse_button)
        self.extractor_layout.addLayout(self.output_layout)
        self.extractor_label = QtWidgets.QLabel(log_file_extractor_widget)
        font = QtGui.QFont()
        font.setPointSize(12)
        font.setBold(True)
        font.setWeight(75)
        self.extractor_label.setFont(font)
        self.extractor_label.setObjectName("extractor_label")
        self.extractor_layout.addWidget(self.extractor_label)
        self.extractor_list = QtWidgets.QListWidget(log_file_extractor_widget)
        sizePolicy = QtWidgets.QSizePolicy(QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.extractor_list.sizePolicy().hasHeightForWidth())
        self.extractor_list.setSizePolicy(sizePolicy)
        font = QtGui.QFont()
        font.setPointSize(11)
        self.extractor_list.setFont(font)
        self.extractor_list.setSelectionMode(QtWidgets.QAbstractItemView.MultiSelection)
        self.extractor_list.setObjectName("extractor_list")
        self.extractor_layout.addWidget(self.extractor_list)
        self.add_jobs_layout = QtWidgets.QHBoxLayout()
        self.add_jobs_layout.setObjectName("add_jobs_layout")
        self.add_jobs_label = QtWidgets.QLabel(log_file_extractor_widget)
        font = QtGui.QFont()
        font.setPointSize(12)
        font.setBold(True)
        font.setWeight(75)
        self.add_jobs_label.setFont(font)
        self.add_jobs_label.setObjectName("add_jobs_label")
        self.add_jobs_layout.addWidget(self.add_jobs_label)
        self.job_button = QtWidgets.QPushButton(log_file_extractor_widget)
        font = QtGui.QFont()
        font.setPointSize(9)
        font.setBold(False)
        font.setWeight(50)
        self.job_button.setFont(font)
        self.job_button.setObjectName("job_button")
        self.add_jobs_layout.addWidget(self.job_button)
        spacerItem1 = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.add_jobs_layout.addItem(spacerItem1)
        self.extractor_layout.addLayout(self.add_jobs_layout)
        self.job_list = QtWidgets.QTreeWidget(log_file_extractor_widget)
        self.job_list.setObjectName("job_list")
        self.extractor_layout.addWidget(self.job_list)
        self.clear_job_list_layout = QtWidgets.QHBoxLayout()
        self.clear_job_list_layout.setObjectName("clear_job_list_layout")
        spacerItem2 = QtWidgets.QSpacerItem(40, 20, QtWidgets.QSizePolicy.Expanding, QtWidgets.QSizePolicy.Minimum)
        self.clear_job_list_layout.addItem(spacerItem2)
        self.clear_job_list_button = QtWidgets.QPushButton(log_file_extractor_widget)
        self.clear_job_list_button.setObjectName("clear_job_list_button")
        self.clear_job_list_layout.addWidget(self.clear_job_list_button)
        self.extractor_layout.addLayout(self.clear_job_list_layout)
        self.job_creation_layout.addLayout(self.extractor_layout)
        self.job_list_layout = QtWidgets.QVBoxLayout()
        self.job_list_layout.setObjectName("job_list_layout")
        self.job_creation_layout.addLayout(self.job_list_layout)
        self.main_layout.addLayout(self.job_creation_layout)
        self.controller_seperator = QtWidgets.QFrame(log_file_extractor_widget)
        self.controller_seperator.setFrameShape(QtWidgets.QFrame.HLine)
        self.controller_seperator.setFrameShadow(QtWidgets.QFrame.Sunken)
        self.controller_seperator.setObjectName("controller_seperator")
        self.main_layout.addWidget(self.controller_seperator)
        self.controller_layout = QtWidgets.QHBoxLayout()
        self.controller_layout.setObjectName("controller_layout")
        self.progress_bar = QtWidgets.QProgressBar(log_file_extractor_widget)
        self.progress_bar.setProperty("value", 0)
        self.progress_bar.setObjectName("progress_bar")
        self.controller_layout.addWidget(self.progress_bar)
        self.execute_button = QtWidgets.QPushButton(log_file_extractor_widget)
        self.execute_button.setObjectName("execute_button")
        self.controller_layout.addWidget(self.execute_button)
        self.main_layout.addLayout(self.controller_layout)

        self.retranslateUi(log_file_extractor_widget)
        QtCore.QMetaObject.connectSlotsByName(log_file_extractor_widget)

    def retranslateUi(self, log_file_extractor_widget):
        _translate = QtCore.QCoreApplication.translate
        log_file_extractor_widget.setWindowTitle(_translate("log_file_extractor_widget", "Form"))
        self.files_label.setText(_translate("log_file_extractor_widget", "1. Select log files:"))
        self.file_tree.headerItem().setText(0, _translate("log_file_extractor_widget", "Drag files/folders containing logs here:"))
        self.clear_file_tree_button.setText(_translate("log_file_extractor_widget", "clear"))
        self.output_label.setText(_translate("log_file_extractor_widget", "2. Select output directory:"))
        self.output_selection.setText(_translate("log_file_extractor_widget", "output directory..."))
        self.browse_button.setText(_translate("log_file_extractor_widget", "Browse"))
        self.extractor_label.setText(_translate("log_file_extractor_widget", "3. Select Extractor:"))
        self.add_jobs_label.setText(_translate("log_file_extractor_widget", "4. Add jobs: "))
        self.job_button.setText(_translate("log_file_extractor_widget", "Add"))
        self.job_list.headerItem().setText(0, _translate("log_file_extractor_widget", "Extractor"))
        self.job_list.headerItem().setText(1, _translate("log_file_extractor_widget", "Target"))
        self.job_list.headerItem().setText(2, _translate("log_file_extractor_widget", "Output"))
        self.job_list.headerItem().setText(3, _translate("log_file_extractor_widget", "Progress"))
        self.clear_job_list_button.setText(_translate("log_file_extractor_widget", "clear"))
        self.execute_button.setText(_translate("log_file_extractor_widget", "Execute"))


from log_extractor_ui.widgets import LogFileTreeWidget