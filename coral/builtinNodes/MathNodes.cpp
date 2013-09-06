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

#include "MathNodes.h"
#include "../src/Numeric.h"
#include "../src/containerUtils.h"
#include "../src/mathUtils.h"

#include <ImathVec.h>
#include <ImathMatrix.h>
#include <ImathQuat.h>

#include <limits>

using namespace coral;

Length::Length(const std::string &name, Node *parent): 
Node(name, parent),
_selectedOperation(0){
	setSliceable(true);

	_element = new NumericAttribute("element", this);
	_length = new NumericAttribute("length", this);
	
	addInputAttribute(_element);
	addOutputAttribute(_length);
	
	setAttributeAffect(_element, _length);
	
	std::vector<std::string> vectorSpecializations;
	vectorSpecializations.push_back("Vec3");
	vectorSpecializations.push_back("Vec3Array");
	vectorSpecializations.push_back("Quat");
	vectorSpecializations.push_back("QuatArray");
	
	setAttributeAllowedSpecializations(_element, vectorSpecializations);
	
	std::vector<std::string> lengthSpecializations;
	lengthSpecializations.push_back("Float");
	lengthSpecializations.push_back("FloatArray");
	
	setAttributeAllowedSpecializations(_length, lengthSpecializations);
	
	addAttributeSpecializationLink(_element, _length);
}

void Length::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	std::vector<std::string> newSpecA;
	std::vector<std::string> newSpecB;
	for(int i = 0; i < specializationA.size(); ++i){
		bool isArrayA = stringUtils::endswith(specializationA[i], "Array");
		bool isArrayB = false;
		for(int j = 0; j < specializationB.size(); ++j){
			bool isArrayB = stringUtils::endswith(specializationB[j], "Array");
			if(isArrayA == isArrayB){
				newSpecA.push_back(specializationA[i]);
				if(!containerUtils::elementInContainer(specializationB[j], newSpecB)){
					newSpecB.push_back(specializationB[j]);
				}
				break;
			}
		}
	}

	specializationA = newSpecA;
	specializationB = newSpecB;
}

void Length::attributeSpecializationChanged(Attribute *attribute){
	_selectedOperation = 0;
	
	Numeric::Type type = _element->outValue()->type();
	
	if(type == Numeric::numericTypeVec3 || type == Numeric::numericTypeVec3Array){
		_selectedOperation = &Length::updateVec3;
	}
	else if(type == Numeric::numericTypeQuat || type == Numeric::numericTypeQuatArray){
		_selectedOperation = &Length::updateQuat;
	}
}

void Length::updateVec3(Numeric *element, Numeric *length, unsigned int slice){
	const std::vector<Imath::V3f> &elementValues = element->vec3ValuesSlice(slice);
	unsigned int size = elementValues.size();

	std::vector<float> lengthValues(size);
	for(int i = 0; i < size; ++i){
		float lengthValue = elementValues[i].length();
		lengthValues[i] = lengthValue;
	}

	length->setFloatValuesSlice(slice, lengthValues);
}

void Length::updateQuat(Numeric *element, Numeric *length, unsigned int slice){
	const std::vector<Imath::Quatf> &elementValues = element->quatValuesSlice(slice);
	unsigned int size = elementValues.size();

	std::vector<float> lengthValues(size);
	for(int i = 0; i < size; ++i){
		float lengthValue = elementValues[i].length();
		lengthValues[i] = lengthValue;
	}

	length->setFloatValuesSlice(slice, lengthValues);
}

void Length::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		(this->*_selectedOperation)(_element->value(), _length->outValue(), slice);
	}
}

Inverse::Inverse(const std::string &name, Node *parent): 
Node(name, parent),
_selectedOperation(0){	
	setSliceable(true);

	_element = new NumericAttribute("element", this);
	_inverse = new NumericAttribute("inverse", this);
	
	addInputAttribute(_element);
	addOutputAttribute(_inverse);
	
	setAttributeAffect(_element, _inverse);
	
	std::vector<std::string> specialization;
	specialization.push_back("Matrix44");
	specialization.push_back("Matrix44Array");
	specialization.push_back("Quat");
	specialization.push_back("QuatArray");
	
	setAttributeAllowedSpecializations(_element, specialization);
	setAttributeAllowedSpecializations(_inverse, specialization);
	
	addAttributeSpecializationLink(_element, _inverse);
}

void Inverse::attributeSpecializationChanged(Attribute *attribute){
	_selectedOperation = 0;
	
	Numeric::Type type = _element->outValue()->type();
	
	if(type == Numeric::numericTypeMatrix44 || type == Numeric::numericTypeMatrix44Array){
		_selectedOperation = &Inverse::updateMatrix44;
	}
	else if(type == Numeric::numericTypeQuat || type == Numeric::numericTypeQuatArray){
		_selectedOperation = &Inverse::updateQuat;
	}
}

void Inverse::updateMatrix44(Numeric *element, Numeric *inverse, unsigned int slice){
	std::vector<Imath::M44f> elementValues = element->matrix44ValuesSlice(slice);

	unsigned int size = elementValues.size();
	std::vector<Imath::M44f> inverseValues(size);
	for(int i = 0; i < size ;++i){
		inverseValues[i] = elementValues[i].inverse();
	}
	
	inverse->setMatrix44ValuesSlice(slice, inverseValues);
}

void Inverse::updateQuat(Numeric *element, Numeric *inverse, unsigned int slice){
	std::vector<Imath::Quatf> elementValues = element->quatValuesSlice(slice);
	
	unsigned int size = elementValues.size();
	std::vector<Imath::Quatf> inverseValues(size);
	for(int i = 0; i < size ;++i){
		inverseValues[i] = elementValues[i].inverse();
	}
	
	inverse->setQuatValuesSlice(slice, inverseValues);
}

void Inverse::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		(this->*_selectedOperation)(_element->value(), _inverse->outValue(), slice);
	}
}

