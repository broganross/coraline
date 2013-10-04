""" Nodes for executing file operations """

import  fnmatch
import  logging
import  shutil
import  os

from    coral   import  nodes
from    coral   import  coralApp
from    coral   import  BoolAttribute
from    coral   import  EnumAttribute
from    coral   import  StringAttribute
from    coral   import  coralApp
from    coral   import  Node


logger = logging.getLogger("coraline")


class CopyFilesNode(nodes.ExecutableNode):
    def __init__(self, name, parent):
        super(CopyFilesNode, self).__init__(name, parent)
        self.setClassName("CopyFilesNode")
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
            logger.info("Copying from: %s\n\tTo: %s"%(fromPath, toPath))
            shutil.copy2(fromPath, toPath)
            outPaths.append(toPath)
        self._out.outValue().setPathValues(outPaths)

    def update(self, attribute):
        logger.debug("update")
        self._filePairs = []
        if self._from.specialization() == ["Path"]:
            if self._to.specialization() == ["Path"]:
                fromPath = self._from.value().pathValueAt(0)
                toPath = self._to.value().pathValueAt(0)
                if os.path.isdir(toPath):
                    toPath = os.path.join(toPath, os.path.basename(fromPath))
                self._filePairs = [(fromPath, toPath)]
            else:
                raise ValueError("Can't copy one file to multiple locations... yet.")

        elif self._from.specialization() == ["PathArray"]:
            if self._to.specialization() == ["Path"]:
                fromPaths = self._from.value().pathValues()
                toBasePath = self._to.value().pathValuesAt(0)
                if os.path.isfile(toBasePath):
                    raise ValueError("Can't copy multiple files to one file.  Use directory instead")
                for path in enumerate(fromPaths):
                    toPath = os.path.join(toBasePath, os.path.basename(path))
                    self._filePairs.append((path, toPath))
            elif self._to.specialization() == ["PathArray"]:
                fromPaths = self._from.value().pathValues()
                toPaths = self._to.value().pathValues()
                if len(fromPaths) != len(toPaths):
                    raise ValueError("number of paths doesn't match.")
                for num, fromPath in enumerate(fromPaths):
                    self._filePairs.append(fromPath, toPaths[num])
                    
        out = [tp for fp, tp in self._filePairs]
        self._out.outValue().setPathValues(out)
        self.addProcToQueue(self._copyFiles, None, None)
        logger.debug("update: complete")

    def process(self):
        self.run()
        self._out.value().pathValues()


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
        self._from = StringAttribute("input", self)
        self._out = StringAttribute("outDir", self)
        self._outfiles = StringAttribute("output", self)

        self._setAttributeAllowedSpecializations(self._from, ["PathArray", "Path"])
        self._setAttributeAllowedSpecializations(self._to, ["Path", "PathArray"])
        self._setAttributeAllowedSpecializations(self._outfiles, ["PathArray"])

        self.addInputAttribute(self._from)
        self.addInputAttribute(self._to)
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
        self._filePair = []
        inp = self._inFiles.value().stringValues()
        outdir = self._outdir.value().stringValueAt(0)
        indata = {}
        indata["files"] = inp
        indata["outDir"] = outdir
        self.addProcToQueue(self._moveFiles, indata, [])

    def process(self):
        self.run()
        self._outfiles.value()


class StringSort(Node):
    def __init__(self, name, parent):
        super(StringSort, self).__init__(name, parent)
        self._setSliceable(True)

        self._input = StringAttribute("input", self)
        self._key = StringAttribute("key", self)
        self._output = StringAttribute("output", self)

        self._setAttributeAllowedSpecializations(self._input, ["StringArray", "PathArray"])
        self._setAttributeAllowedSpecializations(self._key, ["String"])
        self._setAttributeAllowedSpecializations(self._output, ["StringArray", "PathArray"])

        self.addInputAttribute(self._input)
        self.addInputAttribute(self._key)
        self.addOutputAttribute(self._output)

        self._setAttributeAffect(self._input, self._output)
        self._setAttributeAffect(self._key, self._output)
        self._addAttributeSpecializationLink(self._input, self._output)

    def updateSlice(self, attribute, slice):
        inp = self._input.value().stringValuesSlice(slice)
        key = self._key.value().stringValues()[0]
        out = []
        if key:
            out = sorted(inp, key=eval(key))
        else:
            out = sorted(inp)
        self._output.outValue().setStringValuesSlice(slice, out)


class PathNode(Node):
    """Node for doing different pathing operations
    """
    def __init__(self, name, parent):
        super(PathNode, self).__init__(name, parent)
        self._setSliceable(True)

        self._inPath = StringAttribute("input", self)
        self._op = EnumAttribute("operation", self)
        self._out = StringAttribute("output", self)

        self._allOps = [("split", os.path.split, "StringArray"),
                        ("splitAll", None, "StringArray"),
                        ("dirname", os.path.dirname, "Path"),
                        ("basename", os.path.basename, "String"),
                        ("splitext", os.path.splitext, "StringArray")
                        ]

        for n, op in enumerate(self._allOps):
            self._op.value().addEntry(n, op[0])

        self._setAttributeAllowedSpecializations(self._inPath, ["Path", "PathArray"])
        self._setAttributeAllowedSpecializations(self._out, ["String", "Path", "StringArray"])

        self.addInputAttribute(self._inPath)
        self.addInputAttribute(self._op)
        self.addOutputAttribute(self._out)

        self._setAttributeAffect(self._inPath, self._out)
        self._setAttributeAffect(self._op, self._out)

        # self._catchAttributeDirtied(self._inPath, True)
        self._catchAttributeDirtied(self._op, True)

    def attributeDirtied(self, attribute):
        if attribute == self._op:
            index = self._op.value().currentIndex()
            self._out._setSpecialization([self._allOps[index][2]])

    def updateSlice(self, attribute, slice):
        logger.debug("PathNode - updateSlice")
        inPaths = self._inPath.value().stringValuesSlice(slice)
        opIndex = self._op.value().currentIndex()
        for ip in inPaths:
            if ip == "":
                continue

            out = []
            if self._allOps[opIndex][1] == None:
                if self._allOps[opIndex][0] == "splitAll":
                    out = ip.split(os.sep)
            else:
                op = self._allOps[opIndex]
                out = op[1](ip)

            if isinstance(out, tuple):
                out = list(out)

            if not isinstance(out, list):
                out = [out]

            self._out.outValue().setPathValuesSlice(slice, out)

        logger.debug("PathNode - updateSlice: complete")