
#ifndef CORAL_PARALLEL_TBB
	#include <tbb/mutex.h>
#endif

#include "PolyNodes.h"
#include "../src/containerUtils.h"


using namespace coral;

BuildArray::BuildArray(const std::string &name, Node *parent):
	Node(name, parent),
	_selectedOperation(0){
	setSliceable(true);

	setAllowDynamicAttributes(true);

	_array = new PolyAttribute("array", this);
	addOutputAttribute(_array);
}

void BuildArray::addAttribute(){
	std::string numStr = stringUtils::intToString(inputAttributes().size());
	PolyAttribute *attr = new PolyAttribute("in" + numStr, this);
	addInputAttribute(attr);
	setAttributeAffect(attr, _array);

	std::vector<std::string> spec;
	spec.push_back("Int");
	spec.push_back("Float");
	spec.push_back("Vec3");
	spec.push_back("Col4");
	spec.push_back("Matrix44");
	spec.push_back("String");
	spec.push_back("Path");
	spec.push_back("Bool");
	setAttributeAllowedSpecializations(attr, spec);

	addAttributeSpecializationLink(attr, _array);

	addDynamicAttribute(attr);
	updateAttributeSpecialization(attr);
}

void BuildArray::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(specializationA.size() <= specializationB.size()){
		specializationB.resize(specializationA.size());
		for(int i = 0; i < specializationA.size(); ++i){
			specializationB[i] = specializationA[i] + "Array";
		}
	}
	else{
		specializationA.resize(specializationB.size());
		for(int i = 0; i < specializationB.size(); ++i){
			specializationA[i] = stringUtils::strip(specializationB[i], "Array");
		}
	}
}

void BuildArray::attributeSpecializationChanged(Attribute *attribute){
	std::cout << "BuildArray.attributeSpecializationChanged" <<std::endl;
	if(attribute == _array){
		_selectedOperation = 0;
		PolyValue::ValueType type = _array->outValue()->type();
		if(type != PolyValue::numericTypeAny &&
				type != PolyValue::typeAny &&
				type != PolyValue::stringTypeAny &&
				type != PolyValue::boolTypeAny){
			if(type == PolyValue::numericTypeIntArray){
				_selectedOperation = &BuildArray::updateInt;
			}
			else if(type == PolyValue::numericTypeFloatArray){
				_selectedOperation = &BuildArray::updateFloat;
			}
			else if(type == PolyValue::numericTypeVec3Array){
				_selectedOperation = &BuildArray::updateVec3;
			}
			else if(type == PolyValue::numericTypeCol4Array){
				_selectedOperation = &BuildArray::updateCol4;
			}
			else if(type == PolyValue::numericTypeMatrix44Array){
				_selectedOperation = &BuildArray::updateMatrix44;
			}
			else if (type == PolyValue::stringTypeArray) {
				_selectedOperation = &BuildArray::updateString;
			}
			else if (type == PolyValue::pathTypeArray) {
				_selectedOperation = &BuildArray::updatePath;
			}
			else if (type == PolyValue::boolTypeArray) {
				_selectedOperation = &BuildArray::updateBool;
			}
		}
	}
	std::cout << "BuildArray.attributeSpecializationChanged: Done" <<std::endl;
}

void BuildArray::updateInt(const std::vector<Attribute*> &inAttrs, int arraySize, PolyValue *array, unsigned int slice){
	std::vector<int> arrayValues(arraySize);
	for(int i = 0; i < arraySize; ++i){
		PolyAttribute *attr = (PolyAttribute*)inAttrs[i];
		PolyValue *inNum = attr->value();
		arrayValues[i] = inNum->intValueAtSlice(slice, 0);
	}
	array->setIntValuesSlice(slice, arrayValues);
}

