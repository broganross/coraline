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
from PyQt4 import QtGui, QtCore

import nodeEditor

class NodeUiScene(QtGui.QGraphicsScene):
    def __init__(self, parentNodeUi):
        QtGui.QGraphicsScene.__init__(self)
        
        self._backgroundColor = QtGui.QColor(50, 55, 60)
        self._parentNodeUi = weakref.ref(parentNodeUi)
        self._gridPen = QtGui.QPen(self._backgroundColor.lighter(120))
        self._zoom = 1.0
        self._centerPos = QtCore.QPointF(0.0, 0.0)
        self._firstTimeEntering = True
                 
        self._gridPen.setWidth(1)
        
        self.setItemIndexMethod(QtGui.QGraphicsScene.NoIndex) # fixes bug with scene.removeItem()
        self.setBackgroundBrush(self._backgroundColor)
    
    def helpEvent(self, event):
        items = self.items(event.scenePos())
        for item in items:
            if hasattr(item, "updateToolTip"):
                item.updateToolTip()
                break
    
        QtGui.QGraphicsScene.helpEvent(self, event)
    
    def mouseReleaseEvent(self, event):
        QtGui.QGraphicsScene.mouseReleaseEvent(self, event)
        
        if event.isAccepted() == False:
            if event.button() == QtCore.Qt.LeftButton:
                from connectionHook import ConnectionHook
                if ConnectionHook._outstandingDraggingConnection:
                    ConnectionHook._removeOutstandingDraggingConnection()  

            elif event.button() == QtCore.Qt.RightButton:
                self._parentNodeUi().showRightClickMenu()
        
        self._selectionChanged()
    
    def _selectionChanged(self):
        nodes = []
        attributes = []
        
        for item in self.selectedItems():
            if hasattr(item, "coralNode"):
                nodes.append(item.coralNode())
            elif hasattr(item, "coralAttribute"):
                attributes.append(item.coralAttribute())
        
        nodeEditor.NodeEditor._setSelection(nodes, attributes, updateSelected = False)
        
    def setZoom(self, zoom):
        self._zoom = zoom
    
    def zoom(self):
        return self._zoom
    
    def setCenterPos(self, pos):
        self._centerPos = QtCore.QPointF(pos)
    
    def centerPos(self):
        return QtCore.QPointF(self._centerPos)
    
    def parentNodeUi(self):
        return self._parentNodeUi()
        
    def drawBackground(self, painter, rect):
        QtGui.QGraphicsScene.drawBackground(self, painter, rect)
        
        painter.setPen(self._gridPen)
        
        gridSize = 50

        left = int(rect.left()) - (int(rect.left()) % gridSize)
        top = int(rect.top()) - (int(rect.top()) % gridSize)

        lines = []
        
        x = left
        while x < rect.right():
            painter.drawLine(x, rect.top(), x, rect.bottom())
            x += gridSize
        
        y = top
        while y < rect.bottom():
            painter.drawLine(rect.left(), y, rect.right(), y)
            y += gridSize

