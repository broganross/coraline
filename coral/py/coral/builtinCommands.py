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


from _coral import Command
import coralApp
from _coral import NetworkManager, ErrorObject
from plugin import Plugin

class CreateNode(Command):
    def __init__(self):
        Command.__init__(self)
        
        self.setArgString("className", "")
        self.setArgString("name", "")
        self.setArgString("parentNode", "")
        self.setArgString("specializationPreset", "")
        
    def doIt(self):
        className = self.argAsString("className")
        name = self.argAsString("name")
        parentNode = self.argAsString("parentNode")
        
        newNode = None
        
        parentNode = coralApp.findNode(parentNode)
        if parentNode:
            newNode = coralApp.createNode(className, name, parentNode)
            
            if newNode:
                specializationPreset = self.argAsString("specializationPreset")
                if specializationPreset:
                    newNode.enableSpecializationPreset(specializationPreset)
                
                self.setResultString(newNode.fullName())
                
        if newNode is None:
            coralApp.logDebug("CreateNode Command: failed to create new node.")

class CreateAttribute(Command):
    def __init__(self):
        Command.__init__(self)
        
        self.setArgString("className", "")
        self.setArgString("name", "")
        self.setArgString("parentNode", "")
        self.setArgBool("input", False)
        self.setArgBool("output", False)
        self.setArgString("specializationOverride", "none")
        
    def doIt(self):
        className = self.argAsString("className")
        name = self.argAsString("name")
        parentNode = self.argAsString("parentNode")
        input = self.argAsBool("input")
        output = self.argAsBool("output")
        
        newAttr = None
        
        parentNode = coralApp.findNode(parentNode)
        if parentNode:
            newAttr = coralApp.createAttribute(className, name, parentNode, input, output)
            if newAttr:
                specializationOverride = self.argAsString("specializationOverride")
                if specializationOverride != "" and specializationOverride != "none":
                    newAttr.setSpecializationOverride(specializationOverride)
                    
                self.setResultString(newAttr.fullName())
        
        if newAttr is None:
            coralApp.logDebug("CreateAttribute Command: failed to create new attribute.")

class SetupDynamicAttribute(Command):
    def __init__(self):
        Command.__init__(self)
        
        self.setArgString("attribute", "")
        self.setArgUndefined("affect", "[]")
        self.setArgUndefined("affectedBy", "[]")
        self.setArgUndefined("allowedSpecialization", "[]")
        self.setArgUndefined("specializationLinkedTo", "[]")
        self.setArgUndefined("specializationLinkedBy", "[]")
    
    def doIt(self):
        attribute = coralApp.findAttribute(self.argAsString("attribute"))
        affect = eval(self.argAsString("affect"))
        affectedBy = eval(self.argAsString("affectedBy"))
        allowedSpecialization = eval(self.argAsString("allowedSpecialization"))
        specLinkedTo = eval(self.argAsString("specializationLinkedTo"))
        specLinkedBy = eval(self.argAsString("specializationLinkedBy"))
        
        parentNode = attribute.parent()
        parentNodeDynamicAttrs = parentNode.dynamicAttributes()
        if attribute in parentNodeDynamicAttrs:
            for attrName in affect:
                attr = coralApp.findAttribute(attrName)
                if attr.isAffectedBy(attribute) == False:
                    parentNode._setAttributeAffect(attribute, attr)
            
            for attrName in affectedBy:
                attr = coralApp.findAttribute(attrName)
                if attribute.isAffectedBy(attr) == False:
                    parentNode._setAttributeAffect(attr, attribute)
            
            for attrName in specLinkedTo:
                attr = coralApp.findAttribute(attrName)
                if attr not in attribute.specializationLinkedTo():
                    parentNode._addAttributeSpecializationLink(attribute, attr)
            
            for attrName in specLinkedBy:
                attr = coralApp.findAttribute(attrName)
                if attribute not in attr.specializationLinkedTo():
                    parentNode._addAttributeSpecializationLink(attr, attribute)
            
            parentNode._setAttributeAllowedSpecializations(attribute, allowedSpecialization)
            parentNode._updateAttributeSpecialization(attribute)