void BuildArray::updateFloat(const std::vector<Attribute*> &inAttrs, int arraySize, PolyValue *array, unsigned int slice){
	std::vector<float> arrayValues(arraySize);
	for(int i = 0; i < arraySize; ++i){
		PolyAttribute *attr = (PolyAttribute*)inAttrs[i];
		PolyValue *inNum = attr->value();
		arrayValues[i] = inNum->floatValueAtSlice(slice, 0);
	}

	array->setFloatValuesSlice(slice, arrayValues);
}

void BuildArray::updateVec3(const std::vector<Attribute*> &inAttrs, int arraySize, PolyValue *array, unsigned int slice){
	std::vector<Imath::V3f> arrayValues(arraySize);
	for(int i = 0; i < arraySize; ++i){
		PolyAttribute *attr = (PolyAttribute*)inAttrs[i];
		PolyValue *inNum = attr->value();
		arrayValues[i] = inNum->vec3ValueAtSlice(slice, 0);
	}
	array->setVec3ValuesSlice(slice, arrayValues);
}

void BuildArray::updateCol4(const std::vector<Attribute*> &inAttrs, int arraySize, PolyValue *array, unsigned int slice){
	std::vector<Imath::Color4f> arrayValues(arraySize);
	for(int i = 0; i < arraySize; ++i){
		PolyAttribute *attr = (PolyAttribute*)inAttrs[i];
		PolyValue *inNum = attr->value();
		arrayValues[i] = inNum->col4ValueAtSlice(slice, 0);
	}
	array->setCol4ValuesSlice(slice, arrayValues);
}

void BuildArray::updateMatrix44(const std::vector<Attribute*> &inAttrs, int arraySize, PolyValue *array, unsigned int slice){
	std::vector<Imath::M44f> arrayValues(arraySize);
	for(int i = 0; i < arraySize; ++i){
		PolyAttribute *attr = (PolyAttribute*)inAttrs[i];
		PolyValue *inNum = attr->value();
		arrayValues[i] = inNum->matrix44ValueAtSlice(slice, 0);
	}
	array->setMatrix44ValuesSlice(slice, arrayValues);
}

void BuildArray::updateString(const std::vector<Attribute*> &inAttrs, int arraySize, PolyValue *array, unsigned int slice){
//	std::cout << "BuildArray.updateString" << std::endl;
	std::vector<std::string> arrayValues(arraySize);
	for (int i=0; i < arraySize; ++i){
		PolyAttribute *attr = (PolyAttribute*)inAttrs[i];
		PolyValue *inVal = attr->value();
		arrayValues[i] = inVal->stringValueAtSlice(slice, 0);
	}
	array->setStringValuesSlice(slice, arrayValues);
//	std::cout << "BuildArray.updateString: Done" << std::endl;
}

void BuildArray::updatePath(const std::vector<Attribute*> &inAttrs, int arraySize, PolyValue *array, unsigned int slice){
	std::vector<std::string> arrayValues(arraySize);
	for (int i=0; i < arraySize; ++i){
		PolyAttribute *attr = (PolyAttribute*)inAttrs[i];
		PolyValue *inVal = attr->value();
		arrayValues[i] = inVal->pathValueAtSlice(slice, 0);
	}
	array->setPathValuesSlice(slice, arrayValues);
}

void BuildArray::updateBool(const std::vector<Attribute*> &inAttrs, int arraySize, PolyValue *array, unsigned int slice){
	std::vector<bool> arrayValues(arraySize);
	for (int i=0; i<arraySize; ++i){
		PolyAttribute *attr = (PolyAttribute*)inAttrs[i];
		PolyValue *inVal = attr->value();
		arrayValues[i] = inVal->boolValueAtSlice(slice, 0);
	}
	array->setBoolValuesSlice(slice, arrayValues);
}

void BuildArray::updateSlice(Attribute *attribute, unsigned int slice){
//	std::cout << "BuildArray.updateSlice" <<std::endl;
	if(_selectedOperation){
		std::vector<Attribute*> inAttrs = inputAttributes();
		int arraySize = inAttrs.size();
		PolyValue *array = _array->outValue();

		(this->*_selectedOperation)(inAttrs, arraySize, array, slice);
	}
//	std::cout << "BuildArray.updateSlice: Done" <<std::endl;
}