Abs::Abs(const std::string &name, Node *parent): 
	Node(name, parent),
	_selectedOperation(0){
	setSliceable(true);
	
	_inNumber = new NumericAttribute("inNumber", this);
	_outNumber = new NumericAttribute("ouNumber", this);
	
	addInputAttribute(_inNumber);
	addOutputAttribute(_outNumber);
	
	setAttributeAffect(_inNumber, _outNumber);
	
	std::vector<std::string> specialization;
	specialization.push_back("Int");
	specialization.push_back("IntArray");
	specialization.push_back("Float");
	specialization.push_back("FloatArray");
	
	setAttributeAllowedSpecializations(_inNumber, specialization);
	setAttributeAllowedSpecializations(_outNumber, specialization);
	
	addAttributeSpecializationLink(_inNumber, _outNumber);
}

void Abs::attributeSpecializationChanged(Attribute *attribute){
	_selectedOperation = 0;

	Numeric::Type type = _inNumber->outValue()->type();
	
	if(type == Numeric::numericTypeInt || type == Numeric::numericTypeIntArray){
		_selectedOperation = &Abs::abs_int;
	}
	else if(type == Numeric::numericTypeFloat || type == Numeric::numericTypeFloatArray){
		_selectedOperation = &Abs::abs_float;
	}
}

void Abs::abs_int(Numeric *inNumber, Numeric *outNumber, unsigned int slice){
	std::vector<int> inValues = inNumber->intValuesSlice(slice);
	std::vector<int> outValues(inValues.size());
	
	for(int i = 0; i < inValues.size(); ++i){
		outValues[i] = abs(inValues[i]);
	}
	
	outNumber->setIntValuesSlice(slice, outValues);
}

void Abs::abs_float(Numeric *inNumber, Numeric *outNumber, unsigned int slice){
	std::vector<float> inValues = inNumber->floatValuesSlice(slice);
	std::vector<float> outValues(inValues.size());
	
	for(int i = 0; i < inValues.size(); ++i){
		outValues[i] = fabs(inValues[i]);
	}
	
	outNumber->setFloatValuesSlice(slice, outValues);
}

void Abs::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		(this->*_selectedOperation)(_inNumber->value(), _outNumber->outValue(), slice);
	}
}

CrossProduct::CrossProduct(const std::string &name, Node *parent): 
Node(name, parent){
	setSliceable(true);

	_vector0 = new NumericAttribute("vector0", this);
	_vector1 = new NumericAttribute("vector1", this);
	_crossProduct = new NumericAttribute("crossProduct", this);
	
	addInputAttribute(_vector0);
	addInputAttribute(_vector1);
	addOutputAttribute(_crossProduct);
	
	setAttributeAffect(_vector0, _crossProduct);
	setAttributeAffect(_vector1, _crossProduct);
	
	std::vector<std::string> specialization;
	specialization.push_back("Vec3");
	specialization.push_back("Vec3Array");
	setAttributeAllowedSpecializations(_vector0, specialization);
	setAttributeAllowedSpecializations(_vector1, specialization);
	setAttributeAllowedSpecializations(_crossProduct, specialization);
	
	addAttributeSpecializationLink(_vector0, _crossProduct);
	addAttributeSpecializationLink(_vector1, _crossProduct);
}

void CrossProduct::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<Imath::V3f> &vectorValues0 = _vector0->value()->vec3ValuesSlice(slice);
	const std::vector<Imath::V3f> &vectorValues1 = _vector1->value()->vec3ValuesSlice(slice);
	
	int size0 = vectorValues0.size();
	int size1 = vectorValues1.size();
	
	int minSize = size0;
	if(size0 < size1){
		minSize = size1;
	}
	
	std::vector<Imath::V3f> crossedValues(minSize);
	
	for(int i = 0; i < minSize; ++i){
		crossedValues[i] = vectorValues0[i].cross(vectorValues1[i]);
	}
	
	_crossProduct->outValue()->setVec3ValuesSlice(slice, crossedValues);
}

DotProduct::DotProduct(const std::string &name, Node *parent): 
Node(name, parent),
_selectedOperation(0){
	setSliceable(true);

	_element0 = new NumericAttribute("element0", this);
	_element1 = new NumericAttribute("element1", this);
	_dotProduct = new NumericAttribute("dotProduct", this);

	addInputAttribute(_element0);
	addInputAttribute(_element1);
	addOutputAttribute(_dotProduct);

	setAttributeAffect(_element0, _dotProduct);
	setAttributeAffect(_element1, _dotProduct);

	std::vector<std::string> specialization;
	specialization.push_back("Vec3");
	specialization.push_back("Vec3Array");
	specialization.push_back("Quat");
	specialization.push_back("QuatArray");

	std::vector<std::string> outSpecialization;
	outSpecialization.push_back("Float");
	outSpecialization.push_back("FloatArray");

	setAttributeAllowedSpecializations(_element0, specialization);
	setAttributeAllowedSpecializations(_element1, specialization);
	setAttributeAllowedSpecializations(_dotProduct, outSpecialization);

	addAttributeSpecializationLink(_element0, _dotProduct);
	addAttributeSpecializationLink(_element1, _dotProduct);
}

void DotProduct::attributeSpecializationChanged(Attribute *attribute){
	_selectedOperation = 0;

	Numeric::Type type = _dotProduct->outValue()->type();
	
	if(type == Numeric::numericTypeVec3 || type == Numeric::numericTypeVec3Array){
		_selectedOperation = &DotProduct::updateVec3;
	}
	else if(type == Numeric::numericTypeQuat || type == Numeric::numericTypeQuatArray){
		_selectedOperation = &DotProduct::updateQuat;
	}
}

