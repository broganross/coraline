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

##@package coral.coralApp
# Main module to perform the most basic operations.
import traceback
import os, sys
import copy 
import weakref
import traceback

import _coral
import utils
from observer import Observer, ObserverCollector
from rootNode import RootNode
from valueChangedObserverCollector import ValueChangedObserverCollector

class VerboseLevel:
    logNothing = 0
    logInfos = 1
    logErrors = 2
    logDebugs = 3

class CoralAppData:
    version = 0.3
    nodeClasses = {}
    classNameTags = {}
    attributeClasses = {}
    commandClasses = {}
    shouldLogInfos = True
    executedCommands = []
    undoLimit = 100
    undoneCommands = []
    rootNode = None
    autoLoadPaths = []
    instantiatedNodes = []
    loadingNetwork = False
    registeredNodeDescriptions = {}
    appendToLastCreatedNodes = False
    lastCreatedNodes = []
    currentNetworkDir = ""
    verboseLevel = VerboseLevel.logDebugs
    
    #observer lists
    registeredNodeClassesObservers = ObserverCollector()
    createdNodeObservers = ObserverCollector()
    deletingNodeObservers = ObserverCollector()
    connectedAttributesObservers = ObserverCollector()
    disconnectedInputObservers = ObserverCollector()
    attributeSpecializedObservers = ObserverCollector()
    createdAttributeObservers = ObserverCollector()
    deletingAttributeObservers = ObserverCollector()
    removedNodeObservers = ObserverCollector()
    addedNodeObservers = ObserverCollector()
    addedAttributeObservers = ObserverCollector()
    collapsedNodeObservers = ObserverCollector()
    nameChangedObservers = ObserverCollector()
    connectedInputObservers = ObserverCollector()
    removedAttributeObservers = ObserverCollector()
    initializedNewNetworkObservers = ObserverCollector()
    initializingNewNetworkObservers = ObserverCollector()
    generatingSaveScriptObservers = ObserverCollector()
    networkLoadedObservers = ObserverCollector()
    nodeConnectionChangedObservers = ObserverCollector()
    networkLoadingObservers = ObserverCollector()
    messageLoggedObservers = ObserverCollector()

def registeredNodeDescription(className):
    description = ""
    if CoralAppData.registeredNodeDescriptions.has_key(className):
        description = CoralAppData.registeredNodeDescriptions[className]
    
    return description

def instantiatedNodes():
    nodes = []
    for node in CoralAppData.instantiatedNodes:
        nodes.append(node())
    
    return nodes

def addAutoLoadPath(path):
    if path not in CoralAppData.autoLoadPaths:
        CoralAppData.autoLoadPaths.append(path)

def setVerboseLevel(level):
    CoralAppData.verboseLevel = level

def verboseLevel():
    return CoralAppData.verboseLevel

def scanPathForPlugins(path):
    verbLev = verboseLevel()
    setVerboseLevel(0)

    entries = os.listdir(path)
    for entry in entries:
        if entry.split(".")[-1] == "py":
            filename = os.path.join(path, entry)
            loadPlugin(filename)
    
    setVerboseLevel(verbLev)

def scanAutoLoadPaths():
    for path in CoralAppData.autoLoadPaths:
        scanPathForPlugins(path)

def version():
    return str(CoralAppData.version)

def _notifyMessageLogged(message, verboseLevel):
    for observer in CoralAppData.messageLoggedObservers.observers():
        observer.setData("message", message)
        observer.setData("verboseLevel", verboseLevel)
        observer.notify()

def addMessageLoggedObserver(observer, callback):
    CoralAppData.messageLoggedObservers.add(observer)
    observer.setNotificationCallback(callback)

def addNetworkLoadedObserver(observer, callback):
    CoralAppData.networkLoadedObservers.add(observer)
    observer.setNotificationCallback(callback)

def _notifyNetworkLoadedObservers(topNodeName):
    for observer in CoralAppData.networkLoadedObservers.observers():
        observer.setData("topNodeName", topNodeName)
        observer.notify()

