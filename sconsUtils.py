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
import fnmatch
import string

if os.environ.has_key("CORAL_BUILD_FLAVOUR") == False:
    os.environ["CORAL_BUILD_FLAVOUR"] = ""

def findFiles(root, recurse = True, pattern = '*.*', return_folders = False):
    result = []

    try:
        names = os.listdir(root)
    except os.error:
        return result

    pattern = pattern or '*'
    pat_list = string.splitfields( pattern , ';' )

    for name in names:
        fullname = os.path.normpath(os.path.join(root, name))

        for pat in pat_list:
            if fnmatch.fnmatch(name, pat):
                if os.path.isfile(fullname) or (return_folders and os.path.isdir(fullname)):
                    result.append(fullname)
                continue

        if recurse:
            if os.path.isdir(fullname) and not os.path.islink(fullname):
                result = result + findFiles( fullname, recurse, pattern, return_folders )

    return result

def autoFixDynamicLinks(lib, verbose = False):
    f = os.popen('otool -L "%s"'%(lib,))
    entries = f.read().split('\n')[1:]
    for entry in entries:
        oldLibName = entry.split(" ")[0].strip()
        if not fnmatch.fnmatch(oldLibName, "/System/Library/Frameworks/*"): #exclude system frameworks
            newLibName = entry.split(" ")[0].split("/")[-1].strip()
            if verbose:
                print "Changing lib path:", oldLibName, "from:", newLibName, "to:", lib
            
            os.system('install_name_tool -change "' + oldLibName + '" "' + newLibName + '" "' + lib + '"')
    
    if verbose:
        print "- changeDynamicLink result-------"
        print os.system("otool -L %s" %(lib))
        print "---------------------------------"
        
def changeDynamicLink(lib, old_path_prefix, new_path_prefix, quiet = False):
    f = os.popen('otool -L "%s"'%(lib,))
    x = f.read().split('\n')[1:]
    for l in x:
        if l.count(old_path_prefix):
            ol = l[1:].split(' ')[0]
            nw = new_path_prefix + ol.split(old_path_prefix)[-1]
            
            if not quiet:
                print "Changing lib path:", lib, "from:", ol, "to:", nw
        
            os.system('install_name_tool -change %s %s %s' %(ol,nw,lib))
            
    if not quiet:
        print "- changeDynamicLink result-------"
        print os.system("otool -L %s" %(lib))
        print "---------------------------------"

tryedImportingBuildEnv = False
def importBuildEnvs():
    global tryedImportingBuildEnv
    
    if "buildEnv" not in sys.modules.keys():
        try:
            import buildEnv
            print "* Coral Build: imported buildEnv.py"
        except:
            if tryedImportingBuildEnv == False:
                print "Coral Build: Tried importing buildEnv.py but failed."
                tryedImportingBuildEnv = True

def getEnvVar(varName):
    var = ""
    if varName in os.environ:
        var = os.environ[varName]
    else:
        raise Exception("Could not find env var: " + str(varName))
    
    return var
