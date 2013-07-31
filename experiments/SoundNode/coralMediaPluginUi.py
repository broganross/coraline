
from PyQt4 import QtGui, QtCore
from coral.coralUi.pluginUi import PluginUi
from coral.coralUi.nodeInspector.nodeInspector import NodeInspectorWidget
from coral.coralUi.nodeEditor.nodeUi import NodeUi
from coral.coralUi.nodeEditor.nodeEditor import NodeEditor
from coral.coralUi.mainWindow import MainWindow
from coral.coralUi import viewport
from coral import coralApp

class SoundStreamNodeInspectorWidget(NodeInspectorWidget):
    def __init__(self, coralNode, parentWidget):
        NodeInspectorWidget.__init__(self, coralNode, parentWidget)
        
        self._playButton = None
        
    def build(self):
        NodeInspectorWidget.build(self)
        
        self._playButton = QtGui.QToolButton(self)
        
        self._playButton.setText("Play")
        self._playButton.setCheckable(True)
        self._playButton.setChecked(self.coralNode().isPlaying())
        self.layout().addWidget(self._playButton)
        self.connect(self._playButton, QtCore.SIGNAL("toggled(bool)"), self._playButtonToggled)
    
    def _playButtonToggled(self, play):
        if play:
            viewport.ViewportWidget._activateTimedRefresh()
            self.coralNode().play(True)
        else:
            self.coralNode().play(False)
            viewport.ViewportWidget._activateImmediateRefresh()

def loadPluginUi():
    pluginUi = PluginUi("pipelineUi")
    pluginUi.registerInspectorWidget("SoundStream", SoundStreamNodeInspectorWidget)
    
    return pluginUi
