
from PyQt4 import QtGui, QtCore
from coral.coralUi.pluginUi import PluginUi
from coral.coralUi.nodeInspector.nodeInspector import NodeInspectorWidget

class JitterNodeInspectorWidget(NodeInspectorWidget):
    def build(self):
        NodeInspectorWidget.build(self)

        button = QtGui.QPushButton("Say Hi!", self)
        self.layout().addWidget(button)
        self.connect(button, QtCore.SIGNAL("clicked()"), self._buttonClicked)

    def _buttonClicked(self):
        print self.coralNode().sayHi()

def loadPluginUi():
    pluginUi = PluginUi("JitterNodePluginUi")
    pluginUi.registerInspectorWidget("JitterNode", JitterNodeInspectorWidget)

    return pluginUi