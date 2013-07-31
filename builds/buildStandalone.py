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
import compileall

import sconsUtils

sconsUtils.importBuildEnvs()

os.environ["CORAL_BUILD_MODE"] = "DEBUG"
if ARGUMENTS.get("mode", 0) == "release":
    print "* Building standalone app in release mode"
    os.environ["CORAL_BUILD_MODE"] = "RELEASE"
else:
    print "* Building standalone dev tree in debug mode"

buildMode = os.environ["CORAL_BUILD_MODE"]

installDir = ARGUMENTS.get("install-dir", 0)

if not installDir:
    installDir = os.path.join(os.getcwd(), "build", "coralStandaloneBuild")

print "* Install Dir = " + installDir

sdkInstallDir = ""
buildSdk = False
if buildMode == "RELEASE":
    buildSdk = True
elif ARGUMENTS.get("build-sdk", 0) == "1":
    buildSdk = True
    sdkInstallDir = ARGUMENTS.get("sdk-install-dir", 0)

    if not sdkInstallDir:
        sdkInstallDir = os.path.join(installDir, "sdk")

if buildSdk:
    print "* Build Sdk = true"
    print "* Sdk Install Dir = " + sdkInstallDir

coralLib = SConscript(os.path.join("coral", "SConstruct"))
coralUiLib = SConscript(os.path.join("coralUi", "SConstruct"), exports = {"coralLib": coralLib})
imathLib = SConscript(os.path.join("imath", "SConstruct"))

def buildSdkHeaders(buildDir):
    coralIncludesDir = os.path.join(buildDir, "coral", "includes", "coral")
    coralUiIncludesDir = os.path.join(buildDir, "coralUi", "includes", "coralUi")
    
    os.makedirs(coralIncludesDir)
    os.makedirs(coralUiIncludesDir)
    
    coralHeaders = sconsUtils.findFiles(os.path.join("coral", "src"), pattern = "*.h")
    
    for header in coralHeaders:
        file = open(header)
        fileContent = file.read()
        file.close()
        
        if "CORAL_EXPORT" in fileContent:
            shutil.copy(header, coralIncludesDir)
    
    coralUiHeaders = sconsUtils.findFiles(os.path.join("coralUi", "src"), pattern = "*.h")
    
    for header in coralUiHeaders:
        file = open(header)
        fileContent = file.read()
        file.close()
        
        if "CORALUI_EXPORT" in fileContent:
            shutil.copy(header, coralUiIncludesDir)
    
    shutil.copytree(sconsUtils.getEnvVar("CORAL_IMATH_INCLUDES_PATH"), os.path.join(buildDir, "Imath", "includes"))
    shutil.copytree(sconsUtils.getEnvVar("CORAL_BOOST_INCLUDES_PATH"), os.path.join(buildDir, "boost", "includes", "boost"))

def buildSdkLibs(coralLib, coralUiLib, buildDir):
    coralLibsDir = os.path.join(buildDir, "coral", "libs")
    coralUiLibsDir = os.path.join(buildDir, "coralUi", "libs")
    imathLibsDir = os.path.join(buildDir, "Imath", "libs")
    boostLibsDir = os.path.join(buildDir, "boost", "libs")
    
    os.makedirs(coralLibsDir)
    os.makedirs(coralUiLibsDir)
    os.makedirs(imathLibsDir)
    os.makedirs(boostLibsDir)
    
    targetCoralLibName = str(SharedLibrary("coral")[0])
    targetCoralUiLibName = str(SharedLibrary("coralUi")[0])
    
    sourceCoralLibName = os.path.split(coralLib)[-1]
    sourceCoralUiLibName = os.path.split(coralUiLib)[-1]
    
    os.symlink(os.path.join(os.pardir, os.pardir, os.pardir, "coral", sourceCoralLibName), os.path.join(coralLibsDir, targetCoralLibName))
    os.symlink(os.path.join(os.pardir, os.pardir, os.pardir, "coral", "coralUi", sourceCoralUiLibName), os.path.join(coralUiLibsDir, targetCoralUiLibName))
    
    ImathLibName = str(SharedLibrary(sconsUtils.getEnvVar("CORAL_IMATH_LIB"))[0])
    ImathIexLibName = str(SharedLibrary(sconsUtils.getEnvVar("CORAL_IMATH_IEX_LIB"))[0])
    boostPythonLibName = str(SharedLibrary(sconsUtils.getEnvVar("CORAL_BOOST_PYTHON_LIB"))[0])
    
    ImathLib = os.path.join(sconsUtils.getEnvVar("CORAL_IMATH_LIBS_PATH"), ImathLibName)
    ImathIexLib = os.path.join(sconsUtils.getEnvVar("CORAL_IMATH_LIBS_PATH"), ImathIexLibName)
    boostPythonLib = os.path.join(sconsUtils.getEnvVar("CORAL_BOOST_LIBS_PATH"), boostPythonLibName)

    shutil.copy(ImathLib, imathLibsDir)
    shutil.copy(ImathIexLib, imathLibsDir)
    shutil.copy(boostPythonLib, boostLibsDir)
    
    if sys.platform == "darwin":
        sconsUtils.autoFixDynamicLinks(os.path.join(imathLibsDir, ImathLibName))
        sconsUtils.autoFixDynamicLinks(os.path.join(imathLibsDir, ImathIexLibName))
        sconsUtils.autoFixDynamicLinks(os.path.join(boostLibsDir, boostPythonLibName))

