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


import sys
from PyQt4 import QtGui, QtCore

from mainWindow import MainWindow

class ConsoleOut(object):
    def __init__(self, textEdit, out, color):
        self._textEdit = textEdit
        self._out = out
        self._color = color
        
    def write(self, stream):
        self._textEdit.moveCursor(QtGui.QTextCursor.End)
        self._textEdit.setTextColor(self._color)
        self._textEdit.insertPlainText(stream)
        
        self._out.write(stream)

class ScriptEditorBar(QtGui.QWidget):
    def __init__(self, parent):
        QtGui.QWidget.__init__(self, parent)
        
        self._executeButton = QtGui.QToolButton(self)
        
        self.setLayout(QtGui.QHBoxLayout(self))
        self.layout().setContentsMargins(0, 0, 0, 0)
        self.layout().setAlignment(QtCore.Qt.AlignLeft)
        self.layout().addWidget(self._executeButton)
        
        self._executeButton.setText("execute script")
    
    def executeButton(self):
        return self._executeButton
        
class ScriptEditor(QtGui.QWidget):
    def __init__(self, parent):
        QtGui.QWidget.__init__(self)
        
        self._consoleTextEdit = QtGui.QTextEdit(self)
        self._stdOut = ConsoleOut(self._consoleTextEdit, sys.stdout, QtGui.QColor(200, 190, 200))
        self._stdErr = ConsoleOut(self._consoleTextEdit, sys.stderr, QtGui.QColor(255, 92, 133))
        self._bar = ScriptEditorBar(self)
        self._scriptTextEdit = QtGui.QPlainTextEdit(self)
        
        self.setWindowTitle("script editor")
        self.setLayout(QtGui.QVBoxLayout(self))
        self.layout().setSpacing(5)
        self.layout().setContentsMargins(5, 5, 5, 5)
        self.layout().addWidget(self._consoleTextEdit)
        self.layout().addWidget(self._bar)
        self.layout().addWidget(self._scriptTextEdit)
        self._consoleTextEdit.setReadOnly(True)
        self._consoleTextEdit.setLineWrapMode(QtGui.QTextEdit.NoWrap)
        self._scriptTextEdit.setLineWrapMode(QtGui.QPlainTextEdit.NoWrap)
        
        palette = self._consoleTextEdit.palette()
        palette.setColor(QtGui.QPalette.Base, QtGui.QColor(50, 55, 60))
        self._consoleTextEdit.setPalette(palette)
        
        sys.stdout = self._stdOut
        sys.stderr = self._stdErr
        
        self.connect(self._bar.executeButton(), QtCore.SIGNAL("clicked()"), self._executeScript)
        
        self._restoreSettings()
    
    def _restoreSettings(self):
        mainWin = MainWindow.globalInstance()
        settings = mainWin.settings()
        self._scriptTextEdit.setPlainText(settings.value("scriptEditorText").toString())
        
    def closeEvent(self, event):
        mainWin = MainWindow.globalInstance()
        settings = mainWin.settings()
        settings.setValue("scriptEditorText", self._scriptTextEdit.toPlainText())
        
    def _executeScript(self):
        from .. import coralApp

        exec(str(self._scriptTextEdit.toPlainText()), 
                coralApp.__dict__ # allows for a script to run every coralApp function without the coralApp module being imported
            )

