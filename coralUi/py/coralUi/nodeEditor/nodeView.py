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

from ... import coralApp
from .. import coralUi
from .. import nodeBox
import nodeEditor

class NodeView(QtGui.QGraphicsView):
    _lastHoveredItem = None
    _animSpeed = 50.0
    _animSteps = 50.0
    _panning = False
    
    def __init__(self, parent):
        QtGui.QGraphicsView.__init__(self, parent)
        
        self._zoom = 1.0
        self._currentNodeUi = None
        self._panning = False
        self._currentCenterPoint = QtCore.QPointF()
        self._lastPanPoint = QtCore.QPoint()
        
        self.setFocusPolicy(QtCore.Qt.ClickFocus)
        self.setAcceptDrops(True)
        self.setRenderHint(QtGui.QPainter.Antialiasing)
        self.setDragMode(QtGui.QGraphicsView.RubberBandDrag)
        self.setSceneRect(-5000, -5000, 10000, 10000)
        self.setHorizontalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
        self.setVerticalScrollBarPolicy(QtCore.Qt.ScrollBarAlwaysOff)
        
        QtGui.QShortcut(QtGui.QKeySequence("Delete"), self, self._deleteKey)
        QtGui.QShortcut(QtGui.QKeySequence("Backspace"), self, self._deleteKey)
        QtGui.QShortcut(QtGui.QKeySequence("Ctrl+C"), self, self._copyKey)
        QtGui.QShortcut(QtGui.QKeySequence("Ctrl+V"), self, self._pasteKey)
    
    def _selectedNodesName(self):
        selectedNodes = self._selectedNodes()
        
        nodesName = []
        for node in selectedNodes:
            nodesName.append(node.fullName())
        
        return nodesName
    
    def _selectedAttributesName(self):
        selectedAttributes = self._selectedAttributes()
        attributesName = []
        for attribute in selectedAttributes:
            attributesName.append(attribute.fullName())
        
        return attributesName
    
    def _copyKey(self):
        nodesName = self._selectedNodesName()
        attributesName = self._selectedAttributesName()
        
        if nodesName or attributesName:
            clipboardData = {"nodesName": nodesName}
            coralUi.copyToClipboard(clipboardData)
    
    def _pasteKey(self):
        clipboardData = coralUi.clipboardData()
        if clipboardData:
            nodesName = []
            if clipboardData.has_key("nodesName"):
                nodesName = clipboardData["nodesName"]
            
            pastedNodesStr = coralApp.executeCommand("Paste", nodes = nodesName, parentNode = self._currentNodeUi().coralNode().fullName())
            pastedNodes = eval(pastedNodesStr)
            
            self.scene().clearSelection()
            for pastedNode in pastedNodes:
                node = coralApp.findNode(pastedNode)
                nodeUi = nodeEditor.NodeEditor.findNodeUi(node.id())
                nodeUi.setPos(nodeUi.pos().x() + 10.0, nodeUi.pos().y() + 20.0)
                nodeUi.setSelected(True)
            
            self.scene()._selectionChanged()
        
    def _deleteKey(self):
        nodesName = self._selectedNodesName()
        attributesName = self._selectedAttributesName()

        if nodesName or attributesName:
            self.setSelectedItems([])
            coralApp.executeCommand("DeleteObjects", nodes = nodesName, attributes = attributesName)
    
    def currentNodeUi(self):
        nodeUi = None
        
        if self._currentNodeUi:
            nodeUi = self._currentNodeUi()
        
        return nodeUi
    
    def focusInEvent(self, focusEvent):
        nodeEditor.NodeEditor._setFocusedInstance(self.parentWidget())
    
    def setCurrentNodeUi(self, nodeUi):
        if self._currentNodeUi:
            oldNodeUi = self._currentNodeUi()
            if oldNodeUi:
                oldNodeUi.containedScene().clearSelection()
        
        newScene = nodeUi.containedScene()
        self._currentNodeUi = weakref.ref(nodeUi)
        self.setScene(newScene)
        
        self.setZoom(newScene.zoom())

        newCenter = newScene.centerPos()
        self.centerOn(newCenter)
        self._currentCenterPoint = newCenter
        
        newScene.clearSelection()
        newScene._selectionChanged()

        if newScene._firstTimeEntering:
            self.frameSceneContent()
            newScene._firstTimeEntering = False
        
        self.emit(QtCore.SIGNAL("currentNodeUiChanged"))
    
    def frameSceneContent(self):
        self.scene().setCenterPos(self.scene().itemsBoundingRect().center())
        newCenter = self.scene().centerPos()
        self.centerOn(newCenter)
        self._currentCenterPoint = newCenter
        
    def dragMoveEvent(self, event):
        event.accept()
    
    def dropEvent(self, event):
        if coralUi.dropDataType() == "nodeBoxItemData":
            event.accept()
            sceneItem = nodeBox.NodeBox._createFromItemData(coralUi.dropData())
            
            if sceneItem:
                scenePos = self.mapToScene(event.pos())
                sceneItem.setPos(scenePos)
                self.setSelectedItems([sceneItem])
        
    def setSelectedItems(self, items):
        self.scene().clearSelection()
        for item in items:
            item.setSelected(True)
        
        self.scene()._selectionChanged()
    
    def getCenter(self):
        return self._currentCenterPoint
    
    def setCenter(self, centerPoint):
        self._currentCenterPoint = centerPoint
        self.scene().setCenterPos(centerPoint)
        self.centerOn(self._currentCenterPoint);
    
    def mousePressEvent(self, mouseEvent):
        if mouseEvent.modifiers() == QtCore.Qt.AltModifier:
            self._lastPanPoint = mouseEvent.pos()
            self.setCursor(QtCore.Qt.ClosedHandCursor)
            self._panning = True
            NodeView._panning = True
        else:
            QtGui.QGraphicsView.mousePressEvent(self, mouseEvent)
    
    def mouseMoveEvent(self, mouseEvent):
        if self._panning:
            delta = self.mapToScene(self._lastPanPoint) - self.mapToScene(mouseEvent.pos())
            self._lastPanPoint = mouseEvent.pos()
            
            self.setCenter(self.getCenter() + delta)
        else:
            QtGui.QGraphicsView.mouseMoveEvent(self, mouseEvent)
    
    def mouseReleaseEvent(self, mouseEvent):
        if self._panning:
            self.setCursor(QtCore.Qt.ArrowCursor)
            self._lastPanPoint = QtCore.QPoint()
            self._panning = False
            NodeView._panning = False
        else:
            QtGui.QGraphicsView.mouseReleaseEvent(self, mouseEvent)
    
    def wheelEvent(self, event):
        if event.orientation() == QtCore.Qt.Vertical:
            delta = event.delta()
            zoom = self._zoom
            if delta > 0:
                zoom += 0.05
            else:
                zoom -= 0.05
            
            self.setZoom(zoom)
    
    def _selectedNodes(self):
        nodes = []
        
        sel = self.scene().selectedItems()
        for item in sel:
            if hasattr(item, "coralNode"):
                nodes.append(item.coralNode())
        
        return nodes
    
    def _selectedAttributes(self):
        attributes = []
        
        sel = self.scene().selectedItems()
        for item in sel:
            if hasattr(item, "coralAttribute"):
                attributes.append(item.coralAttribute())
        
        return attributes
    
    def setZoom(self, zoom):
        self._zoom = zoom
        
        if zoom >= 1.0:
            self._zoom = 1.0
        elif zoom <= 0.1:
            self._zoom = 0.1
        
        transform = self.transform()
        newTransform = QtGui.QTransform.fromTranslate(transform.dx(), transform.dy())
        newTransform.scale(self._zoom, self._zoom)
        self.setTransform(newTransform)
        
        self.scene().setZoom(self._zoom)

