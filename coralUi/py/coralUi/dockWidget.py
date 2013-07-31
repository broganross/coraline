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


from PyQt4 import QtGui, QtCore
import mainWindow

class DockWidget(QtGui.QDockWidget):
    def __init__(self, contentWidget, parent):
        QtGui.QDockWidget.__init__(self, contentWidget.windowTitle(), parent)
        
        contentWidget.setParent(parent)
        self.setWidget(contentWidget)
        self.setAcceptDrops(True)
        
        self._restoreSettings()
    
    def _restoreSettings(self):
        mainWin = mainWindow.MainWindow.globalInstance()
        geometry = str(mainWin.settings().value(self.windowTitle() + "_geometry").toString())
        if geometry:
            geometry = eval(geometry)
            self.move(geometry[0], geometry[1])
            self.resize(geometry[2], geometry[3])
        
    def _storeSettings(self):
        mainWin = mainWindow.MainWindow.globalInstance()
        mainWin.settings().setValue(self.windowTitle() + "_geometry", str([self.pos().x(), self.pos().y(), self.size().width(), self.size().height()]))
        
    def closeEvent(self, event):
        self._storeSettings()
        
        if self.widget():
            self.widget().close()
        
