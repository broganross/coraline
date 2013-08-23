""" Nodes for executing file operations """

import  fnmatch
import  shutil
import  os

from    coral   import  coralApp
from    coral   import  StringAttribute
from    coral   import  coralApp
from    coral   import  Node

# from    coral.attributes    import  iteratorattribute
# from    coral.nodes         import  pipelinenode


class CopyFilesNode(Node):
    def __init__(self, name, parent):
        super(CopyFilesNode, self).__init__(name, parent)
        self._inFiles = StringAttribute("input", self)
        self._outdir = StringAttribute("outDir", self)
        self._outfiles = StringAttribute("output", self)
        self._setAttributeAllowedSpecializations(self._inFiles, ["PathArray"])
        self._setAttributeAllowedSpecializations(self._outdir, ["PathArray"])
        self.addInputAttribute(self._inFiles)
        self.addInputAttribute(self._outdir)
        self.addOutputAttribute(self._outfiles)
        self._setAttributeAffect(self._inFiles, self._outfiles)
        self._setAttributeAffect(self._outdir, self._outfiles)
        self.initDone()

    def _copyFiles(self, inData, outData):
        for fromPath in inData["files"]:
            toPath = os.path.join(inData["outdir"],
                                  os.path.basename(fromPath))
            shutil.copy2(fromPath, toPath)
            outData.append(toPath)

    def update(self, attribute):
        inp = self._inFiles.value().list()
        outdir = self._outdir.value().stringValue()
        indata = {}
        indata["files"] = inp
        indata["outdir"] = outdir
        out = []
        self.enqueueTask(self._copyFiles, indata, out)
        self.processTassk()
        self._outfiles.outValue().setList(out)
        self.setLog("\n".join(out))
      

class FindFilesNode(Node):
    """ This node uses fnmatch to find matching file paths in the given folder
        @ivar _location: base folder to search in
        @ivar _pattern: fnmatch pattern to search for
        @ivar _files: all files found
    """
    def __init__(self, name, parent):
        super(FindFilesNode, self).__init__(name, parent)

        self._location = StringAttribute("location", self)
        self._location.setSpecializationOverride("Path")
        self._pattern = StringAttribute("pattern", self)
        self._files = StringAttribute("files", self)
        self._setAttributeAllowedSpecializations(self._location, ["Path"])
        self._setAttributeAllowedSpecializations(self._pattern, ["String"])
        self._setAttributeAllowedSpecializations(self._files, ["PathArray"])

        self.addInputAttribute(self._location)
        self.addInputAttribute(self._pattern)
        self.addOutputAttribute(self._files)

        self._setAttributeAffect(self._location, self._files)
        self._setAttributeAffect(self._pattern, self._files)

        self._setSliceable(True)
    
    def updateSlice(self, attribute, slice):
        location = self._location.value().stringValueAt(1)
        pattern = self._pattern.value().stringValueAt(1)

        assets = []
        if not os.path.isdir(location):
            coralApp.logError("Location does not exist.")
        filenames = os.listdir(location)
        for filename in filenames:
            if fnmatch.fnmatch(filename, pattern):
                fullPath = os.path.join(location, filename)
                if os.path.isfile(fullPath):
                    assets.append(fullPath)

        self._files.outValue().setStringValuesAtSlice(slice, assets)

#         if assets:
#             self.setLog("Found files:\n" + "\n".join(assets))
    

class MoveFilesNode(Node):
    def __init__(self, name, parent):
        super(MoveFilesNode, self).__init__(name, parent)
        self._inFiles = iteratorattribute.ListAttribute("input", self)
        self._outdir = StringAttribute("outDir", self)
        self._outfiles = iteratorattribute.ListAttribute("output", self)

        self._setAttributeAllowedSpecializations(self._outdir, ["Path"])

        self.addInputAttribute(self._inFiles)
        self.addInputAttribute(self._outdir)
        self.addOutputAttribute(self._outfiles)
        
        self._setAttributeAffect(self._inFiles, self._outfiles)
        self._setAttributeAffect(self._outdir, self._outfiles)
        self.initDone()
        
    def _moveFiles(self, inData, out):
        for fromPath in inData["files"]:
            toPath = os.path.join(inData["outdir"],
                                  os.path.basename(fromPath))
            shutil.move(fromPath, toPath)
            out.append(toPath)
        
    def update(self, attribute):
        inp = self._inFiles.value().list()
        outdir = self._outdir.value().stringValue()
        indata = {}
        indata["files"] = inp
        indata["outdir"] = outdir
        out = []
        self.enqueueTask(self._moveFiles, indata, out)
        self.processTasks()
        self._outfiles.outValue().setList(out)
        self.setLog("\n".join(out))        

    def process(self):
        self._outfiles.value().list()



