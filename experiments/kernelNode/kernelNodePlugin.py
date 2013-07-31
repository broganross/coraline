

class KernelNodeInspectorWidget(NodeInspectorWidget):
    def __init__(self, coralNode, parentWidget):
        NodeInspectorWidget.__init__(self, coralNode, parentWidget)

        self._kernelSourceEdit = None
        self._kernelBuildConsole = None
        self._attrOrderWidgets = {}
        self._useSizeButtons = {}
        self._attrNameWidgets = {}

    def _addInputClicked(self):
        coralApp.createAttribute("NumericAttribute", "input", self.coralNode(), input = True)
        
        self.nodeInspector().refresh()

    def _addOutputClicked(self):
        coralApp.createAttribute("NumericAttribute", "output", self.coralNode(), output = True)
        
        self.nodeInspector().refresh()
    
    def _popupSpecCombo(self, comboBox):
        coralAttribute = comboBox.coralAttribute()
        
        coralAttribute.removeSpecializationOverride()
        coralAttribute.forceSpecializationUpdate()
        
        attrSpecialization = coralAttribute.specialization()
        
        comboBox.clear()
        for spec in coralAttribute.specialization():
            comboBox.addItem(spec)
        
        comboBox.addItem("none")
        comboBox.setCurrentIndex(len(attrSpecialization))
    
    def _currentSpecChanged(self, comboBox):
        specialization = str(comboBox.currentText())
        attr = comboBox.coralAttribute()
        
        if specialization != "" and specialization != "none":
            attr.setSpecializationOverride(str(specialization));
        else:
            attr.removeSpecializationOverride()
            
        attr.forceSpecializationUpdate()
    
    def _setKernelSource(self):
        if self._kernelSourceEdit:
            kernelSource = str(self._kernelSourceEdit.toPlainText())
            kernelSourceAttr = self.coralNode().findObject("_kernelSource")
            kernelSourceAttr.value().setStringValue(kernelSource)
            kernelSourceAttr.valueChanged()
            self._kernelBuildConsole.setPlainText(self.coralNode().buildInfo())
        
    def _openTextEditor(self):
        mainWin = mainWindow.MainWindow.globalInstance()
        dialog = QtGui.QDialog(mainWin)
        dialog.setWindowTitle("kernel editor")
        dialog.resize(500, 500)

        vlayout = QtGui.QVBoxLayout(dialog)
        vlayout.setContentsMargins(5, 5, 5, 5)
        vlayout.setSpacing(5)
        dialog.setLayout(vlayout)

        self._kernelSourceEdit = QtGui.QPlainTextEdit(dialog)
        self._kernelSourceEdit.setLineWrapMode(QtGui.QPlainTextEdit.NoWrap)
        self._kernelSourceEdit.setTabStopWidth(8)
        vlayout.addWidget(self._kernelSourceEdit)

        compileButton = QtGui.QPushButton("compile kernel", dialog)
        vlayout.addWidget(compileButton)

        dialog.connect(compileButton, QtCore.SIGNAL("clicked()"), self._setKernelSource)

        kernelSource = self.coralNode().findObject("_kernelSource").value().stringValue()
        self._kernelSourceEdit.setPlainText(kernelSource)

        self._kernelBuildConsole = QtGui.QTextEdit(dialog)
        vlayout.addWidget(self._kernelBuildConsole)
        self._kernelBuildConsole.setReadOnly(True)
        self._kernelBuildConsole.setLineWrapMode(QtGui.QTextEdit.NoWrap)
        
        palette = self._kernelBuildConsole.palette()
        palette.setColor(QtGui.QPalette.Base, QtGui.QColor(50, 55, 60))
        self._kernelBuildConsole.setPalette(palette)
        self._kernelBuildConsole.setTextColor(QtGui.QColor(200, 190, 200))

        palette = self._kernelSourceEdit.palette()
        palette.setColor(QtGui.QPalette.Base, QtGui.QColor(79, 87, 95))
        palette.setColor(QtGui.QPalette.Text, QtGui.QColor(225, 225, 225))
        self._kernelSourceEdit.setPalette(palette)

        self._kernelBuildConsole.setPlainText("Kernel compiler console")

        dialog.show()

    def _attrOrderChanged(self, argId):
        node = self.coralNode()
        dynAttrs = node.dynamicAttributes()
        node.clearDynamicAttributes()

        orderedDynAttrs = {} 
        for attr in dynAttrs:
            newOrder = self._attrOrderWidgets[attr.id()].value()

            while newOrder in orderedDynAttrs.keys():
                newOrder += 1

            orderedDynAttrs[newOrder] = attr
        
        orderIds = orderedDynAttrs.keys()
        orderIds.sort()

        for orderId in orderIds:
            node.addDynamicAttribute(orderedDynAttrs[orderId])
        
    def _useSizeToggled(self, value):
        node = self.coralNode()
        dynAttrs = node.dynamicAttributes()

        useSize = node.findObject("_useSize").value()

        useSize.resize(len(dynAttrs))
        i = 0
        for attr in dynAttrs:
            if attr.isInput():
                checked = self._useSizeButtons[attr.id()].isChecked()
                useSize.setBoolValueAt(i, checked)
            i += 1
    
    def _attrNameChanged(self):
        node = self.coralNode()
        dynAttrs = node.dynamicAttributes()

        for attr in dynAttrs:
            widgetValue = str(self._attrNameWidgets[attr.id()].text())
            if attr.name() != widgetValue:
                attr.setName(widgetValue)

    def _removeAttrClicked(self, button):
        attr = button._attr()
        node = self.coralNode()
        node.removeAttribute(attr)

        nodeUi = NodeEditor.findNodeUi(node.id())
        attrUi = NodeEditor.findAttributeUi(attr.id())
        nodeUi.removeAttributeUi(attrUi)
        attrUi.setParentNodeUi(None)
        nodeUi.updateLayout()

        self.nodeInspector().refresh()

    def build(self):
        NodeInspectorWidget.build(self)

        openTextEditorButton = QtGui.QPushButton("edit kernel source", self)
        self.layout().addWidget(openTextEditorButton)

        self.connect(openTextEditorButton, QtCore.SIGNAL("clicked()"), self._openTextEditor)

        groupBox = QtGui.QGroupBox("attribute editor", self)
        vlayout = QtGui.QVBoxLayout()
        vlayout.setSpacing(5)
        vlayout.setContentsMargins(0, 0, 0, 0)
        groupBox.setLayout(vlayout)
        
        addInAttrButton = QtGui.QPushButton("Add Input", groupBox)
        addOutAttrButton = QtGui.QPushButton("Add Output", groupBox)

        vlayout.addWidget(addInAttrButton)
        vlayout.addWidget(addOutAttrButton)

        self.connect(addInAttrButton, QtCore.SIGNAL("clicked()"), self._addInputClicked)
        self.connect(addOutAttrButton, QtCore.SIGNAL("clicked()"), self._addOutputClicked)

        useSize = self.coralNode().findObject("_useSize").outValue()

        order = 0
        for attr in self.coralNode().dynamicAttributes():
            hlayout = QtGui.QHBoxLayout()
            hlayout.setSpacing(5)
            hlayout.setContentsMargins(0, 0, 0, 0)

            attrOrder = QtGui.QSpinBox(groupBox)
            self._attrOrderWidgets[attr.id()] = attrOrder
            attrOrder.setMinimum(0)
            attrOrder.setValue(order)
            
            hlayout.addWidget(attrOrder)
            self.connect(attrOrder, QtCore.SIGNAL("valueChanged(int)"), self._attrOrderChanged)

            attrName = QtGui.QLineEdit(attr.name(), groupBox);
            self._attrNameWidgets[attr.id()] = attrName
            attrName.setFixedWidth(60)

            hlayout.addWidget(attrName)

            self.connect(attrName, QtCore.SIGNAL("editingFinished()"), self._attrNameChanged)

            specCombo = AttributeSpecializationComboBox(attr, groupBox)
            attrSpec = attr.specialization()
            if len(attrSpec) == 1:
                specCombo.addItem(attrSpec[0])
            else:
                specCombo.addItem("none")

            specCombo.setShowPopupCallback(self._popupSpecCombo)
            specCombo.setCurrentItemChangedCallback(self._currentSpecChanged)
            hlayout.addWidget(specCombo)

            useSizeButton = QtGui.QToolButton(self)
            self._useSizeButtons[attr.id()] = useSizeButton
            useSizeButton.setText("use size")
            useSizeButton.setCheckable(True)
            useSizeButton.setChecked(useSize.boolValueAt(order))
            hlayout.addWidget(useSizeButton)

            self.connect(useSizeButton, QtCore.SIGNAL("toggled(bool)"), self._useSizeToggled)

            if attr.isOutput():
                useSizeButton.setDisabled(True)
        
            removeButton = QtGui.QPushButton("remove", self)
            removeButton._attr = weakref.ref(attr)
            hlayout.addWidget(removeButton)

            self.connect(removeButton, QtCore.SIGNAL("clicked()"), utils.CallbackWithArgs(self._removeAttrClicked, removeButton))

            vlayout.addLayout(hlayout)

            order += 1

        self.layout().addWidget(groupBox)


def loadPluginUi():
    plugin = PluginUi("kernelNodePluginUi")
    
    plugin.registerInspectorWidget("KernelNode", KernelNodeInspectorWidget)
    
    return plugin

