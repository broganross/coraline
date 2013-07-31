
import sys
import thread
import time

from coral import Node, NumericAttribute, StringAttribute
from coral.plugin import Plugin
from coral.coralUi import mainWindow

from coralMedia import SoundManager


class SoundStreamNode(Node):
    def __init__(self, name, parent):
        Node.__init__(self, name, parent)

        self._fileName = StringAttribute("fileName", self)
        self._leftSpectrum = NumericAttribute("leftSpectrum", self)
        self._rightSpectrum = NumericAttribute("rightSpectrum", self)
        self._time = NumericAttribute("time", self)
        self._play = False

        self.addInputAttribute(self._fileName)
        self.addOutputAttribute(self._time)
        self.addOutputAttribute(self._leftSpectrum)
        self.addOutputAttribute(self._rightSpectrum)

        self._setAttributeAffect(self._fileName, self._time)
        self._setAttributeAffect(self._fileName, self._leftSpectrum)
        self._setAttributeAffect(self._fileName, self._rightSpectrum)

        self._setAttributeAllowedSpecializations(self._time, ["Float"])
        self._setAttributeAllowedSpecializations(self._leftSpectrum, ["FloatArray"])
        self._setAttributeAllowedSpecializations(self._rightSpectrum, ["FloatArray"])

        self._setUpdateEnabled(False)

        self._leftSpectrum.outValue().resize(64)
        self._rightSpectrum.outValue().resize(64)

        SoundManager.init()
    
    def isPlaying(self):
        return self._play

    def _advanceTime(self):
        leftSpectrum = self._leftSpectrum.outValue()
        rightSpectrum = self._rightSpectrum.outValue()
        timeVal = self._time.outValue()

        framesPerSecond = 24.0
        timeStep = 1.0 / framesPerSecond
        
        enlapsedTime = 0.0
        while self._play:
            enlapsedTime = enlapsedTime + timeStep
            
            SoundManager.setSpectrumOnNumeric(leftSpectrum, 0)
            SoundManager.setSpectrumOnNumeric(rightSpectrum, 1)
            
            self._leftSpectrum.valueChanged()
            self._rightSpectrum.valueChanged()

            timeVal.setFloatValueAt(0, enlapsedTime)
            self._time.valueChanged()

            time.sleep(timeStep)

    def play(self, value = True):
        self._play = value
        if self._play:
            SoundManager.load(self._fileName.value().stringValue())
            SoundManager.play()
            thread.start_new_thread(self._advanceTime, ())
        else:
            SoundManager.stop()
            
            self._time.outValue().setFloatValueAt(0, 0.0)
            self._time.valueChanged()

    def __del__(self):
        self._play = False
        SoundManager.terminate()

def loadPlugin():
    plugin = Plugin("coralMedia")

    plugin.registerNode("SoundStream", SoundStreamNode, tags = ["media"])
    
    return plugin
