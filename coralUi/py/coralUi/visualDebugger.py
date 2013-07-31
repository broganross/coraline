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

from nodeEditor import nodeEditor
from ..observer import Observer
from .. import coralApp
from .. import _coral

class NodeDebugInfo(QtGui.QWidget):
    def __init__(self, parent):
        QtGui.QWidget.__init__(self)
        
        self.setLayout(QtGui.QVBoxLayout(self))
        self._textEdit = QtGui.QPlainTextEdit(self)
        self._selectedNodesChangedObserver = Observer()
        self._selectedNode = None
        
        self.layout().addWidget(self._textEdit)
        self.layout().setContentsMargins(5, 5, 5, 5)
        self.layout().setSpacing(5)
        self._textEdit.setLineWrapMode(QtGui.QPlainTextEdit.NoWrap)
        self._textEdit.setReadOnly(True)
        
        nodeEditor.NodeEditor.addSelectedNodesChangedObserver(self._selectedNodesChangedObserver, self._selectionChanged)
        
        self._selectionChanged()
        
    def _selectionChanged(self):
        self._selectedNode = None
        
        nodes = nodeEditor.NodeEditor.selectedNodes()
        if nodes:
            self._selectedNode = weakref.ref(nodes[0])
        
        self.update()
    
    def update(self):
        text = "# node debug info\n\n"
        
        if self._selectedNode:
            selectedNode = self._selectedNode()
            if selectedNode:
                text += selectedNode.debugInfo()
        
        self._textEdit.setPlainText(text)

class ProfiledAttribute(QtGui.QWidget):
    def __init__(self, parent):
        QtGui.QWidget.__init__(self)
        
        self._attributeLabel = QtGui.QLabel("attribute:", self)
        self._lineEdit = QtGui.QLineEdit(self)
        
        self.setLayout(QtGui.QHBoxLayout(self))
        self.layout().setContentsMargins(5, 5, 5, 5)
        self.layout().setSpacing(5)
        self.layout().addWidget(self._attributeLabel)
        self.layout().addWidget(self._lineEdit)

class Profiler(QtGui.QWidget):
    def __init__(self, parent):
        QtGui.QWidget.__init__(self)
        
        self._profiledAttribute = ProfiledAttribute(self)
        self._table = QtGui.QTableWidget(self)
        self._timer = QtCore.QTimer(self)
        
        self.setLayout(QtGui.QVBoxLayout(self))
        self.layout().setContentsMargins(5, 5, 5, 5)
        self.layout().setSpacing(5)
        self._table.setColumnCount(2)
        self._table.setHorizontalHeaderLabels(["Node", "Time"])
        self._table.horizontalHeader().setResizeMode(QtGui.QHeaderView.Stretch)
        self.layout().addWidget(self._profiledAttribute)
        self.layout().addWidget(self._table)
        
        self.connect(self._profiledAttribute._lineEdit, QtCore.SIGNAL("editingFinished()"), self.update)
        
        self.connect(self._timer, QtCore.SIGNAL("timeout()"), self._time)
        self._timer.setInterval(1000)
        self._timer.setSingleShot(False)
        self._timer.start()
        
    def _time(self):
        self.update()
        
    def _collectNodes(self, attribute, nodes):
        if attribute.isOutput():
            parentNode = attribute.parent()
            if parentNode and parentNode not in nodes:
                nodes.append(parentNode)
        
        inputAttr = attribute.input()
        if inputAttr:
            self._collectNodes(inputAttr, nodes)
        
        for attr in attribute.affectedBy():
             self._collectNodes(attr, nodes)
    
    def update(self):
        profiledAttr = str(self._profiledAttribute._lineEdit.text())
        attr = None
        if profiledAttr:
            attr = coralApp.findAttribute(profiledAttr)
            if attr:
                nodes = []
                self._collectNodes(attr, nodes)
                
                profiledNodes = {}
                for node in nodes:
                    computeTicks = node.computeTimeTicks()
                    if computeTicks not in profiledNodes.keys():
                        profiledNodes[computeTicks] = []
                        
                    profiledNodes[computeTicks].append(node)
                
                ticks = profiledNodes.keys()
                self._table.setRowCount(len(ticks))
                ticks.sort()
                ticks.reverse()
                
                row = 0
                for tick in ticks:
                    nodes = profiledNodes[tick]
                    for node in nodes:
                        cell1 = QtGui.QTableWidgetItem(node.fullName())
        
                        computeTime = "secs:" + str(node.computeTimeSeconds()) + " millisecs:" + str(node.computeTimeMilliseconds())
                        cell2 = QtGui.QTableWidgetItem(computeTime)
        
                        self._table.setItem(row, 0, cell1)
                        self._table.setItem(row, 1, cell2)
        
                        row += 1
        
        if attr is None:
            self._table.setRowCount(1)
            self._table.setItem(0, 0, QtGui.QTableWidgetItem("no attribute to profile"))
            self._table.setItem(0, 1, QtGui.QTableWidgetItem("-- : --"))

class VisualDebugger(QtGui.QWidget):
    def __init__(self, parent):
        QtGui.QWidget.__init__(self)
        
        self.setLayout(QtGui.QVBoxLayout(self))
        self._tabWidget = QtGui.QTabWidget(self)
        self.layout().addWidget(self._tabWidget)
        
        self._nodeDebugInfo = NodeDebugInfo(self)
        self._profiler = Profiler(self)
        
        self._tabWidget.addTab(self._nodeDebugInfo, "Node Debug Info")
        self._tabWidget.addTab(self._profiler, "Profiler")
        
    def sizeHint(self):
        return QtCore.QSize(250, 300)
        
    def closeEvent(self, event):
        self._profiler._timer.stop()
        self._nodeDebugInfo._selectedNodesChangedObserver = Observer()
        
