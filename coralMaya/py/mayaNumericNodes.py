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

from coralMayaNode import CoralMayaNode
import _coralMaya
            
class MayaFloatInput(CoralMayaNode):
    def __init__(self, name, parent):
        CoralMayaNode.__init__(self, name, parent)
        self.setClassName("MayaFloatInput")
        
        if self.isValid():
            self._value = _coralMaya.MayaFloatAttribute("value", self)
            self.addOutputAttribute(self._value)
        
            mayaAttr = self.createMayaAttribute(CoralMayaNode.floatAttributeType) 
            self.associateMayaAttribute(mayaAttr, self._value)
        
class MayaFloatOutput(CoralMayaNode):
    def __init__(self, name, parent):
        CoralMayaNode.__init__(self, name, parent)
        self.setClassName("MayaFloatOutput")
        
        if self.isValid():
            self._value = _coralMaya.MayaFloatAttribute("value", self)
            self.addInputAttribute(self._value)
        
            mayaAttr = self.createMayaAttribute(CoralMayaNode.floatAttributeType) 
            self.associateMayaAttribute(mayaAttr, self._value)

class MayaIntInput(CoralMayaNode):
    def __init__(self, name, parent):
        CoralMayaNode.__init__(self, name, parent)
        self.setClassName("MayaIntInput")
        
        if self.isValid():
            self._value = _coralMaya.MayaIntAttribute("value", self)
            self.addOutputAttribute(self._value)
        
            mayaAttr = self.createMayaAttribute(CoralMayaNode.intAttributeType) 
            self.associateMayaAttribute(mayaAttr, self._value)
        
class MayaIntOutput(CoralMayaNode):
    def __init__(self, name, parent):
        CoralMayaNode.__init__(self, name, parent)
        self.setClassName("MayaIntOutput")
        
        if self.isValid():
            self._value = _coralMaya.MayaIntAttribute("value", self)
            self.addInputAttribute(self._value)
        
            mayaAttr = self.createMayaAttribute(CoralMayaNode.intAttributeType) 
            self.associateMayaAttribute(mayaAttr, self._value)

class MayaMatrixInput(CoralMayaNode):
    def __init__(self, name, parent):
        CoralMayaNode.__init__(self, name, parent)
        self.setClassName("MayaMatrixInput")
        
        if self.isValid():
            self._value = _coralMaya.MayaMatrixAttribute("value", self)
            self.addOutputAttribute(self._value)
            
            mayaAttr = self.createMayaAttribute(CoralMayaNode.matrixAttributeType)
            self.associateMayaAttribute(mayaAttr, self._value)

class MayaMatrixOutput(CoralMayaNode):
    def __init__(self, name, parent):
        CoralMayaNode.__init__(self, name, parent)
        self.setClassName("MayaMatrixOutput")
        
        if self.isValid():
            self._value = _coralMaya.MayaMatrixAttribute("value", self)
            self.addInputAttribute(self._value)
            
            mayaAttr = self.createMayaAttribute(CoralMayaNode.matrixAttributeType)
            self.associateMayaAttribute(mayaAttr, self._value)
    
class MayaAngleInput(CoralMayaNode):
    def __init__(self, name, parent):
        CoralMayaNode.__init__(self, name, parent)
        self.setClassName("MayaAngleInput")
        
        if self.isValid():
            self._value = _coralMaya.MayaAngleAttribute("value", self)
            self.addOutputAttribute(self._value)
        
            mayaAttr = self.createMayaAttribute(CoralMayaNode.angleAttributeType) 
            self.associateMayaAttribute(mayaAttr, self._value)
        
class MayaAngleOutput(CoralMayaNode):
    def __init__(self, name, parent):
        CoralMayaNode.__init__(self, name, parent)
        self.setClassName("MayaAngleOutput")
        
        if self.isValid():
            self._value = _coralMaya.MayaAngleAttribute("value", self)
            self.addOutputAttribute(self._value)
        
            mayaAttr = self.createMayaAttribute(CoralMayaNode.angleAttributeType) 
            self.associateMayaAttribute(mayaAttr, self._value)

class MayaAngle3ArrayOutput(CoralMayaNode):
    def __init__(self, name, parent):
        CoralMayaNode.__init__(self, name, parent)
        self.setClassName("MayaAngle3ArrayOutput")
        
        if self.isValid():
            self._value = _coralMaya.MayaAngle3ArrayAttribute("value", self)
            self.addInputAttribute(self._value)
        
            mayaAttr = self.createMayaAttribute(CoralMayaNode.angle3ArrayAttributeType) 
            self.associateMayaAttribute(mayaAttr, self._value)

class MayaFloat3ArrayOutput(CoralMayaNode):
    def __init__(self, name, parent):
        CoralMayaNode.__init__(self, name, parent)
        self.setClassName("MayaFloat3ArrayOutput")
        
        if self.isValid():
            self._value = _coralMaya.MayaFloat3ArrayAttribute("value", self)
            self.addInputAttribute(self._value)
        
            mayaAttr = self.createMayaAttribute(CoralMayaNode.float3ArrayAttributeType) 
            self.associateMayaAttribute(mayaAttr, self._value)

