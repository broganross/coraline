# <license>
# Copyright (C) 2011 Andrea Interguglielmi, All rights reserved.
# This file is part of the coral repository downloaded from http://code.google.com/p/coral-repo.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
# 
#    * Redistributions of source code must retain the above copyright
#      notice, this list of conditions and the following disclaimer.
# 
#    * Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
# 
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
# IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# </license>

import math
import weakref
import os
from PyQt4 import QtGui, QtCore
from .. import coralApp
from ..observer import Observer
import dockWidget

class InfoBox(QtGui.QWidget):
    def __init__(self, parent):
        QtGui.QWidget.__init__(self, parent)
        
        layout = QtGui.QVBoxLayout(self)
        self.setLayout(layout)

        self._message = QtGui.QLabel("whatever")
        self._animStep = 0.0
        self._animMaxFrame = 5000.0
        self._animStopFrame = 50.0
        self._color = QtGui.QColor(0, 0, 0, 100)

        layout.setContentsMargins(2, 2, 2, 2)
        layout.addWidget(self._message)

        palette = self._message.palette()
        palette.setColor(self._message.foregroundRole(), QtGui.QColor(200, 190, 200))
        self._message.setPalette(palette)

        self.resize(0, 0)

    def paintEvent(self, event):
        painter = QtGui.QPainter()
        painter.begin(self)

        shape = QtGui.QPainterPath()
        parentWidget = self.parentWidget()
        shape.addRoundedRect(QtCore.QRectF(self.rect()), 2, 2)

        painter.setPen(QtCore.Qt.NoPen)
        painter.setBrush(self._color)
        painter.drawPath(shape)

    def _advanceAnimStep(self, frame):
        if frame <= self._animStopFrame:
            self._animStep = frame / self._animStopFrame
            self.updateSize()
        elif frame >= (self._animMaxFrame - self._animStopFrame):
            self._animStep = (self._animMaxFrame - frame) / self._animStopFrame
            self.updateSize()

    def showMessage(self, message):
        self._message.setText(message)
        self._color = QtGui.QColor(0, 0, 0, 100)
        self._startAnim()

    def showError(self, message):
        self._message.setText(message)
        self._color = QtGui.QColor(100, 0, 20, 100)
        self._startAnim()

    def _startAnim(self):
        if self._animStep == 0.0:
            timer = QtCore.QTimeLine(self._animMaxFrame, self)
            timer.setFrameRange(0, self._animMaxFrame)

            self.connect(timer, QtCore.SIGNAL("frameChanged(int)"), self._advanceAnimStep);

            timer.start()

    def updateSize(self):
        parent = self.parentWidget()

        self.resize(self._message.fontMetrics().width(self._message.text()) + 4, (self._message.fontMetrics().height() + 8) * self._animStep)
        self.move((parent.width() / 2) - (self.width() / 2), parent.height() - (self.height() + 6))