def buildSdkTree(coralLib, coralUiLib, buildDir):
    print "* Building sdk..."
    shutil.rmtree(buildDir, ignore_errors = True)

    buildSdkHeaders(buildDir)
    buildSdkLibs(coralLib, coralUiLib, buildDir)

def buildMainTree(coralLib, coralUiLib, imathLib, buildDir):
    if sys.platform == "darwin":
        # fix lib link path on Mac Os X
        sconsUtils.autoFixDynamicLinks(coralLib)
        sconsUtils.autoFixDynamicLinks(coralUiLib)
        sconsUtils.autoFixDynamicLinks(imathLib)
    
    shutil.copytree(os.path.join("coral", "py"), buildDir)
    shutil.copy(coralLib, os.path.join(buildDir, "coral"))
    
    shutil.copy(imathLib, buildDir)
    
    shutil.copytree(os.path.join("coralUi", "py", "coralUi"), os.path.join(buildDir, "coral", "coralUi"))
    shutil.copy(coralUiLib, os.path.join(buildDir, "coral", "coralUi"))
        
    pycFiles = sconsUtils.findFiles(os.path.join(buildDir), pattern = "*.pyc")
    for pycFile in pycFiles:
        os.remove(pycFile)
    
    compileall.compile_dir(buildDir, quiet = True)
    
    if buildSdk:
       buildSdkTree(coralLib, coralUiLib, sdkInstallDir)

def buildDevTree(coralLib, coralUiLib, imathLib):
    shutil.rmtree(installDir, ignore_errors = True)
    
    buildMainTree(coralLib, coralUiLib, imathLib, installDir)
    
    
