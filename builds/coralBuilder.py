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
import platform
import sconsUtils
import SCons

sconsUtils.importBuildEnvs()

def buildEnv():
    env = SCons.Environment.Environment()
    
    if sys.platform.startswith("win"):
        coralLib = coralLib[1]
        
    env.Append(
        CPPPATH = [
        sconsUtils.getEnvVar("CORAL_INCLUDES_PATH"),
        sconsUtils.getEnvVar("CORALUI_INCLUDES_PATH"),
        sconsUtils.getEnvVar("CORAL_PYTHON_INCLUDES_PATH"),
        sconsUtils.getEnvVar("CORAL_IMATH_INCLUDES_PATH"),
        sconsUtils.getEnvVar("CORAL_BOOST_INCLUDES_PATH")])
    
    env.Append(
        LIBS = [
        "coral",
        "coralUi",
        sconsUtils.getEnvVar("CORAL_IMATH_LIB"),
        sconsUtils.getEnvVar("CORAL_IMATH_IEX_LIB"),
        sconsUtils.getEnvVar("CORAL_PYTHON_LIB"), 
        sconsUtils.getEnvVar("CORAL_BOOST_PYTHON_LIB")])
    
    env.Append(
        LIBPATH = [
        sconsUtils.getEnvVar("CORAL_LIBS_PATH"),
        sconsUtils.getEnvVar("CORALUI_LIBS_PATH"),
        sconsUtils.getEnvVar("CORAL_IMATH_LIBS_PATH"),
        sconsUtils.getEnvVar("CORAL_PYTHON_LIBS_PATH"), 
        sconsUtils.getEnvVar("CORAL_BOOST_LIBS_PATH")])
    
    env["SHLIBPREFIX"] = ""
    env["TARGET_ARCH"] = platform.machine()
    
    if sys.platform.startswith("linux"):
        pass
    elif sys.platform == "darwin":
        env["SHLIBSUFFIX"] = ".so"
        env["FRAMEWORKS"] = ["OpenGL"]
    elif sys.platform.startswith("win"):
        env.Append(CPPPATH = sconsUtils.getEnvVar("CORAL_OPENGL_INCLUDES_PATH"))
        env.Append(LIBS = sconsUtils.getEnvVar("CORAL_OPENGL_LIB"))
        env.Append(LIBPATH = sconsUtils.getEnvVar("CORAL_OPENGL_LIBS_PATH"))
        env["SHLIBSUFFIX"] = ".pyd"
        env["CXXFLAGS"] = Split("/Zm800 -nologo /EHsc /DBOOST_PYTHON_DYNAMIC_LIB /Z7 /Od /Ob0 /GR /MD /wd4675 /Zc:forScope /Zc:wchar_t")
        env["CCFLAGS"] = "-DCORAL_UI_COMPILE"
        env["LINKFLAGS"] = ["/MANIFEST:NO"]
    
    return env

def _postBuildAction(target, source, env):
    if sys.platform == "darwin":
        # fix lib link path on Mac Os X
        dylib = str(source[0])
        sconsUtils.autoFixDynamicLinks(dylib)

def buildModule(moduleName, sourceFles, env = None):
    if env is None:
        env = buildEnv()
    
    target = env.SharedLibrary(
        target = moduleName,
        source = sourceFles,
        OBJPREFIX = os.path.join("debug", ""))
    
    postBuildTarget = env.Command("postBuildTarget", target[0], _postBuildAction)
    env.Default(postBuildTarget)
    
    return target