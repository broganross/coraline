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
import coral
import coralMayaApp
from maya import cmds, OpenMaya

class NodeChangedNameCallback(object):
    #fixes a problem with the scriptJob callback that doesn't pass the new name.
    def __init__(self, mayaNodeName, callback):
        self._instance = weakref.ref(callback.im_self)
        self._callback = callback.im_func
        
        sel = OpenMaya.MSelectionList()
        sel.add(mayaNodeName)
        obj = OpenMaya.MObject()
        sel.getDependNode(0, obj)
        
        self._nodeFn = OpenMaya.MFnDependencyNode(obj)
    
    def __call__(self):
        self._callback(self._instance(), self._nodeFn.name())

# The network of nodes contained in a MayaContextNode forms the algorithm processed in CoralNetworkNode::compute, 
# all the CoralMayaNode found under this node wil form the input and output attributes of the CoralNetworkNode in maya.
class MayaContextNode(coral.Node):
    def __init__(self, name, parent):
        coral.Node.__init__(self, name, parent)
        self.setClassName("MayaContext")
        
        self._mayaNode = ""
        
        if self.className() in parent.classNames():
            self._setIsInvalid(True, self.className() + " can only be created under the root!")
            return
        
        associateCoralNetworkNode = coralMayaApp.FiberMayaAppData._associateCoralNetworkNode
        if associateCoralNetworkNode: # this var is set from coralMayaApp
            self._mayaNode = str(associateCoralNetworkNode)
        else:
            self._mayaNode = str(cmds.createNode("coralNetworkNode", name = "coralNetworkNode"))
            cmds.lockNode(self._mayaNode, lock = True)
        
        cmds.setAttr(self._mayaNode + ".coralNodeId", self.id())
        
        cmds.scriptJob(nodeNameChanged = [self._mayaNode, NodeChangedNameCallback(self._mayaNode, self._mayaNodeChangedName)])
    
    def deleteIt(self):
        coral.Node.deleteIt(self)
        
        if self._mayaNode:
            cmds.lockNode(self._mayaNode, lock = False)
            cmds.delete(self._mayaNode)
            self._mayaNode = ""
    
    def _mayaNodeChangedName(self, newName):
        self._mayaNode = str(newName)
        
        for node in self.nodes():
            if "CoralMayaNode" in node.classNames():
                for attr in node.attributes():
                    if hasattr(attr, "setMayaNode"):
                        attr.setMayaNode(self._mayaNode)
    
    def mayaNode(self):
        return self._mayaNode;
    
    def addNode(self, node):
        coral.Node.addNode(self, node)
        
        if "CoralMayaNode" in node.classNames():
            # we have to call updateCoralAttributeMap here because doing so in a CoralMayaNode contructor we would be wrong (the CoralMayaNode wouldn't be under its parent yet). 
            cmds.coralMayaCmd("updateCoralAttributeMap", self._mayaNode)
            
            for attr in node.inputAttributes():
                if hasattr(attr, "mayaAttribute"):
                    # this will guarantee things are up to date from the start
                    cmds.dgdirty(attr.mayaNode() + "." + attr.mayaAttribute())
