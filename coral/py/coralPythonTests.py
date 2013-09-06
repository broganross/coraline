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
from coral import _coral
from coral import coralApp
import Imath

class TestValue(_coral.Value):
    def __init__(self, value):
        _coral.Value.__init__(self)
        self.value = value

class TestAttribute(_coral.Attribute):
    def __init__(self, name, parent):
        _coral.Attribute.__init__(self, name, parent)

        self._setValuePtr(TestValue(0))

class TestNode(_coral.Node):
    def __init__(self, name, parent):
        _coral.Node.__init__(self, name, parent)
        
        self.inAttr = TestAttribute("inAttr", self)
        self.outAttr = TestAttribute("outAttr", self)
        
        self.addInputAttribute(self.inAttr)
        self.addOutputAttribute(self.outAttr)
        
        self._setAttributeAffect(self.inAttr, self.outAttr)
    
    def update(self, attribute):
        self.outAttr.outValue().value = self.inAttr.value().value

def testPythonOverloading():
    node = TestNode("node", None)
    parentNode = _coral.Node("testNode", None)
    attr = TestAttribute("attr", parentNode)
    parentNode.addOutputAttribute(attr)
    
    _coral.NetworkManager.connect(attr, node.inAttr)
    
    print "testing returned attribute is python subclass"
    assert type(node.inAttr.input()) == TestAttribute
    
    print "testing python overrides c++ virtual functions"
    attr.value().value = 5
    attr.valueChanged()
    assert node.outAttr.value().value == 5

def testInterface():
    root = _coral.Node("root", None)
    
    node = _coral.Node("testNode", root)
    attribute = _coral.Attribute("testAttr", node)
    node.addInputAttribute(attribute)
    node1 = _coral.Node("testNode1", node)
    node.addNode(node1)
    assert node1.parent().name() == node.name()
    assert node.nodeAt(0).name() == node1.name()
    assert node.inputAttributeAt(0).name() == attribute.name()
    assert node.containsNode(node1)
    assert len(node.nodes()) == 1
    node.removeNode(node1)
    assert node.containsNode(node1) == False
    
    attribute1 = _coral.Attribute("testAttr1", root)
    root.addOutputAttribute(attribute1)
    _coral.NetworkManager.connect(attribute1, attribute)

    assert attribute1.isConnectedTo(attribute)
    assert attribute.input().name() == attribute1.name()
    
    nestedNode = _coral.Node("nestedNode", node)
    assert nestedNode.fullName() == "root.testNode.nestedNode"
    
def testObjectsLifespan():
    node0 = _coral.Node("node0", None)
    node1 = _coral.Node("node1", node0)
    
    node0.addNode(node1)
    
    print "testing children nodes are kept alive by father node"
    del node1
    assert _coral.NetworkManager.objectCount() == 2
    
    print "testing children nodes are destroyed by father node"
    del node0
    assert _coral.NetworkManager.objectCount() == 0
    
    print "testing input attributes are destroyed by father node"
    node = _coral.Node("node", None)
    attribute = _coral.Attribute("testAttr", node)
    node.addInputAttribute(attribute)
    
    del node
    del attribute
    assert _coral.NetworkManager.objectCount() == 0
    
    print "testing output attributes are destroyed by father node"
    node = _coral.Node("node", None)
    attribute = _coral.Attribute("testAttr", node)
    node.addOutputAttribute(attribute)
    
    del node
    del attribute
    assert _coral.NetworkManager.objectCount() == 0

def testNodeDeletion():
    coralApp.init()
    
    coralApp.createNode("Float", "test", coralApp.findNode("root"))
    coralApp.deleteNodes([coralApp.findNode("root.test")])
    
    assert _coral.NetworkManager.objectCount() == 1
    
    coralApp.finalize()

def testOwnership():
    n1 = _coral.Node("test", None)
    n2 = _coral.Node("nested", None)
    
    n1.addNode(n2)
    del n2
    
    assert n1.nodeAt(0)

def testNesting():
    coralApp.init()
    
    root = coralApp.findNode("root")
    n1 = coralApp.createNode("CollapsedNode", "n1", root)
    n2 = coralApp.createNode("CollapsedNode", "n2", n1)

    coralApp.finalize()

