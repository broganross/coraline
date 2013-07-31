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

from ...import coralApp
from ...observer import Observer
import nodeEditor
from nodeUiScene import NodeUiScene
from connection import Connection
from connectionHook import ConnectionHook
from attributeUi import AttributeUi
import nodeView

class NodeUi(QtGui.QGraphicsWidget):    
    def __init__(self, coralNode):
        QtGui.QGraphicsWidget.__init__(self)
        
        self._coralNode = weakref.ref(coralNode)
        self._coralNodeId = coralNode.id()
        self._spacerConstant = 5.0
        self._label = QtGui.QGraphicsSimpleTextItem(self)
        self._containedScene = NodeUiScene(self)
        self._shapePen = QtGui.QPen(QtCore.Qt.NoPen)
        self._attributeUis = []
        self._brush = QtGui.QBrush(self.color())
        self._selectedColor = QtGui.QColor(255, 255, 255)
        self._dropShadowEffect = QtGui.QGraphicsDropShadowEffect()
        self._canOpenThis = False
        self._parentNodeUi = None
        self._nodeUis = []
        self._showingRightClickMenu = False
        self._rightClickMenuItems = []
        self._nameChangedObserver = Observer()
        self._doubleClicked = False
        self._nodeViewWatching = None
        self._attributesProxyEnabled = False
        self._zValue = self.zValue()
        self._currentMagnifyFactor = 1.0
        
        parentNodeUi = self._addToParentScene(coralNode)
        if parentNodeUi:
            self._parentNodeUi = weakref.ref(parentNodeUi)
        
        self.setGraphicsEffect(self._dropShadowEffect)
        
        self._dropShadowEffect.setOffset(0.0, 10.0)
        self._dropShadowEffect.setBlurRadius(8.0)
        self._dropShadowEffect.setColor(QtGui.QColor(0, 0, 0, 50))
        
        self.setFlag(QtGui.QGraphicsItem.ItemIsMovable)
        self.setFlag(QtGui.QGraphicsItem.ItemIsSelectable)
        self.setAcceptHoverEvents(True)
        
        self._label.setBrush(self.labelsColor())
        
        self._shapePen.setColor(self._selectedColor)
        self._shapePen.setWidthF(1.5)
        
        coralApp.addNameChangedObserver(self._nameChangedObserver, self.coralNode(), self._coralNodeNameChanged)
    
    def _magnifyAnimStep(self, frame):
        step = frame / nodeView.NodeView._animSteps
        invStep = 1.0 - step
        
        self.setScale((self.scale() * invStep) + ((1.0 * self._currentMagnifyFactor) * step))
        
        for attr in self._attributeUis:
            if attr._inputHook:
                attr._inputHook.updateWorldPos()
            if attr._outputHook:
                attr._outputHook.updateWorldPos()
                
        self.scene().update()
        
    def _magnify(self, factor):
        self._currentMagnifyFactor = factor
        timer = QtCore.QTimeLine(nodeView.NodeView._animSpeed, self)
        timer.setFrameRange(0, nodeView.NodeView._animSteps)
        
        self.connect(timer, QtCore.SIGNAL("frameChanged(int)"), self._magnifyAnimStep);
        
        timer.start()
    
    def hoverEnterEvent(self, event):
        if not nodeView.NodeView._panning:
            if nodeView.NodeView._lastHoveredItem is not self:
                if nodeView.NodeView._lastHoveredItem:
                    nodeView.NodeView._lastHoveredItem.hoverLeaveEvent(None)
                    
                zoom = self.scene().zoom()
                if zoom < 0.6:
                    factor =  0.7 / zoom
                    
                    self.setTransformOriginPoint(self.rect().center())
                    self._magnify(factor)
                
                    nodeView.NodeView._lastHoveredItem = self
                
                    self.setZValue(9999999)
        
    def hoverLeaveEvent(self, event):
        if nodeView.NodeView._lastHoveredItem is self:
            self._magnify(1.0)
            
            self.setZValue(self._zValue)
            nodeView.NodeView._lastHoveredItem = None
    
    def setAttributesProxyEnabled(self, value = True):
        self._attributesProxyEnabled = value
    
    def attributesProxyEnabled(self):
        return self._attributesProxyEnabled
    
    def buildFromCoralNode(self):
        coralNode = self._coralNode()
        self._label.setText(coralNode.name())
        
        self._clearAttributeUis()
        
        attrs = []
        for attribute in coralNode.inputAttributes():
            attributeUi = nodeEditor.NodeEditor._createAttributeUi(attribute, self)
            self.addInputAttributeUi(attributeUi)
            
            attrs.append(attributeUi)
            
        for attribute in coralNode.outputAttributes():
            attributeUi = nodeEditor.NodeEditor._createAttributeUi(attribute, self)
            self.addOutputAttributeUi(attributeUi)
            
            attrs.append(attributeUi)
        
        if self._attributesProxyEnabled:
            offset = 0
            for attr in attrs:
                proxy = attr.proxy()
                proxy.setPos(0, offset)
                
                offset += proxy.size().height() * 2
                
        self.updateLayout()
    
    def _clearAttributeUis(self):
        for attrUi in self._attributeUis:
            attrUi.setParent(None)
        
        self._attributeUis = []
        
    def _coralNodeNameChanged(self):
        self._label.setText(self.coralNode().name())
        self.updateLayout()
        self.scene().update()
        
    def updateToolTip(self):
        self.setToolTip(self.toolTip())
    
    def toolTip(self):
        return self.coralNode().shortDebugInfo()
    
    def addRightClickMenuItem(self, label, callback):
        self._rightClickMenuItems.append({label: callback})
    
    def showRightClickMenu(self):
        menu = QtGui.QMenu(nodeEditor.NodeEditor.focusedInstance())
        titleAction = menu.addAction(self.coralNode().name() + ":")
        titleAction.setDisabled(True)
        
        for item in self._rightClickMenuItems:
            label = item.keys()[0]
            callback = item.values()[0]

            menu.addAction(label, callback)
        
        cursorPos = QtGui.QCursor.pos()
        menu.move(cursorPos.x(), cursorPos.y())
        menu.show()
        
    def attributeUis(self, includeHidden = False):
        orderedAttributes = []
        if self._canOpenThis:
            inputAttributes = {}
            outputAttributes = {}
            for attributeUi in self._attributeUis:
                if attributeUi.isVisible() or includeHidden == True:
                    if attributeUi.inputHook():
                        inputAttributes[attributeUi.coralAttribute().name()] = attributeUi
                    else:
                        outputAttributes[attributeUi.coralAttribute().name()] = attributeUi
            
            inputKeys = inputAttributes.keys()
            inputKeys.sort()
            outputKeys = outputAttributes.keys()
            outputKeys.sort()
            for key in inputKeys:
                orderedAttributes.append(inputAttributes[key])
            
            for key in outputKeys:
                orderedAttributes.append(outputAttributes[key])
        else:
            inputAttributes = []
            outputAttributes = []
            for attributeUi in self._attributeUis:
                if attributeUi.isVisible() or includeHidden == True:
                    if attributeUi.inputHook():
                        inputAttributes.append(attributeUi)
                    else:
                        outputAttributes.append(attributeUi)
        
            orderedAttributes = inputAttributes
            orderedAttributes.extend(outputAttributes)
        
        return orderedAttributes
    
    def findAttributeUi(self, name):
        attributeUi = None
        for attr in self._attributeUis:
            if attr.coralAttribute().name() == name:
                attributeUi = attr
        
        return attributeUi
    
    def nodeUis(self):
        return copy.copy(self._nodeUis)
    
    def addNodeUi(self, nodeUi):
        nodeUi.setParentNodeUi(self)
    
    def removeNodeUi(self, nodeUi):
        nodeUiRef = weakref.ref(nodeUi)
        if nodeUiRef in self._nodeUis:
            nodeUiRef().setParentNodeUi(None)
    
    def setParentNodeUi(self, nodeUi):
        if self.scene():
            self.scene().removeItem(self)
        
        if self._parentNodeUi:
            parentNodeUi = self._parentNodeUi()
            if self in parentNodeUi._nodeUis:
                del parentNodeUi._nodeUis[parentNodeUi._nodeUis.index(self)]
        
        if nodeUi:
            self._parentNodeUi = weakref.ref(nodeUi)
            
            if self not in nodeUi.containedScene().items():
                nodeUi.containedScene().addItem(self)
            
            if self not in nodeUi._nodeUis:
                nodeUi._nodeUis.append(self)
                
        else:
            self._parentNodeUi = None
        
        for attr in self._attributeUis:
            attr.onChangedScene(self.scene())
        
    def shapePen(self):
        return QtGui.QPen(self._shapePen)
    
    def parentNodeUi(self):
        parent = None
        if self._parentNodeUi:
            parent = self._parentNodeUi()
        
        return parent
    
    def setCanOpenThis(self, value = True):
        self._canOpenThis = value
    
    def canOpenThis(self):
        return self._canOpenThis
    
    def color(self):
        return QtGui.QColor(0, 0, 0)
    
    def labelsColor(self):
        return QtGui.QColor(255, 255, 255)
    
    def onSelected(self):
        if self.isSelected():
            self._shapePen.setStyle(QtCore.Qt.SolidLine)
        else:
            self._shapePen.setStyle(QtCore.Qt.NoPen)
        
    def itemChange(self, change, value):
        if change == QtGui.QGraphicsItem.ItemSelectedHasChanged:
            self.onSelected()
            
        return value
    
    def _addToParentScene(self, coralNode):
        parentNodeUi = None
        coralNodeParent = coralNode.parent()
        if coralNodeParent:
            parentNodeUi = nodeEditor.NodeEditor.findNodeUi(coralNodeParent.id())
            parentNodeUi.containedScene().addItem(self)
        
        return parentNodeUi
    
    def coralNode(self):
        return self._coralNode()
    
    def containedScene(self):
        return self._containedScene
    
    def removeAttributeUi(self, attributeUi):
        if attributeUi in self._attributeUis:
            del self._attributeUis[self._attributeUis.index(attributeUi)]
        
    def addInputAttributeUi(self, attributeUi):
        if attributeUi not in self._attributeUis:
            self._attributeUis.append(attributeUi)
    
    def addOutputAttributeUi(self, attributeUi):
        if attributeUi not in self._attributeUis:
            self._attributeUis.append(attributeUi)
    
    def updateLayout(self):
        labelWidth = self._label.boundingRect().width()
        width = labelWidth
        yPos = self._label.boundingRect().bottom() + self._spacerConstant
        
        attributes = self.attributeUis()
        
        for attributeUi in attributes:
            if attributeUi.isVisible():
                attributeUi.updateLayout()
            
                attributeUi.setPos(self._spacerConstant, yPos)
                yPos += attributeUi.boundingRect().height()
            
                attributeWidth = attributeUi.boundingRect().width()
                if attributeWidth > width:
                    width = attributeWidth
        
        for attributeUi in self._attributeUis:
            if attributeUi.isVisible():
                outHook = attributeUi.outputHook()
                if outHook:
                    outHook.setPos(width - outHook.boundingRect().width(), outHook.pos().y())
        
        width = self._spacerConstant + width + self._spacerConstant
        self._label.setPos((width - labelWidth) / 2.0, self._spacerConstant)
        
        self.resize(width, yPos + self._spacerConstant)
        self.update()
        
    def paint(self, painter, option, widget):
        shape = QtGui.QPainterPath()
        shape.addRoundedRect(self.rect(), 2, 2)
        
        painter.setPen(self._shapePen)
        painter.setBrush(self._brush)
        painter.drawPath(shape)
    
    def deleteIt(self):
        if self.isSelected():
            nodeEditor.NodeEditor._setSelectedNodes([])
        
        if self._nodeViewWatching:
            parentNodeUi = self.parentNodeUi()
            if parentNodeUi:
                self._nodeViewWatching().setCurrentNodeUi(parentNodeUi)
        
        attrs = copy.copy(self._attributeUis)
        for attr in attrs:
            attr.deleteIt()
        
        nodes = copy.copy(self._nodeUis)
        for node in nodes:
            node.deleteIt()
            
        self.setParentNodeUi(None)
        
        if nodeEditor.NodeEditor._nodeUis.has_key(self._coralNodeId):
            del nodeEditor.NodeEditor._nodeUis[self._coralNodeId]

    def _openThis(self):
        if self._canOpenThis:
            focusedNodeEditor = nodeEditor.NodeEditor.focusedInstance()
            nodeView = focusedNodeEditor.nodeView()
            if nodeView:
                nodeView.setCurrentNodeUi(self)
                self._nodeViewWatching = weakref.ref(nodeView)
                
    def mousePressEvent(self, event):
        if event.button() == QtCore.Qt.RightButton:
            pass
        else:
            QtGui.QGraphicsWidget.mousePressEvent(self, event)
    
    def mouseReleaseEvent(self, event):
        if self._doubleClicked:
            self._doubleClicked = False
            self._openThis()
        elif event.button() == QtCore.Qt.RightButton:
            self.showRightClickMenu()
        else:
            QtGui.QGraphicsWidget.mouseReleaseEvent(self, event)
    
    def mouseMoveEvent(self, event):
        if self._doubleClicked:
            return
        else:
            QtGui.QGraphicsWidget.mouseMoveEvent(self, event)
    
    def mouseDoubleClickEvent(self, event):
        self._doubleClicked = True
    
    # returns the data of this node in a dictionary of primitive data types.
    def data(self):
        data = {"pos": [self.pos().x(), self.pos().y()]}
        
        if self._attributesProxyEnabled:
            attributes = []
            for attrUi in self.attributeUis():
                attributes.append(attrUi.proxy().data())
            
            data["attributes"] = attributes
        
        return data
    
    # takes care of setting the internal data of this node by reading the content of the data dictionary passed in.
    def setData(self, data):
        if data.has_key("pos"):
            pos = data["pos"]
            self.setPos(pos[0], pos[1])
        
        if self._attributesProxyEnabled and data.has_key("attributes"):
            for attrData in data["attributes"]:
                attrName = attrData["name"]
                attrUi = self.findAttributeUi(attrName)
                if attrUi:
                    attrUi.proxy().setData(attrData)