void DotProduct::updateVec3(Numeric *element0, Numeric *element1, Numeric *dotProduct, unsigned int slice){
	const std::vector<Imath::V3f> &elementValues0 = element0->vec3ValuesSlice(slice);
	const std::vector<Imath::V3f> &elementValues1 = element1->vec3ValuesSlice(slice);

	int size0 = elementValues0.size();
	int size1 = elementValues1.size();

	int minSize = size0;
	if(size0 < size1){
		minSize = size1;
	}

	std::vector<float> dotValues(minSize);

	for(int i = 0; i < minSize; ++i){
		dotValues[i] = elementValues0[i].dot(elementValues1[i]);
	}

	dotProduct->setFloatValuesSlice(slice, dotValues);
}

void DotProduct::updateQuat(Numeric *element0, Numeric *element1, Numeric *dotProduct, unsigned int slice){
	const std::vector<Imath::Quatf> &elementValues0 = element0->quatValuesSlice(slice);
	const std::vector<Imath::Quatf> &elementValues1 = element1->quatValuesSlice(slice);

	int size0 = elementValues0.size();
	int size1 = elementValues1.size();

	int minSize = size0;
	if(size0 < size1){
		minSize = size1;
	}

	std::vector<float> dotValues(minSize);

	for(int i = 0; i < minSize; ++i){
		dotValues[i] = elementValues0[i] ^ elementValues1[i];
	}

	dotProduct->setFloatValuesSlice(slice, dotValues);
}

void DotProduct::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	std::vector<std::string> newSpecA;
	std::vector<std::string> newSpecB;
	for(int i = 0; i < specializationA.size(); ++i){
		bool isArrayA = stringUtils::endswith(specializationA[i], "Array");
		bool isArrayB = false;
		for(int j = 0; j < specializationB.size(); ++j){
			bool isArrayB = stringUtils::endswith(specializationB[j], "Array");
			if(isArrayA == isArrayB){
				newSpecA.push_back(specializationA[i]);
				if(!containerUtils::elementInContainer(specializationB[j], newSpecB)){
					newSpecB.push_back(specializationB[j]);
				}
				break;
			}
		}
	}
}

void DotProduct::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		(this->*_selectedOperation)(_element0->value(), _element1->value(), _dotProduct->outValue(), slice);
	}
}

Normalize::Normalize(const std::string &name, Node *parent): 
Node(name, parent),
_selectedOperation(0){
	setSliceable(true);

	_element = new NumericAttribute("element", this);
	_normalized = new NumericAttribute("normalized", this);
	
	addInputAttribute(_element);
	addOutputAttribute(_normalized);
	
	setAttributeAffect(_element, _normalized);
	
	std::vector<std::string> specialization;
	specialization.push_back("Vec3");
	specialization.push_back("Vec3Array");
	specialization.push_back("Quat");
	specialization.push_back("QuatArray");
	setAttributeAllowedSpecializations(_element, specialization);
	setAttributeAllowedSpecializations(_normalized, specialization);
	
	addAttributeSpecializationLink(_element, _normalized);
}

void Normalize::attributeSpecializationChanged(Attribute *attribute){
	_selectedOperation = 0;

	Numeric::Type type = _element->outValue()->type();
	if(type == Numeric::numericTypeVec3 || type == Numeric::numericTypeVec3Array){
		_selectedOperation = &Normalize::updateVec3;
	}
	else if(type == Numeric::numericTypeQuat || type == Numeric::numericTypeQuatArray){
		_selectedOperation = &Normalize::updateQuat;
	}
}

void Normalize::updateVec3(Numeric *element, Numeric *normalized, unsigned int slice){
	const std::vector<Imath::V3f> &elementValues = element->vec3ValuesSlice(slice);
	int size = elementValues.size();
	
	std::vector<Imath::V3f> normalizedValues(size);
	for(int i = 0; i < size; ++i){
		normalizedValues[i] = elementValues[i].normalized();
	}
	
	normalized->setVec3ValuesSlice(slice, normalizedValues);
}

void Normalize::updateQuat(Numeric *element, Numeric *normalized, unsigned int slice){
	const std::vector<Imath::Quatf> &elementValues = element->quatValuesSlice(slice);
	int size = elementValues.size();
	
	std::vector<Imath::Quatf> normalizedValues(size);
	for(int i = 0; i < size; ++i){
		normalizedValues[i] = elementValues[i].normalized();
	}
	
	normalized->setQuatValuesSlice(slice, normalizedValues);
}

void Normalize::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		(this->*_selectedOperation)(_element->value(), _normalized->outValue(), slice);
	}
}

TrigonometricFunctions::TrigonometricFunctions(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);

	_inNumber = new NumericAttribute("inNumber", this);
	_outNumber = new NumericAttribute("outNumber", this);
	_function = new EnumAttribute("func", this);

	addInputAttribute(_inNumber);
	addInputAttribute(_function);
	addOutputAttribute(_outNumber);

	setAttributeAffect(_inNumber, _outNumber);
	setAttributeAffect(_function, _outNumber);

	std::vector<std::string> specialization;
	specialization.push_back("Float");
	specialization.push_back("FloatArray");

	std::vector<std::string> funcSpecialization;
	funcSpecialization.push_back("Int");

	setAttributeAllowedSpecializations(_inNumber, specialization);
	setAttributeAllowedSpecializations(_outNumber, specialization);
	
	addAttributeSpecializationLink(_inNumber, _outNumber);
	
	Enum *func = _function->outValue();
	func->addEntry(0, "cos");
	func->addEntry(1, "sin");
	func->addEntry(2, "tan");
	func->addEntry(3, "acos");
	func->addEntry(4, "asin");
	func->addEntry(5, "atan");
	func->addEntry(6, "cosh");
	func->addEntry(7, "sinh");
	func->addEntry(8, "tanh");
}

