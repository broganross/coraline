
import time
import thread
from _coral import Node, NumericAttribute

class TimeNode(Node):
    def __init__(self, name, parent):
        Node.__init__(self, name, parent)
        self.setClassName("Time")
        
        self._framesPerSecond = NumericAttribute("framesPerSecond", self)
        self._time = NumericAttribute("time", self)
        self._play = False
        
        self.addInputAttribute(self._framesPerSecond)
        self.addOutputAttribute(self._time)
        
        self._setAttributeAllowedSpecializations(self._framesPerSecond, ["Float"])
        self._setAttributeAllowedSpecializations(self._time, ["Float"])
                
        self._setUpdateEnabled(False)
        
        self._framesPerSecond.outValue().setFloatValueAt(0, 24.0)
        
    def _advanceTime(self):
        timeValue = self._time.outValue()
        
        framesPerSecond = self._framesPerSecond.outValue().floatValueAt(0)
        timeStep = 1.0 / framesPerSecond
        
        enlapsedTime = 0.0
        while self._play:
            enlapsedTime = enlapsedTime + timeStep
            
            timeValue.setFloatValueAt(0, enlapsedTime * framesPerSecond)
            self._time.valueChanged()
            
            time.sleep(timeStep)
    
    def isPlaying(self):
        return self._play
    
    def play(self, value = True):
        self._play = value
        if self._play:
            thread.start_new_thread(self._advanceTime, ())
        else:
            self._time.outValue().setFloatValueAt(0, 0.0)
            self._time.valueChanged()