def addNetworkLoadingObserver(observer, callback):
    CoralAppData.networkLoadingObservers.add(observer)
    observer.setNotificationCallback(callback)

def _notifyNetworkLoadingObservers():
    for observer in CoralAppData.networkLoadingObservers.observers():
        observer.notify()

def addGeneratingSaveScriptObserver(observer, callback):
    CoralAppData.generatingSaveScriptObservers.add(observer)
    observer.setNotificationCallback(callback)

def _notifyGeneratingSaveScriptObservers(node, saveScriptRef):
    for observer in CoralAppData.generatingSaveScriptObservers.observers():
        observer.setData("saveScript", saveScriptRef)
        observer.setData("nodeId", node.id())
        observer.notify()

def addInitializedNewNetworkObserver(observer, callback):
    CoralAppData.initializedNewNetworkObservers.add(observer)
    observer.setNotificationCallback(callback)

def _notifyInitializedNewNetworkObservers():
    for observer in CoralAppData.initializedNewNetworkObservers.observers():
        observer.notify()

def addInitializingNewNetworkObserver(observer, callback):
    CoralAppData.initializingNewNetworkObservers.add(observer)
    observer.setNotificationCallback(callback)

def _notifyInitializingNewNetworkObservers():
    for observer in CoralAppData.initializingNewNetworkObservers.observers():
        observer.notify()

def addConnectedInputObserver(observer, attribute, callback):
    CoralAppData.connectedInputObservers.add(observer, subject = attribute.id())
    observer.setNotificationCallback(callback)

def _notifyConnectedInputObservers(attribute):
    for observer in CoralAppData.connectedInputObservers.observers(attribute.id()):
        observer.notify()

def addNameChangedObserver(observer, nestedObject, callback):
    CoralAppData.nameChangedObservers.add(observer, subject = nestedObject.id())
    observer.setNotificationCallback(callback)

def _notifyNameChangedObservers(nestedObject):
    nestedObjectId = nestedObject.id()
    for observer in CoralAppData.nameChangedObservers.observers(nestedObject.id()):
        observer.notify()

def addCollapsedNodeObserver(observer, callback):
    CoralAppData.collapsedNodeObservers.add(observer)
    observer.setNotificationCallback(callback)

def _notifyCollapsedNodeObservers(collapsedNode):
    for observer in CoralAppData.collapsedNodeObservers.observers():
        observer.setData("collapsedNodeId", collapsedNode.id())
        observer.notify()
        
def addAddedAttributeObserver(observer, callback):
    CoralAppData.addedAttributeObservers.add(observer)
    observer.setNotificationCallback(callback)

def _notifyAddedAttributeObservers(parentNode, attributeAdded, input = False, output = False):
    for observer in CoralAppData.addedAttributeObservers.observers():
        observer.setData("parentNodeId", parentNode.id())
        observer.setData("attributeAddedId", attributeAdded.id())
        observer.setData("input", input)
        observer.setData("output", output)
        observer.notify()

def addRemovedAttributeObserver(observer, callback):
    CoralAppData.removedAttributeObservers.add(observer)
    observer.setNotificationCallback(callback)

def _notifyRemovedAttributeObserver(parentNode, attributeRemoved):
    for observer in CoralAppData.removedAttributeObservers.observers():
        observer.setData("parentNodeId", parentNode.id())
        observer.setData("attributeRemovedId", attributeRemoved.id())
        observer.notify()


def _notifyNodeConnectionChanged(node, attribute):
    for observer in CoralAppData.nodeConnectionChangedObservers.observers(node.id()):
        observer.setData("attributeId", attribute.id())
        observer.notify()

def addNodeConnectionChangedObserver(observer, node, callback):
    CoralAppData.nodeConnectionChangedObservers.add(observer, subject = node.id())
    observer.setNotificationCallback(callback)

def addAddedNodeObserver(observer, callback):
    CoralAppData.addedNodeObservers.add(observer)
    observer.setNotificationCallback(callback)