ConstantArray::ConstantArray(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);

	_size = new NumericAttribute("size", this);
	_constant = new PolyAttribute("constant", this);
	_array = new PolyAttribute("array", this);

	addInputAttribute(_size);
	addInputAttribute(_constant);
	addOutputAttribute(_array);

	setAttributeAffect(_size, _array);
	setAttributeAffect(_constant, _array);

	std::vector<std::string> constantSpecializations;
	constantSpecializations.push_back("Int");
	constantSpecializations.push_back("Float");
	constantSpecializations.push_back("Vec3");
	constantSpecializations.push_back("Col4");
	constantSpecializations.push_back("Matrix44");
	constantSpecializations.push_back("String");
	constantSpecializations.push_back("Bool");
	constantSpecializations.push_back("Path");

	std::vector<std::string> arraySpecializations;
	arraySpecializations.push_back("IntArray");
	arraySpecializations.push_back("FloatArray");
	arraySpecializations.push_back("Vec3Array");
	arraySpecializations.push_back("Col4Array");
	arraySpecializations.push_back("Matrix44Array");
	arraySpecializations.push_back("StringArray");
	arraySpecializations.push_back("PathArray");
	arraySpecializations.push_back("BoolArray");

	setAttributeAllowedSpecialization(_size, "Int");
	setAttributeAllowedSpecializations(_constant, constantSpecializations);
	setAttributeAllowedSpecializations(_array, arraySpecializations);

	addAttributeSpecializationLink(_constant, _array);
}

void ConstantArray::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(specializationA.size() < specializationB.size()){
		std::vector<std::string> newSpecialization;

		for(int i = 0; i < specializationA.size(); ++i){
			newSpecialization.push_back(specializationA[i] + "Array");
		}

		specializationB = newSpecialization;
	}
	else{
		std::vector<std::string> newSpecialization;

		for(int i = 0; i < specializationB.size(); ++i){
			newSpecialization.push_back(stringUtils::replace(specializationB[i], "Array", ""));
		}

		specializationA = newSpecialization;
	}
}

void ConstantArray::updateSlice(Attribute *attribute, unsigned int slice){
	PolyValue *constant = _constant->value();
	if(constant->type() != PolyValue::numericTypeAny &&
			constant->type() != PolyValue::typeAny &&
			constant->type() != PolyValue::stringTypeAny &&
			constant->type() != PolyValue::boolTypeAny){
		int size = _size->value()->intValueAtSlice(slice, 0);
		if(size < 0){
			size = 0;
			_size->outValue()->setIntValueAtSlice(slice, 0, 0);
		}

		PolyValue *array = _array->outValue();
		if(constant->type() == PolyValue::numericTypeInt){
			int constantValue = constant->intValueAtSlice(slice, 0);
			std::vector<int> values(size);
			for(int i = 0; i < size; ++i){
				values[i] = constantValue;
			}
			array->setIntValuesSlice(slice, values);
		}
		else if(constant->type() == PolyValue::numericTypeFloat){
			float constantValue = constant->floatValueAtSlice(slice, 0);
			std::vector<float> values(size);
			for(int i = 0; i < size; ++i){
				values[i] = constantValue;
			}
			array->setFloatValuesSlice(slice, values);
		}
		else if(constant->type() == PolyValue::numericTypeVec3){
			Imath::V3f constantValue = constant->vec3ValueAtSlice(slice, 0);
			std::vector<Imath::V3f> values(size);
			for(int i = 0; i < size; ++i){
				values[i] = constantValue;
			}
			array->setVec3ValuesSlice(slice, values);
		}
		else if(constant->type() == PolyValue::numericTypeCol4){
			Imath::Color4f constantValue = constant->col4ValueAtSlice(slice, 0);
			std::vector<Imath::Color4f> values(size);
			for(int i = 0; i < size; ++i){
				values[i] = constantValue;
			}
			array->setCol4ValuesSlice(slice, values);
		}
		else if(constant->type() == PolyValue::numericTypeMatrix44){
			Imath::M44f constantValue = constant->matrix44ValueAtSlice(slice, 0);
			std::vector<Imath::M44f> values(size);
			for(int i = 0; i < size; ++i){
				values[i] = constantValue;
			}
			array->setMatrix44ValuesSlice(slice, values);
		}
		else if (constant->type() == PolyValue::stringType){
			std::string constantValue = constant->stringValueAtSlice(slice, 0);
			std::vector<std::string> values(size);
			for (int i=i; i<size; ++i){
				values[i] = constantValue;
			}
			array->setStringValuesSlice(slice, values);
		}
		else if (constant->type() == PolyValue::pathType){
			std::string constantValue = constant->stringValueAtSlice(slice, 0);
			std::vector<std::string> values(size);
			for (int i=0; i<size; ++i){
				values[i] = constantValue;
			}
			array->setStringValuesSlice(slice, values);
		}
		else if (constant->type() == PolyValue::boolType){
			bool constantValue = constant->boolValueAtSlice(slice, 0);
			std::vector<bool> values(size);
			for (int i=0; i<size; ++i){
				values[i] = constantValue;
			}
			array->setBoolValuesSlice(slice, values);
		}
	}
	else{
		setAttributeIsClean(_array, false);
	}
}


