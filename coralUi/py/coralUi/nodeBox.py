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
import copy
import fnmatch
from PyQt4 import QtGui, QtCore

from ..observer import Observer
from .. import coralApp
import coralUi
import mainWindow

class NodeSearchField(QtGui.QLineEdit):
    def __init__(self, parent):
        QtGui.QLineEdit.__init__(self, parent)
        
        self._originalPalette = self.palette()
        
    def focusInEvent(self, event):
        QtGui.QLineEdit.focusInEvent(self, event)
        
        palette = self.palette()
        palette.setColor(QtGui.QPalette.Normal, QtGui.QPalette.Base, QtGui.QColor(255, 255, 200))
        self.setPalette(palette);
    
    def focusOutEvent(self, event):
        QtGui.QLineEdit.focusOutEvent(self, event)
        
        self.setPalette(self._originalPalette)
        
    def keyPressEvent(self, qKeyEvent):
        if qKeyEvent.key() == QtCore.Qt.Key_Up:
            self.emit(QtCore.SIGNAL("nodeSearchFieldMovedEntry(QString)"), "up")
        elif qKeyEvent.key() == QtCore.Qt.Key_Down:
            self.emit(QtCore.SIGNAL("nodeSearchFieldMovedEntry(QString)"), "down")
        else:
            QtGui.QLineEdit.keyPressEvent(self, qKeyEvent)

class NodeShelf(QtGui.QListWidget):
    def __init__(self, parent):
        QtGui.QListWidget.__init__(self, parent)
        
        self.setSelectionMode(QtGui.QAbstractItemView.SingleSelection)
        self.setDragEnabled(True)

class NodeBoxMenu(QtGui.QMenu):
    def __init__(self, parent):
        QtGui.QMenu.__init__(self, parent)
        
        self.setLayout(QtGui.QVBoxLayout(self))
        self._nodeBox = NodeBox(parent)
        self.layout().setContentsMargins(5, 5, 5, 5)
        self.layout().setSpacing(5)
        self.layout().addWidget(self._nodeBox)
        self.resize(self._nodeBox.sizeHint())
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose)
        
        self.connect(self._nodeBox, QtCore.SIGNAL("coralDataDropped"), self._done)
    
    def hideEvent(self, event):
        self.close()
    
    def _done(self):
        self.close()
            