void TrigonometricFunctions::updateSlice(Attribute *attribute, unsigned int slice){
	std::vector<float> inValues = _inNumber->value()->floatValuesSlice(slice);
	int inFunction = _function->value()->currentIndex();
	std::vector<float> outValues(inValues.size());

	for(int i = 0; i < inValues.size(); ++i){
		switch(inFunction)
		{
		case 0:
			outValues[i] = cos(inValues[i]);
			break;
		case 1:
			outValues[i] = sin(inValues[i]);
			break;
		case 2:
			outValues[i] = tan(inValues[i]);
			break;
		case 3:
			outValues[i] = acos(inValues[i]);
			break;
		case 4:
			outValues[i] = asin(inValues[i]);
			break;
		case 5:
			outValues[i] = atan(inValues[i]);
			break;
		case 6:
			outValues[i] = cosh(inValues[i]);
			break;
		case 7:
			outValues[i] = sinh(inValues[i]);
			break;
		case 8:
			outValues[i] = tanh(inValues[i]);
			break;
		}
	}

	_outNumber->outValue()->setFloatValuesSlice(slice, outValues);
}

Radians::Radians(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);

	_inNumber = new NumericAttribute("in", this);
	_outNumber = new NumericAttribute("out", this);

	addInputAttribute(_inNumber);
	addOutputAttribute(_outNumber);

	setAttributeAffect(_inNumber, _outNumber);

	std::vector<std::string> specialization;
	specialization.push_back("Float");
	specialization.push_back("FloatArray");
	setAttributeAllowedSpecializations(_inNumber, specialization);
	setAttributeAllowedSpecializations(_outNumber, specialization);

	addAttributeSpecializationLink(_inNumber, _outNumber);
}

void Radians::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<float> &in = _inNumber->value()->floatValuesSlice(slice);
	int size = in.size();

	std::vector<float> outValues(size);
	for(int i = 0; i < size; ++i){
		outValues[i] = in[i]*M_PI/180.0f;
	}

	_outNumber->outValue()->setFloatValuesSlice(slice, outValues);
}

Degrees::Degrees(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);

	_inNumber = new NumericAttribute("in", this);
	_outNumber = new NumericAttribute("out", this);

	addInputAttribute(_inNumber);
	addOutputAttribute(_outNumber);

	setAttributeAffect(_inNumber, _outNumber);

	std::vector<std::string> specialization;
	specialization.push_back("Float");
	specialization.push_back("FloatArray");
	setAttributeAllowedSpecializations(_inNumber, specialization);
	setAttributeAllowedSpecializations(_outNumber, specialization);

	addAttributeSpecializationLink(_inNumber, _outNumber);
}

void Degrees::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<float> &in = _inNumber->value()->floatValuesSlice(slice);
	int size = in.size();

	std::vector<float> outValues(size);
	for(int i = 0; i < size; ++i){
		outValues[i] = in[i]*180.0f/float(M_PI);
	}

	_outNumber->outValue()->setFloatValuesSlice(slice, outValues);
}

Floor::Floor(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);

	_inNumber = new NumericAttribute("in", this);
	_outNumber = new NumericAttribute("out", this);

	addInputAttribute(_inNumber);
	addOutputAttribute(_outNumber);

	setAttributeAffect(_inNumber, _outNumber);

	std::vector<std::string> specialization;
	specialization.push_back("Float");
	specialization.push_back("FloatArray");
	setAttributeAllowedSpecializations(_inNumber, specialization);
	setAttributeAllowedSpecializations(_outNumber, specialization);

	addAttributeSpecializationLink(_inNumber, _outNumber);
}

void Floor::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<float> &in = _inNumber->value()->floatValuesSlice(slice);
	int size = in.size();

	std::vector<float> outValues(size);
	for(int i = 0; i < size; ++i){
		outValues[i] = std::floor(in[i]);
	}

	_outNumber->outValue()->setFloatValuesSlice(slice, outValues);
}

Ceil::Ceil(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);

	_inNumber = new NumericAttribute("in", this);
	_outNumber = new NumericAttribute("out", this);

	addInputAttribute(_inNumber);
	addOutputAttribute(_outNumber);

	setAttributeAffect(_inNumber, _outNumber);

	std::vector<std::string> specialization;
	specialization.push_back("Float");
	specialization.push_back("FloatArray");
	setAttributeAllowedSpecializations(_inNumber, specialization);
	setAttributeAllowedSpecializations(_outNumber, specialization);

	addAttributeSpecializationLink(_inNumber, _outNumber);
}

void Ceil::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<float> &in = _inNumber->value()->floatValuesSlice(slice);
	int size = in.size();

	std::vector<float> outValues(size);
	for(int i = 0; i < size; ++i){
		outValues[i] = std::ceil(in[i]);
	}

	_outNumber->outValue()->setFloatValuesSlice(slice, outValues);
}

Round::Round(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);

	_inNumber = new NumericAttribute("in", this);
	_outNumber = new NumericAttribute("out", this);

	addInputAttribute(_inNumber);
	addOutputAttribute(_outNumber);

	setAttributeAffect(_inNumber, _outNumber);

	std::vector<std::string> specialization;
	specialization.push_back("Float");
	specialization.push_back("FloatArray");
	setAttributeAllowedSpecializations(_inNumber, specialization);
	setAttributeAllowedSpecializations(_outNumber, specialization);

	addAttributeSpecializationLink(_inNumber, _outNumber);
}

void Round::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<float> &in = _inNumber->value()->floatValuesSlice(slice);
	int size = in.size();

	std::vector<float> outValues(size);
	for(int i = 0; i < size; ++i){
		outValues[i] = std::floor(in[i]+0.5);
	}

	_outNumber->outValue()->setFloatValuesSlice(slice, outValues);
}