def _notifyAddedNodeObservers(parentNode, nodeAdded):
    for observer in CoralAppData.addedNodeObservers.observers():
        observer.setData("parentNodeId", parentNode.id())
        observer.setData("nodeAddedId", nodeAdded.id())
        observer.notify()

def addRemovedNodeObserver(observer, callback):
    CoralAppData.removedNodeObservers.add(observer)
    observer.setNotificationCallback(callback)

def _notifyRemovedNodeObservers(parentNode, nodeRemoved):
    for observer in CoralAppData.removedNodeObservers.observers():
        observer.setData("parentNodeId", parentNode.id())
        observer.setData("nodeRemovedId", nodeRemoved.id())
        observer.notify()

def addCreatedAttributeObserver(observer, callback):
    CoralAppData.createdAttributeObservers.add(observer)
    observer.setNotificationCallback(callback)
    
def _notifyCreatedAttributeObservers(attribute):
    for observer in CoralAppData.createdAttributeObservers.observers():
        observer.setData("attributeId", attribute.id())
        observer.notify()

def addAttributeSpecializedObserver(observer, attribute, callback):
    CoralAppData.attributeSpecializedObservers.add(observer, subject = attribute.id())
    observer.setNotificationCallback(callback)

def _notifyAttributeSpecialized(attribute):
    for observer in CoralAppData.attributeSpecializedObservers.observers(attribute.id()):
        observer.notify()

def addDisconnectedInputObserver(observer, attribute, callback):
    CoralAppData.disconnectedInputObservers.add(observer, subject = attribute.id())
    observer.setNotificationCallback(callback)

def _notifyDisconnectedInputObservers(attribute):
    for observer in CoralAppData.disconnectedInputObservers.observers(attribute.id()):
        observer.notify()

def addConnectedAttributesObserver(observer, callback):
    CoralAppData.connectedAttributesObservers.add(observer)
    observer.setNotificationCallback(callback)

def _notifyConnectedAttributesObservers(sourceAttribute, destinationAttribute):
    for observer in CoralAppData.connectedAttributesObservers.observers():
        observer.setData("sourceAttributeId", sourceAttribute.id())
        observer.setData("destinationAttributeId", destinationAttribute.id())
        observer.notify()

def addDeletingAttributeObserver(observer, callback):
    CoralAppData.deletingAttributeObservers.add(observer)
    observer.setNotificationCallback(callback)

def _notifyDeletingAttributeObservers(attribute):
    for observer in CoralAppData.deletingAttributeObservers.observers():
        observer.setData("attributeId", attribute.id())
        observer.notify()

def _notifyDeletingNodeObservers(node):
    for observer in CoralAppData.deletingNodeObservers.observers():
        observer.setData("nodeId", node.id())
        observer.notify()

def addDeletingNodeObserver(observer, callback):
    CoralAppData.deletingNodeObservers.add(observer)
    observer.setNotificationCallback(callback)

def _notifyCreatedNodeObservers(node):
    for observer in CoralAppData.createdNodeObservers.observers():
        observer.setData("nodeId", node.id())
        observer.notify()

def addCreatedNodeObserver(observer, callback):
    CoralAppData.createdNodeObservers.add(observer)
    observer.setNotificationCallback(callback)

def _notifyRegisteredNodeClassesObservers(nodeClasses, tags):
    for observer in CoralAppData.registeredNodeClassesObservers.observers():
        observer.setData("nodeClasses", nodeClasses)
        observer.setData("tags", tags)
        observer.notify()

def addRegisteredNodeClassesObserver(observer, callback):
    CoralAppData.registeredNodeClassesObservers.add(observer)
    observer.setNotificationCallback(callback)

def nodeClasses():
    return copy.copy(CoralAppData.nodeClasses)

def classNameTags():
    return copy.copy(CoralAppData.classNameTags)

def setShouldLogInfos(value = True):
    CoralAppData.shouldLogInfos = value

def logError(message):
    exc = Exception("# error: " + str(message))
    if(CoralAppData.verboseLevel >= VerboseLevel.logErrors):
        print exc
        _notifyMessageLogged(message, VerboseLevel.logErrors)
    
    return exc

