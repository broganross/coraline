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


from maya import cmds

import coral

# This is the base class for those nodes that intend to exchanged data with maya.
# Each one of these nodes is associated with a maya-attribute under a CoralNetworkNode.
class CoralMayaNode(coral.Node):
    floatAttributeType = 1
    intAttributeType = 2
    meshAttributeType = 3
    floatArrayAttributeType = 4
    matrixAttributeType = 5
    matrixArrayAttributeType = 6
    angleAttributeType = 7
    angle3ArrayAttributeType = 8
    float3ArrayAttributeType = 9
    
    def __init__(self, name, parent):
        coral.Node.__init__(self, name, parent)
        self.setClassName("CoralMayaNode")
        
        if parent.className() != "MayaContext":
            self._setIsInvalid(True, self.className() + " must be created under a MayaContextNode!")
            return
    
    def deleteIt(self):
        mayaNode = self.parent().mayaNode()
        cmds.lockNode(mayaNode, lock = False)
        
        if self.isValid():
            for attr in self.attributes():
                if hasattr(attr, "mayaAttribute"):
                    cmds.deleteAttr(attr.mayaNode() + "." + attr.mayaAttribute())
        
            cmds.coralMayaCmd("updateCoralAttributeMap", self.parent().mayaNode())
        
        coral.Node.deleteIt(self)
        
        cmds.lockNode(mayaNode, lock = True)
        
    def setName(self, name):
        coral.Node.setName(self, str(name))
        newName = str(self.name())
        
        mayaNode = self.parent().mayaNode()
        cmds.lockNode(mayaNode, lock = False)
        
        for attr in self.attributes():
            if hasattr(attr, "setMayaAttribute"):
                cmds.renameAttr(attr.mayaNode() + "." + attr.mayaAttribute(), newName)
                attr.setMayaAttribute(newName)
        
        cmds.lockNode(mayaNode, lock = True)
                
    # this is just a utility method to create the most common maya-attribute types,
    def createMayaAttribute(self, type):
        mayaNode = self.parent().mayaNode()
        attrName = self.name()
        
        if cmds.listAttr(mayaNode, string = attrName) is None:
            cmds.select(mayaNode, replace = True)
            cmds.lockNode(mayaNode, lock = False)
        
            if type == CoralMayaNode.floatAttributeType:
                cmds.addAttr(longName = attrName, attributeType = "float", keyable = True)
            elif type == CoralMayaNode.intAttributeType:
                cmds.addAttr(longName = attrName, attributeType = "long", keyable = True)
            elif type == CoralMayaNode.meshAttributeType:
                cmds.addAttr(longName = attrName, dataType = "mesh")
            elif type == CoralMayaNode.floatArrayAttributeType:
                cmds.addAttr(longName = attrName, attributeType = "float", multi = True)
            elif type == CoralMayaNode.matrixAttributeType:
                cmds.addAttr(longName = attrName, attributeType = "matrix")
            elif type == CoralMayaNode.matrixArrayAttributeType:
                cmds.addAttr(longName = attrName, dataType = "matrix", multi = True)
            elif type == CoralMayaNode.angleAttributeType:
                cmds.addAttr(longName = attrName, attributeType = "doubleAngle", keyable = True)
            elif type == CoralMayaNode.angle3ArrayAttributeType:
                cmds.addAttr(longName = attrName, numberOfChildren = 3, attributeType =  "compound", multi = True)
                cmds.addAttr(longName = attrName + "_angleX", attributeType = "doubleAngle", parent = attrName)
                cmds.addAttr(longName = attrName + "_angleY", attributeType = "doubleAngle", parent = attrName)
                cmds.addAttr(longName = attrName + "_angleZ", attributeType = "doubleAngle", parent = attrName)
                
            elif type == CoralMayaNode.float3ArrayAttributeType:
                cmds.addAttr(longName = attrName, numberOfChildren = 3, attributeType =  "compound", multi = True)
                cmds.addAttr(longName = attrName + "_valueX", attributeType = "float", parent = attrName)
                cmds.addAttr(longName = attrName + "_valueY", attributeType = "float", parent = attrName)
                cmds.addAttr(longName = attrName + "_valueZ", attributeType = "float", parent = attrName)
            
            cmds.lockNode(mayaNode, lock = True)
                
        return attrName
    
    # This method has to be called in the constructor of the subclass in order
    # to associate a maya-attribute to a coral-attribute.
    def associateMayaAttribute(self, mayaAttribute, coralAttribute):
        mayaNodeName = self.parent().mayaNode()
        
        coralAttribute.setMayaNode(mayaNodeName)
        coralAttribute.setMayaAttribute(mayaAttribute)