Exp::Exp(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);

	_inNumber = new NumericAttribute("in", this);
	_outNumber = new NumericAttribute("out", this);

	addInputAttribute(_inNumber);
	addOutputAttribute(_outNumber);

	setAttributeAffect(_inNumber, _outNumber);

	std::vector<std::string> specialization;
	specialization.push_back("Float");
	specialization.push_back("FloatArray");
	setAttributeAllowedSpecializations(_inNumber, specialization);
	setAttributeAllowedSpecializations(_outNumber, specialization);

	addAttributeSpecializationLink(_inNumber, _outNumber);
}

void Exp::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<float> &in = _inNumber->value()->floatValuesSlice(slice);
	int size = in.size();

	std::vector<float> outValues(size);
	for(int i = 0; i < size; ++i){
		outValues[i] = std::exp(in[i]);
	}

	_outNumber->outValue()->setFloatValuesSlice(slice, outValues);
}

Log::Log(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);

	_inNumber = new NumericAttribute("in", this);
	_outNumber = new NumericAttribute("out", this);

	addInputAttribute(_inNumber);
	addOutputAttribute(_outNumber);

	setAttributeAffect(_inNumber, _outNumber);

	std::vector<std::string> specialization;
	specialization.push_back("Float");
	specialization.push_back("FloatArray");
	setAttributeAllowedSpecializations(_inNumber, specialization);
	setAttributeAllowedSpecializations(_outNumber, specialization);

	addAttributeSpecializationLink(_inNumber, _outNumber);
}

void Log::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<float> &in = _inNumber->value()->floatValuesSlice(slice);
	int size = in.size();

	std::vector<float> outValues(size);
	for(int i = 0; i < size; ++i){
		outValues[i] = std::log(in[i]);
	}

	_outNumber->outValue()->setFloatValuesSlice(slice, outValues);
}

Pow::Pow(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);

	_base = new NumericAttribute("base", this);
	_exponent = new NumericAttribute("exponent", this);
	_outNumber = new NumericAttribute("out", this);

	addInputAttribute(_base);
	addInputAttribute(_exponent);
	addOutputAttribute(_outNumber);

	setAttributeAffect(_base, _outNumber);
	setAttributeAffect(_exponent, _outNumber);

	std::vector<std::string> specialization;
	specialization.push_back("Float");
	specialization.push_back("FloatArray");
	setAttributeAllowedSpecializations(_base, specialization);
	setAttributeAllowedSpecializations(_exponent, specialization);
	setAttributeAllowedSpecializations(_outNumber, specialization);

	addAttributeSpecializationLink(_base, _outNumber);
	addAttributeSpecializationLink(_exponent, _outNumber);
}

void Pow::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<float> &base = _base->value()->floatValuesSlice(slice);
	const std::vector<float> &exponent = _exponent->value()->floatValuesSlice(slice);
	int size = base.size();

	std::vector<float> outValues(size);
	for(int i = 0; i < size; ++i){
		outValues[i] = std::pow(base[i],exponent[i]);
	}

	_outNumber->outValue()->setFloatValuesSlice(slice, outValues);
}

Sqrt::Sqrt(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);

	_inNumber = new NumericAttribute("in", this);
	_outNumber = new NumericAttribute("out", this);

	addInputAttribute(_inNumber);
	addOutputAttribute(_outNumber);

	setAttributeAffect(_inNumber, _outNumber);

	std::vector<std::string> specialization;
	specialization.push_back("Float");
	specialization.push_back("FloatArray");
	setAttributeAllowedSpecializations(_inNumber, specialization);
	setAttributeAllowedSpecializations(_outNumber, specialization);

	addAttributeSpecializationLink(_inNumber, _outNumber);
}

void Sqrt::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<float> &in = _inNumber->value()->floatValuesSlice(slice);
	int size = in.size();

	std::vector<float> outValues(size);
	for(int i = 0; i < size; ++i){
		outValues[i] = std::sqrt(in[i]);
	}

	_outNumber->outValue()->setFloatValuesSlice(slice, outValues);
}

Atan2::Atan2(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);

	_inNumberY = new NumericAttribute("y", this);
	_inNumberX = new NumericAttribute("x", this);
	_outNumber = new NumericAttribute("out", this);

	addInputAttribute(_inNumberY);
	addInputAttribute(_inNumberX);
	addOutputAttribute(_outNumber);

	setAttributeAffect(_inNumberY, _outNumber);
	setAttributeAffect(_inNumberX, _outNumber);

	std::vector<std::string> specialization;
	specialization.push_back("Float");
	specialization.push_back("FloatArray");
	setAttributeAllowedSpecializations(_inNumberY, specialization);
	setAttributeAllowedSpecializations(_inNumberX, specialization);
	setAttributeAllowedSpecializations(_outNumber, specialization);

	addAttributeSpecializationLink(_inNumberY, _outNumber);
	addAttributeSpecializationLink(_inNumberX, _outNumber);
}

void Atan2::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<float> &y = _inNumberY->value()->floatValuesSlice(slice);
	const std::vector<float> &x = _inNumberX->value()->floatValuesSlice(slice);
	int size = y.size();

	std::vector<float> outValues(size);
	for(int i = 0; i < size; ++i){
		outValues[i] = std::atan2(y[i],x[i]);
	}

	_outNumber->outValue()->setFloatValuesSlice(slice, outValues);
}

Min::Min(const std::string &name, Node *parent):
	Node(name, parent),
	_selectedOperation(0){
	setSliceable(true);

	_inNumber = new NumericAttribute("in", this);
	_outNumber = new NumericAttribute("out", this);

	addInputAttribute(_inNumber);
	addOutputAttribute(_outNumber);

	setAttributeAffect(_inNumber, _outNumber);

	std::vector<std::string> specialization;
	specialization.push_back("IntArray");
	specialization.push_back("FloatArray");

	std::vector<std::string> outSpecialization;
	outSpecialization.push_back("Int");
	outSpecialization.push_back("Float");

	setAttributeAllowedSpecializations(_inNumber, specialization);
	setAttributeAllowedSpecializations(_outNumber, outSpecialization);

	addAttributeSpecializationLink(_inNumber, _outNumber);
}

