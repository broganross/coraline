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


import os

from coral import coralApp
from coral.coralUi import coralUi

from maya import cmds, OpenMaya

class FiberMayaAppData:
    _initialized = False
    _associateCoralNetworkNode = ""
    _coralDrawNode = ""

def _saveFiberNetwork(coralNetworkNode):
    coralNodeId = cmds.getAttr(coralNetworkNode + ".coralNodeId")
    coralNode = coralApp.findObjectById(coralNodeId) # should return a MayaContextNode
    
    saveScript = coralApp._generateNetworkScript(coralNode, saveTopNode = True)
    cmds.setAttr(coralNetworkNode + ".saveData", saveScript, type = "string")

def _mayaSavingScene(arg):
    mayaNodes = cmds.ls(type = "coralNetworkNode")
    for mayaNode in mayaNodes:
        _saveFiberNetwork(mayaNode)

def _loadFiberNetwork(coralNetworkNode):
    saveScript = cmds.getAttr(coralNetworkNode + ".saveData")
    
    FiberMayaAppData._associateCoralNetworkNode = coralNetworkNode # the MayaContextNode created by the current saveScript wil catch this global variable and use it
    
    coralApp._loadNetworkScript(saveScript, topNode = "root")
    
    FiberMayaAppData._associateCoralNetworkNode = ""

def _mayaOpenedScene(arg):
    mayaNodes = cmds.ls(type = "coralNetworkNode")
    for mayaNode in mayaNodes:
        _loadFiberNetwork(mayaNode)

def _mayaClearScene(arg):
    coralApp.newNetwork()

def _initMayaEnvironment():
    OpenMaya.MSceneMessage.addCallback(OpenMaya.MSceneMessage.kBeforeNew, _mayaClearScene)
    OpenMaya.MSceneMessage.addCallback(OpenMaya.MSceneMessage.kBeforeSave, _mayaSavingScene)
    OpenMaya.MSceneMessage.addCallback(OpenMaya.MSceneMessage.kAfterOpen, _mayaOpenedScene)
    OpenMaya.MSceneMessage.addCallback(OpenMaya.MSceneMessage.kBeforeOpen, _mayaClearScene)
    
def start():
    if FiberMayaAppData._initialized == False:
        import coralMayaConfig
        coralUi.init(configModule = coralMayaConfig)
    
        import coralMayaPlugin
        coralApp.loadPluginModule(coralMayaPlugin)
            
        import coralMayaPluginUi
        coralUi.loadPluginUiModule(coralMayaPluginUi)

        mayaPluginPath = os.path.dirname(__file__)
        cmds.loadPlugin(os.path.join(mayaPluginPath, "coralMayaPlugin"))
    
        _initMayaEnvironment()
        
        FiberMayaAppData._coralDrawNode = cmds.createNode("coralDrawNode")
        
        coralApp.scanAutoLoadPaths()
        
        FiberMayaAppData._initialized = True
    
    coralUi.mainWindow().restoreSettings()
        
def showWindow():
    coralUi.mainWindow().show()
