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


import weakref
from PyQt4 import QtGui, QtCore

import nodeInspector
from .. import mainWindow
from ... import coralApp
from ... import utils
from ...observer import Observer
from ... import Imath

class ObjectField(QtGui.QWidget):
    def __init__(self, label, coralObject, parentWidget):
        QtGui.QWidget.__init__(self, parentWidget)
        
        self._mainLayout = QtGui.QHBoxLayout(self)
        self._label = QtGui.QLabel(label, self)
        self._valueWidget = None
        self._coralObject = weakref.ref(coralObject)
        
        self.setLayout(self._mainLayout)
        self._mainLayout.setContentsMargins(0, 0, 0, 0)
        self._mainLayout.addWidget(self._label)
    
    def label(self):
        return self._label
    
    def valueWidget(self):
        return self._valueWidget
    
    def setObjectWidget(self, widget, endOfEditSignal = None, endOfEditCallback = None):
        self._valueWidget = widget
        self._mainLayout.addWidget(widget)
        
        if endOfEditSignal:
            self.connect(self._valueWidget, QtCore.SIGNAL(endOfEditSignal), endOfEditCallback)
    
    def coralObject(self):
        return self._coralObject()

class AttributeField(ObjectField):
    def __init__(self, coralAttribute, parentWidget):
        ObjectField.__init__(self, coralAttribute.name().split(":")[-1], coralAttribute, parentWidget)
        
        self._sourceAttributes = self._findSourceAttributes()
        self._timer = self.startTimer(500)
    
    def timerEvent(self, event):
        valueWidget = self.valueWidget()
        if valueWidget:
            if valueWidget.hasFocus() == False:
                self.attributeValueChanged()

    def widgetValueChanged(self):
        value = self.getWidgetValue(self.valueWidget())
        somethingChanged = False
        for sourceAttr in self._sourceAttributes:
            attr = sourceAttr()
            if self.getAttributeValue(attr) != value:
                self.setAttributeValue(attr, value)
                somethingChanged = True
        
        if somethingChanged:
            self.coralObject().forceDirty()
    
    def attributeValueChanged(self):
        value = self.getAttributeValue(self._sourceAttributes[0]())
        if value != self.getWidgetValue(self.valueWidget()):
            self.setWidgetValue(self.valueWidget(), value)
    
    def getWidgetValue(self, widget):
        return None
    
    def setWidgetValue(self, widget):
        pass
    
    def getAttributeValue(self, attribute):
        return None
    
    def setAttributeValue(self, attribute, value):
        pass
    
    def setAttributeWidget(self, widget, endOfEditSignal = None):
        ObjectField.setObjectWidget(self, widget, endOfEditSignal, self.widgetValueChanged)

        attribute = self.coralObject()
        if attribute.input() or attribute.affectedBy():
            self.label().setText(">" + self.label().text())
            
        self.attributeValueChanged()
    
    def _collectOutAttrsNonPass(self, attribute, outAttrs):
        if attribute.isPassThrough() == False:
            outAttrs.append(weakref.ref(attribute))
        else:
            for outAttr in attribute.outputs():
                self._collectOutAttrsNonPass(outAttr, outAttrs)
	
    def _findSourceAttributes(self):
        attr = self.coralObject()
        attrs = []
        if attr.isPassThrough():
            self._collectOutAttrsNonPass(attr, attrs)
        if len(attrs) == 0:
            attrs = [weakref.ref(attr)]
        
        return attrs

class CustomDoubleSpinBox(QtGui.QDoubleSpinBox):
    def __init__(self, parent):
        QtGui.QDoubleSpinBox.__init__(self, parent)
        self.setDecimals(4)
        self._wheelCallback = None

    def wheelEvent(self, wheelEvent):
        QtGui.QDoubleSpinBox.wheelEvent(self, wheelEvent)
        
        self._wheelCallback()