void Min::attributeSpecializationChanged(Attribute *attribute){
	_selectedOperation = 0;

	Numeric::Type type = _inNumber->outValue()->type();

	if( type == Numeric::numericTypeIntArray){
		_selectedOperation = &Min::min_int;
	}
	else if( type == Numeric::numericTypeFloatArray){
		_selectedOperation = &Min::min_float;
	}
}

void Min::min_int(Numeric *inNumber, Numeric *outNumber, unsigned int slice){
	std::vector<int> inValues = inNumber->intValuesSlice(slice);
	std::vector<int> outValues(1);

	int min = std::numeric_limits<int>::max();
	for(int i = 0; i < inValues.size(); ++i){
		min = (inValues[i]<min?inValues[i]:min);
	}
	outValues[0] = min;

	outNumber->setIntValuesSlice(slice, outValues);
}

void Min::min_float(Numeric *inNumber, Numeric *outNumber, unsigned int slice){
	std::vector<float> inValues = inNumber->floatValuesSlice(slice);
	std::vector<float> outValue(1);

	float min = std::numeric_limits<float>::max();
	for(int i = 0; i < inValues.size(); ++i){
		min = (inValues[i]<min?inValues[i]:min);
	}
	outValue[0] = min;

	outNumber->setFloatValuesSlice(slice, outValue);
}

void Min::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		(this->*_selectedOperation)(_inNumber->value(), _outNumber->outValue(), slice);
	}
}

void Min::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(attributeA == _inNumber && attributeB == _outNumber){
		std::vector<std::string> inSpecializations = _inNumber->allowedSpecialization();
		std::vector<std::string> outSpecializations = _outNumber->allowedSpecialization();

		if(specializationA.size() == 1){
			specializationB.clear();

			if(specializationA[0] == "IntArray"){
				specializationB.push_back(outSpecializations[0]);
			}
			else if(specializationA[0] == "FloatArray"){
				specializationB.push_back(outSpecializations[1]);
			}
		}
		else if(specializationB.size() == 1){
			specializationA.clear();

			if(specializationB[0] == "Int"){
				specializationA.push_back(inSpecializations[0]);
			}
			else if(specializationB[0] == "Float"){
				specializationA.push_back(inSpecializations[1]);
			}
		}
	}
}

Max::Max(const std::string &name, Node *parent):
	Node(name, parent),
	_selectedOperation(0){
	setSliceable(true);

	_inNumber = new NumericAttribute("in", this);
	_outNumber = new NumericAttribute("out", this);

	addInputAttribute(_inNumber);
	addOutputAttribute(_outNumber);

	setAttributeAffect(_inNumber, _outNumber);

	std::vector<std::string> specialization;
	specialization.push_back("IntArray");
	specialization.push_back("FloatArray");

	std::vector<std::string> outSpecialization;
	outSpecialization.push_back("Int");
	outSpecialization.push_back("Float");

	setAttributeAllowedSpecializations(_inNumber, specialization);
	setAttributeAllowedSpecializations(_outNumber, outSpecialization);

	addAttributeSpecializationLink(_inNumber, _outNumber);
}

void Max::attributeSpecializationChanged(Attribute *attribute){
	_selectedOperation = 0;

	Numeric::Type type = _inNumber->outValue()->type();

	if( type == Numeric::numericTypeIntArray){
		_selectedOperation = &Max::max_int;
	}
	else if( type == Numeric::numericTypeFloatArray){
		_selectedOperation = &Max::max_float;
	}
}

void Max::max_int(Numeric *inNumber, Numeric *outNumber, unsigned int slice){
	std::vector<int> inValues = inNumber->intValuesSlice(slice);
	std::vector<int> outValues(1);

	int max = std::numeric_limits<int>::min();
	for(int i = 0; i < inValues.size(); ++i){
		max = (inValues[i]>max?inValues[i]:max);
	}
	outValues[0] = max;

	outNumber->setIntValuesSlice(slice, outValues);
}

void Max::max_float(Numeric *inNumber, Numeric *outNumber, unsigned int slice){
	std::vector<float> inValues = inNumber->floatValuesSlice(slice);
	std::vector<float> outValue(1);

	float max = std::numeric_limits<float>::min();
	for(int i = 0; i < inValues.size(); ++i){
		max = (inValues[i]>max?inValues[i]:max);
	}
	outValue[0] = max;

	outNumber->setFloatValuesSlice(slice, outValue);
}

void Max::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		(this->*_selectedOperation)(_inNumber->value(), _outNumber->outValue(), slice);
	}
}

void Max::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(attributeA == _inNumber && attributeB == _outNumber){
		std::vector<std::string> inSpecializations = _inNumber->allowedSpecialization();
		std::vector<std::string> outSpecializations = _outNumber->allowedSpecialization();

		if(specializationA.size() == 1){
			specializationB.clear();

			if(specializationA[0] == "IntArray"){
				specializationB.push_back(outSpecializations[0]);
			}
			else if(specializationA[0] == "FloatArray"){
				specializationB.push_back(outSpecializations[1]);
			}
		}
		else if(specializationB.size() == 1){
			specializationA.clear();

			if(specializationB[0] == "Int"){
				specializationA.push_back(inSpecializations[0]);
			}
			else if(specializationB[0] == "Float"){
				specializationA.push_back(inSpecializations[1]);
			}
		}
	}
}

