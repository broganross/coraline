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


import weakref
from PyQt4 import QtGui, QtCore, QtOpenGL
import _coralUi
from .. import _coral
from ..observer import Observer
from .. import coralApp
import mainWindow

class ViewportData:
    _viewports = []
    _cameraNodes = []

def addCameraNode(node):
    if hasattr(node, "cameraChanged"):
        ViewportData._cameraNodes.append(weakref.ref(node))

def removeCameraNode(node):
    for nodeRef in ViewportData._cameraNodes:
        currNode = nodeRef()
        if currNode is node:
            del ViewportData._cameraNodes[ViewportData._cameraNodes.index(nodeRef)]
            break

def instancedViewports():
    vports = []
    for viewport in ViewportData._viewports:
        vports.append(viewport())
    
    return vports

class ViewportGlWidget(QtOpenGL.QGLWidget):
    orbit = 1
    pan = 2
    timedRefresh = 0
    immediateRefresh = 1
    _sharedWidget = None
        
    def __init__(self, parent = None):
        if ViewportGlWidget._sharedWidget:
            QtOpenGL.QGLWidget.__init__(self, parent, ViewportGlWidget._sharedWidget)
        else:
            ViewportGlWidget._sharedWidget = self
            QtOpenGL.QGLWidget.__init__(self, QtOpenGL.QGLFormat(QtOpenGL.QGL.SampleBuffers), parent)
        
        self.makeCurrent()
        self._viewport = _coralUi.Viewport()
        self._oldPos = QtCore.QPoint(0, 0)
        self._pressed = False
        self._mode = 0
        
        self.setFocusPolicy(QtCore.Qt.StrongFocus)
        self.connect(mainWindow.MainWindow.globalInstance(), QtCore.SIGNAL("coralViewportUpdateGL"), QtCore.SLOT("updateGL()"))
        self.connect(mainWindow.MainWindow.globalInstance(), QtCore.SIGNAL("coralViewportRefreshMode(int)"), self._changeRefreshMode)

        ViewportData._viewports.append(weakref.ref(self._viewport))

        self._timer = -1

    def _changeRefreshMode(self, mode):
        if mode == ViewportGlWidget.timedRefresh:
            if self._timer == -1:
                self._timer = self.startTimer(50)
        elif mode == ViewportGlWidget.immediateRefresh:
            if self._timer != -1:
                self.killTimer(self._timer)
                self._timer = -1
        
    def timerEvent(self, event):
        self.makeCurrent()
        self.updateGL()

    def closeEvent(self, event):
        self._viewport = None

        self._removeGlobally()

        QtCore.QObject.disconnect(mainWindow.MainWindow.globalInstance(), QtCore.SIGNAL("coralViewportUpdateGL"), self, QtCore.SLOT("updateGL()"))
        QtCore.QObject.disconnect(mainWindow.MainWindow.globalInstance(), QtCore.SIGNAL("coralViewportRefreshMode(int)"), self._changeRefreshMode)

        if self._timer:
            self.killTimer(self._timer)
            self._timer = -1

    def _removeGlobally(self):
        for viewportRef in ViewportData._viewports:
            viewport = viewportRef()
            if viewport is self._viewport:
                del ViewportData._viewports[ViewportData._viewports.index(viewportRef)]
                break
    
    def __del__(self):
        self._removeGlobally()
    
    def minimumSizeHint(self):
        return QtCore.QSize(100, 100)
    
    def sizeHint(self):
        return QtCore.QSize(500, 500)
    
    def initializeGL(self):
        self.makeCurrent()
        self._viewport.initializeGL()

        self._dirtyCameraNodes()

    def resizeGL(self, w, h):
        self.makeCurrent()
        self._viewport.resizeGL(w, h)

        self._dirtyCameraNodes()

    def paintGL(self):
        self.makeCurrent()
        self._viewport.draw()

    def keyPressEvent(self, qKeyEvent):
        pass
        
    def keyReleaseEvent(self, qKeyEvent):
        pass
        
    def mousePressEvent(self, qMouseEvent):
        self.setFocus()
        self._pressed = True
        self._oldPos = qMouseEvent.pos()
        
        if qMouseEvent.button() == QtCore.Qt.LeftButton and qMouseEvent.modifiers() == QtCore.Qt.AltModifier:
            self._mode = ViewportGlWidget.orbit
        elif qMouseEvent.button() == QtCore.Qt.MiddleButton and qMouseEvent.modifiers() == QtCore.Qt.AltModifier:
            self._mode = ViewportGlWidget.pan
        elif qMouseEvent.button() == QtCore.Qt.LeftButton and qMouseEvent.modifiers() == QtCore.Qt.AltModifier | QtCore.Qt.ControlModifier: # Os X
            self._mode = ViewportGlWidget.pan
        
    def mouseReleaseEvent(self, qMouseEvent):
        self.makeCurrent()
        self._pressed = False
        self._mode = 0

        self._dirtyCameraNodes()
        
    def mouseMoveEvent(self, qMouseEvent):
        self.makeCurrent()
        if self._pressed:
            pos = qMouseEvent.pos()
            deltaPos = pos - self._oldPos
            
            if self._mode == ViewportGlWidget.orbit:
                self._viewport.orbit(deltaPos.x(), deltaPos.y())
            elif self._mode == ViewportGlWidget.pan:
                self._viewport.pan(deltaPos.x(), deltaPos.y())
            
            self._oldPos = qMouseEvent.pos()
            
            self.updateGL()
            
    def wheelEvent(self, qWheelEvent):
        self.makeCurrent()
        if qWheelEvent.orientation() == QtCore.Qt.Vertical:
            self._viewport.dolly(int(qWheelEvent.delta() * -0.1))
            
            self.updateGL()

            self._dirtyCameraNodes()
    
    def _dirtyCameraNodes(self):
        self.makeCurrent()
        for cameraNodeRef in ViewportData._cameraNodes:
            cameraNode = cameraNodeRef()
            cameraNode.cameraChanged()

