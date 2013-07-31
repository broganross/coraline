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


import random
from PyQt4 import QtGui, QtCore

def _aboutClicked():
    from coral.coralUi.mainWindow import MainWindow
    
    mainWin = MainWindow.globalInstance()
    mainWin.about()
    
def _openNodeEditorClicked():
    from coral.coralUi.mainWindow import MainWindow
    
    MainWindow.openRegisteredWidget("node editor")

def _openNodeBoxClicked():
    from coral.coralUi.mainWindow import MainWindow
    from coral.coralUi.nodeBox import NodeBox
    
    if NodeBox.globalInstance() is None:
        MainWindow.openRegisteredWidget("node box")

def _openNodeInspectorClicked():
    from coral.coralUi.mainWindow import MainWindow
    
    MainWindow.openRegisteredWidget("node inspector")

def _openViewportClicked():
    from coral.coralUi.mainWindow import MainWindow
    
    MainWindow.openRegisteredWidget("viewport")

def _openVisualDebuggerClicked():
    from coral.coralUi.mainWindow import MainWindow
    
    MainWindow.openRegisteredWidget("visual debugger")
    
def _collapseClicked():
    from coral.coralUi.nodeEditor.nodeEditor import NodeEditor
    from coral import coralApp
    
    nodesName = []
    for node in NodeEditor.selectedNodes():
        nodesName.append(node.fullName())
    
    if nodesName:
        coralApp.executeCommand("CollapseNodes", nodes = nodesName)

def _explodeClicked():
    from coral.coralUi.nodeEditor.nodeEditor import NodeEditor
    from coral import coralApp
    
    sel = NodeEditor.selectedNodes()
    if sel:
        coralApp.executeCommand("ExplodeCollapsedNode", collapsedNode = sel[0].fullName())

def _clearAllClicked():
    from coral import coralApp
    from coral.coralUi.mainWindow import MainWindow
    
    if MainWindow.okCancelDialog("Clear the current network?"):
        coralApp.newNetwork()

def _saveNetworkClicked():
    from coral import coralApp
    from coral.coralUi.mainWindow import MainWindow
    
    filename = MainWindow.saveFileDialog("save network file", "Coral Network (*.crl)")
    if filename:
        coralApp.saveNetworkFile(filename)

def _openNetworkClicked():
    from coral import coralApp
    from coral.coralUi.mainWindow import MainWindow
    
    filename = MainWindow.openFileDialog("open network file", "Coral Network (*.crl)")
    if filename:
        coralApp.openNetworkFile(filename)

def _saveCollapsedNodeClicked():
    from coral import coralApp
    from coral.coralUi.mainWindow import MainWindow
    from coral.coralUi.nodeEditor.nodeEditor import NodeEditor
    
    filename = MainWindow.saveFileDialog("save collapsed node", "Coral CollapsedNode (*.py)")
    sel = NodeEditor.selectedNodes()
    if filename and sel:
        coralApp.saveCollapsedNodeFile(sel[0], filename)

def _openScriptEditorClicked():
    from coral.coralUi.mainWindow import MainWindow

    MainWindow.openRegisteredWidget("script editor")

def _nodeBoxSearch():
    from coral.coralUi.nodeBox import NodeBox 
    NodeBox.enableQuickSearch()

gridVisibilityToggle = True
def _toggleGrid():
    global gridVisibilityToggle
    gridVisibilityToggle = not gridVisibilityToggle

    from coral.coralUi import viewport
    for v in viewport.instancedViewports():
        v.setGridVisible(gridVisibilityToggle)
    
    viewport.ViewportWidget.refreshViewports()
    
def apply():
    #imports here to avoid cycles
    from coral.coralUi import coralUi
    from coral.coralUi.mainWindow import MainWindow
    from coral.coralUi.nodeEditor.nodeEditor import NodeEditor
    from coral.coralUi.nodeBox import NodeBox
    from coral.coralUi.nodeInspector.nodeInspector import NodeInspector
    from coral.coralUi.scriptEditor import ScriptEditor
    from coral.coralUi.viewport import ViewportWidget
    from coral.coralUi.visualDebugger import VisualDebugger
    
    mainWin = MainWindow.globalInstance()
    mainWin.setWindowTitle("coral standalone")
    
    settingsStored = mainWin.settings().value("settingsStored").toBool()
    if not settingsStored:
        nodeBox = NodeBox(mainWin)
        nodeBoxDock = mainWin.dockWidget(nodeBox, QtCore.Qt.LeftDockWidgetArea)
    
        nodeInspector = NodeInspector(mainWin)
        mainWin.dockWidget(nodeInspector, QtCore.Qt.RightDockWidgetArea)
    
    nodeEditor = NodeEditor(mainWin)
    mainWin.setCentralWidget(nodeEditor)
    nodeEditor.nodeView().centerOn(0.0, 0.0)

    mainWin.registerWidget("node editor", NodeEditor)
    mainWin.registerWidget("node box", NodeBox)
    mainWin.registerWidget("node inspector", NodeInspector)
    mainWin.registerWidget("viewport", ViewportWidget)
    mainWin.registerWidget("script editor", ScriptEditor)
    mainWin.registerWidget("visual debugger", VisualDebugger)
    
    # menu config
    fileMenu = mainWin.menuBar().addMenu("File")
    fileMenu.addAction("Clear All...", _clearAllClicked)
    fileMenu.addAction("Save Netwok...", _saveNetworkClicked)
    fileMenu.addAction("Open Network...", _openNetworkClicked)
    fileMenu.addSeparator()
    fileMenu.addAction("Save CollapsedNode...", _saveCollapsedNodeClicked)
    
    editMenu = mainWin.menuBar().addMenu("Edit")
    editMenu.addAction("Collpase Nodes", _collapseClicked)
    editMenu.addAction("Explode Collapsed Node", _explodeClicked)
    editMenu.addSeparator()
    
    windowMenu = mainWin.menuBar().addMenu("Window")
    windowMenu.addAction("About", _aboutClicked)
    windowMenu.addAction("Open Node Editor", _openNodeEditorClicked)
    windowMenu.addAction("Open Node Box", _openNodeBoxClicked)
    windowMenu.addAction("Open Node Inspector", _openNodeInspectorClicked)
    windowMenu.addAction("Open Script Editor", _openScriptEditorClicked)
    windowMenu.addAction("Open Viewport", _openViewportClicked)
    windowMenu.addAction("Open Visual Debugger", _openVisualDebuggerClicked)
    
    # shortcuts
    shprtcutsMap = {
        "Shift+G": _explodeClicked,
        "Ctrl+S": _saveNetworkClicked,
        "Ctrl+O": _openNetworkClicked,
        "Shift+Tab": _nodeBoxSearch,
        "H": _toggleGrid}
    
    mainWin.setShortcutsMap(shprtcutsMap)

    mainWin.restoreSettings()

    mainWin.show()

    from coral import coralApp
    coralApp.logInfo("coral v" + coralApp.version())

