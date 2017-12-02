# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'LogPlayer.ui'
#
# Created by: PyQt4 UI code generator 4.12.1
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_PlayerForm(object):
    def setupUi(self, PlayerForm):
        PlayerForm.setObjectName(_fromUtf8("PlayerForm"))
        PlayerForm.resize(639, 175)
        self.playButton = QtGui.QPushButton(PlayerForm)
        self.playButton.setGeometry(QtCore.QRect(20, 70, 80, 25))
        self.playButton.setObjectName(_fromUtf8("playButton"))
        self.seekSlider = QtGui.QSlider(PlayerForm)
        self.seekSlider.setGeometry(QtCore.QRect(140, 70, 461, 21))
        self.seekSlider.setOrientation(QtCore.Qt.Horizontal)
        self.seekSlider.setObjectName(_fromUtf8("seekSlider"))
        self.frameCounter = QtGui.QLCDNumber(PlayerForm)
        self.frameCounter.setGeometry(QtCore.QRect(453, 100, 151, 51))
        self.frameCounter.setObjectName(_fromUtf8("frameCounter"))

        self.retranslateUi(PlayerForm)
        QtCore.QMetaObject.connectSlotsByName(PlayerForm)

    def retranslateUi(self, PlayerForm):
        PlayerForm.setWindowTitle(_translate("PlayerForm", "Form", None))
        self.playButton.setText(_translate("PlayerForm", "Play/Pause", None))

