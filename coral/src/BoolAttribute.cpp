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
#include "BoolAttribute.h"
#include "stringUtils.h"

using namespace coral;

Bool::Bool():
_isArray(false),
_slices(1){
	_boolValuesSliced.resize(1);
	_boolValuesSliced[0].resize(1);
	_boolValuesSliced[0][0] = false;
}

unsigned int Bool::slices(){
	return _slices;
}

unsigned int Bool::sizeSlice(unsigned int slice){
	if(slice >= _slices){
		slice = _slices - 1;
	}

	return _boolValuesSliced[slice].size();
}

unsigned int Bool::size(){
	return _boolValuesSliced[0].size();
}

void Bool::resizeSlices(unsigned int slices){
	if(slices == 0){
		slices = 1;
	}

	if(slices != _slices){
		_boolValuesSliced.resize(slices);
		for(int i = 0; i < slices; ++i){
			std::vector<bool> &slicevec = _boolValuesSliced[i];
			if(!slicevec.size()){
				slicevec.resize(1);
			}
		}
		
		_slices = slices;
	}
}

void Bool::setBoolValueAtSlice(unsigned int slice, unsigned int id, bool value){
	if(slice < _boolValuesSliced.size()){
		std::vector<bool> &slicevec = _boolValuesSliced[slice];
		if(id < slicevec.size()){
			slicevec[id] = value;
		}
	}
}

void Bool::setBoolValueAt(unsigned int id, bool value){
	if(id < _boolValuesSliced[0].size())
		_boolValuesSliced[0][id] = value;
}

bool Bool::boolValueAtSlice(unsigned int slice, unsigned int id){
	if(slice >= _boolValuesSliced.size()){
		slice = _boolValuesSliced.size() - 1;
	}

	std::vector<bool> &slicevec = _boolValuesSliced[slice];

	int size = slicevec.size();
	if(id < size){
		return slicevec[id];
	}
	else if(size){
		return slicevec[size - 1];
	}
	
	return false;
}

bool Bool::boolValueAt(unsigned int id){
	bool value = false;

	if(id < _boolValuesSliced[0].size()){
		value = _boolValuesSliced[0][id];
	}
		
	return value;
}

const std::vector<bool> &Bool::boolValuesSlice(unsigned int slice){
	if(slice >= _boolValuesSliced.size()){
		slice = _boolValuesSliced.size() - 1;
	}
	
	return _boolValuesSliced[slice];
}

const std::vector<bool> &Bool::boolValues(){
	return _boolValuesSliced[0];
}

void Bool::setBoolValuesSlice(unsigned int slice, const std::vector<bool> &values){
	if(slice < _boolValuesSliced.size()){
		_boolValuesSliced[slice] = values;
	}
}

void Bool::setBoolValues(const std::vector<bool> &values){
	_boolValuesSliced[0] = values;
}

void Bool::setIsArray(bool value){
	_isArray = value;
}

bool Bool::isArray(){
	return _isArray;
}

void Bool::resize(unsigned int size){
	_boolValuesSliced[0].resize(size);
}

std::string Bool::sliceAsString(unsigned int slice){
	if(slice >= _slices){
		slice = _slices - 1;
	}

	std::string script;
	if(_isArray){
		script = "[";
		for(int i = 0; i < _boolValuesSliced[slice].size(); ++i){
			std::string value = "False";
			if(_boolValuesSliced[slice][i]){
				value = "True";
			}
		
			script += value + ",";
		
			if(i % 20 == 19)
				value += "\n";
		}
	
		script += "]";
	}
	else{
		bool value = _boolValuesSliced[slice][0];
		if(value){
			script = "True";
		}
		else{
			script = "False";
		}
	}
	
	return script;
}

std::string Bool::asString(){
	return sliceAsString(0);
}

void Bool::setFromString(const std::string &value){
	if(stringUtils::startswith(value, "[")){
		std::string tmp = stringUtils::replace(value, "\n", "");
		std::string valuesStr = stringUtils::strip(value, "[]");
		std::vector<std::string> values;
		stringUtils::split(valuesStr, values, ",");
	
		_boolValuesSliced[0].clear();
		_isArray = true;
		for(int i = 0; i < values.size(); ++i){
			std::string valueStr = values[i];
			if(valueStr == "True"){
				_boolValuesSliced[0].push_back(true);
			}
			else if(valueStr == "False"){
				_boolValuesSliced[0].push_back(false);
			}
		}
	}
	else{
		_isArray = false;
		_boolValuesSliced[0].resize(1);
		if(value == "True"){
			_boolValuesSliced[0][0] = true;
		}
		else if(value == "False"){
			_boolValuesSliced[0][0] = false;
		}
	}
}

BoolAttribute::BoolAttribute(const std::string &name, Node *parent) : Attribute(name, parent){
	setClassName("BoolAttribute");

	setValuePtr(new Bool());
	
	std::vector<std::string> allowedSpecialization;
	allowedSpecialization.push_back("Bool");
	setAllowedSpecialization(allowedSpecialization);
}

Bool *BoolAttribute::value(){
	return (Bool*)Attribute::value();
}
Bool *BoolAttribute::outValue(){
	return (Bool*)Attribute::outValue();
}

void BoolAttribute::onSettingSpecialization(const std::vector<std::string> &specialization){
	Bool *boolVal = outValue();

	boolVal->setIsArray(false);
	if(boolVal->size() == 0){
		boolVal->resize(1);
		boolVal->setBoolValueAt(0, false);
	}

	if(specialization.size() == 1){
		if(specialization[0] == "BoolArray"){
			boolVal->setIsArray(true);
		}
	}
}

std::string BoolAttribute::shortDebugInfo(){
	std::string info = Attribute::shortDebugInfo() + "\n";

	Bool *val = value();
	int slices = val->slices();

	bool isArray = val->isArray();

	info += "slices: " + stringUtils::intToString(slices) + "\n";
	for(int i = 0; i < val->slices(); ++i){
		info += "slice: " + 	stringUtils::intToString(i) + ", ";
		if(isArray){
			info += "size: " + stringUtils::intToString(val->sizeSlice(i)) + ", ";
		}

		std::string valStr = val->sliceAsString(i);
		std::vector<std::string> split;
		stringUtils::split(valStr, split, " ");
		valStr = split[0];

		std::string trimmedValStr;
		if(valStr.size() < 100){
			trimmedValStr = valStr;
		}
		else{
			for(int i = 0; i < 100; ++i){
				trimmedValStr += valStr[i];
			}
			trimmedValStr += " ...]";
		}
		
		info += trimmedValStr + "\n";

		if(i > 3){
			info += "(trimming remaining slices)\n";
			break;
		}
	}

	return info;
}