class DeleteObjects(Command):
    def __init__(self):
        Command.__init__(self)
        
        self.setArgUndefined("nodes", "[]")
        self.setArgUndefined("attributes", "[]")
    
    def _deleteAttributes(self, attributesName):
        attributes = []
        for attrName in attributesName:
            attr = coralApp.findAttribute(attrName)
            if attr:
                attributes.append(attr)
        
        if attributes:
            coralApp.deleteAttributes(attributes)
    
    def _deleteNodes(self, nodesName):
        nodes = [] 
        for nodeName in nodesName:
            node = coralApp.findNode(nodeName)
            if node:
                nodes.append(node)
        
        if nodes:
            coralApp.deleteNodes(nodes)
    
    def doIt(self):
        attributes = eval(self.argAsString("attributes"))
        self._deleteAttributes(attributes)
        
        nodes = eval(self.argAsString("nodes"))
        self._deleteNodes(nodes)

class ConnectAttributes(Command):
    def __init__(self):
        Command.__init__(self)
        
        self.setArgString("sourceAttribute", "")
        self.setArgString("destinationAttribute", "")
        
    def doIt(self):
        success = False
        error = None
        
        sourceAttributeName = self.argAsString("sourceAttribute")
        destinationAttributeName = self.argAsString("destinationAttribute")
        
        sourceAttribute = coralApp.findAttribute(sourceAttributeName)
        destinationAttribute = coralApp.findAttribute(destinationAttributeName)
        
        if sourceAttribute and destinationAttribute:
            if destinationAttribute.input() is not None:
                destinationAttribute.disconnectInput()
            
            error = ErrorObject()
            success = NetworkManager.connect(sourceAttribute, destinationAttribute, error)
            
        if not success:
            errorMessage = "error during connection between " + sourceAttributeName + " and " + destinationAttributeName + "\n"
            if error:
                if error.message():
                    errorMessage += "extended info: " + error.message()
                
            coralApp.logDebug(errorMessage)
            
class DisconnectInput(Command):
    def __init__(self):
        Command.__init__(self)
        
        self.setArgString("attribute", "")
        
    def doIt(self):
        attribute = self.argAsString("attribute")
        
        attribute = coralApp.findAttribute(attribute)
        attribute.disconnectInput()

class CollapseNodes(Command):
    def __init__(self):
        Command.__init__(self)
        
        self.setArgUndefined("nodes", "[]")
        
    def doIt(self):
        nodes = eval(self.argAsString("nodes"))
        
        nodeNames = nodes
        nodes = [] 
        for nodeName in nodeNames:
            node = coralApp.findNode(nodeName)
            if node:
                nodes.append(node)
        
        coralApp.collapseNodes(nodes)

class ExplodeCollapsedNode(Command):
    def __init__(self):
        Command.__init__(self)
        
        self.setArgString("collapsedNode", "")
    
    def doIt(self):
        from collapsedNode import CollapsedNode
        collapsedNodeName = self.argAsString("collapsedNode")
        
        collapsedNode = coralApp.findNode(collapsedNodeName)
        if collapsedNode:
            if collapsedNode.__class__ is CollapsedNode:
                coralApp.explodeCollapsedNode(collapsedNode)
            else:
                coralApp.logDebug("ExplodeCollapsedNode Command: input node must be of type CollapsedNode")

class SetAttributeValue(Command):
    def __init__(self):
        Command.__init__(self)
        
        self.setArgString("attribute", "")
        self.setArgString("value", "")
    
    def doIt(self):
        attribute = self.argAsString("attribute")
        value = self.argAsString("value")
        
        attribute = coralApp.findAttribute(attribute)
        if attribute:
            if attribute.outValue():
                attribute.outValue().setFromString(value);
                attribute.valueChanged()

def loadPlugin():
    plugin = Plugin("builtinCommands")
    
    plugin.registerCommand(CreateNode)
    plugin.registerCommand(CreateAttribute)
    plugin.registerCommand(DeleteObjects)
    plugin.registerCommand(ConnectAttributes)
    plugin.registerCommand(DisconnectInput)
    plugin.registerCommand(CollapseNodes)
    plugin.registerCommand(ExplodeCollapsedNode)
    plugin.registerCommand(SetAttributeValue)
    plugin.registerCommand(SetupDynamicAttribute)

    return plugin