def logInfo(message):
    if(CoralAppData.verboseLevel >= VerboseLevel.logInfos):
        print "# info: " + (message)
        _notifyMessageLogged(message, VerboseLevel.logInfos)

def logDebug(message):
    if(CoralAppData.verboseLevel >= VerboseLevel.logDebugs):
        print "# debug: " + (message)

def _instantiateNode(className, name, parent):
    coralNodeClass = findNodeClass(className)
    
    coralNode = None
    if coralNodeClass.__dict__.has_key("createUnwrapped"):
        coralNode = coralNodeClass.createUnwrapped(name, parent)
    else:
        coralNode = coralNodeClass(name, parent)
    
    if coralNode.isInvalid():
        logError("failed to create node " + className + ".\n" + coralNode.invalidityMessage())
        return None
    
    slicer = coralNode.slicer()
    if not coralNode.sliceable() and slicer:
        logError("This node can't be nested under a slicer node like " + slicer.className() + ", sorry : (")
        return None

    if coralNode.className() != className:
        coralNode.setClassName(className)

    coralNode._postInit()
    
    parent.addNode(coralNode)
    
    if hasattr(coralNode, "postInit") and CoralAppData.loadingNetwork == False:
        coralNode.postInit()
    
    CoralAppData.instantiatedNodes.append(weakref.ref(coralNode))
    
    return coralNode

def createNode(className, name, parent):
    nodeClass = findNodeClass(className)
    
    node = None
    if nodeClass:
        node = _instantiateNode(className, name, parent)
        
        if CoralAppData.appendToLastCreatedNodes:
            CoralAppData.lastCreatedNodes.append(node.fullName())
    else:
        logError("could not find any className named '" + className + "'")
    
    return node

def createAttribute(className, name, parent, input = False, output = False):
    attr = None
    
    if parent.allowDynamicAttributes():
        attrClass = findAttributeClass(className)
        
        if attrClass:
            attr = attrClass(name, parent)
        
            attr._setIsInput(input)
            attr._setIsOutput(output)
            
            attr._postInit()
        
            if input:
                parent.addInputAttribute(attr)
            elif output:
                parent.addOutputAttribute(attr)
            
            parent.addDynamicAttribute(attr)
    else:
        logError("failed to create attribute, the parent node has allowDynamicAttributes set to False.")
    
    return attr

def rootNode():
    return CoralAppData.rootNode

def findNode(fullName):
    names = fullName.split(".")
    firstName = names.pop(0)
    
    node = None
    if firstName == rootNode().name():
        node = rootNode()
        for name in names:
            node = node.findNode(name)
        
            if node is None:
                break
    
    return node

def findAttribute(fullName):
    names = fullName.split(".")
    attrName = names.pop(-1)
    
    fullNodeName = ".".join(names)
    node = findNode(fullNodeName)
    
    attribute = None
    if node:
        attribute = node.findAttribute(attrName)
    
    return attribute

def findObject(fullName):
    names = fullName.split(".")
    rootName = names.pop(0)

    object = None
    root = rootNode()
    if rootName == root.name():
        object = root
        for name in names:
            object = object.findObject(name)
            
            if object is None:
                break
    
    return object

def findObjectById(id):
    return _coral.NetworkManager.findObjectById(id)

def registerCommandClass(className, commandClass):
    CoralAppData.commandClasses[className] = commandClass

def _initCollapsedNodes():
    from collapsedNode import CollapsedNode
    
    registerNodeClass("CollapsedNode", CollapsedNode, tags = ["encapsulation"])
    registerAttributeClass("PassThroughAttribute", _coral.PassThroughAttribute)

