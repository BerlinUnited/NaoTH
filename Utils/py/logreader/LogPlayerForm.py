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
        self.verticalLayout_3 = QtWidgets.QVBoxLayout()
        self.verticalLayout_3.setObjectName("verticalLayout_3")
        self.playButton = QtWidgets.QPushButton(PlayerForm)
        self.playButton.setObjectName("playButton")
        self.verticalLayout_3.addWidget(self.playButton)
        self.horizontalLayout_2 = QtWidgets.QHBoxLayout()
        self.horizontalLayout_2.setObjectName("horizontalLayout_2")
        self.loopCheckBox = QtWidgets.QCheckBox(PlayerForm)
        self.loopCheckBox.setObjectName("loopCheckBox")
        self.horizontalLayout_2.addWidget(self.loopCheckBox)
        self.realTimeCheckBox = QtWidgets.QCheckBox(PlayerForm)
        self.realTimeCheckBox.setObjectName("realTimeCheckBox")
        self.horizontalLayout_2.addWidget(self.realTimeCheckBox)
        self.multiplierBox = QtWidgets.QSpinBox(PlayerForm)
        self.multiplierBox.setMinimum(1)
        self.multiplierBox.setMaximum(10)
        self.multiplierBox.setObjectName("multiplierBox")
        self.horizontalLayout_2.addWidget(self.multiplierBox)
        self.verticalLayout_3.addLayout(self.horizontalLayout_2)
        self.horizontalLayout.addLayout(self.verticalLayout_3)
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
        self.loopCheckBox.setText(_translate("PlayerForm", "loop"))
        self.realTimeCheckBox.setText(_translate("PlayerForm", "realtime"))
        self.multiplierBox.setToolTip(_translate("PlayerForm", "multiplier"))
        self.frameCounter.setToolTip(_translate("PlayerForm", "Current frame"))

