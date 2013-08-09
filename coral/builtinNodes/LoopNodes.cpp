#include "LoopNodes.h"
#include "../src/Numeric.h"
#include "../src/containerUtils.h"
#include "../src/NetworkManager.h"
#include "../src/AttributeAccessor.h"
#include "../src/stringUtils.h"

using namespace coral;


LoopInputNode::LoopInputNode(const std::string &name, Node *parent):
Node(name, parent),
_selectedOperation(0){
	setSliceable(true);

	_globalArray = new PolyAttribute("globalArray", this);
	_localElement = new PolyAttribute("localElement", this);
	_localIndex = new NumericAttribute("localIndex", this);

	addInputAttribute(_globalArray);
	addOutputAttribute(_localIndex);
	addOutputAttribute(_localElement);

	setAttributeAffect(_globalArray, _localIndex);
	setAttributeAffect(_globalArray, _localElement);

	std::vector<std::string> arraySpec;
	arraySpec.push_back("IntArray");
	arraySpec.push_back("FloatArray");
	arraySpec.push_back("Vec3Array");
	arraySpec.push_back("Col4Array");
	arraySpec.push_back("Matrix44Array");
	arraySpec.push_back("StringArray");
	arraySpec.push_back("PathArray");
	arraySpec.push_back("BoolArray");
	setAttributeAllowedSpecializations(_globalArray, arraySpec);

	std::vector<std::string> elementSpec;
	elementSpec.push_back("Int");
	elementSpec.push_back("Float");
	elementSpec.push_back("Vec3");
	elementSpec.push_back("Col4");
	elementSpec.push_back("Matrix44");
	elementSpec.push_back("String");
	elementSpec.push_back("Path");
	elementSpec.push_back("Bool");
	setAttributeAllowedSpecializations(_localElement, elementSpec);

	setAttributeAllowedSpecialization(_localIndex, "Int");

	addAttributeSpecializationLink(_globalArray, _localElement);
}

void LoopInputNode::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(specializationA.size() < specializationB.size()){
		specializationB.resize(specializationA.size());
		for(int i = 0; i < specializationA.size(); ++i){
			specializationB[i] = stringUtils::strip(specializationA[i], "Array");
		}
	}
	else if(specializationB.size() < specializationA.size()){
		specializationA.resize(specializationB.size());
		for(int i = 0; i < specializationB.size(); ++i){
			specializationA[i] = specializationB[i] + "Array";
		}
	}
}

void LoopInputNode::attributeSpecializationChanged(Attribute *attribute){
	if(attribute == _globalArray){
		PolyValue::ValueType type = _globalArray->outValue()->type();
		if(type != PolyValue::numericTypeAny &&
				type != PolyValue::typeAny &&
				type != PolyValue::stringTypeAny &&
				type != PolyValue::boolTypeAny){
			if(type == PolyValue::numericTypeIntArray){
				_selectedOperation = &LoopInputNode::updateInt;
			}
			else if(type == PolyValue::numericTypeFloatArray){
				_selectedOperation = &LoopInputNode::updateFloat;
			}
			else if(type == PolyValue::numericTypeVec3Array){
				_selectedOperation = &LoopInputNode::updateVec3;
			}
			else if(type == PolyValue::numericTypeCol4Array){
				_selectedOperation = &LoopInputNode::updateCol4;
			}
			else if(type == PolyValue::numericTypeMatrix44Array){
				_selectedOperation = &LoopInputNode::updateMatrix44;
			}
			else if (type == PolyValue::stringTypeArray){
				_selectedOperation = &LoopInputNode::updateString;
			}
			else if (type == PolyValue::pathTypeArray){
				_selectedOperation = &LoopInputNode::updatePath;
			}
			else if (type == PolyValue::boolTypeArray) {
				_selectedOperation = &LoopInputNode::updateBool;
			}
		}
	}
}

void LoopInputNode::updateInt(unsigned int slice, PolyValue *globalArray, PolyValue *localElement){
	localElement->setIntValueAtSlice(slice, 0, globalArray->intValueAtSlice(0, slice));
}

void LoopInputNode::updateFloat(unsigned int slice, PolyValue *globalArray, PolyValue *localElement){
	localElement->setFloatValueAtSlice(slice, 0, globalArray->floatValueAtSlice(0, slice));
}

