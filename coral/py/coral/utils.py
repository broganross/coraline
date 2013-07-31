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
import sys
import types
import math
import weakref

class NoneRef(object):
    def __call__(self):
        return None

class WeakMethod(object):
    def __init__(self, method):
        self._method = method.im_func
        self._object = weakref.ref(method.im_self)
    
    def __nonzero__(self):
        return self._object() is not None
        
    def __call__(self, *args):
        obj = self._object()
        if obj:
            return self._method(obj, *args)

def weakRef(object):
    ref = NoneRef()
    if object:
        if hasattr(object, "im_func"):
            ref = WeakMethod(object)
        else:
            ref = weakref.ref(object)
    
    return ref

class CallbackWithArgs(object):
    def __init__(self, callback, *args):
        self._callback = weakRef(callback)
        self._args = args

    def __call__(self, *args):
        joinedArgs = args + self._args
        self._callback(*joinedArgs)
        
def removeFromList(element, array):
    deleted = False
    if array.count(element):
        elementId = array.index(element)
        del array[elementId]
        deleted = True

    return deleted

def runtimeImport(filename):
    modulePath = os.path.dirname(filename)
    fileName = os.path.split(filename)[1]
    moduleName = fileName.split(".")[0]

    alreadyPresent = True
    if modulePath not in sys.path:
        alreadyPresent = False
        sys.path.append(modulePath)

    module = __import__(moduleName)

    if not alreadyPresent:
       sys.path.remove(modulePath)

    return module

def isModule(path):
    result = False
    files = os.listdir(path)
    for f in files:
        if f == '__init__.py':
            result = True
            break
    return result

def inspectModulePath(modulePath, returnModules):
    files = os.listdir(modulePath)
    for f in files:
        dirfile = os.path.join(modulePath, f)
        if os.path.isfile(dirfile):
            if os.path.splitext(dirfile)[1] == ".py":
                moduleName = os.path.splitext(os.path.basename(f))[0]
                returnModules.append(moduleName)
    return returnModules

def findSubModulesInModule(module):
    modulePath = os.path.dirname(module.__file__)
    subModulesNames = inspectModulePath(modulePath, [])

    __import__(module.__name__, fromlist=subModulesNames)

    returnModules = []
    for subModuleName in subModulesNames:
        if hasattr(module, subModuleName):
            subModule = getattr(module, subModuleName)
            if type(subModule).__name__ == "module":
                returnModules.append(subModule)

    return returnModules

def _findClassesRecursive(module, baseClass, collectedClasses, processedModules = []):
    processedModules.append(module)

    for attr in dir(module):
        moduleAttribute = getattr(module, attr)
        if type(moduleAttribute) is types.TypeType:
            if issubclass(moduleAttribute, baseClass):
                if moduleAttribute not in collectedClasses:
                    collectedClasses.append(moduleAttribute)

    for subModule in findSubModulesInModule(module):
        if subModule not in processedModules:
            _findClassesRecursive(subModule, baseClass, collectedClasses, processedModules)

def findClasses(module, baseClass, importedModules = None):
    foundClasses = []

    if importedModules is None:
        importedModules = []

    _findClassesRecursive(module, baseClass, foundClasses, importedModules)

    return foundClasses

def getNumber(name):
    number = ""
    for char in name:
        if char.isdigit():
            number += char

    return number

def increaseNameNumber(name):
    newName = name
    numberStr = getNumber(name)

    number = 1
    if numberStr:
        number = int(numberStr)
        number += 1
        newName = name.replace(numberStr, "")

    newName += str(number)

    return newName

def getAllParentClasses(classType):
    parentClasses = []
    parentClass = classType.__bases__
    while parentClass:
        parentClasses.append(parentClass[0])
        parentClass = parentClass[0].__bases__

    if parentClasses[-1] is object:
        parentClasses.pop(-1)

    return parentClasses
