# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'ConfigViewer.ui'
#
# Created by: PyQt5 UI code generator 5.12.1
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
        self.tabWidget = QtWidgets.QTabWidget(Form)
        self.tabWidget.setTabPosition(QtWidgets.QTabWidget.South)
        self.tabWidget.setTabShape(QtWidgets.QTabWidget.Rounded)
        self.tabWidget.setObjectName("tabWidget")
        self.tab = QtWidgets.QWidget()
        self.tab.setObjectName("tab")
        self.verticalLayout_4 = QtWidgets.QVBoxLayout(self.tab)
        self.verticalLayout_4.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout_4.setObjectName("verticalLayout_4")
        self.modules = QtWidgets.QTreeView(self.tab)
        self.modules.setFrameShape(QtWidgets.QFrame.NoFrame)
        self.modules.setFrameShadow(QtWidgets.QFrame.Plain)
        self.modules.setLineWidth(0)
        self.modules.setObjectName("modules")
        self.modules.header().setStretchLastSection(False)
        self.verticalLayout_4.addWidget(self.modules)
        self.tabWidget.addTab(self.tab, "")
        self.tab_2 = QtWidgets.QWidget()
        self.tab_2.setObjectName("tab_2")
        self.verticalLayout_5 = QtWidgets.QVBoxLayout(self.tab_2)
        self.verticalLayout_5.setContentsMargins(0, 0, 0, 0)
        self.verticalLayout_5.setObjectName("verticalLayout_5")
        self.parameters = QtWidgets.QTreeView(self.tab_2)
        self.parameters.setFrameShape(QtWidgets.QFrame.NoFrame)
        self.parameters.setFrameShadow(QtWidgets.QFrame.Plain)
        self.parameters.setLineWidth(0)
        self.parameters.setObjectName("parameters")
        self.parameters.header().setStretchLastSection(False)
        self.verticalLayout_5.addWidget(self.parameters)
        self.tabWidget.addTab(self.tab_2, "")
        self.verticalLayout.addWidget(self.tabWidget)
        self.config_dir = QtWidgets.QLabel(Form)
        self.config_dir.setText("")
        self.config_dir.setAlignment(QtCore.Qt.AlignRight|QtCore.Qt.AlignTrailing|QtCore.Qt.AlignVCenter)
        self.config_dir.setObjectName("config_dir")
        self.verticalLayout.addWidget(self.config_dir)

        self.retranslateUi(Form)
        self.tabWidget.setCurrentIndex(0)
        QtCore.QMetaObject.connectSlotsByName(Form)

    def retranslateUi(self, Form):
        _translate = QtCore.QCoreApplication.translate
        Form.setWindowTitle(_translate("Form", "Form"))
        self.platforms.setItemText(0, _translate("Form", "Platform"))
        self.schemes.setItemText(0, _translate("Form", "Scheme"))
        self.robots.setItemText(0, _translate("Form", "Robots"))
        self.bodies.setItemText(0, _translate("Form", "Bodies"))
        self.heads.setItemText(0, _translate("Form", "Heads"))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab), _translate("Form", "Modules"))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_2), _translate("Form", "Parameters"))