def testDeletingAttributes():
    coralApp.init()
    
    root = coralApp.findNode("root")
    n1 = coralApp.createNode("Float", "n1", root)
    n2 = coralApp.createNode("Float", "n2", root)
    n3 = coralApp.createNode("Add", "n3", root)
    n4 = coralApp.createNode("Add", "n4", root)
    n5 = coralApp.createNode("Float", "n5", root)
    n6 = coralApp.createNode("Add", "n6", root)

    _coral.NetworkManager.connect(n1.outputAttributeAt(0), n3.inputAttributeAt(0))
    _coral.NetworkManager.connect(n2.outputAttributeAt(0), n3.inputAttributeAt(1))
    _coral.NetworkManager.connect(n3.outputAttributeAt(0), n4.inputAttributeAt(0))
    _coral.NetworkManager.connect(n5.outputAttributeAt(0), n4.inputAttributeAt(1))
    _coral.NetworkManager.connect(n3.outputAttributeAt(0), n6.inputAttributeAt(1))

    coralApp.deleteNodes([n3, n5, n6])
    
    coralApp.finalize()

def testFindObject():
    coralApp.init()
    
    root = coralApp.findNode("root")
    n1 = coralApp.createNode("CollapsedNode", "n1", root)
    n2 = coralApp.createNode("CollapsedNode", "n2", n1)
    
    assert coralApp.findObject(n2.fullName()) is n2
    
    coralApp.finalize()

def testObjectsHierarchy():
    parentNode = _coral.Node("parentNode", None)
    childNode = _coral.Node("childNode", parentNode)
    
    parentNode.addNode(childNode)
    
    assert childNode.fullName() == "parentNode.childNode"
    
    parentNode.removeNode(childNode)
    
    assert childNode.fullName() == "childNode"

def testCollapsedNode():
    coralApp.init()
    
    root = coralApp.findNode("root")
    n1 = coralApp.createNode("Float", "n1", root)
    n2 = coralApp.createNode("Float", "n2", root)
    n3 = coralApp.createNode("Add", "n3", root)
    
    _coral.NetworkManager.connect(n1.outputAttributeAt(0), n3.inputAttributeAt(0))
    
    collapsedNode = coralApp.collapseNodes([n3])
    
    assert collapsedNode in root.nodes()
    
    coralApp.finalize()

def testCommand():
    cmd = _coral.Command()
    cmd.setName("test")
    cmd.setArgBool("boolArg", True)
    cmd.setArgInt("intArg", 1)
    cmd.setArgFloat("floatArg", 1.0)
    cmd.setArgString("stringArg", "hello")
    cmd.setArgUndefined("undefinedArg", "int(1)")
    
    assert cmd.asScript() == "executeCommand('test', boolArg = True, intArg = 1, floatArg = 1.000000, stringArg = 'hello', undefinedArg = int(1))"
    assert cmd.argType("undefinedArg") is 0
    assert cmd.argType("boolArg") is 1
    assert cmd.argType("intArg") is 2
    assert cmd.argType("floatArg") is 3
    assert cmd.argType("stringArg") is 4
    
def testSaveScript():
    coralApp.init()
    
    root = coralApp.findNode("root")
    n1 = coralApp.createNode("Float", "n1", root)
    n2 = coralApp.createNode("Float", "n2", root)
    n3 = coralApp.createNode("Add", "n3", root)
    
    n1.outputAttributeAt(0).outValue().setFloatValueAt(0, 5.0)
    
    _coral.NetworkManager.connect(n1.outputAttributeAt(0), n3.inputAttributeAt(0))
    
    saveScript = root.contentAsScript()
    
    coralApp.finalize()
    
    coralApp.init()
    coralApp.setShouldLogInfos(False)
    
    coralApp._executeNetworkScript(saveScript)
    
    n1 = coralApp.findNode("root.n1")
    n2 = coralApp.findNode("root.n2")
    n3 = coralApp.findNode("root.n3")
    
    assert n1 is not None
    assert n2 is not None
    assert n3 is not None
    
    n1Floats = n1.outputAttributeAt(0).outValue().getFloat()
    assert n1Floats[0] == 5.0
    
    assert n3.inputAttributeAt(0).input() is n1.outputAttributeAt(0)
    
    collapsedNode = coralApp.collapseNodes([n3])
    
    root = coralApp.findNode("root")
    
    saveScript = root.contentAsScript()
    coralApp.finalize()
    
    coralApp.init()
    coralApp._executeNetworkScript(saveScript)
    
    collapseNode = coralApp.findNode("root.CollapsedNode")
    assert collapseNode is not None
    assert len(collapseNode.attributes()) > 0
    
    coralApp.finalize()

