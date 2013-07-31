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


import sys
import os
import traceback

from PyQt4 import QtGui, QtCore

from .. import coralApp
from .. import utils
from mainWindow import MainWindow


class DropData(object):
    def __init__(self, data = None, type = ""):
        self._data = data
        self._type = type
    
    def data(self):
        MainWindow.globalInstance().emit(QtCore.SIGNAL("coralDataDropped"))
        return self._data
    
    def type(self):
        return self._type

class CoralUiData:
    app = None
    lastNetworkFileDir = ""
    configModule = None
    dropData = DropData()
    clipboardData = {}

def init(configModule = None):
    from nodeEditor.nodeEditor import NodeEditor
    
    coralApp.init()
    
    CoralUiData.app = QtCore.QCoreApplication.instance()
    if CoralUiData.app is None:
        CoralUiData.app = QtGui.QApplication(sys.argv)
    else:
        coralApp.logInfo("using existing QApplication")
    
    MainWindow._init()
    NodeEditor._init()
    
    import builtinUis
    loadPluginUiModule(builtinUis)
    
    import builtinDrawNodes
    coralApp.loadPluginModule(builtinDrawNodes)
    
    if configModule:
        configModule.apply()
    
    if os.environ.has_key("CORAL_STARTUP_SCRIPT"):
        startupScriptFile = os.environ["CORAL_STARTUP_SCRIPT"]
        if startupScriptFile:
            utils.runtimeImport(startupScriptFile)

def scanPathForPlugins(path):
    verbLev = coralApp.verboseLevel()
    coralApp.setVerboseLevel(0)

    entries = os.listdir(path)
    for entry in entries:
        if entry.split(".")[-1] == "py":
            filename = os.path.join(path, entry)
            loadPluginUi(filename)
    
    coralApp.setVerboseLevel(verbLev)

def scanAutoLoadPaths():
    for path in coralApp.CoralAppData.autoLoadPaths:
        scanPathForPlugins(path)

def application():
    return CoralUiData.app

def startApp():
    return CoralUiData.app.exec_()

def mainWindow():
    return MainWindow.globalInstance()

# Sets arbitrary data that can be retrieved later using dropData().
# This global data is usefull for simplified drag & drop data retrieving.
def setDropData(data, type):
    CoralUiData.dropData = DropData(data, type)

# Returns the data set with setDropData(),
# once the data is returned, the global data stored is erased.
def dropData():
    data = CoralUiData.dropData
    CoralUiData.dropData = DropData()
    
    return data.data()

def dropDataType():
    return CoralUiData.dropData.type()

def loadPluginUiModule(pluginUiModule):
    from nodeEditor.nodeEditor import NodeEditor
    from nodeInspector.nodeInspector import NodeInspector
    
    pluginUi = pluginUiModule.loadPluginUi()
    for attributeClassName in pluginUi._registeredAttributeUis.keys():
        NodeEditor.registerAttributeUiClass(attributeClassName, pluginUi._registeredAttributeUis[attributeClassName])
        
    for nodeClassName in pluginUi._registeredNodeUis.keys():
        NodeEditor.registerNodeUiClass(nodeClassName, pluginUi._registeredNodeUis[nodeClassName])

    for nestedObjectClassName in pluginUi._registeredInspectorWidgets.keys():
        NodeInspector.registerInspectorWidget(nestedObjectClassName, pluginUi._registeredInspectorWidgets[nestedObjectClassName])

def loadPluginUi(filename):
    searchPaths = ["", os.getcwd()]
    found = False
    for searchPath in searchPaths:
        path = os.path.join(searchPath, filename)
        if os.path.isfile(path):
            found = True

            module = None
            try:
                module = utils.runtimeImport(filename)
            except:
                coralApp.logError("skipping pluginUi " + str(filename))
                
                exc_type, exc_value, exc_traceback = sys.exc_info()
                traceback.print_exception(exc_type, exc_value, exc_traceback, limit = 2, file = sys.stdout)

            if module:
                if hasattr(module, "loadPluginUi"):
                    found = True
                    loadPluginUiModule(module)
                else:
                    coralApp.logError("no loadPluginUi function found in file " + str(filename))
                    
            break
    
    if not found:
        coralApp.logError("could not find file " + str(filename))

def copyToClipboard(data):
    CoralUiData.clipboardData = data

def clipboardData():
    return CoralUiData.clipboardData
