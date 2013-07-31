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
from ... import utils
from ...observer import Observer
from ..nodeEditor import nodeEditor
from .. import mainWindow

import fields

class CustomComboBox(QtGui.QComboBox):
    def __init__(self, parent):
        QtGui.QComboBox.__init__(self, parent)
        
        self._showPopupCallback = None
        self._currentItemChangedCallback = None
        self._currentItemChangedCallbackEnabled = True
        
        self.connect(self, QtCore.SIGNAL("currentIndexChanged(QString)"), self._currentItemChanged)
    
    def setShowPopupCallback(self, callback):
        self._showPopupCallback = utils.weakRef(callback)
 
    def setCurrentItemChangedCallback(self, callback):
        self._currentItemChangedCallback = utils.weakRef(callback)
    
    def _currentItemChanged(self, itemText):
        if self._currentItemChangedCallbackEnabled:
            if self._currentItemChangedCallback:
                self._currentItemChangedCallback()
    
    def showPopup(self):
        self._currentItemChangedCallbackEnabled = False
        
        if self._showPopupCallback:
            self._showPopupCallback()
        
        QtGui.QComboBox.showPopup(self)
        
        self._currentItemChangedCallbackEnabled = True

class SpecializationCombo(QtGui.QWidget):
    def __init__(self, parent):
        QtGui.QWidget.__init__(self, parent)
        
        self._label = QtGui.QLabel("specialization:", self)
        self._combo = CustomComboBox(self)
        
        self.setLayout(QtGui.QHBoxLayout(self))
        self.layout().setContentsMargins(0, 0, 0, 0)
        self.layout().setSpacing(0)
        self.layout().addWidget(self._label)
        self.layout().addWidget(self._combo)

class NodeInspectorWidget(QtGui.QWidget):
    def __init__(self, coralNode, parent):
        QtGui.QWidget.__init__(self, parent)
        
        self._mainLayout = QtGui.QVBoxLayout(self)
        self._coralNode = weakref.ref(coralNode)
        self._nameField = None
        self._nameEditable = False
        self._attributeWidgets = {}
        self._presetCombo = None
        self._nodeInspector = weakref.ref(parent)
        
        self.setLayout(self._mainLayout)
        self._mainLayout.setContentsMargins(0, 0, 0, 0)
        self._mainLayout.setSpacing(2)
    
    def nodeInspector(self):
        return self._nodeInspector()

    def attributeWidget(self, name):
        widget = None
        if self._attributeWidgets.has_key(name):
            if self._attributeWidgets[name]() is not None:
                widget = self._attributeWidgets[name]()
        
        return widget
    
    def setNameEditable(self, value = True):
        self._nameEditable = value
    
    def coralNode(self):
        node = None
        if self._coralNode:
            node = self._coralNode()
        
        return node
    
    def _hasDefaultSpecialization(self, coralNode):
        for attr in coralNode.attributes():
            if attr.defaultSpecialization():
                return True
        
        return False
    
    def _nodeIsConnected(self, coralNode):
        for attr in coralNode.attributes():
            if attr.input():
                return True
            elif attr.outputs():
                return True
        
        return False
    
    def _presetComboItemChanged(self):
        coralNode = self._coralNode()
        combo = self._presetCombo._combo
        selectedPreset = str(combo.currentText())
        
        if selectedPreset.endswith("(not allowed)") == False:
            coralNode.enableSpecializationPreset(selectedPreset)
        else:
            coralNode.enableSpecializationPreset("none")
            combo.setCurrentIndex(combo.count() - 1)
        
    def _populatePresetCombo(self):
        coralNode = self._coralNode()
        coralNode.enableSpecializationPreset("none")
        
        combo = self._presetCombo._combo
        combo.clear()
        
        presets = coralNode.specializationPresets()
        
        for preset in presets:
            presetAllowed = True
            if preset != "none":
                for attr in coralNode.attributes():
                    presetSpecialization = coralNode.attributeSpecializationPreset(preset, attr)
                    if presetSpecialization:
                        currentSpecialization = attr.specialization()
                        if presetSpecialization not in currentSpecialization:
                            presetAllowed = False
                            break
                
            if presetAllowed:
                combo.addItem(preset)
            else:
                combo.addItem(preset + " (not allowed)")
        
        currentPreset = coralNode.enabledSpecializationPreset()
        combo.setCurrentIndex(presets.index(currentPreset))
        
    def _updatePresetCombo(self):
        coralNode = self._coralNode()
        
        presets = coralNode.specializationPresets()
        
        if len(presets) > 1:
            self._presetCombo = SpecializationCombo(self)
            self.layout().addWidget(self._presetCombo)
            
            currentPreset = coralNode.enabledSpecializationPreset()
            self._presetCombo._combo.addItem(currentPreset)
            self._presetCombo._combo.setCurrentIndex(0)
            
            self._presetCombo._combo.setShowPopupCallback(self._populatePresetCombo)
            self._presetCombo._combo.setCurrentItemChangedCallback(self._presetComboItemChanged)
            
            #if self._nodeIsConnected(coralNode):
            #    self._presetCombo._combo.setDisabled(True)
    
    def _findFirstConnectedAtributeNonPassThrough(self, coralAttribute, processedAttributes):
        foundAttr = None
        if coralAttribute not in processedAttributes:
            processedAttributes.append(coralAttribute)
    
            if coralAttribute.isPassThrough() == False:
                return coralAttribute
            else:
                if coralAttribute.input():
                    foundAttr = self._findFirstConnectedAtributeNonPassThrough(coralAttribute.input(), processedAttributes)
                    if foundAttr:
                        return foundAttr
                
                for out in coralAttribute.outputs():
                    foundAttr = self._findFirstConnectedAtributeNonPassThrough(out, processedAttributes)
                    if foundAttr:
                        return foundAttr

        return foundAttr
    
    def build(self):
        coralNode = self.coralNode()
        
        if self._nameEditable:
            self._nameField = fields.NameField(coralNode, self)
            self.layout().addWidget(self._nameField)
        
        self._updatePresetCombo()
        
        nodeUi = nodeEditor.NodeEditor.findNodeUi(coralNode.id())
        attrUis = nodeUi.attributeUis(includeHidden = True)
        
        for attrUi in attrUis:
            attribute = attrUi.coralAttribute()
            if attribute.name().startswith("_") == False:
                className = attribute.className()
                if className == "PassThroughAttribute":
                    processedAttrs = []
                    sourceAttr = self._findFirstConnectedAtributeNonPassThrough(attribute, processedAttrs)
                    if sourceAttr:
                        className = sourceAttr.className()
                    
                if NodeInspector._inspectorWidgetClasses.has_key(className):
                    inspectorWidgetClass = NodeInspector._inspectorWidgetClasses[className]
                    inspectorWidget = inspectorWidgetClass(attribute, self)
                    self._attributeWidgets[attribute.name()] = weakref.ref(inspectorWidget)
                    self._mainLayout.addWidget(inspectorWidget)
                    inspectorWidget.build()