def testSpecializationBug1():
    n = _coral.Node("n", None)
    a = _coral.Attribute("a", n)
    n.addOutputAttribute(a)
    n._setAttributeAllowedSpecializations(a, ["Spec1", "Spec3"])
    
    n1 = _coral.Node("n1", None)
    a1 = _coral.Attribute("a1", n1)
    n1.addInputAttribute(a1)
    n1._setAttributeAllowedSpecializations(a1, ["Spec1", "Spec2"])
    
    _coral.NetworkManager.connect(a, a1)
    
    assert a1.specialization() == ["Spec1"] and a.specialization() == ["Spec1"]

def testCollapsingBug1():
    coralApp.init()
    
    collapsed = coralApp.createNode("CollapsedNode", "collapsed", coralApp.rootNode())
    in1 = coralApp.createAttribute("PassThroughAttribute", "in", collapsed, input = True)
    add = coralApp.createNode("Add", "addNode", collapsed)
    
    _coral.NetworkManager.connect(in1, add.inputAttributeAt(0))

    float1 = coralApp.createNode("Float", "float1", coralApp.rootNode())
        
    _coral.NetworkManager.connect(float1.outputAttributeAt(0), in1)
    
    newCollapsed = coralApp.collapseNodes([collapsed])
    print "testing collapsing operation won't add erroneous output attributes"
    assert newCollapsed.outputAttributes() == []
    
    print "testing collapsing operation won't disconnect erroneous attributes"
    assert in1.isConnectedTo(add.inputAttributeAt(0))
    
    coralApp.finalize()

def testSpecializingPass():
    coralApp.init()
    
    collapsed0 = coralApp.createNode("CollapsedNode", "collapsed0", coralApp.rootNode())
    in0 = coralApp.createAttribute("PassThroughAttribute", "in0", collapsed0, input = True)
    out0 = coralApp.createAttribute("PassThroughAttribute", "out0", collapsed0, output = True)
    
    collapsed = coralApp.createNode("CollapsedNode", "collapsed", collapsed0)
    in1 = coralApp.createAttribute("PassThroughAttribute", "in", collapsed, input = True)
    out1 = coralApp.createAttribute("PassThroughAttribute", "out", collapsed, output = True)
    add = coralApp.createNode("Add", "addNode", collapsed)
    
    _coral.NetworkManager.connect(in1, add.inputAttributeAt(0))
    _coral.NetworkManager.connect(add.outputAttributeAt(0), out1)
    
    _coral.NetworkManager.connect(in0, in1)
    _coral.NetworkManager.connect(out1, out0)

    float1 = coralApp.createNode("Float", "float1", coralApp.rootNode())
    floats = coralApp.createNode("Vec3", "floats", coralApp.rootNode())
    
    _coral.NetworkManager.connect(out0, floats.inputAttributeAt(0))
    _coral.NetworkManager.connect(float1.outputAttributeAt(0), in0)
    
    assert add.inputAttributeAt(0).specialization() == ["Float"]
    assert add.outputAttributeAt(0).specialization() == ["Float"]
    
    assert in0.specialization() == ["Float"]
    assert in1.specialization() == ["Float"]
    
    assert out1.specialization() == ["Float"] 
    
    coralApp.finalize()

def runTest(function):
    print "* running", function.__name__

    function()
    assert _coral.NetworkManager.objectCount() == 0
    
    print "*", function.__name__, "done!"

if __name__ == "__main__":
    runTest(testObjectsLifespan)
    runTest(testInterface)
    runTest(testPythonOverloading)
    runTest(testNodeDeletion)
    runTest(testOwnership)
    runTest(testNesting)
    runTest(testDeletingAttributes)
    runTest(testFindObject)
    runTest(testObjectsHierarchy)
    runTest(testCollapsedNode)
    runTest(testCommand)
    runTest(testCollapsingBug1)
    runTest(testSpecializingPass)
    runTest(testSpecializationBug1)
    
    # _coral.runTests()
