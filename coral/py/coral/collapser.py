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


import coralApp
import utils
from _coral import NetworkManager

class CollapserData:
    collapsedNodeClassName = "CollapsedNode"

def setCollapsedNodeClassName(className):
    CollapserData.collapsedNodeClassName = className

def _checkNodesShareSameParent(nodes):
    parentNode = None
    
    for node in nodes:
        if parentNode is None:
            parentNode = node.parent()
        
        if parentNode is not node.parent():
            raise Exception("the nodes being collapsed must be under the same parent")
    
    return parentNode

def _disconnectConnectedInputs(node, nodes):
    inputAttrs = [[], []]
    
    for attr in node.inputAttributes():
        inputAttr = attr.input()
        
        if inputAttr:
            if inputAttr.parent() not in nodes:
                inputAttrs[0].append([inputAttr, attr])
                attr.disconnectInput()
        
    return inputAttrs

def _disconnectConnectedOutputs(node, nodes):
    outputAttrs = []
    
    for attr in node.outputAttributes():
        outAttrs = attr.outputs()
        
        for outAttr in outAttrs:
            if outAttr.parent() not in nodes:
                outputAttrs.append([attr, outAttr])
                outAttr.disconnectInput()
    
    return outputAttrs

def collapseNodes(nodes, collapsedNode = None):
    parentNode = _checkNodesShareSameParent(nodes)
    
    if collapsedNode is None:
        collapsedNode = coralApp.createNode(CollapserData.collapsedNodeClassName, CollapserData.collapsedNodeClassName, parentNode)
    
    inPassAttrs = {}
    outPassAttrs = {}
    
    for node in nodes:
        disconnectedAttributes = _disconnectConnectedInputs(node, nodes)
        
        inputAttributes = disconnectedAttributes[0]
        inputAttributesWithOutputConnections = disconnectedAttributes[1]
        
        outputAttributes = _disconnectConnectedOutputs(node, nodes)
        
        parentNode.removeNode(node)
        collapsedNode.addNode(node)
        
        for attr in inputAttributes:
            sourceAttr = attr[0]
            destAttr = attr[1]
            
            passAttrName = sourceAttr.parent().name() + "_" + sourceAttr.name()
            if inPassAttrs.has_key(passAttrName) == False:
                passAttr = coralApp.createAttribute("PassThroughAttribute", "input", collapsedNode, input = True)
                NetworkManager.connect(sourceAttr, passAttr)
                inPassAttrs[passAttrName] = passAttr
            else:
                passAttr = inPassAttrs[passAttrName]
            
            NetworkManager.connect(passAttr, destAttr)
        
        for attr in outputAttributes:
            sourceAttr = attr[0]
            destAttr = attr[1]
            
            passAttrName = sourceAttr.parent().name() + "_" + sourceAttr.name()
            if  outPassAttrs.has_key(passAttrName) == False:
                passAttr = coralApp.createAttribute("PassThroughAttribute", "output", collapsedNode, output = True)
                NetworkManager.connect(sourceAttr, passAttr)
                outPassAttrs[passAttrName] = passAttr
            else:
                passAttr = outPassAttrs[passAttrName]
                
            NetworkManager.connect(passAttr, destAttr)
    
    return collapsedNode

def explodeCollapsedNode(collapsedNode):
    parentNode = collapsedNode.parent()
    extractedNodes = collapsedNode.nodes()
    if parentNode:
        connections = []
        for attribute in collapsedNode.attributes():
            input = attribute.input()
            outputs = attribute.outputs()
            if input and outputs:
                connections.append([input, outputs])
        
            if input:
                attribute.disconnectInput()
        
            for output in outputs:
                output.disconnectInput()
    
    
        for node in collapsedNode.nodes():
            collapsedNode.removeNode(node)
            parentNode.addNode(node)
    
        for connection in connections:
            source = connection[0]
            dests = connection[1]
            
            for dest in dests:
                NetworkManager.connect(source, dest)
    else:
        raise coralApp.logError("collapsedNode has no parent to explode onto.")
    
    return extractedNodes