void LoopInputNode::updateVec3(unsigned int slice, PolyValue *globalArray, PolyValue *localElement){
	localElement->setVec3ValueAtSlice(slice, 0, globalArray->vec3ValueAtSlice(0, slice));
}

void LoopInputNode::updateCol4(unsigned int slice, PolyValue *globalArray, PolyValue *localElement){
	localElement->setCol4ValueAtSlice(slice, 0, globalArray->col4ValueAtSlice(0, slice));
}

void LoopInputNode::updateMatrix44(unsigned int slice, PolyValue *globalArray, PolyValue *localElement){
	localElement->setMatrix44ValueAtSlice(slice, 0, globalArray->matrix44ValueAtSlice(0, slice));
}

void LoopInputNode::updateString(unsigned int slice, PolyValue *globalArray, PolyValue *localElement){
	std::string v = globalArray->stringValueAtSlice(0, slice);
	localElement->setStringValueAtSlice(slice, 0, v);
}

void LoopInputNode::updatePath(unsigned int slice, PolyValue *globalArray, PolyValue *localElement){
	std::string v = globalArray->pathValueAtSlice(0, slice);
	localElement->setPathValueAtSlice(slice, 0, v);
}

void LoopInputNode::updateBool(unsigned int slice, PolyValue *globalArray, PolyValue *localElement){
	localElement->setBoolValueAtSlice(slice, 0, globalArray->boolValueAtSlice(0, slice));
}

void LoopInputNode::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		PolyValue *globalArray = _globalArray->value();
		PolyValue *localElement = _localElement->outValue();
		Numeric *localIndex = _localIndex->outValue();

		localIndex->setIntValueAtSlice(slice, 0, slice);

		(this->*_selectedOperation)(slice, globalArray, localElement);
	}
}

LoopOutputNode::LoopOutputNode(const std::string &name, Node* parent): 
Node(name, parent),
_selectedOperation(0){
	setSliceable(true);
	
	_localElement = new PolyAttribute("localElement", this);
	_globalArray = new PolyAttribute("globalArray", this);

	addInputAttribute(_localElement);
	addOutputAttribute(_globalArray);

	setAttributeAffect(_localElement, _globalArray);
	
	std::vector<std::string> elementSpec;
	elementSpec.push_back("Int");
	elementSpec.push_back("Float");
	elementSpec.push_back("Vec3");
	elementSpec.push_back("Col4");
	elementSpec.push_back("Matrix44");
	elementSpec.push_back("String");
	elementSpec.push_back("Path");
	elementSpec.push_back("Bool");
	setAttributeAllowedSpecializations(_localElement, elementSpec);
	
	std::vector<std::string> arraySpec;
	arraySpec.push_back("IntArray");
	arraySpec.push_back("FloatArray");
	arraySpec.push_back("Vec3Array");
	arraySpec.push_back("Col4Array");
	arraySpec.push_back("Matrix44Array");
	arraySpec.push_back("StringArray");
	arraySpec.push_back("PathArray");
	arraySpec.push_back("BoolArray");
	setAttributeAllowedSpecializations(_globalArray, arraySpec);

	addAttributeSpecializationLink(_localElement, _globalArray);
}

void LoopOutputNode::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(specializationA.size() < specializationB.size()){
		specializationB.resize(specializationA.size());
		for(int i = 0; i < specializationA.size(); ++i){
			specializationB[i] = specializationA[i] + "Array";
		}
	}
	else if(specializationB.size() < specializationA.size()){
		specializationA.resize(specializationB.size());
		for(int i = 0; i < specializationB.size(); ++i){
			specializationA[i] = stringUtils::strip(specializationB[i], "Array");
		}
	}
}