def init():
    CoralAppData.rootNode = RootNode("root")
    
    _initCollapsedNodes()
    
    import builtinCommands
    loadPluginModule(builtinCommands)
    
    import builtinNodes
    loadPluginModule(builtinNodes)
    
    _coral.setCallback("node_created", _node_created)
    _coral.setCallback("node_addNode", _node_addNode)
    _coral.setCallback("node_removeNode", _node_removeNode)
    _coral.setCallback("attribute_created", _attribute_created)
    _coral.setCallback("attribute_connectTo", _attribute_connectTo)
    _coral.setCallback("attribute_disconnectInput", _attribute_disconnectInput)
    _coral.setCallback("node_addInputAttribute", _node_addInputAttribute)
    _coral.setCallback("node_addOutputAttribute", _node_addOutputAttribute)
    _coral.setCallback("node_removeAttribute", _node_removeAttribute)
    _coral.setCallback("node_deleteIt", _node_deleteIt)
    _coral.setCallback("node_connectionChanged", _node_connectionChanged)
    _coral.setCallback("attribute_deleteIt", _attribute_deleteIt)
    _coral.setCallback("nestedObject_setName", _nestedobject_setName)
    _coral.setCallback("attribute_specialization", _attribute_specialization)


    if os.environ.has_key("CORAL_PLUGINS_PATH"):
        path = os.environ["CORAL_PLUGINS_PATH"]
        paths = path.split(os.pathsep)
        for path in paths:
            addAutoLoadPath(path)

# def _attribute_valueChanged(attribute):
#     _notifyAttributeValueChangedObservers(attribute)

def _nestedobject_setName(nestedObject, newName):
    _notifyNameChangedObservers(nestedObject)

def _attribute_deleteIt(attribute):
    _notifyDeletingAttributeObservers(attribute)

def _node_deleteIt(node):
    nodeRef = weakref.ref(node)
    if nodeRef in CoralAppData.instantiatedNodes:
        del CoralAppData.instantiatedNodes[CoralAppData.instantiatedNodes.index(nodeRef)]
        
    _notifyDeletingNodeObservers(node)

def _node_addOutputAttribute(parentNode, attributeAdded):
    _notifyAddedAttributeObservers(parentNode, attributeAdded, output = True)

def _node_removeAttribute(parentNode, attributeRemoved):
    _notifyRemovedAttributeObserver(parentNode, attributeRemoved)

def _node_connectionChanged(node, attribute):
    _notifyNodeConnectionChanged(node, attribute)

def _node_addInputAttribute(parentNode, attributeAdded):
    _notifyAddedAttributeObservers(parentNode, attributeAdded, input = True)

def _node_addNode(parentNode, nodeAdded):
    _notifyAddedNodeObservers(parentNode, nodeAdded)

def _node_removeNode(parentNode, nodeRemoved):
    _notifyRemovedNodeObservers(parentNode, nodeRemoved)

def _attribute_created(attribute):
    _notifyCreatedAttributeObservers(attribute)

def _attribute_specialization(attribute):
    _notifyAttributeSpecialized(attribute)

def _attribute_disconnectInput(attribute):
    _notifyDisconnectedInputObservers(attribute)

def _node_created(node):
    _notifyCreatedNodeObservers(node)

def _attribute_connectTo(sourceAttribute, destinationAttribute):
    _notifyConnectedAttributesObservers(sourceAttribute, destinationAttribute)
    _notifyConnectedInputObservers(destinationAttribute)

def loadPluginModule(pluginModule):
    plugin = pluginModule.loadPlugin()
    for nodeClassDict in plugin._registeredNodes:
        className = nodeClassDict["className"]
        nodeClass = nodeClassDict["nodeClass"]
        tags = nodeClassDict["tags"]
        description = nodeClassDict["description"]
        
        registerNodeClass(className, nodeClass, tags, description)
        
    for attrClassDict in plugin._registeredAttributes:
        className = attrClassDict["className"]
        attributeClass = attrClassDict["attributeClass"]
        registerAttributeClass(className, attributeClass)
        
    for commandClass in plugin._registeredCommands:
        registerCommandClass(commandClass.__name__, commandClass)
    
    if plugin._registeredNodes:
        _notifyRegisteredNodeClassesObservers(CoralAppData.nodeClasses, CoralAppData.classNameTags)

