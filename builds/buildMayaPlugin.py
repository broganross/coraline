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


import sys
import os
import shutil

import sconsUtils

os.environ["CORAL_BUILD_FLAVOUR"] = "coralMaya"
os.environ["CORAL_BUILD_MODE"] = "DEBUG"

sconsUtils.importBuildEnvs()

coralLib = SConscript(os.path.join("coral", "SConstruct"))
coralUiLib = SConscript(os.path.join("coralUi", "SConstruct"), exports = {"coralLib": coralLib})
imathLib = SConscript(os.path.join("imath", "SConstruct"))
coralMaya = SConscript(os.path.join("coralMaya", "SConstruct"), exports = {"coralLib": coralLib})
mayaPlugin = SConscript(os.path.join("coralMaya", "plugin", "SConstruct"), exports = {"coralLib": coralLib, "coralUiLib": coralUiLib, "coralMayaLib": coralMaya})

def postBuildAction(target, source, env):
    coralLib = str(source[0])
    coralUiLib = str(source[1])
    imathLib = str(source[2])
    mayaPlugin = str(source[3])
    coralMaya = str(source[4])
    
    buildDir = os.path.join("build", "coralMayaBuild")
    
    shutil.rmtree(buildDir, ignore_errors = True)
    shutil.copytree(os.path.join("coralMaya", "py"), os.path.join(buildDir, "coralMaya"))
    shutil.copytree(os.path.join("coral", "py", "coral"), os.path.join(buildDir, "coral"))
    shutil.copy(coralLib, os.path.join(buildDir, "coral"))
    shutil.copy(coralMaya, os.path.join(buildDir, "coralMaya"))
    shutil.copy(mayaPlugin, os.path.join(buildDir, "coralMaya"))
    shutil.copy(imathLib, buildDir)
    
    shutil.copytree(os.path.join("coralUi", "py", "coralUi"), os.path.join(buildDir, "coral", "coralUi"))
    shutil.copy(coralUiLib, os.path.join(buildDir, "coral", "coralUi"))
    
    if sys.platform == "darwin":
        # fix lib link path on Mac Os X
        sconsUtils.autoFixDynamicLinks(os.path.join(buildDir, "coralMaya", "coralMayaPlugin.bundle"))
        sconsUtils.autoFixDynamicLinks(os.path.join(buildDir, "coral", "_coral.so"))
        sconsUtils.autoFixDynamicLinks(os.path.join(buildDir, "coral", "coralUi", "_coralUi.so"))
        sconsUtils.autoFixDynamicLinks(os.path.join(buildDir, "coralMaya", "_coralMaya.so"))
        
    pycFiles = sconsUtils.findFiles(os.path.join(buildDir), pattern = "*.pyc")
    for pycFile in pycFiles:
        os.remove(pycFile)

posBuildTarget = Command("postBuildTarget", [coralLib[0], coralUiLib[0], imathLib[0], mayaPlugin[0], coralMaya[0]], postBuildAction)
Default(posBuildTarget)