class MainWindow(QtGui.QMainWindow):
    _globalInstance = None
    _lastFileDialogDir = ""
    _closedWidgets = {}
    _registeredWidgets = {}
    
    @staticmethod
    def openRegisteredWidget(widgetName):
        if MainWindow._registeredWidgets.has_key(widgetName):
            widgetClass = MainWindow._registeredWidgets[widgetName]
            MainWindow._globalInstance.dockWidget(widgetClass(MainWindow._globalInstance))
    
    @staticmethod
    def registerWidget(widgetName, widget):
        MainWindow._registeredWidgets[widgetName] = widget
    
    @staticmethod
    def _init():
        MainWindow._globalInstance = MainWindow()
    
    @staticmethod
    def globalInstance():
        return MainWindow._globalInstance
    
    @staticmethod
    def okCancelDialog(message):
        dialog = QtGui.QMessageBox(MainWindow._globalInstance)
        dialog.setText(message)
        dialog.addButton("No", QtGui.QMessageBox.NoRole)
        dialog.addButton("Yes", QtGui.QMessageBox.YesRole)
        
        result = dialog.exec_()
        return result
    
    @staticmethod
    def saveFileDialog(title, fileType):
        filename = QtGui.QFileDialog.getSaveFileName(MainWindow._globalInstance, title, MainWindow._lastFileDialogDir, fileType)
    
        MainWindow._lastFileDialogDir = os.path.split(str(filename))[0]
    
        return str(filename)
    
    @staticmethod
    def openFileDialog(title, fileType):
        filename = QtGui.QFileDialog.getOpenFileName(MainWindow._globalInstance, title, MainWindow._lastFileDialogDir, fileType)
    
        MainWindow._lastFileDialogDir = os.path.split(str(filename))[0]
    
        return str(filename)
    
    def __init__(self, parent = None):
        QtGui.QMainWindow.__init__(self, parent)
        
        self._menuBar = QtGui.QMenuBar()
        self._messageLoggedObserver = Observer()
        
        self.setWindowTitle("coral")
        self.resize(900, 500)
        self.setMenuBar(self._menuBar)
        self.setDockOptions(QtGui.QMainWindow.AllowTabbedDocks | QtGui.QMainWindow.AllowNestedDocks | QtGui.QMainWindow.AnimatedDocks)
        
        self._menuBar.clear()

        coralApp.addMessageLoggedObserver(self._messageLoggedObserver, self._messageLogged)
    
    def _messageLogged(self):
        message = self._messageLoggedObserver.data("message")
        verboseLevel = self._messageLoggedObserver.data("verboseLevel")

        if verboseLevel == coralApp.VerboseLevel.logInfos:
            self._infoBox.showMessage(message)
        elif verboseLevel == coralApp.VerboseLevel.logErrors:
            self._infoBox.showError(message)

    def about(self):
        aboutBox = QtGui.QMessageBox(self)
        aboutBox.setWindowTitle("About Coral")
        
        aboutMsg = '<h2>Coral Version ' + coralApp.version() + '</h2>'
        aboutMsg += '<p style="color: #333333; font-size: small;">Created by Andrea Interguglielmi</p>'
        aboutMsg += '<p style="color: #333333; font-size: small;">Developed at <a href="http://code.google.com/p/coral-repo/">code.google.com/p/coral-repo</a></p>'
        
        aboutBox.setTextFormat(QtCore.Qt.RichText)
        aboutBox.setText(aboutMsg)
        aboutBox.exec_()
        
    def menuBar(self):
        return self._menuBar
    
    def dockWidget(self, widget, area = None):
        dock = dockWidget.DockWidget(widget, self)
        dock.setObjectName(widget.windowTitle())
        dock.show()
        
        if area is None:
            dock.setFloating(True)
        else:
            self.addDockWidget(area, dock)
        
        return dock
    
    def _restoreLastOpenWindows(self, settings):
        widgetsLastOpen = settings.value("openWidgets").toString()
        if widgetsLastOpen:
            widgetsLastOpen = eval(str(widgetsLastOpen))
        
        openWidgetsName = []
        for dockWidget in self.dockWidgets():
            openWidgetsName.append(str(dockWidget.windowTitle()))
        
        for widgetName in widgetsLastOpen:
            if widgetName not in openWidgetsName:
                if MainWindow._registeredWidgets.has_key(widgetName):
                    widgetClass = MainWindow._registeredWidgets[widgetName]
                    self.dockWidget(widgetClass(self))
        
    def restoreSettings(self):
        settings = self.settings()
        
        self._restoreLastOpenWindows(settings)
        geometry = str(settings.value("mainWinGeometry").toString())
        if geometry:
            geometry = eval(geometry)
            self.move(geometry[0], geometry[1])
            self.resize(geometry[2], geometry[3])
        
        self.restoreState(settings.value("mainWinState").toByteArray())
        
        lastFileDialogDir = settings.value("lastFileDialogDir").toString()
        if lastFileDialogDir:
            MainWindow._lastFileDialogDir = lastFileDialogDir
    
    def settings(self):
        return QtCore.QSettings(self.windowTitle())
    
    def dockWidgets(self):
        dockWidgets = []
        for obj in self.children():
            if type(obj) is dockWidget.DockWidget:
                dockWidgets.append(obj)
        
        return dockWidgets
    
    def closeEvent(self, event):
        settings = self.settings()
        settings.setValue("mainWinGeometry", str([self.pos().x(), self.pos().y(), self.size().width(), self.size().height()]))
        settings.setValue("mainWinState", self.saveState())
        
        openWidgets = []
        for dockWidget in self.dockWidgets():
            widgetName = str(dockWidget.windowTitle())
            if dockWidget.isVisible() and widgetName not in openWidgets:
                openWidgets.append(widgetName)
                
            dockWidget.closeEvent(event)
        
        settings.setValue("openWidgets", str(openWidgets))
        settings.setValue("lastFileDialogDir", MainWindow._lastFileDialogDir)
        settings.setValue("settingsStored", True)
        
        QtGui.QMainWindow.closeEvent(self, event)

    def setShortcutsMap(self, shortcuts):
        for key, function in shortcuts.iteritems():
            QtGui.QShortcut(QtGui.QKeySequence(key), self, function)

    def setCentralWidget(self, widget):
        QtGui.QMainWindow.setCentralWidget(self, widget)
        self._infoBox = InfoBox(widget)

    def resizeEvent(self, event):
        self._infoBox.updateSize()