class ViewportWidget(QtGui.QWidget):
    _mainWin = mainWindow.MainWindow.globalInstance()
    _initialized = False
    _networkLoadedObserver = Observer()
    
    @staticmethod
    def refreshViewports():
        ViewportWidget._mainWin.emit(QtCore.SIGNAL("coralViewportUpdateGL"))
    
    @staticmethod
    def _activateTimedRefresh():
        ViewportWidget._disableRefreshCallback()
        ViewportWidget._mainWin.emit(QtCore.SIGNAL("coralViewportRefreshMode(int)"), ViewportGlWidget.timedRefresh)
    
    @staticmethod
    def _activateImmediateRefresh():
        ViewportWidget._mainWin.emit(QtCore.SIGNAL("coralViewportRefreshMode(int)"), ViewportGlWidget.immediateRefresh)
        ViewportWidget._enableRefreshcallback()
    
    @staticmethod
    def _disableRefreshCallback():
        _coral.setCallback("mainDrawRoutine_viewportRefresh", None)
    
    @staticmethod
    def _enableRefreshcallback():
        _coral.setCallback("mainDrawRoutine_viewportRefresh", ViewportWidget.refreshViewports)
        ViewportWidget.refreshViewports()
        
    def __init__(self, parent):
        QtGui.QWidget.__init__(self, parent)
        
        self.setWindowTitle("viewport")
        
        self._viewportGlWidget = ViewportGlWidget(self)
        self._networkLoadingObserver = Observer()
        self._networkLoadedObserver = Observer()
        self._initializingNewNetworkObserver = Observer()
        self._initializedNewNetworkObserver = Observer() 

        self.setLayout(QtGui.QVBoxLayout(self))
        self.setContentsMargins(0, 0, 0, 0)
        self.layout().setContentsMargins(5, 5, 5, 5)
        self.layout().setSpacing(0)
        self.layout().addWidget(self._viewportGlWidget)
        
        _coral.setCallback("mainDrawRoutine_viewportRefresh", ViewportWidget.refreshViewports)

        coralApp.addInitializingNewNetworkObserver(self._initializingNewNetworkObserver, ViewportWidget._disableRefreshCallback)
        coralApp.addInitializedNewNetworkObserver(self._initializedNewNetworkObserver, ViewportWidget._enableRefreshcallback)
        coralApp.addNetworkLoadingObserver(self._networkLoadingObserver, ViewportWidget._disableRefreshCallback)
        coralApp.addNetworkLoadedObserver(self._networkLoadedObserver, ViewportWidget._enableRefreshcallback)
        
        if not ViewportWidget._initialized:
            coralApp.addNetworkLoadedObserver(ViewportWidget._networkLoadedObserver, ViewportWidget.refreshViewports)
            ViewportWidget._initialized = True

    def closeEvent(self, event):
        if self._viewportGlWidget:
            self._viewportGlWidget.close()

            if self._viewportGlWidget is not ViewportGlWidget._sharedWidget:
                # we will leave ViewportGlWidget._sharedWidget alive to guarantee the first context created doesn't get destroyed
                self._viewportGlWidget = None
                self.destroy()

