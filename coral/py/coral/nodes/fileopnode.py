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
        self._filePairs = [] # list of tuples, (fromPath, toPath)
        self._from = StringAttribute("from", self)
        self._to = StringAttribute("to", self)
        self._out = StringAttribute("output", self)
        self._setAttributeAllowedSpecializations(self._from, ["Path", "PathArray"])
        self._setAttributeAllowedSpecializations(self._to, ["Path", "PathArray"])
        self._setAttributeAllowedSpecializations(self._out, ["Path", "PathArray"])

        self.addInputAttribute(self._from)
        self.addInputAttribute(self._to)
        self.addOutputAttribute(self._out)
        self._setAttributeAffect(self._from, self._out)
        self._setAttributeAffect(self._to, self._out)
        
        self._addAttributeSpecializationLink(self._from, self._to)
        self._addAttributeSpecializationLink(self._from, self._out)
        
        self.initDone()

    def _copyFiles(self, inData, outData):
        """ Do the actual file copy """
        outPaths = []
        for fromPath, toPath in self._filePairs:
            print "from", fromPath
            print "to  ", toPath
#             shutil.copy2(fromPath, toPath)
            outPaths.append(toPath)
        self._outfiles.outValue().setPathValues(outPaths)

    def update(self, attribute):
        self._filePairs = []
        if self._from.specialization() == "Path":
            if self._to.specialization() == "Path":
                fromPath = self._from.value().pathValueAt(0)
                toPath = self._to.value().pathValueAt(0)
                if os.path.isdir(toPath):
                    toPath = os.path.join(toPath, os.path.basename(fromPath))
                self._filePairs = [(fromPath, toPath)]
        elif self._from.specialization() == "PathArray":
            if self._to.specialization() == "Path":
                fromPaths = self._from.value().pathValues()
                toBasePath = self._to.value().pathValuesAt(0)
                if os.path.isfile(toBasePath):
                    raise ValueError("Can't copy multiple files to one file.  Use directory instead")
                for path in enumerate(fromPaths):
                    toPath = os.path.join(toBasePath, os.path.basename(path))
                    self._filePairs.append((path, toPath))
            elif self._to.specialization() == "PathArray":
                fromPaths = self._from.value().pathValues()
                toPaths = self._to.value().pathValues()
                if len(fromPaths) != len(toPaths):
                    raise ValueError("number of paths doesn't match.")
                for num, fromPath in enumerate(fromPaths):
                    self._filePairs.append(fromPath, toPaths[num])
                    
        out = [tp for fp, tp in self._filePairs]
        self._out.outValue().setPathValues(out)
        self.addProcToQueue(self._copyFiles, None, None)

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



