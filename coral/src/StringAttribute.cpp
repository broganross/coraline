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

#include <typeinfo>
#include "StringAttribute.h"
#include "PolyValue.h"


using namespace coral;
String::String():
	_type(stringTypeAny),
	_isArray(false),
	_slices(1){

	_stringValuesSliced.resize(1);
	_stringValuesSliced[0].resize(1);
	_stringValuesSliced[0][0] = "";
}

bool String::isArray(){
	return _isArray;
}

void String::setIsArray(bool array){
	_isArray = array;
}

void String::setSlices(unsigned int slices){
	_slices = slices;
}

unsigned int String::size(){
	return sizeSlice(0);
}

unsigned int String::sizeSlice(unsigned int slice){
	if(slice >= _slices){
		slice = _slices - 1;
	}

	if (_type == stringTypeAny){
		return 0;
	} else if (_type == stringType || _type == pathType || _type == stringTypeArray || _type == pathTypeArray) {
		return _stringValuesSliced[slice].size();
	}
	return 0;
}

String::Type String::type(){
	return _type;
}

void String::setType(String::Type type){
	_type = type;
	_isArray = false;

	if(type == stringType || type == pathType){
		_stringValuesSliced.resize(_slices);
		for (int i=0; i < _slices; ++i){
			_stringValuesSliced[i].resize(1);
		}
	}
	else if (type == stringTypeArray || type == pathTypeArray){
		_stringValuesSliced.resize(_slices);
		_isArray = true;
	}

}

void String::resize(unsigned int newSize){
	resizeSlice(0, newSize);
}

bool String::isArrayType(String::Type type){
	bool arrayType = false;
	if( type == stringTypeArray || type == pathTypeArray){
		arrayType = true;
	}
	return arrayType;
}

void String::setStringValueAt(unsigned int id, std::string& value)
{
	setStringValueAtSlice(0, id, value);
}

void String::setPathValueAt(unsigned int id, std::string& value){
	setStringValueAt(id, value);
}

const std::string String::stringValueAt(unsigned int id){
	return stringValueAtSlice(0, id);
}

const std::string String::pathValueAt(unsigned int id){
	return stringValueAt(id);
}

const std::vector<std::string> &String::stringValues()
{
	return _stringValuesSliced[0];
}

const std::vector<std::string> &String::pathValues(){
	return _stringValuesSliced[0];
}

std::string String::sliceAsString(unsigned int slice){
//	std::cout << "PolyValue.stringSliceAsString" << std::endl;
	std::string script;
	std::ostringstream stream;
	if (slice >= _slices){
		slice = _slices - 1;
	}
	for (int i=0; i < _stringValuesSliced[slice].size(); ++i){
		std::string val = stringUtils::replace(_stringValuesSliced[slice][i], "\n", "\\n");
		stream << stringUtils::replace(val, " ", "___");
		if (i < _stringValuesSliced[slice].size() - 1){
			stream << ",,";
		}
	}
	script = stream.str();
	std::ostringstream type;
	type << _type;
	script = "[" + stream.str() + "] " + type.str();

	//	std::cout << "PolyValue.stringSliceAsString: Done" << std::endl;

	return script;
}

std::string String::asScript(){
	return asString();
}

std::string String::asString(){
	return sliceAsString(0);
}

void String::setFromString(const std::string &value)
{
	std::string tmp = stringUtils::replace(value, "\\n", "\n");
	tmp = stringUtils::replace(tmp, "\\t", "\t");
	std::vector<std::string> fields;
	stringUtils::split(tmp, fields, " ");
	if (fields.size() == 2){
		std::string valuesStr = fields[0];
		valuesStr = stringUtils::replace(valuesStr, "___", " ");
		valuesStr = valuesStr.substr(0, valuesStr.length() -1);
		valuesStr = valuesStr.substr(1, valuesStr.length() - 1);
		String::Type type = String::Type(stringUtils::parseInt(fields[1]));
		_stringValuesSliced.resize(1);
		_stringValuesSliced[0].clear();
		std::vector<std::string> values;
		stringUtils::split(valuesStr, values, ",,");
		for (int i=0; i<values.size(); ++i){
			_stringValuesSliced[0].push_back(values[i]);
		}
	}
}