GetArrayElement::GetArrayElement(const std::string &name, Node *parent):
	Node(name, parent),
	_selectedOperation(0){
	setSliceable(true);

	_array = new PolyAttribute("array", this);
	_index = new NumericAttribute("index", this);
	_element = new PolyAttribute("element", this);

	addInputAttribute(_array);
	addInputAttribute(_index);
	addOutputAttribute(_element);

	setAttributeAffect(_array, _element);
	setAttributeAffect(_index, _element);

	std::vector<std::string> arraySpec;
	arraySpec.push_back("IntArray");
	arraySpec.push_back("FloatArray");
	arraySpec.push_back("Vec3Array");
	arraySpec.push_back("Col4Array");
	arraySpec.push_back("Matrix44Array");
	arraySpec.push_back("StringArray");
	arraySpec.push_back("PathArray");
	arraySpec.push_back("BoolArray");

	std::vector<std::string> elementSpec;
	elementSpec.push_back("Int");
	elementSpec.push_back("IntArray");
	elementSpec.push_back("Float");
	elementSpec.push_back("FloatArray");
	elementSpec.push_back("Vec3");
	elementSpec.push_back("Vec3Array");
	elementSpec.push_back("Col4");
	elementSpec.push_back("Col4Array");
	elementSpec.push_back("Matrix44");
	elementSpec.push_back("Matrix44Array");
	elementSpec.push_back("String");
	elementSpec.push_back("StringArray");
	elementSpec.push_back("Path");
	elementSpec.push_back("PathArray");
	elementSpec.push_back("Bool");
	elementSpec.push_back("BoolArray");
	std::vector<std::string> indexSpec;
	indexSpec.push_back("Int");
	indexSpec.push_back("IntArray");

	setAttributeAllowedSpecializations(_array, arraySpec);
	setAttributeAllowedSpecializations(_index, indexSpec);
	setAttributeAllowedSpecializations(_element, elementSpec);

	addAttributeSpecializationLink(_array, _element);
	addAttributeSpecializationLink(_index, _element);
}