def buildOsXApp(coralLib, coralUiLib, imathLib):
    print "* building app bundle"
    
    # clean
    buildDir = os.path.join("build", "coralStandaloneApp")
    shutil.rmtree(buildDir, ignore_errors = True)
    
    # make dirs
    os.mkdir(buildDir)
    
    appBundle = os.path.join(buildDir, "CoralStandalone.app")
    os.mkdir(appBundle)
    
    contentsDir = os.path.join(appBundle, "Contents")
    os.mkdir(contentsDir)
    
    resourcesDir = os.path.join(contentsDir, "Resources")
    os.mkdir(resourcesDir)
    
    global sdkInstallDir
    sdkInstallDir = os.path.join(contentsDir, "sdk")

    buildMainTree(coralLib, coralUiLib, imathLib, os.path.join(contentsDir, "coral"))
    
    # copy stuff over
    shutil.copy(os.path.join("resources", "macAppBundle", "Info.plist"), contentsDir)
    shutil.copy(os.path.join("resources", "macAppBundle", "PkgInfo"), contentsDir)
    
    macOsDir = os.path.join(contentsDir, "MacOS")
    os.mkdir(macOsDir)
        
    shutil.copy(os.path.join("resources", "macAppBundle", "CoralStandalone"), macOsDir)
    
    for envName in os.environ.keys():
        if envName.startswith("CORAL_Qt"):
            qtLib = os.environ[envName]
            shutil.copy(qtLib, macOsDir)
    
    shutil.copy("/usr/local/lib/libboost_filesystem.dylib", macOsDir)
    shutil.copy("/usr/local/lib/libboost_regex.dylib", macOsDir)
    shutil.copy("/usr/local/lib/libboost_system.dylib", macOsDir)
    shutil.copy("/usr/local/lib/libboost_thread.dylib", macOsDir)
    shutil.copy("/usr/local/lib/libHalf.6.dylib", macOsDir)
    shutil.copy("/usr/local/lib/libIex.6.dylib", macOsDir)
    shutil.copy("/usr/local/lib/libImath.6.dylib", macOsDir)
    shutil.copy("/usr/local/lib/libIlmThread.6.dylib", macOsDir)
    shutil.copy("/usr/X11/lib/libpng15.15.dylib", macOsDir)
    shutil.copy("/usr/local/lib/libtiff.5.dylib", macOsDir)
    shutil.copy("/usr/local/lib/libjpeg.8.dylib", macOsDir)
    shutil.copy("/usr/local/lib/libIlmImf.6.dylib", macOsDir)

    glewLib = os.path.join(sconsUtils.getEnvVar("CORAL_GLEW_LIBS_PATH"), "lib" + sconsUtils.getEnvVar("CORAL_GLEW_LIB") + ".dylib")
    shutil.copy(glewLib, macOsDir)

    openImageIoLib = os.path.join(sconsUtils.getEnvVar("CORAL_OIIO_LIBS_PATH"), "lib" + sconsUtils.getEnvVar("CORAL_OIIO_LIB") + ".dylib")
    shutil.copy(openImageIoLib, macOsDir)

    tbbLib = os.path.join(sconsUtils.getEnvVar("CORAL_TBB_LIBS_PATH"), "lib" + sconsUtils.getEnvVar("CORAL_TBB_LIB") + ".dylib")
    shutil.copy(tbbLib, macOsDir)
    
    boostPythonLib = os.path.join(sconsUtils.getEnvVar("CORAL_BOOST_LIBS_PATH"), "lib" + sconsUtils.getEnvVar("CORAL_BOOST_PYTHON_LIB") + ".dylib")
    shutil.copy(boostPythonLib, macOsDir)
    
    qtUiPath = os.path.split(sconsUtils.getEnvVar("CORAL_QtGui_LIB"))[0]
    qtnib = os.path.join(qtUiPath, "Resources", "qt_menu.nib")
    shutil.copytree(qtnib, os.path.join(resourcesDir, "qt_menu.nib"))
    
    os.system("chmod +x " + os.path.join(macOsDir, "coralStandalone"))
    
    shutil.copy(os.path.join("resources", "macAppBundle", "coral.icns"), resourcesDir)
    
    pythonVersion = os.path.split(sconsUtils.getEnvVar("CORAL_PYTHON_PATH"))[-1]
    pythonFrameworkDir = os.path.join(contentsDir, "Frameworks", "Python.framework", "Versions", pythonVersion)
    shutil.copytree(sconsUtils.getEnvVar("CORAL_PYTHON_PATH"), pythonFrameworkDir)
    
    shutil.copy(os.path.join(contentsDir, "Frameworks", "Python.framework", "Versions", pythonVersion, "Resources/Python.app/Contents/MacOS/Python-64"), os.path.join(macOsDir, "python"))
    
    # fix dylib links
    libs = sconsUtils.findFiles(macOsDir, pattern = "*.dylib")
    libs.extend(sconsUtils.findFiles(macOsDir, pattern = "Qt*"))
    libs.extend(sconsUtils.findFiles(os.path.join(pythonFrameworkDir, "lib", "python" + pythonVersion, "site-packages", "PyQt4"), pattern = "*.so"))
    
    for lib in libs:
        sconsUtils.autoFixDynamicLinks(lib)
    
def buildApp(coralLib, coralUiLib, imathLib):
    if sys.platform == "darwin":
        buildOsXApp(coralLib, coralUiLib, imathLib)

def postBuildAction(target, source, env):
    coralLib = str(source[0])
    coralUiLib = str(source[1])
    imathLib = str(source[2])
    
    if buildMode == "RELEASE":
        buildApp(coralLib, coralUiLib, imathLib)
    else:
        buildDevTree(coralLib, coralUiLib, imathLib)

postBuildTarget = Command("postBuildTarget", [coralLib[0], coralUiLib[0], imathLib[0]], postBuildAction)
Default(postBuildTarget)


    

