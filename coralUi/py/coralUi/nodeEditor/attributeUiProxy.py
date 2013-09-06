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
import nodeView

class AttributeUiProxy(QtGui.QGraphicsWidget):
    def __init__(self, attributeUi):
        QtGui.QGraphicsWidget.__init__(self)
        
        self._parentNodeUi = weakref.ref(attributeUi.parentNodeUi())
        self._attributeUi = weakref.ref(attributeUi)
        self._shapePen = self._parentNodeUi().shapePen()
        self._inputHook = None
        self._outputHook = None
        self._label = QtGui.QGraphicsSimpleTextItem(self)
        self._spacerConstant = 5.0
        self._currentMagnifyFactor = 0.0
        self._zValue = self.zValue()
        
        hookColor = QtGui.QColor(100, 100, 100)
        if attributeUi.outputHook():
            hookColor = attributeUi.outputHook().color()
        elif attributeUi.inputHook():
            hookColor = attributeUi.inputHook().color()
        
        if attributeUi.inputHook():
            self._outputHook = ConnectionHook(attributeUi, parentItem = self, isOutput = True)
            self._outputHook.setColor(hookColor)
            self._outputHook.setFlags(QtGui.QGraphicsItem.ItemSendsScenePositionChanges)
        else:
            self._inputHook = ConnectionHook(attributeUi, parentItem = self, isInput = True)
            self._inputHook.setColor(hookColor)
            self._inputHook.setFlags(QtGui.QGraphicsItem.ItemSendsScenePositionChanges)
        
        self.setFlag(QtGui.QGraphicsItem.ItemIsMovable)
        self.setFlag(QtGui.QGraphicsItem.ItemIsSelectable)
        
        self._label.setBrush(attributeUi.labelColor())
        self._label.setText(attributeUi.coralAttribute().name())
        
        self._shapePen.setStyle(QtCore.Qt.NoPen)
        self.setAcceptHoverEvents(True)
        
        self.updateLayout()
    
    def _magnifyAnimStep(self, frame):
        step = frame / nodeView.NodeView._animSteps
        invStep = 1.0 - step
        
        self.setScale((self.scale() * invStep) + ((1.0 * self._currentMagnifyFactor) * step))
        
        if self._inputHook:
            self._inputHook.updateWorldPos()
        elif self._outputHook:
            self._outputHook.updateWorldPos()
                
        self.scene().update()
        
    def _magnify(self, factor):
        self._currentMagnifyFactor = factor
        timer = QtCore.QTimeLine(nodeView.NodeView._animSpeed, self)
        timer.setFrameRange(0, nodeView.NodeView._animSteps)
        
        self.connect(timer, QtCore.SIGNAL("frameChanged(int)"), self._magnifyAnimStep);
        
        timer.start()
    
    def hoverEnterEvent(self, event):
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
    
    def deleteInputConnection(self):
        if self._inputHook:
            if self._inputHook._connections:
                self._inputHook._connections[0].deleteIt()
    
    def onChangedScene(self, scene):
        pass
        
    def deleteIt(self):
        if self._outputHook:
            self._outputHook.deleteIt()
        
        if self._inputHook:
            self._inputHook.deleteIt()
            
        self.scene().removeItem(self)
    
    def coralAttribute(self):
        return self._attributeUi().coralAttribute()
    
    def inputHook(self):
        return self._inputHook
    
    def outputHook(self):
        return self._outputHook
    
    def specialized(self):
        attributeUi = self._attributeUi()
        
        hookColor = QtGui.QColor(100, 100, 100)
        hookMixedColor = hookColor
        if attributeUi.outputHook():
            hook = attributeUi.outputHook()
            hookColor = hook.color()
            hookMixedColor = hook.mixedColor()
        elif attributeUi.inputHook():
            hook = attributeUi.inputHook()
            hookColor = hook.color()
            hookMixedColor = hook.mixedColor()
        
        hook = None
        if self._outputHook:
            hook = self._outputHook
        elif self._inputHook:
            hook = self._inputHook
        
        hook.setColor(hookColor)
        hook.setMixedColor(hookMixedColor)

        self._label.setText(attributeUi._labelText + attributeUi._labelSuffix)

        self.updateLayout()
        self.update()
    
    def itemChange(self, change, value):
        if change == QtGui.QGraphicsItem.ItemSelectedHasChanged:
            if self.isSelected():
                self._shapePen.setStyle(QtCore.Qt.SolidLine)
            else:
                self._shapePen.setStyle(QtCore.Qt.NoPen)
            
        return value
    
    def updateLayout(self):
        labelHeight = self._label.boundingRect().height()
        height = self._spacerConstant + labelHeight + self._spacerConstant
        
        inputHookOffset = 0
        if self._inputHook:
            hookY = (height - self._inputHook.boundingRect().height()) / 2.0
            self._inputHook.setPos(self._spacerConstant, hookY)
            inputHookOffset = self._inputHook.boundingRect().width() + self._inputHook.pos().x()
        
        self._label.setPos(inputHookOffset + self._spacerConstant, (height - labelHeight) / 2.0)
        
        outputHookOffset = self._label.pos().x() + self._label.boundingRect().width()
        if self._outputHook:
            hookY = (height - self._outputHook.boundingRect().height()) / 2.0
            self._outputHook.setPos(outputHookOffset + self._spacerConstant, hookY)
            outputHookOffset = self._outputHook.pos().x() + self._outputHook.boundingRect().width()
            
        self.resize(QtCore.QSizeF(outputHookOffset + self._spacerConstant, height))
    
    def paint(self, painter, option, widget):
        attributeUi = self._attributeUi()
        parentNodeUi = self._parentNodeUi()
        
        shape = QtGui.QPainterPath()
        shape.addRoundedRect(self.rect(), 2, 2)
        
        painter.setPen(self._shapePen)
        painter.setBrush(parentNodeUi.color())
        painter.drawPath(shape)
    
    def data(self):
        data = {"name": self.coralAttribute().name(), "pos": [self.pos().x(), self.pos().y()]}
        
        return data
    
    def setData(self, data):
        if data.has_key("pos"):
            pos = data["pos"]
            self.setPos(pos[0], pos[1])