class ProxyAttributeInspectorWidget(QtGui.QWidget):
    def __init__(self, coralAttribute, parent):
        QtGui.QWidget.__init__(self, parent)
        
        self._mainLayout = QtGui.QVBoxLayout(self)
        self._coralAttribute = weakref.ref(coralAttribute)
        self._nameField = None
        self._specializationCombo = None
        
        self.setLayout(self._mainLayout)
        self._mainLayout.setContentsMargins(0, 0, 0, 0)
        self._mainLayout.setSpacing(0)
    
    def _specializationComboChanged(self):
        specialization = str(self._specializationCombo._combo.currentText())
        attr = self._coralAttribute()
        
        if specialization != "" and specialization != "none":
            attr.setSpecializationOverride(str(specialization));
        else:
            attr.removeSpecializationOverride()
            
        attr.forceSpecializationUpdate()
    
    def _populateSpecializationCombo(self):
        coralAttribute = self._coralAttribute()
        
        coralAttribute.removeSpecializationOverride()
        coralAttribute.forceSpecializationUpdate()
        
        attrSpecialization = coralAttribute.specialization()
        
        self._specializationCombo._combo.clear()
        for spec in coralAttribute.specialization():
            self._specializationCombo._combo.addItem(spec)
        
        self._specializationCombo._combo.addItem("none")
        self._specializationCombo._combo.setCurrentIndex(len(attrSpecialization))
    
    def build(self):
        coralAttribute = self._coralAttribute()
        parentNode = coralAttribute.parent()

        if coralAttribute in parentNode.dynamicAttributes():
            self._nameField = fields.NameField(coralAttribute, self)
            self._specializationCombo = SpecializationCombo(self)
            self.layout().addWidget(self._specializationCombo)
            self.layout().addWidget(self._nameField)
            
            self._specializationCombo._combo.setShowPopupCallback(self._populateSpecializationCombo)
            self._specializationCombo._combo.setCurrentItemChangedCallback(self._specializationComboChanged)
            
            spec = coralAttribute.specialization()
            if len(spec) == 1:
                self._specializationCombo._combo.addItem(spec[0])
                self._specializationCombo._combo.setCurrentIndex(0)
            else:
                self._specializationCombo._combo.addItem("none")
                self._specializationCombo._combo.setCurrentIndex(0)
        else:
            label = QtGui.QLabel("name: " + coralAttribute.name(), self)
            self.layout().addWidget(label)
    
class AttributeInspectorWidget(QtGui.QWidget):
    def __init__(self, coralAttribute, parent):
        QtGui.QWidget.__init__(self, parent)
        
        self._mainLayout = QtGui.QVBoxLayout(self)
        self._coralAttribute = weakref.ref(coralAttribute)
        
        self.setLayout(self._mainLayout)
        self._mainLayout.setContentsMargins(0, 0, 0, 0)
        self._mainLayout.setSpacing(0)
    
    def coralAttribute(self):
        attr = None
        if self._coralAttribute:
            attr = self._coralAttribute()
        
        return attr
    
    def build(self):
        pass

