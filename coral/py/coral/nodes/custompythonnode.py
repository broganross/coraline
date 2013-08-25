""" Node used to run custom python code
"""

import  sys
import  traceback

from    coral   import  Node
from    coral   import  BoolAttribute
from    coral   import  StringAttribute
from    coral   import  NumericAttribute
from    coral   import  coralApp


class CustomPythonNode(Node):
    def __init__(self, name, parent):
        super(CustomPythonNode, self).__init__(name, parent)
        self._setSliceable(True)
        self._setAllowDynamicAttributes(True)
        self.setClassName("CustomPythonNode")

        self._filter = StringAttribute("filter", self)
        self._filter.setLongString(True)
        self.addInputAttribute(self._filter)

    def _getInputValues(self):
        ret = {}
        for input in self.inputAttributes():
            if input.name() == "filter":
                continue
            if isinstance(input, StringAttribute):
                value = input.value()
                if value.type() == value.stringTypeAny:
                    val = []
                elif value.type() in [value.stringType, value.stringTypeArray]:
                    val = value.stringValues()
                elif value.type() in [value.pathType, value.pathTypeArray]:
                    val = value.pathValues()
                ret[input.name()] = val
            if isinstance(input, NumericAttribute):
                value = input.value()
                if value.type() == value.numericTypeAny:
                    val = []
                elif value.type() in [value.numericTypeInt, value.numericTypeIntArray]:
                    val = value.intValues()
                elif value.type() in [value.numericTypeFloat, value.numericTypeFloatArray]:
                    val = value.floatValues()
                elif value.type() in [value.numericTypeQuat, value.numericTypeQuatArray]:
                    val = value.quatValues()
                elif value.type() in [value.numericTypeMatrix44, value.numericTypeMatrix44Array]:
                    val = value.matrix44Values()
                elif value.type() in [value.numericTypeVec3, value.numericTypeVec3Array]:
                    val = value.vec3Values()
                elif value.type() in [value.numericTypeCol4, value.numericTypeCol4Array]:
                    val = value.col4Values()

                ret[input.name()] = val
            if isinstance(input, BoolAttribute):
                value = input.value().boolValue()

        return ret

    def _getOutputValues(self):
        ret = {}
        for output in self.outputAttributes():
            if isinstance(output, StringAttribute):
                value = output.outValue()
                ret[output.name()] = value
            elif isinstance(output, NumericAttribute):
                value = output.outValue()
                ret[output.name()] = value
        return ret
    
    def addInput(self, name, attClass, specs):
        """ Adds a new input attribute to the node, and connects it to the outputs """
        coralApp.executeCommand('CreateAttribute', className=attClass.__name__, 
                                name=name, parentNode=self.fullName(), input=True)
        attr = self.findAttribute(name)
        self._setAttributeAllowedSpecializations(attr, specs)
        for outAtt in self.outputAttributes():
            self._setAttributeAffect(attr, outAtt)

    def addOutput(self, name, attClass, specs):
        """ Adds a new output attribute to the node, and connects it to the inputs """
        coralApp.executeCommand("CreateAttribute", className=attClass.__name__,
                                name=name, parentNode=self.fullName(), output=True)
        attr = self.findAttribute(name)
        self._setAttributeAllowedSpecializations(attr, specs)
        for inAtt in self.inputAttributes():
            self._setAttributeAffect(inAtt, attr)

    def update(self, attribute):
        coralApp.logDebug("CustomPythonNode.update")
        inputs = self._getInputValues()
        outputs = self._getOutputValues()

        pyfilter = self._filter.value().stringValues()[0]
        if isinstance(pyfilter, list):
            pyfilter = pyfilter[0]

        try:
            exec(pyfilter)
        except:
            exc_type, exc_value, exc_traceback = sys.exc_info()
            print "*** print_tb:"
            traceback.print_tb(exc_traceback, limit=1, file=sys.stdout)
            print "*** print_exception:"
            traceback.print_exception(exc_type, exc_value, exc_traceback, limit = 2, file = sys.stdout)

        coralApp.logDebug("CustomPythonNode.update: Done")

    def process(self):
        self.run()