Average::Average(const std::string &name, Node *parent):
	Node(name, parent),
	_selectedOperation(0){
	setSliceable(true);

	_inNumber = new NumericAttribute("in", this);
	_outNumber = new NumericAttribute("out", this);

	addInputAttribute(_inNumber);
	addOutputAttribute(_outNumber);

	setAttributeAffect(_inNumber, _outNumber);

	std::vector<std::string> specialization;
	specialization.push_back("IntArray");
	specialization.push_back("FloatArray");
	specialization.push_back("Vec3Array");

	std::vector<std::string> outSpecialization;
	outSpecialization.push_back("Int");
	outSpecialization.push_back("Float");
	outSpecialization.push_back("Vec3");

	setAttributeAllowedSpecializations(_inNumber, specialization);
	setAttributeAllowedSpecializations(_outNumber, outSpecialization);

	addAttributeSpecializationLink(_inNumber, _outNumber);
}

void Average::attributeSpecializationChanged(Attribute *attribute){
	_selectedOperation = 0;

	Numeric::Type type = _inNumber->outValue()->type();

	if( type == Numeric::numericTypeIntArray){
		_selectedOperation = &Average::average_int;
	}
	else if( type == Numeric::numericTypeFloatArray){
		_selectedOperation = &Average::average_float;
	}
	else if( type == Numeric::numericTypeVec3Array){
		_selectedOperation = &Average::average_vec3;
	}
}

void Average::average_int(Numeric *inNumber, Numeric *outNumber, unsigned int slice){
	std::vector<int> inValues = inNumber->intValuesSlice(slice);
	std::vector<int> outValues(1);

	int av = 0;
	for(int i = 0; i < inValues.size(); ++i){
		av += inValues[i];
	}
	outValues[0] = int(av/inValues.size());

	outNumber->setIntValuesSlice(slice, outValues);
}

void Average::average_float(Numeric *inNumber, Numeric *outNumber, unsigned int slice){
	std::vector<float> inValues = inNumber->floatValuesSlice(slice);
	std::vector<float> outValue(1);

	float av = 0;
	for(int i = 0; i < inValues.size(); ++i){
		av += inValues[i];
	}
	outValue[0] = av/inValues.size();

	outNumber->setFloatValuesSlice(slice, outValue);
}

void Average::average_vec3(Numeric *inNumber, Numeric *outNumber, unsigned int slice){
	std::vector<Imath::V3f> inValues = inNumber->vec3ValuesSlice(slice);
	std::vector<Imath::V3f> outValue(1);

	Imath::V3f av(0.0,0.0,0.0);
	for(int i = 0; i < inValues.size(); ++i){
		av += inValues[i];
	}
	outValue[0] = av/float(inValues.size());

	outNumber->setVec3ValuesSlice(slice, outValue);
}

void Average::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		(this->*_selectedOperation)(_inNumber->value(), _outNumber->outValue(), slice);
	}
}

void Average::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(attributeA == _inNumber && attributeB == _outNumber){
		std::vector<std::string> inSpecializations = _inNumber->allowedSpecialization();
		std::vector<std::string> outSpecializations = _outNumber->allowedSpecialization();

		if(specializationA.size() == 1){
			specializationB.clear();

			if(specializationA[0] == "IntArray"){
				specializationB.push_back(outSpecializations[0]);
			}
			else if(specializationA[0] == "FloatArray"){
				specializationB.push_back(outSpecializations[1]);
			}
			else if(specializationA[0] == "Vec3Array"){
				specializationB.push_back(outSpecializations[2]);
			}
		}
		else if(specializationB.size() == 1){
			specializationA.clear();

			if(specializationB[0] == "Int"){
				specializationA.push_back(inSpecializations[0]);
			}
			else if(specializationB[0] == "Float"){
				specializationA.push_back(inSpecializations[1]);
			}
			else if(specializationB[0] == "Vec3"){
				specializationA.push_back(inSpecializations[2]);
			}
		}
	}
}

Slerp::Slerp(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);

	_inQuat1 = new NumericAttribute("q1", this);
	_inQuat2 = new NumericAttribute("q2", this);
	_param = new NumericAttribute("t", this);
	_outNumber = new NumericAttribute("out", this);

	addInputAttribute(_inQuat1);
	addInputAttribute(_inQuat2);
	addInputAttribute(_param);
	addOutputAttribute(_outNumber);

	setAttributeAffect(_inQuat1, _outNumber);
	setAttributeAffect(_inQuat2, _outNumber);
	setAttributeAffect(_param, _outNumber);

	std::vector<std::string> specialization;
	specialization.push_back("Quat");
	specialization.push_back("QuatArray");

	std::vector<std::string> paramSpecialization;
	paramSpecialization.push_back("Float");
	paramSpecialization.push_back("FloatArray");

	setAttributeAllowedSpecializations(_inQuat1, specialization);
	setAttributeAllowedSpecializations(_inQuat2, specialization);
	setAttributeAllowedSpecializations(_param, paramSpecialization);
	setAttributeAllowedSpecializations(_outNumber, specialization);

	addAttributeSpecializationLink(_inQuat1, _outNumber);
	addAttributeSpecializationLink(_inQuat2, _outNumber);
	addAttributeSpecializationLink(_param, _outNumber);
}

