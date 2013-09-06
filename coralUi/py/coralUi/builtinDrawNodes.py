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


from ..plugin import Plugin

def loadPlugin():
    plugin = Plugin("builtinDrawNodes")
    
    import _coralUi
    import viewportCameraNode

    plugin.registerNode("DrawGeo", _coralUi.GeoDrawNode, tags = ["draw"])
    plugin.registerNode("DrawPoint", _coralUi.DrawPointNode, tags = ["draw"])
    plugin.registerNode("DrawLine", _coralUi.DrawLineNode, tags = ["draw"])
    plugin.registerNode("DrawMatrix", _coralUi.DrawMatrixNode, tags = ["draw"])
    plugin.registerNode("DrawGeoInstance", _coralUi.DrawGeoInstance, tags = ["draw"])
    plugin.registerNode("ViewportCamera", viewportCameraNode.ViewportCameraNode, tags = ["render"], description = "A node to extract informations from the viewport camera.")
    plugin.registerNode("Shader", _coralUi.ShaderNode, tags = ["draw"])

    return plugin
