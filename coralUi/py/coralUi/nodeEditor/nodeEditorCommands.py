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


from ..._coral import Command
from ... import coralApp
from ...plugin import Plugin
import nodeEditor

class SetNodeUiData(Command):
    def __init__(self):
        Command.__init__(self)
        
        self.setArgString("node", "")
        self.setArgUndefined("data", "{}")
        
    def doIt(self):
        node = coralApp.findNode(self.argAsString("node"))
        nodeUi = nodeEditor.NodeEditor.findNodeUi(node.id())
        
        data = eval(self.argAsString("data"))
        nodeUi.setData(data)

class Paste(Command):
    def __init__(self):
        Command.__init__(self)
        
        self.setArgUndefined("nodes", "[]")
        self.setArgString("parentNode", "")
    
    def doIt(self):
        nodesName = eval(self.argAsString("nodes"))
        parentNodeName = self.argAsString("parentNode")
        parentNode = coralApp.findNode(parentNodeName)
        
        if parentNode is None:
            return
        
        # create nodes save script
        script = "from coral.coralApp import *\n"
        nodes = []
        sourceParentNode = None
        oldNames = []
        for nodeName in nodesName:
            node = coralApp.findNode(nodeName)
            nodes.append(node)
            
            name = node.name()
            oldNames.append(name)
            node.setName(name + "_copy")
            
            script += node.asScript()
            script += node.contentAsScript()
            
            nodeUi = nodeEditor.NodeEditor.findNodeUi(node.id())
            script += nodeEditor.NodeEditor._saveNodeUiData(nodeUi)
            
            if sourceParentNode is None:
                sourceParentNode = node.parent()
        
        # add connections to save script
        for node in nodes:
            connectCmd = Command()
            connectCmd.setName("ConnectAttributes")
            
            for attr in node.attributes():
                input = attr.input()
                if input:
                    if input.parent().parent() is sourceParentNode:
                        connectCmd.setArgString("sourceAttribute", input.fullName())
                        connectCmd.setArgString("destinationAttribute", attr.fullName())
                        script += connectCmd.asScript() + "\n"
                
                for out in attr.outputs():
                    if out.parent().parent() is sourceParentNode and out.isPassThrough():
                        connectCmd.setArgString("sourceAttribute", attr.fullName())
                        connectCmd.setArgString("destinationAttribute", out.fullName())
                        script += connectCmd.asScript() + "\n"
        i = 0
        for node in nodes:
            node.setName(oldNames[i])
            i += 1
        
        script = script.replace(sourceParentNode.fullName(), parentNodeName)
        
        # execute save script
        coralApp.CoralAppData.lastCreatedNodes = []
        coralApp.CoralAppData.appendToLastCreatedNodes = True
        coralApp.CoralAppData.loadingNetwork = True
        coralApp.setUndoLimit(0)
        exec(script)
        coralApp.setUndoLimit(100)
        coralApp.CoralAppData.loadingNetwork = False
        coralApp.CoralAppData.appendToLastCreatedNodes = False
        
        # return only the top nodes created
        outNodes = []
        for nodeName in coralApp.CoralAppData.lastCreatedNodes:
            node = coralApp.findNode(nodeName)
            if node.parent() is parentNode:
                outNodes.append(nodeName)
        
        self.setResultString(str(outNodes))
        
        coralApp.CoralAppData.lastCreatedNodes = []

def loadPlugin():
    plugin = Plugin("nodeEditorCommands")
    
    plugin.registerCommand(SetNodeUiData)
    plugin.registerCommand(Paste)
    
    return plugin
