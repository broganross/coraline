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
from ...observer import Observer
import nodeEditor
from connection import Connection
from connectionHook import ConnectionHook
from ..._coral import NetworkManager
from attributeUiProxy import AttributeUiProxy


class AttributeUi(QtGui.QGraphicsWidget):
    def __init__(self, coralAttribute, parentNodeUi):
        QtGui.QGraphicsWidget.__init__(self, parentNodeUi)
        
        self._coralAttribute = weakref.ref(coralAttribute)
        self._coralAttributeId = coralAttribute.id()
        self._parentNodeUi = weakref.ref(parentNodeUi)
        self._spacerConstant = 5.0
        self._label = QtGui.QGraphicsSimpleTextItem(self)
        self._inputHook = None
        self._outputHook = None
        self._proxy = None
        self._attributeSpecializedObserver = Observer()
        self._disconnectedInputObserver = Observer()
        self._nameChangedObserver = Observer()
        self._labelSuffix = ""
        self._labelText = ""
        
        if coralAttribute.isInput():
            self._inputHook = ConnectionHook(self, isInput = True)
        else:
            self._outputHook = ConnectionHook(self, isOutput = True)
        
        self._label.setBrush(parentNodeUi.labelsColor())
        self._labelText = coralAttribute.name().split(":")[-1]
        self._label.setText(self._labelText)
        
        self.setHooksColor(self.hooksColor(self._coralAttribute().allowedSpecialization()))
        
        coralApp.addDisconnectedInputObserver(self._disconnectedInputObserver, coralAttribute, self._disconnected)
        coralApp.addAttributeSpecializedObserver(self._attributeSpecializedObserver, coralAttribute, self.specialized)
        coralApp.addNameChangedObserver(self._nameChangedObserver, coralAttribute, self._coralAttributeNameChanged)
        
        if coralAttribute.name().startswith("_"):
            self.setVisible(False)
        
        if parentNodeUi.attributesProxyEnabled():
            self._enableProxy()
        
        self.specialized()
    
    def labelColor(self):
        return self._label.brush().color()
    
    def _coralAttributeNameChanged(self):
        newName = self.coralAttribute().name().split(":")[-1]
        self._labelText = newName
        self._label.setText(self._labelText + self._labelSuffix)
        
        self.parentNodeUi().updateLayout()
        parentNodeScene = self.parentNodeUi().scene()
        if parentNodeScene:
            parentNodeScene.update()
        
        if self._proxy:
            self._proxy()._label.setText(self._labelText + self._labelSuffix)
            self._proxy().updateLayout()
            self._proxy().scene().update()
    
    def label(self):
        return self._label
    
    '''
    Override this method to return a more detailed toolTip.
    '''
    def toolTip(self):
        return self.coralAttribute().shortDebugInfo()
    
    def _disconnected(self):
        if self._inputHook:
            if self._inputHook._connections:
                self._inputHook._connections[0].deleteIt()
        
        elif self._proxy:
            self._proxy().deleteInputConnection()
            
    def onChangedScene(self, scene):
        if self._outputHook:
            self._outputHook.onChangedScene(scene)
        
        if self._inputHook:
            self._inputHook.onChangedScene(scene)
        
        if self._proxy:
            self._proxy().onChangedScene(scene)
    
    def connectTo(self, attributeUi):
        startHook = self._outputHook
        endHook = attributeUi.inputHook()
        
        if endHook is None:
            proxy = attributeUi.proxy()
            if proxy:
                endHook = proxy.inputHook()
        
        if startHook is None:
            if self._proxy:
                startHook = self._proxy().outputHook()
        
        Connection(startHook, endHook)
    
    def _enableProxy(self):
        attr = self._coralAttribute()
        
        if attr.isInput() or attr.isPassThrough():
            proxy = AttributeUiProxy(self)
            self._proxy = weakref.ref(proxy)
            self.parentNodeUi().containedScene().addItem(proxy)
    
    def proxy(self):
        retProxy = None
        
        if self._proxy:
            retProxy = self._proxy()
        
        return retProxy
    
    def hooksColor(self, specialization):
        return QtGui.QColor(255, 255, 255)

    def _updateLabel(self):
        attr = self.coralAttribute()

        val = attr.outValue()
        if attr.isPassThrough():
            source = attr.connectedNonPassThrough()
            if source:
                val = source.outValue()

        self._labelSuffix = ""
        if hasattr(val, "isArray"):
            if val.isArray():
                self._labelSuffix = "[]"
        
        self.label().setText(self._labelText + self._labelSuffix)
    
    def labelSuffix(self):
        return self._labelSuffix

    def specialized(self):
        self._updateLabel()
        
        specialization = self.coralAttribute().specialization()
        color = self.hooksColor(specialization)
        
        mixedColor = False
        if len(specialization) > 1:
            mixedColor = True
            
        self.setHooksColor(color, mixedColor)
        
        self.parentNodeUi().updateLayout()
        if self._outputHook:
            for conn in self._outputHook._connections:
                conn.update()
        
        elif self._inputHook:
            for conn in self._inputHook._connections:
                conn.update()
        
        if self._proxy:
            self._proxy().specialized()
        
    def setHooksColor(self, color, mixedColor = False):
        hook = None
        if self._outputHook:
            hook = self._outputHook
        else:
            hook = self._inputHook
        
        hook.setColor(color)
        hook.setMixedColor(mixedColor)
        
    def coralAttribute(self):
        return self._coralAttribute()
    
    def parentNodeUi(self):
        parent = None
        if self._parentNodeUi:
            parent = self._parentNodeUi()
            
        return parent
    
    def allowConnectionTo(self, targetAttributeUi, errorObject):
        allow = NetworkManager.allowConnection(self._coralAttribute(), targetAttributeUi.coralAttribute(), errorObject)

        return allow
    
    def inputHook(self):
        return self._inputHook
    
    def outputHook(self):
        return self._outputHook
    
    def updateLayout(self):
        height = self._label.boundingRect().height()
        
        hookY = 0
        if self._outputHook:
            hookY = (height - self._outputHook.boundingRect().height()) / 2.0
        elif self._inputHook:
            hookY = (height - self._inputHook.boundingRect().height()) / 2.0
            
        inputHookWidth = self._spacerConstant * 2.0
        if self._inputHook:
            self._inputHook.setPos(0.0, hookY)
        
        self._label.setPos(inputHookWidth + self._spacerConstant, 0)
        
        if self._outputHook:
            self._outputHook.setPos(self._label.pos().x() + self._label.boundingRect().width() + self._spacerConstant, hookY)

            self.resize(self._outputHook.pos().x() + self._outputHook.boundingRect().width(), height)
        else:
            self.resize(self._label.pos().x() + self._label.boundingRect().width(), height)
            
    def setParentNodeUi(self, nodeUi):
        self.setParentItem(None)
        if self.scene():
            if self in self.scene().items():
                self.scene().removeItem(self)
        
        if self._parentNodeUi:
            parentNodeUi = self._parentNodeUi()
            if self in parentNodeUi._attributeUis:
                del parentNodeUi._attributeUis[parentNodeUi._attributeUis.index(self)]
        
        if nodeUi:
            self._parentNodeUi = weakref.ref(nodeUi)
            
            self.setParentItem(nodeUi)
            
            if self not in nodeUi._attributeUis:
                nodeUi._attributeUis.append(self)
            
        else:
            self._parentNodeUi = None
        
    def deleteIt(self):
        if self._outputHook:
            self._outputHook.deleteIt()
        
        elif self._inputHook:
            self._inputHook.deleteIt()
        
        if self._proxy:
            self._proxy().deleteIt()
        
        self.setParentNodeUi(None)
        
        if nodeEditor.NodeEditor._attributeUis.has_key(self._coralAttributeId):
            del nodeEditor.NodeEditor._attributeUis[self._coralAttributeId]