void GetArrayElement::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(attributeA == _array){
		std::vector<std::string> newSpecA;
		for(int i = 0; i < specializationA.size(); ++i){
			std::string &specA = specializationA[i];
			std::string typeA = stringUtils::strip(specA, "Array");
			for(int j = 0; j < specializationB.size(); ++j){
				std::string typeB = stringUtils::strip(specializationB[j], "Array");
				if(typeA == typeB){
					newSpecA.push_back(specA);
					break;
				}
			}
		}

		std::vector<std::string> newSpecB;
		for(int i = 0; i < specializationB.size(); ++i){
			std::string &specB = specializationB[i];
			std::string typeB = stringUtils::strip(specB, "Array");
			for(int j = 0; j < specializationA.size(); ++j){
				std::string typeA = stringUtils::strip(specializationA[j], "Array");
				if(typeB == typeA){
					newSpecB.push_back(specB);
					break;
				}
			}
		}

		specializationA = newSpecA;
		specializationB = newSpecB;
	}
	else{
		if(specializationA.size() == 1){
			std::string specASuffix = "";
			if(stringUtils::endswith(specializationA[0], "Array")){
				specASuffix = "Array";
			}

			std::vector<std::string> newSpecB;
			for(int i = 0; i < specializationB.size(); ++i){
				std::string &specB = specializationB[i];

				std::string specBSuffix = "";
				if(stringUtils::endswith(specB, "Array")){
					specBSuffix = "Array";
				}

				if(specASuffix == specBSuffix){
					newSpecB.push_back(specB);
				}
			}

			specializationB = newSpecB;
		}
		else{
			std::vector<std::string> newSpecA;
			for(int i = 0; i < specializationB.size(); ++i){
				std::string &specB = specializationB[i];
				if(!stringUtils::endswith(specB, "Array")){
					newSpecA.push_back("Int");
					break;
				}
			}

			for(int i = 0; i < specializationB.size(); ++i){
				std::string &specB = specializationB[i];
				if(stringUtils::endswith(specB, "Array")){
					newSpecA.push_back("IntArray");
					break;
				}
			}

			specializationA = newSpecA;
		}
	}
}

void GetArrayElement::attributeSpecializationChanged(Attribute *attribute){
	if(attribute == _array){
		_selectedOperation = 0;

		PolyValue::ValueType type = _array->outValue()->type();
		if(type != PolyValue::numericTypeAny &&
				type != PolyValue::typeAny &&
				type != PolyValue::stringTypeAny &&
				type != PolyValue::boolTypeAny){
			if(type == PolyValue::numericTypeIntArray){
				_selectedOperation = &GetArrayElement::updateInt;
			}
			else if(type == PolyValue::numericTypeFloatArray){
				_selectedOperation = &GetArrayElement::updateFloat;
			}
			else if(type == PolyValue::numericTypeVec3Array){
				_selectedOperation = &GetArrayElement::updateVec3;
			}
			else if(type == PolyValue::numericTypeCol4Array){
				_selectedOperation = &GetArrayElement::updateCol4;
			}
			else if(type == PolyValue::numericTypeMatrix44Array){
				_selectedOperation = &GetArrayElement::updateMatrix44;
			}
			else if (type == PolyValue::stringTypeArray){
				_selectedOperation = &GetArrayElement::updateString;
			}
			else if (type == PolyValue::pathTypeArray){
				_selectedOperation = &GetArrayElement::updatePath;
			}
			else if (type == PolyValue::boolTypeArray){
				_selectedOperation = &GetArrayElement::updateBool;
			}
		}
	}
}

void GetArrayElement::updateInt(PolyValue *array, const std::vector<int> &index, PolyValue *element, unsigned int slice){
	int size = index.size();
	element->resize(size);
	for(int i = 0; i < size; ++i){
		element->setIntValueAtSlice(slice, i, array->intValueAtSlice(slice, index[i]));
	}
}

void GetArrayElement::updateFloat(PolyValue *array,  const std::vector<int> &index, PolyValue *element, unsigned int slice){
	int size = index.size();
	element->resize(size);
	for(int i = 0; i < size; ++i){
		element->setFloatValueAtSlice(slice, i, array->floatValueAtSlice(slice, index[i]));
	}
}