class NodeBox(QtGui.QWidget):
    _globalInstance = None
    
    @staticmethod
    def globalInstance():
        inst = None
        if NodeBox._globalInstance:
            inst = NodeBox._globalInstance()
            
        return inst
    
    @staticmethod
    def enableQuickSearch():
        mainWin = mainWindow.MainWindow.globalInstance()
        
        nodeBox = None
        if NodeBox._globalInstance:
            nodeBox = NodeBox._globalInstance
        else:
            menu = NodeBoxMenu(mainWin)
            nodeBox = menu._nodeBox
            cursorPos = QtGui.QCursor.pos()
            menu.move(cursorPos.x(), cursorPos.y())
            menu.show()
        
        nodeBox = NodeBox._globalInstance()
        nodeBox._nodeSearchField.setFocus()
    
    @staticmethod
    def _createFromItemData(itemData):
        from nodeEditor import nodeEditor
        
        sceneItem = None
        className = itemData["data"]["className"]
        parentNode = nodeEditor.NodeEditor.focusedInstance().currentNode().fullName()
        
        if itemData["type"] == "attributeClassName":
            name = "input"
            if itemData["data"]["output"]:
                name = "output"
            
            newAttributeName = coralApp.executeCommand("CreateAttribute", 
                className = className, 
                name = name, 
                parentNode = parentNode, 
                input = itemData["data"]["input"], output = itemData["data"]["output"])

            if newAttributeName:
                newAttribute = coralApp.findAttribute(newAttributeName)
                attributeUi = nodeEditor.NodeEditor.findAttributeUi(newAttribute.id())
                if attributeUi.proxy():
                    sceneItem = attributeUi.proxy()
        
        elif itemData["type"] == "nodeClassName":
            newNodeName = coralApp.executeCommand("CreateNode", className = className, name = className, parentNode = parentNode)
            if newNodeName:
                newNode = coralApp.findNode(newNodeName)
                sceneItem = nodeEditor.NodeEditor.findNodeUi(newNode.id())
        
        return sceneItem
        
    def __init__(self, parent):
        QtGui.QWidget.__init__(self, parent)

        self._mainLayout = QtGui.QVBoxLayout(self)
        self._header = QtGui.QWidget(self)
        self._headerLayout = QtGui.QHBoxLayout(self._header)
        self._nodeSearchField = NodeSearchField(self._header)
        self._nodeShelf = NodeShelf(self)
        self._nodeHelp = QtGui.QTextEdit(self)
        self._registeredNodeClassesObserver = Observer()
        
        self.setWindowTitle("node box")
        self.setLayout(self._mainLayout)
        
        self._header.setLayout(self._headerLayout)
        
        self._mainLayout.setContentsMargins(5, 5, 5, 5)
        self._mainLayout.setSpacing(5)
        self._mainLayout.addWidget(self._header)
        self._mainLayout.addWidget(self._nodeShelf)
        self._mainLayout.addWidget(self._nodeHelp)
        
        self._headerLayout.addWidget(self._nodeSearchField)
        self._headerLayout.setContentsMargins(0, 0, 0, 0)
        
        self._nodeHelp.setMaximumHeight(150)
        self._nodeHelp.setReadOnly(True)
        palette = self._nodeHelp.palette()
        palette.setColor(QtGui.QPalette.Base, QtGui.QColor(97, 108, 117))
        palette.setColor(QtGui.QPalette.Text, QtGui.QColor(200, 190, 200))
        self._nodeHelp.setPalette(palette)
        
        self.connect(self._nodeSearchField, QtCore.SIGNAL("textChanged(QString)"), self._searchTextChanged)
        self.connect(self._nodeSearchField, QtCore.SIGNAL("returnPressed()"), self._shelfReturnPressed)
        self.connect(self._nodeSearchField, QtCore.SIGNAL("nodeSearchFieldMovedEntry(QString)"), self._searchFieldMovedUpDown)
        self.connect(self._nodeShelf, QtCore.SIGNAL("itemPressed (QListWidgetItem *)"), self._nodeShelfItemClicked)
        self.connect(self._nodeShelf, QtCore.SIGNAL("currentRowChanged (int)"), self._nodeShelfRowChanged)
        self.connect(mainWindow.MainWindow.globalInstance(), QtCore.SIGNAL("coralDataDropped"), self._dropEnd)
        
        coralApp.addRegisteredNodeClassesObserver(self._registeredNodeClassesObserver, self._registeredNodeClassesCallback)
        
        self._rebuildNodeShelf()
        
        NodeBox._globalInstance = weakref.ref(self)
        
        self.setAttribute(QtCore.Qt.WA_DeleteOnClose)
    
    def __del__(self):
        NodeBox._globalInstance = None
    
    def _searchFieldMovedUpDown(self, direction):
        if direction == "up":
            nextItem = self._nodeShelf.item(self._nodeShelf.currentRow() - 1)
            if nextItem:
                self._nodeShelf.setCurrentRow(self._nodeShelf.currentRow() - 1)
                
        elif direction == "down":
            nextItem = self._nodeShelf.item(self._nodeShelf.currentRow() + 1)
            if nextItem:
                self._nodeShelf.setCurrentRow(self._nodeShelf.currentRow() + 1)
        
        if str(self._nodeShelf.currentItem().text()).startswith("::") and self._nodeShelf.currentRow() > 0:
            self._searchFieldMovedUpDown(direction)
                
        self._nodeShelfItemClicked(self._nodeShelf.currentItem())
    
    def _nodeShelfRowChanged(self, row):
        item = self._nodeShelf.currentItem()
        if item:
            className = str(item.text())
            if not str(className).startswith("::"):
                self._nodeShelfItemClicked(item)
    
    def _shelfReturnPressed(self):
        from nodeEditor import nodeEditor
        
        item = self._nodeShelf.currentItem()
        if item:
            className = str(item.text())
            if not str(className).startswith("::"):
                itemData = self._itemData(item)
                sceneItem = NodeBox._createFromItemData(itemData)
                if sceneItem:
                    nodeView = nodeEditor.NodeEditor.focusedInstance().nodeView()
                    if type(self.parent()) is NodeBoxMenu:
                        pos = QtGui.QCursor.pos()
                        scenePos = nodeView.mapToScene(nodeView.mapFromGlobal(pos))
                        sceneItem.setPos(scenePos)
                    else:
                        sceneItem.setPos(nodeView.mapToScene(nodeView.rect().center()))
        
        self._nodeSearchField.setText("")
        self._rebuildNodeShelf()
        self.emit(QtCore.SIGNAL("coralDataDropped"))
    
    def _dropEnd(self):
        self._nodeSearchField.setText("")
    
    def _itemData(self, item):
        data = {}
        value = str(item.text())
        if value == "InputAttribute":
            data = {
                "type": "attributeClassName", 
                "data": {"className": "PassThroughAttribute", "input": True, "output": False}}
        elif value == "OutputAttribute":
            data = {
                "type": "attributeClassName", 
                "data": {"className": "PassThroughAttribute", "input": False, "output": True}}
        else:
            data = {
                "type": "nodeClassName", 
                "data": {"className": value}}
        
        return data
    
    def _nodeShelfItemClicked(self, item):
        data = self._itemData(item)
        coralUi.setDropData(data, "nodeBoxItemData")
        
        type = "Node"
        className = data["data"]["className"]
        if data["type"] == "attributeClassName":
            type = "Attribute"
        
        helpText = "<h4>" + className + " " + type + "</h4>\n"
        helpText += "<p>" + coralApp.registeredNodeDescription(className) + "</p>"
        
        self._nodeHelp.setText(helpText)
        
    def _registeredNodeClassesCallback(self):
        self._rebuildNodeShelf()
        
    def _rebuildNodeShelf(self, filter = ""):
        wildcard = "*" + str(filter).lower() + "*"
        
        classNameTags = copy.copy(coralApp.classNameTags())
        if "InputAttribute" not in classNameTags["encapsulation"] and "OutputAttribute" not in classNameTags["encapsulation"]:
            classNameTags["encapsulation"].extend(["InputAttribute", "OutputAttribute"])
            
        tags = classNameTags.keys()
        tags.sort()
        
        self._nodeShelf.clear()
        for classNameTag in tags:
            tagItem = QtGui.QListWidgetItem(":: " + classNameTag + " ::")
            tagItem.setFlags(QtCore.Qt.NoItemFlags)
            tagItem.setTextColor(QtGui.QColor(QtCore.Qt.darkCyan))
            
            classNameItems = []
            for className in classNameTags[classNameTag]:
                if fnmatch.fnmatch(className.lower(), wildcard):
                    classNameItem = QtGui.QListWidgetItem(className)
                    classNameItem.setFlags(QtCore.Qt.ItemIsEnabled | QtCore.Qt.ItemIsSelectable | QtCore.Qt.ItemIsDragEnabled | QtCore.Qt.ItemIsDropEnabled)
                    classNameItems.append(classNameItem)
            
            if classNameItems:
                self._nodeShelf.addItem(tagItem)
                
                for classNameItem in classNameItems:
                    self._nodeShelf.addItem(classNameItem)
        
        if filter and self._nodeShelf.count():
            self._nodeShelf.setCurrentRow(1)
            self._nodeShelfItemClicked(self._nodeShelf.currentItem())
            
    def sizeHint(self):
        return QtCore.QSize(200, 400)
    
    def _searchTextChanged(self, text):
        self._rebuildNodeShelf(text)
