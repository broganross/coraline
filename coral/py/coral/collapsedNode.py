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


import _coral

## A node to enclose other nodes and provide a simplified interface to a more complex network. 
#@implements Node
class CollapsedNode(_coral.Node):
    def __init__(self, name, parent):
        _coral.Node.__init__(self, name, parent)
        self.setClassName("CollapsedNode")
        
        self._setSliceable(True)
        self._setUpdateEnabled(False)
        self._setAllowDynamicAttributes(True)

    def _attributesAsScript(self):
        script = ""
        
        createAttributeCmd = _coral.Command()
        createAttributeCmd.setName("CreateAttribute")
        createAttributeCmd.setArgString("className", "PassThroughAttribute")
        createAttributeCmd.setArgString("name", "")
        createAttributeCmd.setArgString("parentNode", self.fullName())
        createAttributeCmd.setArgBool("input", False)
        createAttributeCmd.setArgBool("output", False)
        
        for attribute in self.inputAttributes():
            createAttributeCmd.setArgString("name", attribute.name())
            createAttributeCmd.setArgBool("input", True)
            createAttributeCmd.setArgBool("output", False)
            
            script += createAttributeCmd.asScript() + "\n"
        
        for attribute in self.outputAttributes():
            createAttributeCmd.setArgString("name", attribute.name())
            createAttributeCmd.setArgBool("input", False)
            createAttributeCmd.setArgBool("output", True)
            
            script += createAttributeCmd.asScript() + "\n"
        
        return script