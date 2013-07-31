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

from connectionHook import ConnectionHook

class Connection(QtGui.QGraphicsItem):
    def __init__(self, startHook, endHook = None):
        QtGui.QGraphicsItem.__init__(self, None, startHook.scene())
        
        self._rect = QtCore.QRectF(0, 0, 0, 0)
        self._startHook = weakref.ref(startHook)
        self._endHook = None
        self._color = startHook.colorRef()
        self._pen = QtGui.QPen(self._color)
        self._pen.setWidth(1)
        self._isTempConnection = False
        self._path = QtGui.QPainterPath()
        
        startHook.addConnection(self)
        
        if endHook:
            self._endHook = weakref.ref(endHook)
            endHook.addConnection(self)
        else:
            # creating a dummy endHook for temporary connection dragging
            self._endHook = weakref.ref(ConnectionHook(startHook.parentAttributeUi(), parentItem = self))
            self._endHook().boundingRect().setSize(QtCore.QSizeF(2.0, 2.0))
            self._isTempConnection = True
            
        self.updateStartPos()
        
        self.setZValue(-1.0)
    
    def setDashedPen(self, value = True):
        if value:
            self._pen.setStyle(QtCore.Qt.DashLine)
        else:
            self._pen.setStyle(QtCore.Qt.SolidLine)
    
    def deleteIt(self):
        if self.scene():
            self.scene().removeItem(self)
        
        if self._endHook:
            if self._endHook():
                self._endHook().removeConnection(self)
                self._endHook().parentAttributeUi().parentNodeUi().update()
            
            if self._startHook():
                self._startHook().removeConnection(self)
            
    def updateStartPos(self):
        startHook = self._startHook()
        self.setPos(startHook.scenePos() + startHook.boundingRect().center())
        
        self._updatePath()
    
    def updateEndPos(self):
        self._updatePath()
    
    def setColor(self, color):
        self._color = QtGui.QColor(color)
    
    def setEndHook(self, connectionHook):
        self._endHook = weakref.ref(connectionHook)
    
    def endHook(self):
        return self._endHook()
    
    def startHook(self):
        return self._startHook()
    
    def setEndHook(self, connectionHook):
        self._endPos = connectionHook.pos()
        self._endTangent = connectionHook.inputTangent()
    
    def findClosestHook(self):
        closestHook = None
        
        collidingItems = self._endHook().collidingItems(QtCore.Qt.IntersectsItemBoundingRect)
        for collidingItem in collidingItems:
            if type(collidingItem) is ConnectionHook:
                if collidingItem.isInput() and collidingItem.isVisible():
                    closestHook = collidingItem
                break
        
        return closestHook
    
    def _updatePath(self):
        self.prepareGeometryChange()
        
        endHook = self._endHook()
        endPos = self.mapFromItem(endHook, 0.0, 0.0) + endHook.boundingRect().center()
        tangentLength = (abs(endPos.x()) / 2.0) + (abs(endPos.y()) / 4.0)
        
        startTangent = QtCore.QPointF(tangentLength, 0.0)
        endTangent = QtCore.QPointF(endPos.x() - tangentLength, endPos.y())
        
        path = QtGui.QPainterPath()
        path.cubicTo(startTangent, endTangent, endPos)
        
        strokeWidth = self._pen.widthF()
        rect = path.boundingRect().adjusted(-strokeWidth, -strokeWidth, strokeWidth, strokeWidth)
        
        self._path = path
        self._rect = rect
        
    def boundingRect(self):
        return self._rect
    
    def paint(self, painter, option, widget):
        self._pen.setColor(self._color)
        painter.setPen(self._pen)
        painter.drawPath(self._path)
