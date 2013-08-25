""" Base implementation for nodes
"""

import  re
import  thread
import  weakref

from    coral   import  coralApp
from    coral   import  Node
from    coral   import  StringAttribute
from    coral   import  EnumAttribute


class ExecutableNode(Node):
    """ Base executable node """
    STATES  =   ["dirty", "queued", "processing", "clean"]
    def __init__(self, name, parent):
        super(ExecutableNode, self).__init__(name, parent)
        self.setClassName("ExecutableNode")

        self._procQueue = []
        self._log = ""
        self._nodeUi = None
        self._state = 0

    def addProcToQueue(self, func, args, out):
        self._procQueue.append({"func" : func, "args" : args, "out" : out})

    def attributeDirtied(self, attribute):
        self.setLog("ready to process.")
        self._state = 0
        if self._nodeUi:
            self._nodeUi().executableNodeChanged()

    def initDone(self):
        for outAttr in self.outputAttributes():
            self._catchAttributeDirtied(outAttr, True)

    def log(self):
        return self._log

    def processing(self):
        return self._state == 2

    def progressMessage(self):
        return self.STATES[self._state]

    def reset(self):
        for outAttr in self.outputAttributes():
            outAttr.valueChanged()

    def run(self):
        """ Execute each function in the queue, should be run in a seperate thread """
        self._state = 2

        while len(self._procQueue) > 0:
            self.setLog("tasks left: " + str(len(self._procQueue)))
            proc = self._procQueue.pop(0)
            proc["func"](proc["args"], proc["out"])
#             thread.start_new_thread(proc["func"], (proc["args"], proc["out"]))
        self._state = 3
        self.setLog("processing done!")

    def setLog(self, log):
        self._log = log
    
    def setNodeUi(self, nodeUi):
        self._nodeUi = weakref.ref(nodeUi)

    def setQueued(self):
        self._state = 1


class RegexNode(Node):
    def __init__(self, name, parent):
        super(RegexNode, self).__init__(name, parent)
        self._setSliceable(True)
        self._instr = StringAttribute("input", self)
        self._pattern = StringAttribute("pattern", self)
        self._type = EnumAttribute("type", self)
        self._rep = StringAttribute("replace", self)
        self._out = StringAttribute("out", self)
        
        for n, t in enumerate(["search", "replace", "match"]):
            self._type.value().addEntry(n, t)
    
        self._setAllowDynamicAttributes(True)
    
        self.addInputAttribute(self._instr)
        self.addInputAttribute(self._pattern)
        self.addInputAttribute(self._type)
        self.addInputAttribute(self._rep)
        self.addOutputAttribute(self._out)
    
        self._setAttributeAffect(self._instr, self._out)
        self._setAttributeAffect(self._pattern, self._out)
        self._setAttributeAffect(self._type, self._out)
        self._setAttributeAffect(self._rep, self._out)

    def updateSlice(self, attribute, slice):
        coralApp.logDebug("updateSlice")
        inp = self._instr.value().stringValues()
        pat = self._pattern.value().stringValueAt(slice)
        cid = self._type.value().currentIndex()
#         regType = self._type.value().currentString(cid)
        rep = self._rep.value().stringValues()
        if len(rep) == 1:
            rep = [rep[0]]*len(inp)
        out = []
        for cnt, inStr in enumerate(inp):
            # search
            if cid == 0:
                reg = re.search(pat, inStr)
                out.append(reg.group() or "")
            # replace
            elif cid == 1:
                out.append(re.sub(pat, rep[cnt], inStr))
            # match
            elif cid == 2:
                reg = re.match(pat, inStr)
                out.append(reg.group() or "")
            else:
                raise ValueError("Oops, something went wrong.")

#         self.processTasks()
        self._out.outValue().setStringValuesSlice(slice, out)

    def update(self, attribute):
        self.updateSlice(attribute, 0)

    def process(self):
        self.run()
        self._out.outValue().stringValue()