def loadPlugin(filename):
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
                logError("skipping plugin " + str(filename))

                exc_type, exc_value, exc_traceback = sys.exc_info()
                traceback.print_exception(exc_type, exc_value, exc_traceback, limit = 2, file = sys.stdout)
                
            if module:
                if hasattr(module, "loadPlugin"):
                    found = True
                    loadPluginModule(module)
                else:
                    logError("no loadPlugin function found in file " + str(filename))
                    
            break
    
    if not found:
        logError("could not find file " + str(filename))

def finalize():
    _coral.setCallback("node_created", None)
    _coral.setCallback("node_addNode", None)
    _coral.setCallback("node_removeNode", None)
    _coral.setCallback("attribute_created", None)
    _coral.setCallback("attribute_connectTo", None)
    _coral.setCallback("attribute_disconnectInput", None)
    _coral.setCallback("node_addInputAttribute", None)
    _coral.setCallback("node_addOutputAttribute", None)
    _coral.setCallback("node_deleteIt", None)
    _coral.setCallback("attribute_deleteIt", None)
    _coral.setCallback("nestedObject_setName", None)
    _coral.setCallback("attribute_specialization", None)
    _coral.setCallback("attribute_valueChanged", None)
    
    CoralAppData.rootNode.deleteIt()
    CoralAppData.rootNode = None
    
def _setClassNameTag(className, tag):
    if CoralAppData.classNameTags.has_key(tag) == False:
        CoralAppData.classNameTags[tag] = []
    
    if className not in CoralAppData.classNameTags[tag]:
        CoralAppData.classNameTags[tag].append(className)

def registerNodeClass(className, nodeClass, tags = [], description = ""):
    CoralAppData.nodeClasses[className] = nodeClass
    
    if not description:
        description = "Does what its name says...hopefully."
        
    CoralAppData.registeredNodeDescriptions[className] = description
    
    for tag in tags:
        _setClassNameTag(className, tag)
    
def registerAttributeClass(className, coralAttributeClass):
    CoralAppData.attributeClasses[className] = coralAttributeClass

def findNodeClass(className):
    nodeClass = None
    if CoralAppData.nodeClasses.has_key(className):
        nodeClass = CoralAppData.nodeClasses[className]
    
    return nodeClass

def findAttributeClass(className):
    attrClass = None
    if CoralAppData.attributeClasses.has_key(className):
        attrClass = CoralAppData.attributeClasses[className]
    
    return attrClass

def collapseNodes(nodes, collapsedNode = None):
    import collapser
    
    if collapsedNode:
        if collapsedNode.__class__.__name__ != "CollapsedNode":
            logError("invalid collapsedNode passed as argument")
            return None
        
    collapsedNode = collapser.collapseNodes(nodes, collapsedNode)
    _notifyCollapsedNodeObservers(collapsedNode)
    
    return collapsedNode

def explodeCollapsedNode(collapsedNode):
    import collapser
    
    extractedNodes = collapser.explodeCollapsedNode(collapsedNode)
    collapsedNode.deleteIt()
    
    return extractedNodes
    
def _setCommandArgs(command, args):
    for argName in command.argNames():
        if args.has_key(argName):
            arg = args[argName]
            argType = command.argType(argName)
            
            if argType == _coral.Command.boolType:
                command.setArgBool(argName, arg)
            elif argType == _coral.Command.intType:
                command.setArgInt(argName, arg)
            elif argType == _coral.Command.floatType:
                command.setArgFloat(argName, arg)
            elif argType == _coral.Command.stringType:
                command.setArgString(argName, arg)
            elif argType == _coral.Command.undefinedType:
                command.setArgUndefined(argName, str(arg))
        #else:
        #    raise Exception("Could not find expected args for command " + command.name() + ",\nexpected " + str(command.argNames()) + ",\npassed " + str(args.keys()))

