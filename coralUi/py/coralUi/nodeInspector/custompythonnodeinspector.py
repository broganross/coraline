
from    PyQt4   import  QtCore
from    PyQt4   import  QtGui

from    coral   import  BoolAttribute
from    coral   import  Numeric
from    coral   import  NumericAttribute
from    coral   import  StringAttribute
from    coral   import  coralApp
from    nodeInspector   import  NodeInspectorWidget


class CustomPythonNodeInspectorWidget(NodeInspectorWidget):
    def build(self):
        super(CustomPythonNodeInspectorWidget, self).build()
        self._addAttBtn = QtGui.QPushButton(self)
        self._addAttBtn.setText("Add Input")
        self._addAttBtn.clicked.connect(self.addInClicked)
        self._addOutBtn = QtGui.QPushButton(self)
        self._addOutBtn.setText("Add Output")
        self._addOutBtn.clicked.connect(self.addOutClicked)
        self.readdButtons()

    def readdButtons(self):
#         self.layout().removeWidget(self.resetButton)
#         self.layout().removeWidget(self.processButton)
#         self.layout().removeWidget(self._log)
        self.layout().addWidget(self._addAttBtn)
        self.layout().addWidget(self._addOutBtn)
#         self.layout().addWidget(self.resetButton)
#         self.layout().addWidget(self.processButton)
#         self.layout().addWidget(self._log)
#         self.layout().setAlignment(self._log, QtCore.Qt.AlignTop)

    def addInClicked(self):
        diag = AddAttributeDialog(self)
        if diag.exec_():
            attName = diag.getAttributeName()
            if attName == "":
                coralApp.logError("Attribute name cannot be blank.")
                return
            attType = diag.getAttributeType()
            specs = diag.getSpecializations()
            node = self._coralNode()
            node.addInput(attName, attType, specs)
            self.nodeInspector().refresh()
        # find how to redraw the node graph

    def addOutClicked(self):
        diag = AddAttributeDialog(self)
        if diag.exec_():
            attName = diag.getAttributeName()
            if attName == "":
                coralApp.logError("Attribute name cannot be blank.")
                return
            attType = diag.getAttributeType()
            specs = diag.getSpecializations()
            node = self._coralNode()
            node.addOutput(attName, attType, specs)
            self.nodeInspector().refresh()
        # find how to redraw the node graph

class AddAttributeDialog(QtGui.QDialog):
    NUMERIC_TYPES   = ["Int", "IntArray", "Float", "FloatArray", "Vec3", "Vec3Array",
                       "Col4", "Col4Array", "Quat", "QuatArray", "Matrix44",
                       "Matrix44Array"]
    STRING_TYPES    = ["String", "StringArray", "Path", "PathArray"]

    BOOL_TYPES      = ["Bool"]

    def __init__(self, parent):
        super(AddAttributeDialog, self).__init__(parent)
        self.setLayout(QtGui.QVBoxLayout(self))

        self._nameEdit = QtGui.QLineEdit(self)
        self._nameEdit.setPlaceholderText("Attribute Name")
        valid = QtGui.QRegExpValidator(self._nameEdit)
        valid.setRegExp(QtCore.QRegExp("[a-zA-z_]*"))
        self._nameEdit.setValidator(valid)
        self.layout().addWidget(self._nameEdit)
        
        self._attList = QtGui.QComboBox(self)
        attList = [("string", StringAttribute),
                    ("numeric", NumericAttribute),
                    ("bool", BoolAttribute)]
        for att in attList:
            self._attList.addItem(att[0], userData=att[1])
        self._attList.currentIndexChanged.connect(self._attrChanged)
        self.layout().addWidget(self._attList)

        self._specialFrame = QtGui.QFrame()
        self._specialFrame.setLayout(QtGui.QGridLayout(self._specialFrame))
        self.layout().addWidget(self._specialFrame)
        
        bbox = QtGui.QDialogButtonBox(QtGui.QDialogButtonBox.Ok|QtGui.QDialogButtonBox.Cancel, parent=self)
        bbox.accepted.connect(self.accept)
        bbox.rejected.connect(self.reject) 
        self.layout().addWidget(bbox)
        
        self._attrChanged()

    def _clearSpecs(self):
        while self._specialFrame.layout().count() > 0:
            it = self._specialFrame.layout().itemAt(0).widget()
            self._specialFrame.layout().removeWidget(it)
            it.setParent(None)

    def _attrChanged(self):
        self._clearSpecs()
        attName = str(self._attList.currentText())
        if attName == "string":
            for n, t in enumerate(self.STRING_TYPES):
                box = QtGui.QCheckBox(self)
                box.setText(t)
                self._specialFrame.layout().addWidget(box, 0, n)
        elif attName == "bool":
            for n, t in enumerate(self.BOOL_TYPES):
                box = QtGui.QCheckBox(self)
                box.setText(t)
                self._specialFrame.layout().addWidget(box, 0, n)
        elif attName == "numeric":
            xcnt = 0
            ycnt = 0
            mx = 5
            for t in self.NUMERIC_TYPES:
                box = QtGui.QCheckBox(self)
                box.setText(t)
                self._specialFrame.layout().addWidget(box, xcnt, ycnt)
                xcnt += 1
                if xcnt == mx:
                    xcnt = 0
                    ycnt += 1

    def getAttributeType(self):
        index = self._attList.currentIndex()
        return self._attList.itemData(index)
        
    def getAttributeName(self):
        return str(self._nameEdit.text())

    def getSpecializations(self):
        specs = []
        notSel = []
        for i in range(self._specialFrame.layout().count()):
            it = self._specialFrame.layout().itemAt(i).widget()
            if it.isChecked():
                specs.append(str(it.text()))
            else:
                notSel.append(str(it.text()))
        if specs:
            return specs
        else:
            return notSel
            
            
    def setIsOutput(self, val):
        self._isOutput = val
        if val:
            self._clearSpecs()