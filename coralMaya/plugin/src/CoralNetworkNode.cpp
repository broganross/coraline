// <license>
// Copyright (C) 2011 Andrea Interguglielmi, All rights reserved.
// This file is part of the coral repository downloaded from http://code.google.com/p/coral-repo.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
// 
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// </license>

#include <Python.h>

#include "CoralNetworkNode.h"
#include <coral/src/pythonWrapperUtils.h>

MTypeId CoralNetworkNode::id(0x80012);
MObject CoralNetworkNode::coralNodeId;
MObject CoralNetworkNode::saveData;

CoralNetworkNode::CoralNetworkNode(){
}

CoralNetworkNode::~CoralNetworkNode(){
}

void* CoralNetworkNode::creator(){
	return new CoralNetworkNode();
}

std::vector<coral::Attribute*> CoralNetworkNode::findCoralMayaAttributes(coral::Node *coralNode){
	std::vector<coral::Attribute*> coralMayaAttributes;
	
	std::vector<coral::Node*> nodes = coralNode->nodes();
	
	for(int i = 0; i < nodes.size(); ++i){
		if(containerUtils::elementInContainer(std::string("CoralMayaNode"), nodes[i]->classNames())){
			std::vector<coral::Attribute*> attributes = nodes[i]->attributes();
			
			for(int j = 0; j < attributes.size(); ++j){
				if(dynamic_cast<CoralMayaAttribute*>(attributes[j])){
					coralMayaAttributes.push_back(attributes[j]);
				}
			}
		}
	}
	
	return coralMayaAttributes;
}

void CoralNetworkNode::updateCoralAttributeMap(){
	// collect all nodes of type CoralMayaNode under the MayaContextNode associated with this node, then update 
	// _inputPlugs and _outputPlugs.
	// Also update _coralAttributeMap with the pair: mayaAttributeName-coralAttributeId.
	MFnDependencyNode thisNodeFn(thisMObject());
	int coralNodeId = thisNodeFn.findPlug("coralNodeId").asInt();
	coral::Object *coralNodeObj = coral::NetworkManager::findObjectById(coralNodeId);
	
	_coralAttributeMap.clear();
	_inputPlugs.clear();
	_outputPlugs.clear();
	
	if(coralNodeObj){
		coral::Node *coralNode = (coral::Node*)coralNodeObj;
		std::vector<coral::Attribute*> coralAttributes = findCoralMayaAttributes(coralNode);
		
		for(int i = 0; i < coralAttributes.size(); ++i){
			coral::Attribute *coralAttribute = coralAttributes[i];
			CoralMayaAttribute *coralMayaAttribute = dynamic_cast<CoralMayaAttribute*>(coralAttribute);
			MPlug plug = thisNodeFn.findPlug(coralMayaAttribute->mayaAttribute().data());
			if(!plug.isNull()){
				_coralAttributeMap[std::string(plug.partialName().asChar())] = coralAttribute->id();
				if(coralAttribute->isOutput()){
					_inputPlugs.push_back(plug);
				}
				else{
					coralMayaAttribute->setCanDirtyMayaAttribute(true);
					_outputPlugs.append(plug);
				}
			}
		}
	}
}

void CoralNetworkNode::coralCanDirtyOutAttrs(bool value){
	for(int i = 0; i < _outputPlugs.length(); ++i){
        MPlug outPlug = _outputPlugs[i];
        if(!outPlug.isNull()){
	        std::string attrName(outPlug.partialName().asChar());
	        coral::Object *coralObject = coral::NetworkManager::findObjectById(_coralAttributeMap[attrName]);
	        if(coralObject){
	        	CoralMayaAttribute *coralMayaAttr = dynamic_cast<CoralMayaAttribute*>(coralObject);
	        	coralMayaAttr->setCanDirtyMayaAttribute(value);
	        }
	    }
	}
}