void GetArrayElement::updateVec3(PolyValue *array,  const std::vector<int> &index, PolyValue *element, unsigned int slice){
	int size = index.size();
	element->resize(size);
	for(int i = 0; i < size; ++i){
		element->setVec3ValueAtSlice(slice, i, array->vec3ValueAtSlice(slice, index[i]));
	}
}

void GetArrayElement::updateCol4(PolyValue *array, const std::vector<int> &index, PolyValue *element, unsigned int slice){
	int size = index.size();
	element->resize(size);
	for(int i = 0; i < size; ++i){
		element->setCol4ValueAtSlice(slice, i, array->col4ValueAtSlice(slice, index[i]));
	}
}

void GetArrayElement::updateMatrix44(PolyValue *array,  const std::vector<int> &index, PolyValue *element, unsigned int slice){
	int size = index.size();
	element->resize(size);
	for(int i = 0; i < size; ++i){
		element->setMatrix44ValueAtSlice(slice, i, array->matrix44ValueAtSlice(slice, index[i]));
	}
}

void GetArrayElement::updateString(PolyValue *array, const std::vector<int> &index, PolyValue *element, unsigned int slice){
	int size = index.size();
	element->resize(size);
	for (int i=0; i<size; ++i){
		std::string val = array->stringValueAtSlice(slice, index[i]);
		element->setStringValueAtSlice(slice, i, val);
	}
}

void GetArrayElement::updatePath(PolyValue *array, const std::vector<int> &index, PolyValue *element, unsigned int slice){
	int size = index.size();
	element->resize(size);
	for (int i=0; i<size; ++i){
		std::string val = array->pathValueAtSlice(slice, index[i]);
		element->setPathValueAtSlice(slice, i, val);
	}
}

void GetArrayElement::updateBool(PolyValue *array, const std::vector<int> &index, PolyValue *element, unsigned int slice){
	int size = index.size();
	element->resize(size);
	for (int i=0; i<size; ++i){
		element->setBoolValueAtSlice(slice, i, array->boolValueAtSlice(slice, index[i]));
	}
}

void GetArrayElement::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		PolyValue *array = _array->value();
		std::vector<int> index = _index->value()->intValuesSlice(slice);
		PolyValue *element = _element->outValue();

		(this->*_selectedOperation)(array, index, element, slice);
	}
}


SetArrayElement::SetArrayElement(const std::string &name, Node *parent):
Node(name, parent),
_selectedOperation(0){
	setSliceable(true);

	_array = new PolyAttribute("array", this);
	_index = new NumericAttribute("index", this);
	_element = new PolyAttribute("element", this);
	_outArray = new PolyAttribute("outArray", this);

	addInputAttribute(_array);
	addInputAttribute(_index);
	addInputAttribute(_element);
	addOutputAttribute(_outArray);

	setAttributeAffect(_array, _outArray);
	setAttributeAffect(_element, _outArray);
	setAttributeAffect(_index, _outArray);

	std::vector<std::string> elementSpecs;
	elementSpecs.push_back("Int");
	elementSpecs.push_back("IntArray");
	elementSpecs.push_back("Float");
	elementSpecs.push_back("FloatArray");
	elementSpecs.push_back("Vec3");
	elementSpecs.push_back("Vec3Array");
	elementSpecs.push_back("Col4");
	elementSpecs.push_back("Col4Array");
	elementSpecs.push_back("Matrix44");
	elementSpecs.push_back("Matrix44Array");
	elementSpecs.push_back("String");
	elementSpecs.push_back("StringArray");
	elementSpecs.push_back("Path");
	elementSpecs.push_back("PathArray");
	elementSpecs.push_back("Bool");
	elementSpecs.push_back("BoolArray");

	std::vector<std::string> arraySpecs;
	arraySpecs.push_back("IntArray");
	arraySpecs.push_back("FloatArray");
	arraySpecs.push_back("Vec3Array");
	arraySpecs.push_back("Col4Array");
	arraySpecs.push_back("Matrix44Array");
	arraySpecs.push_back("StringArray");
	arraySpecs.push_back("PathArray");
	arraySpecs.push_back("BoolArray");

	std::vector<std::string> indexSpec;
	indexSpec.push_back("Int");
	indexSpec.push_back("IntArray");

	setAttributeAllowedSpecializations(_array, arraySpecs);
	setAttributeAllowedSpecializations(_element, elementSpecs);
	setAttributeAllowedSpecializations(_index, indexSpec);
	setAttributeAllowedSpecializations(_outArray, arraySpecs);

	addAttributeSpecializationLink(_array, _outArray);
	addAttributeSpecializationLink(_element, _outArray);
}