def _getCommandResult(command):
    result = None
    resultType = command.resultType()
    if resultType != _coral.Command.undefinedType:
        if resultType == _coral.Command.boolType:
            result = command.resultAsBool()
        elif resultType == _coral.Command.intType:
            result = command.resultAsInt()
        elif resultType == _coral.Command.floatType:
            result = command.resultAsFloat()
        elif resultType == _coral.Command.stringType:
            result = command.resultAsString()
    
    return result

def _executeCommand(cmdName, redo, **args):
    result = None
    
    if CoralAppData.commandClasses.has_key(cmdName):
        commandClass = CoralAppData.commandClasses[cmdName]
        command = commandClass()
        command.setName(cmdName)
        
        _setCommandArgs(command, args)
            
        if CoralAppData.shouldLogInfos:
            print command.asScript()
        
        command.doIt()
            
        result = _getCommandResult(command)
        
        if len(CoralAppData.executedCommands) > CoralAppData.undoLimit:
            CoralAppData.executedCommands.pop(0)

        if CoralAppData.undoLimit:
            CoralAppData.executedCommands.append(command)

        if not redo:
            CoralAppData.undoneCommands = []
    else:
        raise Exception("could not find any commad named " + str(cmdName))

    return result

def setUndoLimit(limit):
    CoralAppData.undoLimit = limit

def undo():
    if CoralAppData.executedCommands:
        lastCommand = CoralAppData.executedCommands.pop(-1)
        lastCommand.undoIt()

        CoralAppData.undoneCommands.append(lastCommand)
        
        if CoralAppData.shouldLogInfos:
            print "# Undo: " + lastCommand.asScript()
    else:
        print "# nothing to be undone"

def redo():
    if CoralAppData.undoneCommands:
        lastUndone = CoralAppData.undoneCommands.pop(-1)
        _executeCommand(lastUndone.name, True, **lastUndone.args)

def executeCommand(cmdName, **args):
    return _executeCommand(cmdName, False, **args)

def deleteNodes(nodes):
    for node in nodes:
        node.deleteIt()

def deleteAttributes(attributes):
    for attribute in attributes:
        parentNode = attribute.parent()
        if attribute in parentNode.dynamicAttributes():
            attribute.deleteIt()

def _parseNetworkScript(saveScript):
    #returns the variables collected in the saveScript.
    
    # required variables
    version = -1.0
    
    exec(saveScript)
    
    return locals() # this builtin function returns the vars in this function (and those collected executing the saveScript

def _loadNetworkScript(networkScript, topNode = ""):
    _notifyNetworkLoadingObservers()

    networkScriptData = _parseNetworkScript(networkScript)
    if networkScriptData["version"] > CoralAppData.version:
        logError("version mismatch")
        return;
    
    CoralAppData.loadingNetwork = True
    networkScriptData["runScript"](topNode)
    CoralAppData.loadingNetwork = False
    
    _notifyNetworkLoadedObservers(topNode)

def openNetworkFile(filename):
    if filename:
        newNetwork()
        
        file = open(filename, "rU")
    
        saveScript = file.read()
        file.close()

        filePath = os.path.split(filename)[0]
        CoralAppData.currentNetworkDir = filePath

        _coral.NetworkManager.addSearchPath(filePath)
        
        _loadNetworkScript(saveScript, topNode = CoralAppData.rootNode.fullName())
        
        if CoralAppData.shouldLogInfos:
            logInfo("loaded netowrk file: " + filename)

def _generateNetworkScript(topNode, saveTopNode = False):
    saveScript = "# coral save script\n" # idiotic comment to make this file look cool ; )
    saveScript += "version = 0.1\n" # version to guarantee retro compatibility when necessary
    
    saveScript += "\n"
    saveScript += "def runScript(topNode = 'root'):\n" # open a string that will contain the actual script to regenerate the scene
    
    runScript = ""
    if saveTopNode:
        runScript += topNode.asScript()
        
    runScript += topNode.contentAsScript()
    
    saveScriptRef = [""] #guarantees this string is referenced reather than copied
    _notifyGeneratingSaveScriptObservers(topNode, saveScriptRef)
    
    runScript += saveScriptRef[0]
    
    # replace the full name of the parent with a variable that can be set at load time
    parent = ""
    if saveTopNode:
        parent = topNode.parent().fullName()
    else:
        parent = topNode.fullName()
    
    runScript = runScript.replace("'" + parent, "topNode + '")
    runScript = runScript.replace("topNode + ''", "topNode")
    
    # indent script to be run inside runScript() function
    lines = runScript.split("\n")
    for line in lines:
        saveScript += "    " + line + "\n"
    
    return saveScript