class FloatValueField(AttributeField):
    def __init__(self, coralAttribute, parentWidget):
        AttributeField.__init__(self, coralAttribute, parentWidget)
        
        attrWidget = CustomDoubleSpinBox(self)
        attrWidget._wheelCallback = utils.weakRef(self.widgetValueChanged)
        attrWidget.setRange(-99999999999.0, 99999999999.0)
        attrWidget.setSingleStep(0.1)
        
        self.setAttributeWidget(attrWidget, "editingFinished()")
    
    def setAttributeValue(self, attribute, value):
        attribute.outValue().setFloatValueAt(0, value)
    
    def getAttributeValue(self, attribute):
        return attribute.value().floatValueAt(0)
    
    def setWidgetValue(self, widget, value):
        widget.setValue(value)
    
    def getWidgetValue(self, widget):
        return widget.value()

class CustomIntSpinBox(QtGui.QSpinBox):
    def __init__(self, parent):
        QtGui.QSpinBox.__init__(self, parent)
        
        self._wheelCallback = None
    
    def wheelEvent(self, wheelEvent):
        QtGui.QSpinBox.wheelEvent(self, wheelEvent)
        
        self._wheelCallback()
        
class IntValueField(AttributeField):
    def __init__(self, coralAttribute, parentWidget):
        AttributeField.__init__(self, coralAttribute, parentWidget)
        
        attrWidget = CustomIntSpinBox(self)
        attrWidget._wheelCallback = utils.weakRef(self.widgetValueChanged)
        attrWidget.setRange(-999999999, 999999999)
        
        self.setAttributeWidget(attrWidget, "editingFinished()")
        
    def setAttributeValue(self, attribute, value):
        attribute.outValue().setIntValueAt(0, value)
    
    def getAttributeValue(self, attribute):
        return attribute.value().intValueAt(0)
    
    def setWidgetValue(self, widget, value):
        widget.setValue(value)
    
    def getWidgetValue(self, widget):
        return widget.value()

class BoolValueField(AttributeField):
    def __init__(self, coralAttribute, parentWidget):
        AttributeField.__init__(self, coralAttribute, parentWidget)
        
        attrWidget = QtGui.QCheckBox(self)
        
        self.setAttributeWidget(attrWidget, "stateChanged(int)")
        attrWidget.setTristate(False)
        
    def setAttributeValue(self, attribute, value):
        attribute.outValue().setBoolValueAt(0, value)
    
    def getAttributeValue(self, attribute):
        return attribute.value().boolValueAt(0)
    
    def setWidgetValue(self, widget, value):
        widget.setCheckState(value)

    def getWidgetValue(self, widget):
        return widget.isChecked()

class CustomLineEdit(QtGui.QLineEdit):
    def __init__(self, parent):
        QtGui.QLineEdit.__init__(self, parent)

        self.setAcceptDrops(True)

    def dragEnterEvent(self, event):
        if event.mimeData().hasUrls:
            event.accept()
        else:
            event.ignore()
    
    def dragMoveEvent(self, event):
        if event.mimeData().hasUrls:
            event.setDropAction(QtCore.Qt.CopyAction)
            event.accept()
        else:
            event.ignore()

    def dropEvent(self, event):
        if event.mimeData().hasUrls:
            event.setDropAction(QtCore.Qt.CopyAction)
            event.accept()
            
            urls = event.mimeData().urls()
            path = ""
            if urls:
                path = str(urls[0].toLocalFile())
                self.setText(path)
                self.emit(QtCore.SIGNAL("editingFinished()"))
        else:
            event.ignore()

class StringValueField(AttributeField):
    def __init__(self, coralAttribute, parentWidget):
        AttributeField.__init__(self, coralAttribute, parentWidget)

        if coralAttribute.longString():
            textEdit = QtGui.QTextEdit(self)
            textEdit.setLineWrapMode(QtGui.QTextEdit.NoWrap)
            textEdit.setAcceptRichText(False)
            textEdit.setMaximumHeight(100)

            self.setAttributeWidget(textEdit, "textChanged()")

            self.layout().setAlignment(textEdit, QtCore.Qt.AlignTop)
        else:    
            self.setAttributeWidget(CustomLineEdit(self), "editingFinished()")
    
    def setAttributeValue(self, attribute, value):
        attribute.outValue().setStringValue(value)
    
    def getAttributeValue(self, attribute):
        return attribute.value().stringValue()
    
    def setWidgetValue(self, widget, value):
        if type(widget) is QtGui.QTextEdit:
            widget.setPlainText(value)
        else:    
            widget.setText(value)
    
    def getWidgetValue(self, widget):
        text = ""
        if type(widget) is QtGui.QTextEdit:
            text = widget.toPlainText()
        else:
            text = widget.text()

        return str(text)

