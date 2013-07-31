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


from PyQt4 import QtGui, QtCore

from maya import cmds

from coral.coralUi.pluginUi import PluginUi
from coral.coralUi.nodeEditor.nodeUi import NodeUi


mayaColor = QtGui.QColor(154, 103, 205)
class MayaNodeUi(NodeUi):
    def __init__(self, coralNode):
        NodeUi.__init__(self, coralNode)
        
    def color(self):
        return mayaColor

class MayaContextNodeUi(NodeUi):
    def __init__(self, coralNode):
        NodeUi.__init__(self, coralNode)
        
        self.setCanOpenThis(True)
        
        self.addRightClickMenuItem("Select Node In Maya", self._selectNodeInMaya)
    
    def _selectNodeInMaya(self):
        cmds.select(self.coralNode().mayaNode(), replace = True)
    
    def color(self):
        return mayaColor

def loadPluginUi():
    pluginUi = PluginUi("coralMayaUi")
    
    pluginUi.registerNodeUi("MayaContext", MayaContextNodeUi)
    pluginUi.registerNodeUi("CoralMayaNode", MayaNodeUi)
    
    return pluginUi
