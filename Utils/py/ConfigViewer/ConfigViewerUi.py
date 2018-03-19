# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ConfigViewer.ui'
#
# Created by: PyQt5 UI code generator 5.10
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_Form(object):
    def setupUi(self, Form):
        Form.setObjectName("Form")
        Form.resize(600, 600)
        self.verticalLayout = QtWidgets.QVBoxLayout(Form)
        self.verticalLayout.setObjectName("verticalLayout")
        self.widget = QtWidgets.QWidget(Form)
        self.widget.setObjectName("widget")
        self.horizontalLayout = QtWidgets.QHBoxLayout(self.widget)
        self.horizontalLayout.setContentsMargins(0, -1, 0, -1)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.platforms = QtWidgets.QComboBox(self.widget)
        self.platforms.setObjectName("platforms")
        self.platforms.addItem("")
        self.horizontalLayout.addWidget(self.platforms)
        self.schemes = QtWidgets.QComboBox(self.widget)
        self.schemes.setObjectName("schemes")
        self.schemes.addItem("")
        self.horizontalLayout.addWidget(self.schemes)
        self.robots = QtWidgets.QComboBox(self.widget)
        self.robots.setObjectName("robots")
        self.robots.addItem("")
        self.horizontalLayout.addWidget(self.robots)
        self.bodies = QtWidgets.QComboBox(self.widget)
        self.bodies.setObjectName("bodies")
        self.bodies.addItem("")
        self.horizontalLayout.addWidget(self.bodies)
        self.heads = QtWidgets.QComboBox(self.widget)
        self.heads.setObjectName("heads")
        self.heads.addItem("")
        self.horizontalLayout.addWidget(self.heads)
        self.verticalLayout.addWidget(self.widget)
        self.modules = QtWidgets.QTreeView(Form)
        self.modules.setObjectName("modules")
        self.verticalLayout.addWidget(self.modules)
        self.config_dir = QtWidgets.QLabel(Form)
        self.config_dir.setText("")
        self.config_dir.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.config_dir.setObjectName("config_dir")
        self.verticalLayout.addWidget(self.config_dir)

        self.retranslateUi(Form)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        _translate = QtCore.QCoreApplication.translate
        Form.setWindowTitle(_translate("Form", "Form"))
        self.platforms.setItemText(0, _translate("Form", "Platform"))
        self.schemes.setItemText(0, _translate("Form", "Scheme"))
        self.robots.setItemText(0, _translate("Form", "Robots"))
        self.bodies.setItemText(0, _translate("Form", "Bodies"))
        self.heads.setItemText(0, _translate("Form", "Heads"))