void Slerp::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(attributeA == _inQuat1 && attributeB == _outNumber){
		std::vector<std::string> inSpecializations = _inQuat1->allowedSpecialization();
		std::vector<std::string> outSpecializations = _outNumber->allowedSpecialization();

		if(specializationA.size() == 1){
			specializationB.clear();

			if(specializationA[0] == "Quat"){
				specializationB.push_back(outSpecializations[0]);
			}
			else if(specializationA[0] == "QuatArray"){
				specializationB.push_back(outSpecializations[1]);
			}
		}
		else if(specializationB.size() == 1){
			specializationA.clear();

			if(specializationB[0] == "Quat"){
				specializationA.push_back(inSpecializations[0]);
			}
			else if(specializationB[0] == "QuatArray"){
				specializationA.push_back(inSpecializations[1]);
			}
		}
	}
	if(attributeA == _inQuat2 && attributeB == _outNumber){
		std::vector<std::string> inSpecializations = _inQuat2->allowedSpecialization();
		std::vector<std::string> outSpecializations = _outNumber->allowedSpecialization();

		if(specializationA.size() == 1){
			specializationB.clear();

			if(specializationA[0] == "Quat"){
				specializationB.push_back(outSpecializations[0]);
			}
			else if(specializationA[0] == "QuatArray"){
				specializationB.push_back(outSpecializations[1]);
			}
		}
		else if(specializationB.size() == 1){
			specializationA.clear();

			if(specializationB[0] == "Quat"){
				specializationA.push_back(inSpecializations[0]);
			}
			else if(specializationB[0] == "QuatArray"){
				specializationA.push_back(inSpecializations[1]);
			}
		}
	}
	if(attributeA == _param && attributeB == _outNumber){
		std::vector<std::string> inSpecializations = _param->allowedSpecialization();
		std::vector<std::string> outSpecializations = _outNumber->allowedSpecialization();

		if(specializationA.size() == 1){
			specializationB.clear();

			if(specializationA[0] == "Float"){
				specializationB.push_back(outSpecializations[0]);
			}
			else if(specializationA[0] == "FloatArray"){
				specializationB.push_back(outSpecializations[1]);
			}
		}
		else if(specializationB.size() == 1){
			specializationA.clear();

			if(specializationB[0] == "Quat"){
				specializationA.push_back(inSpecializations[0]);
			}
			else if(specializationB[0] == "QuatArray"){
				specializationA.push_back(inSpecializations[1]);
			}
		}
	}
}

void Slerp::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<Imath::Quatf> &q1 = _inQuat1->value()->quatValuesSlice(slice);
	const std::vector<Imath::Quatf> &q2 = _inQuat2->value()->quatValuesSlice(slice);
	const std::vector<float> &t = _param->value()->floatValuesSlice(slice);
	int size = q1.size();
	size = (q2.size()<size)?q2.size():size;
	size = (t.size()<size)?t.size():size;

	std::vector<Imath::Quatf> outValues(size);
	for(int i = 0; i < size; ++i){
		outValues[i] = slerp(q1[i],q2[i],t[i]);
	}

	_outNumber->outValue()->setQuatValues(outValues);
}

QuatMultiply::QuatMultiply(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);

	_quat0 = new NumericAttribute("q0", this);
	_quat1 = new NumericAttribute("q1", this);
	_outQuat = new NumericAttribute("out", this);

	addInputAttribute(_quat0);
	addInputAttribute(_quat1);
	addOutputAttribute(_outQuat);

	setAttributeAffect(_quat0, _outQuat);
	setAttributeAffect(_quat1, _outQuat);

	std::vector<std::string> specialization;
	specialization.push_back("Quat");
	specialization.push_back("QuatArray");
	setAttributeAllowedSpecializations(_quat0, specialization);
	setAttributeAllowedSpecializations(_quat1, specialization);
	setAttributeAllowedSpecializations(_outQuat, specialization);

	addAttributeSpecializationLink(_quat0, _outQuat);
	addAttributeSpecializationLink(_quat1, _outQuat);
}

void QuatMultiply::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<Imath::Quatf> &q0 = _quat0->value()->quatValuesSlice(slice);
	const std::vector<Imath::Quatf> &q1 = _quat1->value()->quatValuesSlice(slice);

	int size0 = q0.size();
	int size1 = q1.size();

	int minSize = size0;
	if(size0 < size1){
		minSize = size1;
	}

	std::vector<Imath::Quatf> outValues(minSize);

	for(int i = 0; i < minSize; ++i){
		outValues[i] = q1[i]*q0[i]*(~q1[i]);
	}

	_outQuat->outValue()->setQuatValues(outValues);
}

Negate::Negate(const std::string &name, Node *parent): 
Node(name, parent),
_selectedOperation(0){	
	setSliceable(true);

	_element = new NumericAttribute("element", this);
	_negated = new NumericAttribute("negated", this);
	
	addInputAttribute(_element);
	addOutputAttribute(_negated);
	
	setAttributeAffect(_element, _negated);
	
	std::vector<std::string> specialization;
	specialization.push_back("Vec3");
	specialization.push_back("Vec3Array");
	specialization.push_back("Matrix44");
	specialization.push_back("Matrix44Array");
	
	setAttributeAllowedSpecializations(_element, specialization);
	setAttributeAllowedSpecializations(_negated, specialization);
	
	addAttributeSpecializationLink(_element, _negated);
}

void Negate::attributeSpecializationChanged(Attribute *attribute){
	_selectedOperation = 0;
	
	Numeric::Type type = _element->outValue()->type();
	
	if(type == Numeric::numericTypeVec3 || type == Numeric::numericTypeVec3Array){
		_selectedOperation = &Negate::updateVec3;
	}
	else if(type == Numeric::numericTypeMatrix44 || type == Numeric::numericTypeMatrix44Array){
		_selectedOperation = &Negate::updateMatrix44;
	}
}

void Negate::updateVec3(Numeric *element, Numeric *negated, unsigned int slice){
	std::vector<Imath::V3f> elementValues = element->vec3ValuesSlice(slice);

	unsigned int size = elementValues.size();
	std::vector<Imath::V3f> negatedValues(size);
	for(int i = 0; i < size ;++i){
		negatedValues[i] = elementValues[i].negate();
	}
	
	negated->setVec3ValuesSlice(slice, negatedValues);
}

void Negate::updateMatrix44(Numeric *element, Numeric *negated, unsigned int slice){
	std::vector<Imath::M44f> elementValues = element->matrix44ValuesSlice(slice);

	unsigned int size = elementValues.size();
	std::vector<Imath::M44f> negatedValues(size);
	for(int i = 0; i < size ;++i){
		negatedValues[i] = elementValues[i].negate();
	}
	
	negated->setMatrix44ValuesSlice(slice, negatedValues);
}

void Negate::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		(this->*_selectedOperation)(_element->value(), _negated->outValue(), slice);
	}
}