class NameField(ObjectField):
    def __init__(self, coralNode, parentWidget):
        ObjectField.__init__(self, "name", coralNode, parentWidget)
        
        self._nameChangedObserver = Observer()
        
        self.setObjectWidget(QtGui.QLineEdit(coralNode.name(), self), "editingFinished()", self.widgetValueChanged)
        
        coralApp.addNameChangedObserver(self._nameChangedObserver, coralNode, self._nameChanged)
    
    def widgetValueChanged(self):
        newName = str(self.valueWidget().text())
        if self.coralObject().name() != newName:
            self.coralObject().setName(newName)
    
    def _nameChanged(self):
        newName = self.coralObject().name()
        if newName != str(self.valueWidget().text()):
            self.valueWidget().setText(newName)


class ColorField(AttributeField):
    def __init__(self, coralAttribute, parentWidget):
        AttributeField.__init__(self, coralAttribute, parentWidget)

        self._colorButton = QtGui.QPushButton(self)
        self.setAttributeWidget(self._colorButton)

        self.connect(self._colorButton, QtCore.SIGNAL("clicked()"), self._colButtonClicked)
    
    def _colorDialogChangedColor(self, color):
        rgbStr = str(255*color.redF()) + "," + str(255*color.greenF()) + "," + str(255*color.blueF()) + "," + str(255*color.alphaF())
        self._colorButton.setStyleSheet("background-color: rgba(" + rgbStr + ");")
        self._colorButton.setText(str(round(color.redF(), 2)) + ", " + str(round(color.greenF(), 2)) + ", " + str(round(color.blueF(), 2)) + ", " + str(round(color.alphaF(), 2)))
        
        self.widgetValueChanged()

    def _colButtonClicked(self):
        self.killTimer(self._timer)

        col = self._colorButton.palette().background().color()
        colDialog = QtGui.QColorDialog(col, self)
        colDialog.setOption(QtGui.QColorDialog.DontUseNativeDialog, True)
        colDialog.setOption(QtGui.QColorDialog.ShowAlphaChannel, True)
        self.connect(colDialog, QtCore.SIGNAL("currentColorChanged(const QColor&)"), self._colorDialogChangedColor)
        colDialog.exec_()

        newCol = col
        if colDialog.result():
            newCol = colDialog.selectedColor()

        rgbStr = str(255*newCol.redF()) + "," + str(255*newCol.greenF()) + "," + str(255*newCol.blueF()) + "," + str(255*newCol.alphaF())
        self._colorButton.setStyleSheet("background-color: rgba(" + rgbStr + ");")
        self._colorButton.setText(str(round(newCol.redF(), 2)) + ", " + str(round(newCol.greenF(), 2)) + ", " + str(round(newCol.blueF(), 2)) + ", " + str(round(newCol.alphaF(), 2)))
        
        self.widgetValueChanged()

        self._timer = self.startTimer(500)

    def setAttributeValue(self, attribute, value):
        attribute.outValue().setCol4ValueAt(0, Imath.Color4f(value[0], value[1], value[2], value[3]))
    
    def getAttributeValue(self, attribute):
        col = attribute.value().col4ValueAt(0)
        return [col.r, col.g, col.b, col.a]
    
    def setWidgetValue(self, widget, value):
        for i in range(4):
            val = value[i]
            if val < 0.0:
                value[i] = 0.0
            elif val > 1.0:
                value[i] = 1.0
        
        rgbStr = str(255*value[0]) + "," + str(255*value[1]) + "," + str(255*value[2]) + "," + str(255.0*value[3])
        self._colorButton.setStyleSheet("background-color: rgba(" + rgbStr + ");")
        self._colorButton.setText(str(round(value[0], 2)) + ", " + str(round(value[1], 2)) + ", " + str(round(value[2], 2)) + ", " + str(round(value[3], 2)))
    
    def getWidgetValue(self, widget):
        col = self._colorButton.palette().background().color()
        return [col.redF(), col.greenF(), col.blueF(), col.alphaF()]