class NodeInspectorHeader(QtGui.QWidget):
    def __init__(self, parent):
        QtGui.QWidget.__init__(self, parent)
        
        self._mainLayout = QtGui.QHBoxLayout(self)
        self._lockButton = QtGui.QToolButton(self)
        self._classNameLabel = QtGui.QLabel(self)
        
        self.setLayout(self._mainLayout)
        self._mainLayout.setContentsMargins(0, 0, 0, 0)
        self._mainLayout.setAlignment(QtCore.Qt.AlignRight)
        self._lockButton.setText("lock")
        self._lockButton.setCheckable(True)
        self._mainLayout.addWidget(self._classNameLabel)
        self._mainLayout.addWidget(self._lockButton)
    
    def lockButton(self):
        return self._lockButton

class NodeInspector(QtGui.QWidget):
    _inspectorWidgetClasses = {}
    
    @staticmethod
    def registerInspectorWidget(nestedObjectClassName, inspectorWidgetClass): 
        NodeInspector._inspectorWidgetClasses[nestedObjectClassName] = inspectorWidgetClass
    
    def __init__(self, parent):
        QtGui.QWidget.__init__(self, parent)
        
        self._mainLayout = QtGui.QVBoxLayout(self)
        self._header = NodeInspectorHeader(self)
        self._contentWidget = QtGui.QWidget(self)
        self._contentLayout = QtGui.QVBoxLayout(self._contentWidget)
        self._isLocked = False
        self._selectedNodesChangedObserver = Observer()
        self._nodeConnectionChangedObserver = Observer()
        self._inspectorWidget = None
        self._node = None
        self._attribute = None
        
        self.setLayout(self._mainLayout)
        self.setWindowTitle("node inspector")
        self._mainLayout.setContentsMargins(5, 5, 5, 5)
        self._mainLayout.setAlignment(QtCore.Qt.AlignTop)
        
        self._contentWidget.setLayout(self._contentLayout)
        self._contentLayout.setContentsMargins(0, 0, 0, 0)
        
        self._mainLayout.addWidget(self._header)
        self._mainLayout.addWidget(self._contentWidget)
        
        self.connect(self._header.lockButton(), QtCore.SIGNAL("toggled(bool)"), self.lock)
        
        nodeEditor.NodeEditor.addSelectedNodesChangedObserver(self._selectedNodesChangedObserver, self._selectionChanged)
        
        self._selectionChanged()
    
    def refresh(self):
        self.clear()
        
        node = None
        attribute = None
        if self._node:
            node = self._node()
        elif self._attribute:
            attribute = self._attribute()
        
        self._rebuild(node, attribute)
    
    def clear(self):
        self._header._classNameLabel.setText("")
        if self._inspectorWidget:
            self._inspectorWidget.setParent(None)
            self._inspectorWidget = None
    
    def _rebuild(self, node = None, attribute = None):
        if node:
            inspectorWidgetClass = NodeInspectorWidget
            
            classNames = node.classNames()
            classNames.reverse()

            for className in classNames:
                if NodeInspector._inspectorWidgetClasses.has_key(className):
                    inspectorWidgetClass = NodeInspector._inspectorWidgetClasses[className]
                    break
        
            self._inspectorWidget = inspectorWidgetClass(node, self)
            self._inspectorWidget.setNameEditable(True)
            self._inspectorWidget.build()
            self._contentLayout.addWidget(self._inspectorWidget)
            self._header._classNameLabel.setText(node.className())
        
            coralApp.addNodeConnectionChangedObserver(self._nodeConnectionChangedObserver, node, self.refresh)
        
        elif attribute:
            self._inspectorWidget = ProxyAttributeInspectorWidget(attribute, self)
            self._inspectorWidget.build()
            self._contentLayout.addWidget(self._inspectorWidget)
            self._header._classNameLabel.setText(attribute.className())
            
            coralApp.addNodeConnectionChangedObserver(self._nodeConnectionChangedObserver, attribute.parent(), self.refresh)
                
    def _selectionChanged(self):
        if self._isLocked:
            return
        
        self.clear()
        self._node = None
        self._attribute = None
        self._nodeConnectionChangedObserver = Observer()
        
        nodes = nodeEditor.NodeEditor.selectedNodes()

        if nodes:
            node = nodes[0]
            self._node = weakref.ref(node)
            self._rebuild(node)
        else:
            attributes = nodeEditor.NodeEditor.selectedAttributes()
            
            if attributes:
                attr = attributes[0]
                self._attribute = weakref.ref(attr)
                self._rebuild(attribute = attr)
        
    def lock(self, value):
        self._isLocked = value
        
        if not self._isLocked:
            self._selectionChanged()
    
    def sizeHint(self):
        return QtCore.QSize(200, 500)
