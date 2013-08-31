""" Base implementation for nodes
"""

import  re
import  thread
import  weakref

from    coral   import  coralApp
from    coral   import  EnumAttribute
from    coral   import  Node
from    coral   import  NumericAttribute
from    coral   import  StringAttribute
from    coral   import  utils



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


class CollapsedExecutableNode(ExecutableNode):
    def __init__(self, name, parent):
        super(CollapsedExecutableNode, self).__init__(name, parent)
        self.setClassName("CollapsedExecutableNode")
        self._setUpdateEnabled(False)
        self._setAllowDynamicAttributes(True)
        
    def addDynamicAttribute(self, attribute):
        if attribute.isOutput():
            self._catchAttributeDirtied(attribute, True)
            
    def process(self):
        for attr in self.outputAttributes():
            attr.value()


class RegexNode(Node):
    """ For doing regular expressions
    """
    def __init__(self, name, parent):
        super(RegexNode, self).__init__(name, parent)
        self._setSliceable(True)

        self._instr = StringAttribute("input", self)
        self._pattern = StringAttribute("pattern", self)
        self._type = EnumAttribute("type", self)
        self._groupNum = EnumAttribute("group", self)
        self._rep = StringAttribute("sub", self)
        self._out = StringAttribute("out", self)

        for n, t in enumerate(["search", "sub", "match"]):
            self._type.value().addEntry(n, t)

        self._setAttributeAllowedSpecializations(self._instr, ["String", "Path"])    
        self._setAttributeAllowedSpecializations(self._pattern, ["String"])
        self._setAttributeAllowedSpecializations(self._out, ["String", "Path"])
        self._out._setSpecialization(["String"])

        self.addInputAttribute(self._type)
        self.addInputAttribute(self._instr)
        self.addInputAttribute(self._pattern)
        self.addInputAttribute(self._groupNum)
        self.addInputAttribute(self._rep)
        self.addOutputAttribute(self._out)

        self._setAttributeAffect(self._instr, self._out)
        self._setAttributeAffect(self._pattern, self._out)
        self._setAttributeAffect(self._type, self._out)
        self._setAttributeAffect(self._groupNum, self._out)
        self._setAttributeAffect(self._rep, self._out)

        self._catchAttributeDirtied(self._type, True)
        self._catchAttributeDirtied(self._pattern, True)

    def _updateGroup(self):
#         prevIndex = self._groupNum.value().currentIndex()
        try:
            reg = re.compile(self._pattern.value().stringValueAt(0))
        except:
            return
        else:
            self._groupNum.value().clear()
            for n in range(reg.groups+1):
                self._groupNum.value().addEntry(n, str(n))
#         if prevIndex <= reg.groups+1:
#             self._groupNum.value().setCurrentIndex(prevIndex)

    def attributeDirtied(self, attr):
        # if the pattern is updated then the group attribute widget needs to be updated as well
        if attr == self._pattern:
            self._updateGroup()
        elif attr == self._type:
            if attr.value().currentIndex() in [0, 2]:
                self._out._setSpecialization(["String"])
            elif attr.value().currentIndex() == 1:
                self._out._setSpecialization(self._instr.specialization())
        else:
            super(RegexNode, self).attributeDirtied(attr)

    def update(self, attribute):
        self.updateSlice(attribute, 0)

    def updateSlice(self, attribute, slice):
        inp = self._instr.value().stringValues()
        pat = self._pattern.value().stringValueAt(slice)
        cid = self._type.value().currentIndex()
        out = []
        for cnt, inStr in enumerate(inp):
            # search
            if cid == 0:
                reg = re.search(pat, inStr)
                gr = self._groupNum.value().currentIndex()
                if reg:
                    out.append(reg.group(gr) or "")
            # replace
            elif cid == 1:
                rep = self._rep.value().stringValueAt(cnt)
                out.append(re.sub(pat, rep, inStr))
            # match
            elif cid == 2:
                reg = re.match(pat, inStr)
                if reg:
                    out.append(reg.group() or "")
            else:
                raise ValueError("Oops, something went wrong.")

        self._out.outValue().setStringValuesSlice(slice, out)


class StringToIntNode(Node):
    """ Converts a string to an integer """
    def __init__(self, name, parent):
        super(StringToIntNode, self).__init__(name, parent)
        self._setSliceable(True)

        self._in = StringAttribute("in", self)
        self._out = NumericAttribute("out", self)
        self._setAttributeAllowedSpecializations(self._in, ["String"])
        self._setAttributeAllowedSpecializations(self._out,["Int"])

        self.addInputAttribute(self._in)
        self.addOutputAttribute(self._out)
        
        self._setAttributeAffect(self._in, self._out)

    def update(self, attr):
        inStr = self._in.value().stringValueAt(0)
        try:
            out = int(inStr)
        except:
            coralApp.logError("Can't convert '%s' to int"%inStr)
        else:
            self._out.outValue().setIntValueAt(0, out)
            
class IntToStringNode(Node):
    """Node that converts an integer to a string """
    def __init__(self, name, parent):
        super(IntToStringNode, self).__init__(name, parent)
        self._setSliceable(True)
        
        self._in = NumericAttribute("in", self)
        self._format = StringAttribute("format", self)
        self._out = StringAttribute("out", self)
        self._setAttributeAllowedSpecializations(self._in, ["Int"])
        self._setAttributeAllowedSpecializations(self._out, ["String"])
        self._setAttributeAllowedSpecializations(self._format, ["String"])
        
        self.addInputAttribute(self._in)
        self.addInputAttribute(self._format)
        self.addOutputAttribute(self._out)
        
        self._setAttributeAffect(self._in, self._out)
        self._setAttributeAffect(self._format, self._out)
        
    def update(self, attr):
        inInt = self._in.value().intValueAt(0)
        format = self._format.value().stringValueAt(0)
        try:
            if format != "":
                out = format%inInt
            else:
                out = str(inInt)
        except:
            coralApp.logError("Can't convert to string")
        else:
            self._out.outValue().setStringValueAt(0, out)