void String::setStringValueAtSlice(unsigned int slice, unsigned int id, std::string& value){
//	std::cout << "String.setStringValueAtSlice" << std::endl;
	if (slice < _stringValuesSliced.size()){
		std::vector<std::string> &slicevec = _stringValuesSliced[slice];
		if (id < slicevec.size()){
			slicevec[id] = value;
		}
	}
//	std::cout << "String.setStringValueAtSlice: Done" << std::endl;
}

void String::setPathValueAtSlice(unsigned int slice, unsigned int id, std::string& value){
	setStringValueAtSlice(slice, id, value);
}

std::string String::stringValueAtSlice(unsigned int slice, unsigned int id){
//	std::cout << "String.stringValueAtSlice" <<std::endl;
	if (slice >= _stringValuesSliced.size()){
		slice = _stringValuesSliced.size() - 1;
	}
	std::vector<std::string> &slicevec = _stringValuesSliced[slice];
	int size = slicevec.size();
	if (id < size){
		return slicevec[id];
	} else if (size){
		return slicevec[size-1];
	}
//	std::cout << "String.stringValueAtSlice: None" <<std::endl;
	return "";
}

std::string String::pathValueAtSlice(unsigned int slice, unsigned int id){
	return stringValueAtSlice(slice, id);
}

void String::setStringValues(const std::vector<std::string> &values){
	setStringValuesSlice(0, values);
}

void String::setPathValues(const std::vector<std::string> &values){
	setStringValues(values);
}

void String::setStringValuesSlice(unsigned int slice, const std::vector<std::string> &values){
	if (slice < _stringValuesSliced.size()){
		_stringValuesSliced[slice] = values;
	}
}

void String::setPathValuesSlice(unsigned int slice, const std::vector<std::string> &values){
	setStringValuesSlice(slice, values);
}

const std::vector<std::string> &String::valuesSlice(unsigned int slice){
	if(slice >= _stringValuesSliced.size()){
		slice = _stringValuesSliced.size() - 1;
	}
	return _stringValuesSliced[slice];
}

void String::resizeSlice(unsigned int slice, unsigned int newSize){
	if (_type != stringTypeAny){
		for(int i=0; i < _stringValuesSliced.size(); i++){
			_stringValuesSliced[i].resize(newSize);
		}
	}
}

void String::resizeSlices(unsigned int slices){
	if (slices == 0){
		slices = 1;
	}
	if (slices != _slices && _type != stringTypeAny){
		if (_type == stringType){
			_stringValuesSliced.resize(slices);
			for (int i=0; i < slices; ++i){
				std::vector<std::string> &slicevec = _stringValuesSliced[i];
				if (!slicevec.size()){
					slicevec.resize(1);
				}
			}
		} else if (_type == stringTypeArray) {
			_stringValuesSliced.resize(slices);
		}
		_slices = slices;
	}
}


StringAttribute::StringAttribute(const std::string &name, Node *parent)
	: Attribute(name, parent)
	, _longString(false)
{
	setClassName("StringAttribute");
	String *ptr = new String();
	setValuePtr(ptr);
	
	std::vector<std::string> allowedSpecialization;
	allowedSpecialization.push_back("String");
	allowedSpecialization.push_back("Path");
	setAllowedSpecialization(allowedSpecialization);
}

String* StringAttribute::value()
{
	Value *v = Attribute::value();
	if (typeid(*v) == typeid(PolyValue)){
		PolyValue *p = (PolyValue*)v;
		String *s = p->copyToString();
		return s;
	} else {
		return (String*)Attribute::value();
	}
}

String* StringAttribute::outValue()
{
	return (String*)Attribute::outValue();
}

void StringAttribute::setLongString(bool value)
{
	_longString = value;
}

bool StringAttribute::longString()
{
	return _longString;
}

void StringAttribute::onSettingSpecialization(const std::vector<std::string> &specialization){
	if (specialization.size() == 1){
		std::string typeStr = specialization[0];
		outValue()->setType(stringTypeFromString(typeStr));
	} else {
		outValue()->setType(String::stringTypeAny);
	}
}

String::Type StringAttribute::stringTypeFromString(const std::string &typeStr){
	String::Type type = String::stringTypeAny;
	if (typeStr == "String"){
		type = String::stringType;
	} else if (typeStr == "Path") {
		type = String::pathType;
	} else if (typeStr == "StringArray") {
		type = String::stringTypeArray;
	} else if (typeStr == "PathArray") {
		type = String::pathTypeArray;
	}
	return type;
}
