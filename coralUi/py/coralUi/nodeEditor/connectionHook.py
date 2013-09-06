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


import copy
import weakref
from PyQt4 import QtGui, QtCore

from ... import coralApp
from ..._coral import ErrorObject
import connection
import nodeView

class ConnectionHook(QtGui.QGraphicsItem):
    _outstandingDraggingConnection = None
    
    @staticmethod
    def _removeOutstandingDraggingConnection():
        if ConnectionHook._outstandingDraggingConnection:
            draggingConnection = ConnectionHook._outstandingDraggingConnection()
            draggingConnection.deleteIt()
            ConnectionHook._outstandingDraggingConnection = None
    
    def __init__(self, parentAttributeUi, parentItem = None, isInput = False, isOutput = False):
        if parentItem is None:# parentItem is used by builtinUis.ContainedAttributeUiProxy
            parentItem = parentAttributeUi
            
        QtGui.QGraphicsItem.__init__(self, parentItem)
        
        self._parentNodeUi = weakref.ref(parentAttributeUi.parentNodeUi())
        self._parentAttributeUi = weakref.ref(parentAttributeUi)
        self._isInput = isInput
        self._isOutput = isOutput
        self._rect = QtCore.QRectF(0, 0, 12, 12)
        self._color = QtGui.QColor(200, 200, 200)
        self._brush = QtGui.QBrush(self.color())
        self._pen = QtGui.QPen(QtCore.Qt.NoPen)
        self._draggingConnection = None
        self._draggingConnectionEndHook = None
        self._connections = []
        self._mixedColor = False
        self._lastDropNode = None
        
        self.setFlags(QtGui.QGraphicsItem.ItemSendsScenePositionChanges)
    
        self._pen.setWidthF(1.0)

        self.setAcceptsHoverEvents(True)
    
    def hoverEnterEvent(self, event):
        for conn in self._connections:
            conn._pen.setWidth(2)
            conn.update()

    def hoverLeaveEvent(self, event):
        for conn in self._connections:
            conn._pen.setWidth(1)
            conn.update()

    def setMixedColor(self, value = True):
        self._mixedColor = value
    
    def setBorderEnabled(self, value = True):
        if value:
            self._pen.setStyle(QtCore.Qt.SolidLine)
        else:
            self._pen.setStyle(QtCore.Qt.NoPen)
    
    def updateToolTip(self):
        self.setToolTip(self._parentAttributeUi().toolTip())
    
    def onChangedScene(self, scene):
        for conn in self._connections:
            if conn.scene():
                conn.scene().removeItem(conn)
            
            if scene:
                scene.addItem(conn)
        
    def itemChange(self, change, value):
        if change == QtGui.QGraphicsItem.ItemScenePositionHasChanged:
            self.updateWorldPos()
                    
        return value
    
    def updateWorldPos(self):
        if self._isInput:
            for conn in self._connections:
                conn.updateEndPos()
        else:
            for conn in self._connections:
                conn.updateStartPos()
    
    def addConnection(self, connection):
        self._connections.append(connection)
    
    def removeConnection(self, connection):
        if connection in self._connections:
            del self._connections[self._connections.index(connection)]
    
    def connections(self):
        return copy.copy(self._connections)
        
    def parentAttributeUi(self):
        return self._parentAttributeUi()
    
    def parentNodeUi(self):
        return self._parentNodeUi()
    
    def setColor(self, color):
        self._color.setRgb(color.red(), color.green(), color.blue())
        self._brush.setColor(self._color)
        self._pen.setColor(self._color.darker(150))
    
    def color(self):
        return QtGui.QColor(self._color)
    
    def mixedColor(self):
        return self._mixedColor
    
    def setColorRef(self, color):
        self._color = color
    
    def colorRef(self):
        return self._color
        
    def mousePressEvent(self, event):
        if self._isOutput:
            self._draggingConnection = connection.Connection(self)
            self._draggingConnection._pen.setWidth(2)
            ConnectionHook._outstandingDraggingConnection = weakref.ref(self._draggingConnection)
        elif self._connections:
            inputConnection = self._connections[0]
            
            coralAttribute = self.parentAttributeUi().coralAttribute()
            coralApp.executeCommand("DisconnectInput", attribute = coralAttribute.fullName())
            self.parentAttributeUi().parentNodeUi().update()
            
            outHook = inputConnection.startHook()
            self._draggingConnection = connection.Connection(outHook)
            
            mousePos = self._draggingConnection.mapFromScene(event.scenePos())
            self._draggingConnection.endHook().setPos(mousePos)
            self._draggingConnection.updateEndPos()
    
    def _handleHover(self, item):
        nodeHovered = None
        
        collidingItems = item.collidingItems(QtCore.Qt.IntersectsItemBoundingRect)
        if collidingItems:
            nodeHovered = collidingItems[0]

        if nodeHovered:
            nodeHovered.hoverEnterEvent(None)
        elif nodeView.NodeView._lastHoveredItem:
            nodeView.NodeView._lastHoveredItem.hoverLeaveEvent(None)
    
    def mouseMoveEvent(self, event):
        if self._draggingConnection:
            connectionStartHook = self._draggingConnection.startHook()
            self._draggingConnectionEndHook = None
            self._draggingConnection.setColor(connectionStartHook.color())
            self._draggingConnection.setDashedPen(True)
            
            connectionEndHook = self._draggingConnection.endHook()
            
            mousePos = self._draggingConnection.mapFromScene(event.scenePos())
            connectionEndHook.setPos(mousePos)
            
            self._handleHover(self._draggingConnection.endHook())
            
            endHook = self._draggingConnection.findClosestHook()
            
            if endHook:
                sourceAttribute = connectionStartHook.parentAttributeUi()
                destinationAttribute = endHook.parentAttributeUi()
                
                errorObject = ErrorObject()
                if sourceAttribute.allowConnectionTo(destinationAttribute, errorObject):
                    hookSize = endHook.boundingRect().bottomRight() / 2.0
                    hookPos = self._draggingConnection.mapFromItem(endHook, hookSize.x(), hookSize.y())
                    connectionEndHook.setPos(hookPos)
                    self._draggingConnectionEndHook = endHook
                    self._draggingConnection.setDashedPen(False)
                else:
                    self._draggingConnection.setColor(QtGui.QColor(255.0, 5.0, 10.0))
                
                message = errorObject.message()
                if message:
                    QtGui.QToolTip.showText(event.screenPos(), message)
            else:
                if QtGui.QToolTip.isVisible():
                    QtGui.QToolTip.hideText()
                
            self._draggingConnection.updateEndPos()
    
    def mouseReleaseEvent(self, event):
        if self._draggingConnection:
            startHook = self._draggingConnection.startHook()
            self._draggingConnection.deleteIt()
            self._draggingConnection = None
            ConnectionHook._outstandingDraggingConnection = None
            
            if self._draggingConnectionEndHook:
                sourceAttribute = startHook.parentAttributeUi().coralAttribute()
                destinationAttribute = self._draggingConnectionEndHook.parentAttributeUi().coralAttribute()
                self._draggingConnectionEndHook = None
                
                success = coralApp.executeCommand("ConnectAttributes", sourceAttribute = sourceAttribute.fullName(), destinationAttribute = destinationAttribute.fullName())
                
                self.parentAttributeUi().parentNodeUi().update()
    
    def boundingRect(self):
        return self._rect
    
    def paint(self, painter, option, widget):
        painter.setBrush(self._brush)
        painter.setPen(self._pen)
        painter.drawEllipse(self._rect)
        
        if self._mixedColor:
            painter.setBrush(painter.brush().color().darker(130))
            painter.drawChord(self._rect, 1 * 16, 180 * 16)
    
    def isInput(self):
        return self._isInput
    
    def isOutput(self):
        return self._isOutput
    
    def deleteIt(self):
        conns = copy.copy(self._connections)
        for conn in conns:
            conn.deleteIt()