void SetArrayElement::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(attributeA == _array){
		Node::updateSpecializationLink(attributeA, attributeB, specializationA, specializationB);
	}
	else{
		std::vector<std::string> newSpecA;
		std::vector<std::string> newSpecB;
		for(int i = 0; i < specializationA.size(); ++i){
			std::string &specA = specializationA[i];
			std::string typeA = stringUtils::strip(specA, "Array");
			for(int j = 0; j < specializationB.size(); ++j){
				std::string &specB = specializationB[j];
				std::string typeB = stringUtils::strip(specB, "Array");
				if(typeA == typeB){
					if(!containerUtils::elementInContainer(specB, newSpecB)){
						newSpecB.push_back(specB);
					}
					if(!containerUtils::elementInContainer(specA, newSpecA)){
						newSpecA.push_back(specA);
					}
				}
			}
		}

		specializationA = newSpecA;
		specializationB = newSpecB;
	}
}

void SetArrayElement::attributeSpecializationChanged(Attribute *attribute){
	if(attribute == _array){
		_selectedOperation = 0;

		PolyValue::ValueType type = _array->outValue()->type();
		if(type != PolyValue::numericTypeAny &&
				type != PolyValue::typeAny &&
				type != PolyValue::stringTypeAny &&
				type != PolyValue::boolTypeAny){
			if(type == PolyValue::numericTypeIntArray){
				_selectedOperation = &SetArrayElement::updateInt;
			}
			else if(type == PolyValue::numericTypeFloatArray){
				_selectedOperation = &SetArrayElement::updateFloat;
			}
			else if(type == PolyValue::numericTypeVec3Array){
				_selectedOperation = &SetArrayElement::updateVec3;
			}
			else if(type == PolyValue::numericTypeCol4Array){
				_selectedOperation = &SetArrayElement::updateCol4;
			}
			else if(type == PolyValue::numericTypeMatrix44Array){
				_selectedOperation = &SetArrayElement::updateMatrix44;
			}
			else if(type == PolyValue::stringTypeArray){
				_selectedOperation = &SetArrayElement::updateString;
			}
			else if (type == PolyValue::pathTypeArray){
				_selectedOperation = &SetArrayElement::updatePath;
			}
			else if(type == PolyValue::boolTypeArray){
				_selectedOperation = &SetArrayElement::updateBool;
			}
		}
	}
}

void SetArrayElement::updateInt(PolyValue *array, const std::vector<int> &index, PolyValue *element, PolyValue *outArray, unsigned int slice){
	std::vector<int> values = array->intValuesSlice(slice);
	int valuesSize = array->sizeSlice(slice);
	for(int i = 0; i < index.size(); ++i){
		int currentIndex = index[i];
		if(currentIndex >= 0 && currentIndex < valuesSize){
			values[currentIndex] = element->intValueAtSlice(slice, i);
		}
	}

	outArray->setIntValuesSlice(slice, values);
}

void SetArrayElement::updateFloat(PolyValue *array, const std::vector<int> &index, PolyValue *element, PolyValue *outArray, unsigned int slice){
	std::vector<float> values = array->floatValuesSlice(slice);
	int valuesSize = array->sizeSlice(slice);
	for(int i = 0; i < index.size(); ++i){
		int currentIndex = index[i];
		if(currentIndex >= 0 && currentIndex < valuesSize){
			values[currentIndex] = element->floatValueAtSlice(slice, i);
		}
	}

	outArray->setFloatValuesSlice(slice, values);
}

