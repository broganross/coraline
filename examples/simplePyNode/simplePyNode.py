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
from coral import Node, NumericAttribute
from coral.plugin import Plugin

class SimplePyNode(Node):
    def __init__(self, name, parent):
        Node.__init__(self, name, parent)
        
        self.input1 = NumericAttribute("input1", self)
        self.input2 = NumericAttribute("input2", self)
        self.output = NumericAttribute("output", self)
        
        self.addInputAttribute(self.input1)
        self.addInputAttribute(self.input2)
        self.addOutputAttribute(self.output)
        
        self._setAttributeAffect(self.input1, self.output)
        self._setAttributeAffect(self.input2, self.output)
        
        self._setAttributeAllowedSpecializations(self.input1, ["Float"])
        self._setAttributeAllowedSpecializations(self.input2, ["Float"])
        self._setAttributeAllowedSpecializations(self.output, ["Float"])
        
    def update(self, attribute):
        value1 = self.input1.value().floatValueAt(0)
        value2 = self.input2.value().floatValueAt(0)
        
        self.output.outValue().setFloatValueAt(0, value1 + value2)

def loadPlugin():
    plugin = Plugin("simplePyNodePlugin")
    
    plugin.registerNode("SimplePyNode", SimplePyNode, tags = ["examples"], description = "Example python node")
    
    return plugin

