""" Nodes for executing file operations """

import  fnmatch
import  shutil
import  os

from    coral   import  nodes
from    coral   import  coralApp
from    coral   import  BoolAttribute
from    coral   import  StringAttribute
from    coral   import  coralApp
from    coral   import  Node


# from    coral.attributes    import  iteratorattribute
# from    coral.nodes         import  pipelinenode


class CopyFilesNode(nodes.ExecutableNode):
    def __init__(self, name, parent):
        super(CopyFilesNode, self).__init__(name, parent)
        self._inFiles = StringAttribute("input", self)
        self._outdir = StringAttribute("outDir", self)
        self._outfiles = StringAttribute("output", self)
        self._setAttributeAllowedSpecializations(self._inFiles, ["PathArray"])
        self._setAttributeAllowedSpecializations(self._outdir, ["Path"])
        self._setAttributeAllowedSpecializations(self._outfiles, ["PathArray"])

        self.addInputAttribute(self._inFiles)
        self.addInputAttribute(self._outdir)
        self.addOutputAttribute(self._outfiles)
        self._setAttributeAffect(self._inFiles, self._outfiles)
        self._setAttributeAffect(self._outdir, self._outfiles)
        self.initDone()

    def _copyFiles(self, inData, outData):
        """ Do the actual file copy """
        for fromPath in inData["inFiles"]:
            toPath = os.path.join(inData["outDir"], os.path.basename(fromPath))
            if not os.path.isfile(fromPath):
                coralApp.logError("File doesn't exist: %s"%fromPath)
                self.setLog("File doesn't exist: %s"%fromPath)
                return
            elif not os.path.exists(inData["outDir"]):
                coralApp.logError("Folder doesn't exist: %s"%inData["outDir"])
                self.setLog("Folder doesn't exist: %s"%inData["outDir"])
                return
            else:
                shutil.copy2(fromPath, toPath)
                outData.append(toPath)
        self._outfiles.outValue().setPathValues(outData)

    def update(self, attribute):
        inp = self._inFiles.value().pathValues()
        outdir = self._outdir.value().pathValueAt(0)
        inData = {"inFiles" : inp,
                  "outDir" : outdir}
        out = []
        self.addProcToQueue(self._copyFiles, inData, out)

    def process(self):
        self.run()
        self._outfiles.value().pathValues()


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
    
    def update(self, attribute):
        coralApp.logDebug("FindFiles.update")
        location = self._location.value().stringValueAt(0)
        pattern = self._pattern.value().stringValueAt(0)

        assets = []
        if location == "":
            return
        if not os.path.isdir(location):
            coralApp.logError("Location does not exist.")
            return
        filenames = os.listdir(location)
        for filename in filenames:
            if fnmatch.fnmatch(filename, pattern):
                fullPath = os.path.join(location, filename)
                if os.path.isfile(fullPath):
                    assets.append(fullPath)

        self._files.outValue().setPathValues(assets)
        coralApp.logDebug("FindFiles.update: Done")
#         if assets:
#             self.setLog("Found files:\n" + "\n".join(assets))
    

class MoveFilesNode(nodes.ExecutableNode):
    def __init__(self, name, parent):
        super(MoveFilesNode, self).__init__(name, parent)
        self._inFiles = StringAttribute("input", self)
        self._outdir = StringAttribute("outDir", self)
        self._outfiles = StringAttribute("output", self)

        self._setAttributeAllowedSpecializations(self._inFiles, ["PathArray"])
        self._setAttributeAllowedSpecializations(self._outdir, ["Path"])
        self._setAttributeAllowedSpecializations(self._outfiles, ["PathArray"])

        self.addInputAttribute(self._inFiles)
        self.addInputAttribute(self._outdir)
        self.addOutputAttribute(self._outfiles)
        
        self._setAttributeAffect(self._inFiles, self._outfiles)
        self._setAttributeAffect(self._outdir, self._outfiles)
        self.initDone()
        
    def _moveFiles(self, inData, out):
        for fromPath in inData["files"]:
            toPath = os.path.join(inData["outDir"],
                                  os.path.basename(fromPath))
            if not os.path.isfile(fromPath):
                coralApp.logError("File doesn't exist: %s"%fromPath)
                self.setLog("File doesn't exist: %s"%fromPath)
                return
            elif not os.path.exists(inData["outDir"]):
                coralApp.logError("Folder doesn't exist: %s"%inData["outDir"])
                self.setLog("Folder doesn't exist: %s"%inData["outDir"])
                return
            else:
                shutil.move(fromPath, toPath)
                out.append(toPath)
        self._outfiles.outValue().setPathValues(out)
        
    def update(self, attribute):
        inp = self._inFiles.value().stringValues()
        outdir = self._outdir.value().stringValueAt(0)
        indata = {}
        indata["files"] = inp
        indata["outDir"] = outdir
        self.addProcToQueue(self._moveFiles, indata, [])

    def process(self):
        self.run()
        self._outfiles.value()