void SetArrayElement::updateVec3(PolyValue *array, const std::vector<int> &index, PolyValue *element, PolyValue *outArray, unsigned int slice){
	std::vector<Imath::V3f> values = array->vec3ValuesSlice(slice);
	int valuesSize = array->sizeSlice(slice);
	for(int i = 0; i < index.size(); ++i){
		int currentIndex = index[i];
		if(currentIndex >= 0 && currentIndex < valuesSize){
			values[currentIndex] = element->vec3ValueAtSlice(slice, i);
		}
	}

	outArray->setVec3ValuesSlice(slice, values);
}

void SetArrayElement::updateCol4(PolyValue *array, const std::vector<int> &index, PolyValue *element, PolyValue *outArray, unsigned int slice){
	std::vector<Imath::Color4f> values = array->col4ValuesSlice(slice);
	int valuesSize = array->sizeSlice(slice);
	for(int i = 0; i < index.size(); ++i){
		int currentIndex = index[i];
		if(currentIndex >= 0 && currentIndex < valuesSize){
			values[currentIndex] = element->col4ValueAtSlice(slice, i);
		}
	}

	outArray->setCol4ValuesSlice(slice, values);
}

void SetArrayElement::updateMatrix44(PolyValue *array, const std::vector<int> &index, PolyValue *element, PolyValue *outArray, unsigned int slice){
	std::vector<Imath::M44f> values = array->matrix44ValuesSlice(slice);
	int valuesSize = array->sizeSlice(slice);
	for(int i = 0; i < index.size(); ++i){
		int currentIndex = index[i];
		if(currentIndex >= 0 && currentIndex < valuesSize){
			values[currentIndex] = element->matrix44ValueAtSlice(slice, i);
		}
	}

	outArray->setMatrix44ValuesSlice(slice, values);
}

void SetArrayElement::updateString(PolyValue *array, const std::vector<int> &index, PolyValue *element, PolyValue *outArray, unsigned int slice){
	std::vector<std::string> values = array->stringValuesSlice(slice);
	int valuesSize = array->sizeSlice(slice);
	for (int i=0; i < index.size(); ++i){
		int currentIndex = index[i];
		if (currentIndex >= 0 && currentIndex < valuesSize){
			values[currentIndex] = element->stringValueAtSlice(slice, i);
		}
	}
	outArray->setStringValuesSlice(slice, values);
}

void SetArrayElement::updatePath(PolyValue *array, const std::vector<int> &index, PolyValue *element, PolyValue *outArray, unsigned int slice){
	std::vector<std::string> values = array->pathValuesSlice(slice);
	int valueSize = array->sizeSlice(slice);
	for (int i=0; i<index.size(); ++i){
		int currentIndex = index[i];
		if (currentIndex >= 0 && currentIndex < valueSize){
			values[currentIndex] = element->stringValueAtSlice(slice, i);
		}
	}
	outArray->setPathValuesSlice(slice, values);
}

void SetArrayElement::updateBool(PolyValue *array, const std::vector<int> &index, PolyValue *element, PolyValue *outArray, unsigned int slice){
	std::vector<bool> values = array->boolValuesSlice(slice);
	int valuesSize = array->sizeSlice(slice);
	for (int i=0; i<index.size(); ++i){
		int currentIndex = index[i];
		if (currentIndex >= 0 && currentIndex < valuesSize){
			values[currentIndex] = element->boolValueAtSlice(slice, i);
		}
	}
	outArray->setBoolValuesSlice(slice, values);
}

void SetArrayElement::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		PolyValue *array = _array->value();
		const std::vector<int> &index = _index->value()->intValuesSlice(slice);
		PolyValue *element = _element->value();
		PolyValue *outArray = _outArray->outValue();

		(this->*_selectedOperation)(array, index, element, outArray, slice);
	}
}

