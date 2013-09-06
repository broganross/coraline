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
import viewport


class ViewportCameraNode(Node):
    def __init__(self, name, parent):
        Node.__init__(self, name, parent)
        
        self.setClassName("ViewportCamera")

        self._modelMatrix = NumericAttribute("modelMatrix", self)
        self._fov = NumericAttribute("fov", self)
        self._zNear = NumericAttribute("zNear", self)
        self._zFar = NumericAttribute("zFar", self)
        
        self.addOutputAttribute(self._modelMatrix)
        self.addOutputAttribute(self._fov)
        self.addOutputAttribute(self._zNear)
        self.addOutputAttribute(self._zFar)
        
        self._setAttributeAllowedSpecializations(self._modelMatrix, ["Matrix44"])
        self._setAttributeAllowedSpecializations(self._fov, ["Float"])
        self._setAttributeAllowedSpecializations(self._zNear, ["Float"])
        self._setAttributeAllowedSpecializations(self._zFar, ["Float"])

        viewport.addCameraNode(self)

        self._extractViewportInfo()
    
    def __del__(self):
        viewport.removeCameraNode(self)
    
    def cameraChanged(self):
        # called by viewport.py
        # here we have to dirty each camera related attribute in order to cause an update

        self._extractViewportInfo()

        self._modelMatrix.valueChanged()
        self._fov.valueChanged()
        self._zNear.valueChanged()
        self._zFar.valueChanged()

    def _extractViewportInfo(self):
        viewports = viewport.instancedViewports()
        if viewports:
            vport = viewports[0]

            modelMatrix = vport.modelMatrix()
            self._modelMatrix.outValue().setMatrix44ValueAt(0, modelMatrix)
            
            fov = vport.fov()
            self._fov.outValue().setFloatValueAt(0, fov)

            zNear = vport.zNear()
            self._zNear.outValue().setFloatValueAt(0, zNear)

            zFar = vport.zFar()
            self._zFar.outValue().setFloatValueAt(0, zFar)