void LoopOutputNode::attributeSpecializationChanged(Attribute *attribute){
	if(attribute == _globalArray){
		PolyValue::ValueType type = _globalArray->outValue()->type();
		if(type != PolyValue::numericTypeAny &&
				type != PolyValue::typeAny &&
				type != PolyValue::stringTypeAny &&
				type != PolyValue::boolTypeAny){
			if(type == PolyValue::numericTypeIntArray){
				_selectedOperation = &LoopOutputNode::updateInt;
			}
			else if(type == PolyValue::numericTypeFloatArray){
				_selectedOperation = &LoopOutputNode::updateFloat;
			}
			else if(type == PolyValue::numericTypeVec3Array){
				_selectedOperation = &LoopOutputNode::updateVec3;
			}
			else if(type == PolyValue::numericTypeCol4Array){
				_selectedOperation = &LoopOutputNode::updateCol4;
			}
			else if(type == PolyValue::numericTypeMatrix44Array){
				_selectedOperation = &LoopOutputNode::updateMatrix44;
			}
			else if(type == PolyValue::stringTypeArray){
				_selectedOperation = &LoopOutputNode::updateString;
			}
			else if(type == PolyValue::pathTypeArray){
				_selectedOperation = &LoopOutputNode::updatePath;
			}
			else if (type == PolyValue::boolTypeArray){
				_selectedOperation = &LoopOutputNode::updateBool;
			}
		}
	}
}

void LoopOutputNode::updateInt(unsigned int slices, PolyValue *element, PolyValue *array){
	for(int i = 0; i < slices; ++i){
		array->setIntValueAtSlice(0, i, element->intValueAtSlice(i, 0));
	}
}

void LoopOutputNode::updateFloat(unsigned int slices, PolyValue *element, PolyValue *array){
	for(int i = 0; i < slices; ++i){
		array->setFloatValueAtSlice(0, i, element->floatValueAtSlice(i, 0));
	}
}

void LoopOutputNode::updateVec3(unsigned int slices, PolyValue *element, PolyValue *array){
	for(int i = 0; i < slices; ++i){
		array->setVec3ValueAtSlice(0, i, element->vec3ValueAtSlice(i, 0));
	}
}

void LoopOutputNode::updateCol4(unsigned int slices, PolyValue *element, PolyValue *array){
	for(int i = 0; i < slices; ++i){
		array->setCol4ValueAtSlice(0, i, element->col4ValueAtSlice(i, 0));
	}
}

void LoopOutputNode::updateMatrix44(unsigned int slices, PolyValue *element, PolyValue *array){
	for(int i = 0; i < slices; ++i){
		array->setMatrix44ValueAtSlice(0, i, element->matrix44ValueAtSlice(i, 0));
	}
}
void LoopOutputNode::updateString(unsigned int slices, PolyValue *element, PolyValue *array){
	for (int i=0; i < slices; ++i){
		std::string v = element->stringValueAtSlice(i, 0);
		array->setStringValueAtSlice(0, i, v);
	}
}

void LoopOutputNode::updatePath(unsigned int slices, PolyValue *element, PolyValue *array){
	for (int i=0; i<slices; ++i){
		std::string v = element->pathValueAtSlice(i, 0);
		array->setPathValueAtSlice(0, i, v);
	}
}

void LoopOutputNode::updateBool(unsigned int slices, PolyValue *element, PolyValue *array){
	for (int i=0; i<slices; ++i){
		array->setBoolValueAtSlice(0, i, element->boolValueAtSlice(i, 0));
	}
}

void LoopOutputNode::update(Attribute *attribute){
	if(_selectedOperation){
		PolyValue *element = _localElement->value();
		PolyValue *array = _globalArray->outValue();

		unsigned int slices = element->slices();
		array->resizeSlice(0, slices);
		(this->*_selectedOperation)(slices, element, array);
	}
}

ForLoopNode::ForLoopNode(const std::string &name, Node *parent): 
Node(name, parent){
	setAllowDynamicAttributes(true);
	setIsSlicer(true);
	setUpdateEnabled(false);

	_globalArray = new PolyAttribute("globalArray", this);
	
	addInputAttribute(_globalArray);
	
	std::vector<std::string> arraySpec;
	arraySpec.push_back("IntArray");
	arraySpec.push_back("FloatArray");
	arraySpec.push_back("Vec3Array");
	arraySpec.push_back("Col4Array");
	arraySpec.push_back("Matrix44Array");
	arraySpec.push_back("StringArray");
	arraySpec.push_back("PathArray");
	arraySpec.push_back("BoolArray");
	setAttributeAllowedSpecializations(_globalArray, arraySpec);
}

unsigned int ForLoopNode::computeSlices(){
	return _globalArray->value()->sizeSlice(0);
}
