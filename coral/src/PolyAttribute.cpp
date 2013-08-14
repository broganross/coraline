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

#include <string>
#include <map>
#include <vector>
#include <typeinfo>

#include "PolyAttribute.h"

using namespace coral;


PolyAttribute::PolyAttribute(const std::string &name, Node *parent)
	: Attribute(name, parent){
	setClassName("PolyAttribute");
	setValuePtr(new PolyValue());
//	std::cout << "PolyAttr Const: " << (PolyValue*)_inputValue->slices() << std::endl;

	std::vector<std::string> spec;
	spec.push_back("Any");
	spec.push_back("StringAny");
	spec.push_back("String");
	spec.push_back("StringArray");
	spec.push_back("Path");
	spec.push_back("PathArray");
	spec.push_back("BoolAny");
	spec.push_back("Bool");
	spec.push_back("BoolArray");
	spec.push_back("NumericAny");
	spec.push_back("Int");
	spec.push_back("IntArray");
	spec.push_back("Float");
	spec.push_back("FloatArray");
	spec.push_back("Vec3");
	spec.push_back("Vec3Array");
	spec.push_back("Col4");
	spec.push_back("Col4Array");
	spec.push_back("Matrix44");
	spec.push_back("Matrix44Array");
	spec.push_back("Quat");
	spec.push_back("QuatArray");
	setAllowedSpecialization(spec);
}

PolyValue *PolyAttribute::value(){
//	std::cout << "PolyAttribute.value " << name() << std::endl;
//	return (PolyValue*)Attribute::value();

	Value *v = Attribute::value();
	if (typeid(*v) == typeid(String)){
		PolyValue *np = new PolyValue();
		np->copyFromString((String*)v);
//		std::cout << "PolyAttribute.value: Done" << std::endl;
		return np;
	} else if (typeid(*v) == typeid(Bool)) {
		PolyValue *np = new PolyValue();
		np->copyFromBool((Bool*)v);
//		std::cout << "PolyAttribute.value: Done" << std::endl;
		return np;
	} else if (typeid(*v) == typeid(Numeric)) {
		PolyValue *np = new PolyValue();
		np->copyFromNumeric((Numeric*)v);
//		std::cout << "PolyAttribute.value: Done" << std::endl;
		return np;
	} else {
//		std::cout << "PolyAttribute.value: Done" << std::endl;
		return (PolyValue*)v;
	}
}

PolyValue *PolyAttribute::outValue(){
//	std::cout << "PolyAttribute.outValue " << name() << std::endl;
	return (PolyValue*)Attribute::outValue();
}

PolyValue::ValueType PolyAttribute::typeFromString(const std::string &typeStr){
	PolyValue::ValueType type = PolyValue::typeAny;
	if (typeStr == "StringAny"){
		type = PolyValue::stringTypeAny;
	}
	else if (typeStr == "String"){
		type = PolyValue::stringType;
	}
	else if (typeStr == "StringArray"){
		type = PolyValue::stringTypeArray;
	}
	else if (typeStr == "Path"){
		type = PolyValue::pathType;
	}
	else if (typeStr == "PathArray"){
		type = PolyValue::pathTypeArray;
	}
	else if (typeStr == "BoolAny"){
		type = PolyValue::boolTypeAny;
	}
	else if (typeStr == "Bool"){
		type = PolyValue::boolType;
	}
	else if (typeStr == "BoolArray"){
		type = PolyValue::boolTypeArray;
	}
	else if (typeStr == "Int"){
		type = PolyValue::numericTypeInt;
	}
	else if (typeStr == "IntArray"){
		type = PolyValue::numericTypeIntArray;
	}
	else if (typeStr == "Float"){
		type = PolyValue::numericTypeFloat;
	}
	else if (typeStr == "FloatArray"){
		type = PolyValue::numericTypeFloatArray;
	}
	else if (typeStr == "Vec3"){
		type = PolyValue::numericTypeVec3;
	}
	else if (typeStr == "Vec3Array"){
		type = PolyValue::numericTypeVec3Array;
	}
	else if (typeStr == "Col4"){
		type = PolyValue:: numericTypeCol4;
	}
	else if (typeStr == "Col4Array"){
		type = PolyValue::numericTypeCol4Array;
	}
	else if (typeStr == "Matrix44"){
		type = PolyValue::numericTypeMatrix44;
	}
	else if (typeStr == "Matrix44Array"){
		type = PolyValue::numericTypeMatrix44Array;
	}
	else if (typeStr == "Quat"){
		type = PolyValue::numericTypeQuat;
	}
	else if (typeStr == "QuatArray"){
		type = PolyValue::numericTypeQuatArray;
	}
	return type;
}

void PolyAttribute::onSettingSpecialization(const std::vector<std::string> &specialization){
	if (specialization.size() == 1){
		std::string typeStr = specialization[0];
		outValue()->setType(typeFromString(typeStr));
	} else {
		outValue()->setType(PolyValue::typeAny);
	}
}

std::string PolyAttribute::shortDebugInfo(){
//	std::cout << "PolyAttribute.shortDebugInfo " << name() << std::endl;
	std::string info = Attribute::shortDebugInfo() + "\n";

	PolyValue *val = value();
	int slices = val->slices();

	bool isArray = val->isArray();

	info += "slices: " + stringUtils::intToString(slices) + "\n";
	for (int i=0; i<val->slices(); ++i){
		info += "slice: " + stringUtils::intToString(i) + ", ";
		if(isArray){
			info += "size: " + stringUtils::intToString(val->sizeSlice(i)) + ", ";
		}
		std::string valStr = val->sliceAsString(i);
		std::vector<std::string> split;
		if (val->type() == PolyValue::stringType ||
				val->type() == PolyValue::pathType ||
				val->type() == PolyValue::stringTypeArray ||
				val->type() == PolyValue::pathTypeArray){
			stringUtils::split(valStr, split, " ");
		} else {
			stringUtils::split(valStr, split, " ");
		}
		valStr = split[0];
		if (val->type() == PolyValue::stringType ||
				val->type() == PolyValue::pathType ||
				val->type() == PolyValue::stringTypeArray ||
				val->type() == PolyValue::pathTypeArray){
			valStr = stringUtils::replace(valStr, "___", " ");
		}

		std::string trimmedValStr;
		if(valStr.size() < 100){
			trimmedValStr = valStr;
		}
		else {
			for (int i=0; i<100; ++i){
				trimmedValStr += valStr[i];
			}
			trimmedValStr += "...]";
		}
		info += trimmedValStr + "\n";

		if (i > 3){
			info += "(trimming remaing slices)\n";
			break;
		}
	}
//	std::cout << "PolyAttribute.shortDebugInfo: Done" << std::endl;
	return info;
}