MStatus CoralNetworkNode::compute(const MPlug& plug, MDataBlock& data){
	coral::pythonWrapperUtils::pyGILEnsured = true;

	for(int i = 0; i < _inputPlugs.size(); ++i){
		MPlug inPlug = _inputPlugs[i];
		
		if(!inPlug.isNull()){
			std::string attrName(inPlug.partialName().asChar());
			coral::Object *coralObject = coral::NetworkManager::findObjectById(_coralAttributeMap[attrName]);

			if(coralObject){
				CoralMayaAttribute *coralMayaAttr = dynamic_cast<CoralMayaAttribute*>(coralObject);
				coralMayaAttr->transferValueFromMaya(inPlug, data);
			}
		}
	}

	for(int i = 0; i < _outputPlugs.length(); ++i){
		MPlug outPlug = _outputPlugs[i];
		if(!outPlug.isNull()){
			std::string attrName(outPlug.partialName().asChar());

			coral::Object *coralObject = coral::NetworkManager::findObjectById(_coralAttributeMap[attrName]);
			if(coralObject){
				CoralMayaAttribute *coralMayaAttr = dynamic_cast<CoralMayaAttribute*>(coralObject);

				PyGILState_STATE state = PyGILState_Ensure();

				coralMayaAttr->transferValueToMaya(outPlug, data);

				PyGILState_Release(state);

				coralMayaAttr->setCanDirtyMayaAttribute(true);
				data.setClean(outPlug);
			}
		}
	}

	coral::pythonWrapperUtils::pyGILEnsured = false;

    return MS::kSuccess;
}

MStatus CoralNetworkNode::setDependentsDirty(MPlug const &inPlug, MPlugArray &affectedPlugs){
	if(inPlug.isDestination() == false){
		coralCanDirtyOutAttrs(false);
	}
	
	if(containerUtils::elementInContainer(inPlug, _inputPlugs)){
		affectedPlugs = _outputPlugs;
		for(int i = 0; i < _outputPlugs.length(); ++i){
			int elements = _outputPlugs[i].numElements();
			for(int j = 0; j < elements; ++j){
				affectedPlugs.append(_outputPlugs[i].elementByPhysicalIndex(j));
				for(int k = 0; k < _outputPlugs[i].elementByPhysicalIndex(j).numChildren(); ++k){
					affectedPlugs.append(_outputPlugs[i].elementByPhysicalIndex(j).child(k));
				}
			}
		}
	}
	
	return MS::kSuccess;
}

MStatus CoralNetworkNode::shouldSave(const MPlug &plug, bool &isSaving){
	isSaving = true;
	
	return MS::kSuccess;
}

void CoralNetworkNode::copyInternalData(MPxNode *node){
	MFnDependencyNode nodeFn(node->thisMObject());
	int coralNodeId = nodeFn.findPlug("coralNodeId").asInt();
	coral::Object *coralNodeObj = coral::NetworkManager::findObjectById(coralNodeId);
	if(coralNodeObj){
		coral::Node *coralNode = (coral::Node*)coralNodeObj;
		MString coralNodeName = coralNode->fullName().data();
		MString coralNodeParentName = coralNode->parent()->fullName().data();
		
		MString pythonCode = "from coralMaya import coralMayaApp;";
		pythonCode += "from coral import coralApp;";
		pythonCode += "coralMayaApp.FiberMayaAppData._associateCoralNetworkNode = '" + name() + "';";
		pythonCode += "coralApp.executeCommand('Paste', nodes = ['" + coralNodeName + "'], parentNode = '" + coralNodeParentName + "');";
		pythonCode += "coralMayaApp.FiberMayaAppData._associateCoralNetworkNode = '';";
		MGlobal::displayInfo("python(\"" + pythonCode + "\");");
		MGlobal::executeCommand("python(\"" + pythonCode + "\");");
	}
}

MStatus CoralNetworkNode::initialize(){
	MFnNumericAttribute numericAttr;
	MFnTypedAttribute typedAttr;
	
	coralNodeId = numericAttr.create("coralNodeId", "fni", MFnNumericData::kLong);
	saveData = typedAttr.create("saveData", "svd", MFnData::kString);
	
	addAttribute(coralNodeId);
	addAttribute(saveData);
	
    return MS::kSuccess;
}
