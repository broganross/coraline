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
from ..._coral import Command, Node
from ...observer import Observer, ObserverCollector

from nodeUi import NodeUi, AttributeUi, Connection
from nodeView import NodeView
from rootNodeUi import RootNodeUi
from addressBar import AddressBar

def _checkClassIsNodeUi(class_):
    if NodeUi in class_.__bases__:
       return True
    else:
        for baseClass in class_.__bases__:
            val = _checkClassIsNodeUi(baseClass)
            if val:
                return val         

class NodeEditor(QtGui.QWidget):
    _rootNodeUi = None
    _nodeUis = {}
    _attributeUis = {}
    _attributeUiClasses = {}
    _nodeUiClasses = {}
    _selectedNodesId = []
    _selectedAttributesId = []
    _selectedNodesChangedObservers = ObserverCollector()
    _instances = []
    _focusedInstance = None
    _initializedNewNetworkObserver = Observer()
    _createdNodeObserver = Observer()
    _deletingNodeObserver = Observer()
    _connectedAttributesObserver = Observer()
    _createdAttributeObserver = Observer()
    _deletingAttributeObserver = Observer()
    _removedNodeObserver = Observer()
    _addedNodeObserver = Observer()
    _addedAttributeObserver = Observer()
    _collapsedNodeObserver = Observer()
    _generatingSaveScriptObserver = Observer()
    _networkLoadedObserver = Observer()
    _removedAttributeObserver = Observer()
    
    @staticmethod
    def instances():
        return NodeEditor._instances

    @staticmethod
    def _addInstance(nodeEditor):
        NodeEditor._instances.append(nodeEditor)
    
    @staticmethod
    def _removeInstance(nodeEditor):
        if nodeEditor in NodeEditor._instances:
            del NodeEditor._instances[NodeEditor._instances.index(nodeEditor)]
    
    @staticmethod
    def addSelectedNodesChangedObserver(observer, callback):
        NodeEditor._selectedNodesChangedObservers.add(observer)
        observer.setNotificationCallback(callback)
    
    @staticmethod
    def _notifySelectedNodesChangedObservers():
        for observer in NodeEditor._selectedNodesChangedObservers.observers():
            observer.notify()
    
    @staticmethod
    def _setSelectedNodes(nodes):
        oldSel = NodeEditor._selectedNodesId
        NodeEditor._selectedNodesId = []
        
        for node in nodes:
            NodeEditor._selectedNodesId.append(node.id())
        
        selectionUpdated = False
        if oldSel != NodeEditor._selectedNodesId:
            selectionUpdated = True
        
        return selectionUpdated
    
    @staticmethod
    def _setSelectedAttributes(attributes):
        oldSel = NodeEditor._selectedAttributesId
        NodeEditor._selectedAttributesId = []
        
        for attr in attributes:
            NodeEditor._selectedAttributesId.append(attr.id())
        
        selectionUpdated = False
        if oldSel != NodeEditor._selectedAttributesId:
            selectionUpdated = True
        
        return selectionUpdated
    
    @staticmethod
    def _setSelection(nodes = [], attributes = [], updateSelected = True):
        if updateSelected:
            for nodeId in NodeEditor._selectedNodesId:
                nodeUi = NodeEditor.findNodeUi(nodeId)
                if nodeUi:
                    nodeUi.setSelected(False)
            
            for attrId in NodeEditor._selectedAttributesId:
                attrUi = NodeEditor.findAttributeUi(attrId)
                if attrUi:
                    attrUi.proxy().setSelected(False)
        
        nodeSelUpdated = NodeEditor._setSelectedNodes(nodes)
        attrSelUpdated = NodeEditor._setSelectedAttributes(attributes)
        
        if nodeSelUpdated or attrSelUpdated:
            NodeEditor._notifySelectedNodesChangedObservers()

            if updateSelected:
                for node in nodes:
                    nodeUi = NodeEditor.findNodeUi(node.id())
                    if nodeUi:
                        nodeUi.setSelected(True)
                
                for attr in attributes:
                    attrUi = NodeEditor.findAttributeUi(attr.id())
                    if attrUi:
                        attrUi.proxy().setSelected(True)
        
    @staticmethod
    def selectedNodes():
        nodes = []
        for nodeId in NodeEditor._selectedNodesId:
            node = coralApp.findObjectById(nodeId)
            if node:
                nodes.append(node)
        
        return nodes
    
    @staticmethod
    def selectedAttributes():
        attrs = []
        for attrId in NodeEditor._selectedAttributesId:
            attr = coralApp.findObjectById(attrId)
            if attr:
                attrs.append(attr)
        
        return attrs
    
    @staticmethod
    def _setFocusedInstance(nodeEditor):
        NodeEditor._focusedInstance = weakref.ref(nodeEditor)
    
    @staticmethod
    def focusedInstance():
        nodeEditor = None
        if NodeEditor._focusedInstance:
            nodeEditor = NodeEditor._focusedInstance()
        
        return nodeEditor
    
    @staticmethod
    def currentNodeUi():
        currentNodeUi = None
        nodeEditor = NodeEditor.focusedInstance()
        if nodeEditor:
            currentNodeUi = nodeEditor.nodeView().currentNodeUi()
            
        return currentNodeUi
    
    @staticmethod
    def currentNode():
        currentNode = None
        currentNodeUi = NodeEditor.currentNodeUi()
        if currentNodeUi:
            currentNode = currentNodeUi.coralNode()
        
        return currentNode
        
    @staticmethod
    def registerNodeUiClass(coralNodeClassName, nodeUiClass):
        if _checkClassIsNodeUi(nodeUiClass):
            NodeEditor._nodeUiClasses[coralNodeClassName] = nodeUiClass
        else:
            coralApp.logError("could not register nodeUi, expected NodeUi subclass: " + str(nodeUiClass))
    
    @staticmethod
    def registerAttributeUiClass(coralAttributeClassName, attributeUiClass):
        if AttributeUi in attributeUiClass.__bases__:
            NodeEditor._attributeUiClasses[coralAttributeClassName] = attributeUiClass
        else:
            coralApp.logError("could not register attributeUi, expected AttributeUi subclass: " + str(attributeUiClass))
    
    @staticmethod
    def _init():
        NodeEditor._coralInitializedNewNetwork()
        
        coralApp.addInitializedNewNetworkObserver(NodeEditor._initializedNewNetworkObserver, NodeEditor._coralInitializedNewNetwork)
        coralApp.addCreatedNodeObserver(NodeEditor._createdNodeObserver, NodeEditor._coralCreatedNodeCallback)
        coralApp.addDeletingNodeObserver(NodeEditor._deletingNodeObserver, NodeEditor._coralDeletingNodeCallback)
        coralApp.addConnectedAttributesObserver(NodeEditor._connectedAttributesObserver, NodeEditor._coralConnectedAttributesCallback)
        coralApp.addCreatedAttributeObserver(NodeEditor._createdAttributeObserver, NodeEditor._coralCreatedAttributeCallback)
        coralApp.addDeletingAttributeObserver(NodeEditor._deletingAttributeObserver, NodeEditor._coralDeletingAttributeCallback)
        coralApp.addRemovedNodeObserver(NodeEditor._removedNodeObserver, NodeEditor._coralRemovedNodeCallback)
        coralApp.addAddedNodeObserver(NodeEditor._addedNodeObserver, NodeEditor._coralAddedNodeCallback)
        coralApp.addAddedAttributeObserver(NodeEditor._addedAttributeObserver, NodeEditor._coralAddedAttributeCallback)
        coralApp.addCollapsedNodeObserver(NodeEditor._collapsedNodeObserver, NodeEditor._coralCollapsedNodeCallback)
        coralApp.addGeneratingSaveScriptObserver(NodeEditor._generatingSaveScriptObserver, NodeEditor._coralGeneratingSaveScriptCallback)
        coralApp.addNetworkLoadedObserver(NodeEditor._networkLoadedObserver, NodeEditor._networkLoadedCallback)
        coralApp.addRemovedAttributeObserver(NodeEditor._removedAttributeObserver, NodeEditor._removedAttributeCallback)
        
        import nodeEditorCommands
        coralApp.loadPluginModule(nodeEditorCommands)
    
    @staticmethod
    def _saveNodeUiData(nodeUi, rootNode = False):
        saveScript = ""
        
        if nodeUi:
            nodeUiData = nodeUi.data()
            if rootNode:
                if nodeUiData.has_key("pos"):
                    del nodeUiData["pos"]
            
            setNodeUiDataCmd = Command()
            setNodeUiDataCmd.setName("SetNodeUiData")
            setNodeUiDataCmd.setArgString("node", nodeUi.coralNode().fullName())
            setNodeUiDataCmd.setArgUndefined("data", str(nodeUiData))
    
            saveScript += setNodeUiDataCmd.asScript() + "\n"
            
            for coralNode in nodeUi.coralNode().nodes():
                nodeUi = NodeEditor.findNodeUi(coralNode.id())
                saveScript += NodeEditor._saveNodeUiData(nodeUi)
    
        return saveScript
    
    @staticmethod
    def _networkLoadedCallback():
        NodeEditor.focusedInstance().nodeView().frameSceneContent()
    
    @staticmethod
    def _coralGeneratingSaveScriptCallback():
        nodeId = NodeEditor._generatingSaveScriptObserver.data("nodeId")
        nodeUi = NodeEditor.findNodeUi(nodeId)
        
        saveScript = NodeEditor._generatingSaveScriptObserver.data("saveScript")
        saveScript[0] += "# nodeEditor save data\n"
        saveScript[0] += NodeEditor._saveNodeUiData(nodeUi, rootNode = True)
        
    @staticmethod
    def _coralInitializedNewNetwork():
        coralRootNode = coralApp.rootNode()
        NodeEditor._rootNodeUi = RootNodeUi(coralRootNode)
        NodeEditor._nodeUis[coralRootNode.id()] = NodeEditor._rootNodeUi
        
        for instance in NodeEditor._instances:
            instance.nodeView().setCurrentNodeUi(NodeEditor._rootNodeUi)
        
    @staticmethod
    def _coralCollapsedNodeCallback():
        collapsedNodeId = NodeEditor._collapsedNodeObserver.data("collapsedNodeId")
        collapsedNodeUi = NodeEditor.findNodeUi(collapsedNodeId)
        
        collapsedNodeUi.repositionAmongConnectedNodes()
        collapsedNodeUi.repositionContainedProxys()
                    
    @staticmethod
    def _coralAddedAttributeCallback():
        parentNodeId = NodeEditor._addedAttributeObserver.data("parentNodeId")
        attributeAddedId = NodeEditor._addedAttributeObserver.data("attributeAddedId")
        
        parentNodeUi = NodeEditor.findNodeUi(parentNodeId)
        
        if parentNodeUi:
            attributeAddedUi = NodeEditor.findAttributeUi(attributeAddedId)
            if attributeAddedUi is None:
                attr = coralApp.findObjectById(attributeAddedId)
                attributeAddedUi = NodeEditor._createAttributeUi(attr, parentNodeUi)
                parentNodeUi.addInputAttributeUi(attributeAddedUi)
                parentNodeUi.updateLayout()
                
            if NodeEditor._addedAttributeObserver.data("input"):
                parentNodeUi.addInputAttributeUi(attributeAddedUi)
            else:
                parentNodeUi.addOutputAttributeUi(attributeAddedUi)
    
    @staticmethod
    def _removedAttributeCallback():
        parentNodeId = NodeEditor._removedAttributeObserver.data("parentNodeId")
        parentNodeUi = NodeEditor.findNodeUi(parentNodeId)
        
        if parentNodeUi:
            attributeAddedId = NodeEditor._removedAttributeObserver.data("attributeRemovedId")
            attributeAddedUi = NodeEditor.findAttributeUi(attributeAddedId)

            if attributeAddedUi:
                parentNodeUi.removeAttributeUi(attributeAddedUi)
                attributeAddedUi.setParentNodeUi(None)
            
                parentNodeUi.updateLayout()

    @staticmethod
    def _coralAddedNodeCallback():
        parentNodeId = NodeEditor._addedNodeObserver.data("parentNodeId")
        nodeAddedId = NodeEditor._addedNodeObserver.data("nodeAddedId")
        
        parentNodeUi = NodeEditor.findNodeUi(parentNodeId)
        nodeAddedUi = NodeEditor.findNodeUi(nodeAddedId)

        parentNodeUi.addNodeUi(nodeAddedUi)
    
    @staticmethod
    def _coralRemovedNodeCallback():
        parentNodeId = NodeEditor._removedNodeObserver.data("parentNodeId")
        nodeRemovedId = NodeEditor._removedNodeObserver.data("nodeRemovedId")
        
        parentNodeUi = NodeEditor.findNodeUi(parentNodeId)
        if parentNodeUi:
            nodeRemovedUi = NodeEditor.findNodeUi(nodeRemovedId)
            
            if nodeRemovedUi:
                parentNodeUi.removeNodeUi(nodeRemovedUi)
        
    @staticmethod
    def _coralDeletingAttributeCallback():
        attributeId = NodeEditor._deletingAttributeObserver.data("attributeId")
        attributeUi = NodeEditor.findAttributeUi(attributeId)
        if attributeUi:
            parentNodeUi = attributeUi.parentNodeUi()
            attributeUi.deleteIt()
            parentNodeUi.updateLayout()
        
    @staticmethod
    def _coralCreatedAttributeCallback():
        attributeId = NodeEditor._createdAttributeObserver.data("attributeId")
        coralAttribute = coralApp.findObjectById(attributeId)
        parentNodeUi = NodeEditor.findNodeUi(coralAttribute.parent().id())
        
        attributeUi = NodeEditor._createAttributeUi(coralAttribute, parentNodeUi)
        
        if coralAttribute.isInput():
            parentNodeUi.addInputAttributeUi(attributeUi)
        else:
            parentNodeUi.addOutputAttributeUi(attributeUi)
            
        parentNodeUi.updateLayout()
        
    @staticmethod
    def _coralConnectedAttributesCallback():
        sourceAttributeId = NodeEditor._connectedAttributesObserver.data("sourceAttributeId")
        destinationAttributeId = NodeEditor._connectedAttributesObserver.data("destinationAttributeId")
        
        sourceAttributeUi = NodeEditor.findAttributeUi(sourceAttributeId)
        destinationAttributeUi = NodeEditor.findAttributeUi(destinationAttributeId)
        
        sourceAttributeUi.connectTo(destinationAttributeUi)
        
    @staticmethod
    def _coralDeletingNodeCallback():
        nodeId = NodeEditor._deletingNodeObserver.data("nodeId")
        nodeUi = NodeEditor.findNodeUi(nodeId)
        if nodeUi:
            nodeUi.deleteIt()
        
    @staticmethod
    def _coralCreatedNodeCallback():
        nodeId = NodeEditor._createdNodeObserver.data("nodeId")
        coralNode = coralApp.findObjectById(nodeId)
        
        NodeEditor._createNodeUi(coralNode)
        NodeEditor._setSelection(nodes = [coralNode])
    
    @staticmethod
    def _createNodeUi(coralNode):
        nodeUiClass = NodeUi
        classNames = coralNode.classNames()
        classNames.reverse()
        
        for className in classNames:
            if NodeEditor._nodeUiClasses.has_key(className):
                nodeUiClass = NodeEditor._nodeUiClasses[className]
                break
        
        nodeUi = nodeUiClass(coralNode)
        nodeUi.buildFromCoralNode()
        
        # this container is meant to keep nodeUi alive when Qt releases ownership
        NodeEditor._nodeUis[coralNode.id()] = nodeUi
        
        return nodeUi
    
    @staticmethod
    def _createAttributeUi(coralAttribute, parentNodeUi):
        attributeUi = None
        attributeUiClass = AttributeUi
        className = coralAttribute.className()

        if NodeEditor._attributeUiClasses.has_key(className):
            attributeUiClass = NodeEditor._attributeUiClasses[className]

        attributeUi = attributeUiClass(coralAttribute, parentNodeUi)
        
        # keeping this instance alive when ownership is released by Qt
        NodeEditor._attributeUis[coralAttribute.id()] = attributeUi
        
        return attributeUi
    
    @staticmethod
    def findNodeUi(coralNodeId):
        nodeUi = None
        if NodeEditor._nodeUis.has_key(coralNodeId):
            nodeUi = NodeEditor._nodeUis[coralNodeId]
        
        return nodeUi
    
    @staticmethod
    def findAttributeUi(coralAttributeId):
        attributeUi = None
        if NodeEditor._attributeUis.has_key(coralAttributeId):
            attributeUi = NodeEditor._attributeUis[coralAttributeId]
        
        return attributeUi
        
    def __init__(self, parent):
        QtGui.QWidget.__init__(self, parent)
        
        self._mainLayout = QtGui.QVBoxLayout(self)
        self._addressBar = AddressBar(self)
        self._nodeView = NodeView(self)
        
        self.setWindowTitle("node editor")
        self.setLayout(self._mainLayout)
        self.setContentsMargins(5, 5, 5, 5)
        
        self._mainLayout.setContentsMargins(0, 0, 0, 0)
        self._mainLayout.setSpacing(5)
        self._mainLayout.addWidget(self._addressBar)
        self._mainLayout.addWidget(self._nodeView)
        
        self.connect(self._nodeView, QtCore.SIGNAL("currentNodeUiChanged"), self._currentNodeUiChanged)
        self.connect(self._addressBar.upButton(), QtCore.SIGNAL("clicked()"), self._upButtonClicked)
        self.connect(self._addressBar, QtCore.SIGNAL("addressBarChanged"), self._addressBarChanged)
        
        self._nodeView.setCurrentNodeUi(NodeEditor._rootNodeUi)
        
        NodeEditor._addInstance(self)
        
        NodeEditor._setFocusedInstance(self)
        
    def closeEvent(self, event):
        NodeEditor._removeInstance(self)
        
    def sizeHint(self):
        return QtCore.QSize(500, 500)
    
    def _addressBarChanged(self):
        address = self._addressBar.address()
        node = coralApp.findNode(address)
        
        opened = False
        if node:
            nodeUi = NodeEditor.findNodeUi(node.id())
            if nodeUi.canOpenThis():
                self._nodeView.setCurrentNodeUi(nodeUi)
                opened = True
        
        if not opened:
            self._addressBar.setAddress(self._nodeView.currentNodeUi().coralNode().fullName())
    
    def _currentNodeUiChanged(self):
        coralNode = self._nodeView.currentNodeUi().coralNode()
        self._addressBar.setAddress(coralNode.fullName())
        
        self.setWindowTitle(coralNode.name())
        if type(self.parentWidget()) is QtGui.QDockWidget:
            self.parentWidget().setWindowTitle(coralNode.name())
    
    def _upButtonClicked(self):
        parentNodeUi = self._nodeView.currentNodeUi().parentNodeUi()
        
        if parentNodeUi:
            self._nodeView.setCurrentNodeUi(parentNodeUi)
        
    def nodeView(self):
        return self._nodeView

