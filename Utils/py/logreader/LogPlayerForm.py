# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'LogPlayer.ui'
#
# Created by: PyQt5 UI code generator 5.9.2
#
# WARNING! All changes made in this file will be lost!

from PyQt5 import QtCore, QtGui, QtWidgets

class Ui_PlayerForm(object):
    def setupUi(self, PlayerForm):
        PlayerForm.setObjectName("PlayerForm")
        PlayerForm.resize(639, 175)
        PlayerForm.setAutoFillBackground(False)
        self.verticalLayout_2 = QtWidgets.QVBoxLayout(PlayerForm)
        self.verticalLayout_2.setObjectName("verticalLayout_2")
        self.verticalLayout = QtWidgets.QVBoxLayout()
        self.verticalLayout.setObjectName("verticalLayout")
        self.seekSlider = QtWidgets.QSlider(PlayerForm)
        self.seekSlider.setOrientation(QtCore.Qt.Horizontal)
        self.seekSlider.setObjectName("seekSlider")
        self.verticalLayout.addWidget(self.seekSlider)
        self.horizontalLayout = QtWidgets.QHBoxLayout()
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.playButton = QtWidgets.QPushButton(PlayerForm)
        self.playButton.setObjectName("playButton")
        self.horizontalLayout.addWidget(self.playButton)
        self.frameCounter = QtWidgets.QLCDNumber(PlayerForm)
        self.frameCounter.setObjectName("frameCounter")
        self.horizontalLayout.addWidget(self.frameCounter)
        self.verticalLayout.addLayout(self.horizontalLayout)
        self.verticalLayout_2.addLayout(self.verticalLayout)

        self.retranslateUi(PlayerForm)
        QtCore.QMetaObject.connectSlotsByName(PlayerForm)

    def retranslateUi(self, PlayerForm):
        _translate = QtCore.QCoreApplication.translate
        PlayerForm.setWindowTitle(_translate("PlayerForm", "LogPlayer"))
        self.playButton.setText(_translate("PlayerForm", "Play/Pause"))

