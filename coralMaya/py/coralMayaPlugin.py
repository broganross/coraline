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

from maya import cmds

from coral.plugin import Plugin
from mayaContextNode import MayaContextNode
import _coralMaya
import mayaNumericNodes
import mayaGeoNodes

def loadPlugin():
    plugin = Plugin("coralMaya")
    
    plugin.registerAttribute("MayaIntAttribute", _coralMaya.MayaIntAttribute)
    plugin.registerAttribute("MayaFloatAttribute", _coralMaya.MayaFloatAttribute)
    plugin.registerAttribute("MayaMatrixAttribute", _coralMaya.MayaMatrixAttribute)
    plugin.registerAttribute("MayaGeoAttribute", _coralMaya.MayaGeoAttribute)
    
    plugin.registerNode("MayaContext", MayaContextNode, tags = ["maya"])
    plugin.registerNode("MayaFloatInput", mayaNumericNodes.MayaFloatInput, tags = ["maya"])
    plugin.registerNode("MayaFloatOutput", mayaNumericNodes.MayaFloatOutput, tags = ["maya"])
    plugin.registerNode("MayaIntInput", mayaNumericNodes.MayaIntInput, tags = ["maya"])
    plugin.registerNode("MayaIntOutput", mayaNumericNodes.MayaIntOutput, tags = ["maya"])
    plugin.registerNode("MayaMatrixInput", mayaNumericNodes.MayaMatrixInput, tags = ["maya"])
    plugin.registerNode("MayaMatrixOutput", mayaNumericNodes.MayaMatrixOutput, tags = ["maya"])
    plugin.registerNode("MayaAngleInput", mayaNumericNodes.MayaAngleInput, tags = ["maya"])
    plugin.registerNode("MayaAngleOutput", mayaNumericNodes.MayaAngleOutput, tags = ["maya"])
    plugin.registerNode("MayaAngle3ArrayOutput", mayaNumericNodes.MayaAngle3ArrayOutput, tags = ["maya"])
    plugin.registerNode("MayaFloat3ArrayOutput", mayaNumericNodes.MayaFloat3ArrayOutput, tags = ["maya"])
    plugin.registerNode("MayaGeoInput", mayaGeoNodes.MayaGeoInput, tags = ["maya"])
    plugin.registerNode("MayaGeoOutput", mayaGeoNodes.MayaGeoOutput, tags = ["maya"])
    
    mayaPluginPath = os.path.dirname(__file__)
    cmds.loadPlugin(os.path.join(mayaPluginPath, "coralMayaPlugin"))
    
    return plugin