def saveNetworkFile(filename):
    if filename:
        saveScript = _generateNetworkScript(topNode = CoralAppData.rootNode)
        
        file = open(filename, "w")
        file.write(saveScript)
        file.close()
        
        if CoralAppData.shouldLogInfos:
            logInfo("saved network file: " + filename)

def newNetwork():
    _notifyInitializingNewNetworkObservers()

    CoralAppData.rootNode = RootNode("root")
    
    _coral.NetworkManager.removeSearchPath(CoralAppData.currentNetworkDir)
    CoralAppData.currentNetworkDir = ""

    _notifyInitializedNewNetworkObservers()

def _convertCollapsedNodeToScript(collapsedNode):
    className = collapsedNode.name().replace(" ", "_")
    if className in CoralAppData.nodeClasses.keys():
        className = className + "_"
    
    script = ""
    script += "from coral.collapsedNode import CollapsedNode\n"
    script += "from coral.coralApp import *\n"
    script += "\n" 
    script += "class " + className + "(CollapsedNode):\n"
    script += "    def __init__(self, name, parent):\n"
    script += "        CollapsedNode.__init__(self, name, parent)\n"
    script += "        self.setClassName('"+ className + "')\n"
    script += "\n"
    script += "    def postInit(self):\n"
    script += "        topNode = self.fullName()\n"
    
    nodeScript = collapsedNode._attributesAsScript()
    nodeScript += collapsedNode.contentAsScript()
    
    saveScriptRef = [""]
    _notifyGeneratingSaveScriptObservers(collapsedNode, saveScriptRef)
    nodeScript += "\n"
    nodeScript += saveScriptRef[0]
    
    parent = collapsedNode.fullName()
    nodeScript = nodeScript.replace("'" + parent, "topNode + '")
    nodeScript = nodeScript.replace("topNode + ''", "topNode")
    
    for line in nodeScript.split("\n"):
        script += "        " + line + "\n"
    
    script += "def loadPlugin():\n"
    script += "    from coral.plugin import Plugin\n"
    script += "    \n"
    script += "    plugin = Plugin('" + className + "_plugin')\n"
    script += "    plugin.registerNode('" + className + "', " + className + ", tags = ['collapsed nodes'])\n"
    script += "    \n"
    script += "    return plugin\n"
    script += "\n"
    
    return script

def saveCollapsedNodeFile(collapsedNode, filename):
    script = _convertCollapsedNodeToScript(collapsedNode)
    
    file = open(filename, "w")
    file.write(script)
    file.close()
    
    if CoralAppData.shouldLogInfos:
        logInfo("saved collapsedNode file: " + filename)
    
    path = os.path.split(filename)[0]
    if path in CoralAppData.autoLoadPaths:
        scanPathForPlugins(path)

def importCollapsedNodeFile(filename, topNode):
    if filename:
        file = open(filename, "rU")
        saveScript = file.read()
        file.close()
        
        saveScriptData = _parseNetworkScript(saveScript)
        if saveScriptData["version"] != CoralAppData.version:
            logError("version mismatch")
            return;
        
        if not saveScriptData.has_key("collapsedNodeFile"):
            if saveScriptData["collapsedNodeFile"] != True:
                logError("not a CollapsedNode file")
                return
            
        saveScriptData["runScript"](topNode = topNode.fullName())
    
        if CoralAppData.shouldLogInfos:
            logInfo("loaded netowrk file: " + filename)

def connectAttributes(sourceAttribute, destinationAttribute):
    _coral.NetworkManager.connect(sourceAttribute, destinationAttribute)
