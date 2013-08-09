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
#include "PolyValue.h"
#include <ImathMatrixAlgo.h>

using namespace coral;

PolyValue::PolyValue():
	_type(typeAny),
	_isArray(false),
	_slices(1){

	_intValuesSliced.resize(1);
	_intValuesSliced[0].resize(1);
	_intValuesSliced[0][0] = 0;

	_floatValuesSliced.resize(1);
	_floatValuesSliced[0].resize(1);
	_floatValuesSliced[0][0] = 0.0;

	_vec3ValuesSliced.resize(1);
	_vec3ValuesSliced[0].resize(1);
	_vec3ValuesSliced[0][0] = Imath::V3f(0.0, 0.0, 0.0);

	_quatValuesSliced.resize(1);
	_quatValuesSliced[0].resize(1);
	_quatValuesSliced[0][0] = Imath::Quatf(0.0, 0.0, 0.0, 1.0);

	_matrix44ValuesSliced.resize(1);
	_matrix44ValuesSliced[0].resize(1);
	_matrix44ValuesSliced[0][0] = Imath::identity44f;

	_col4ValuesSliced.resize(1);
	_col4ValuesSliced[0].resize(1);
	_col4ValuesSliced[0][0] = Imath::Color4f(1.0, 1.0, 1.0, 1.0);

	_stringValuesSliced.resize(1);
	_stringValuesSliced[0].resize(1);
	_stringValuesSliced[0][0] = "";

	_boolValuesSliced.resize(1);
	_boolValuesSliced[0].resize(1);
	_boolValuesSliced[0][0] = false;

}

bool PolyValue::isArray(){
	return _isArray;
}

unsigned int PolyValue::size(){
	return sizeSlice(0);
}

unsigned int PolyValue::sizeSlice(unsigned int slice){
	std::cout << "PolyValue.sizeSlice" << std::endl;
	if (slice >= _slices){
		slice = _slices -1;
	}
	int ret = 0;
	switch (_type){
	case stringTypeArray:
		ret = _stringValuesSliced[slice].size();
		break;
	case numericTypeIntArray:
		ret = _intValuesSliced[slice].size();
		break;
	case numericTypeFloatArray:
		ret = _floatValuesSliced[slice].size();
		break;
	case numericTypeVec3Array:
		ret = _vec3ValuesSliced[slice].size();
		break;
	case numericTypeCol4Array:
		ret = _col4ValuesSliced[slice].size();
		break;
	case numericTypeQuatArray:
		ret = _quatValuesSliced[slice].size();
		break;
	case numericTypeMatrix44Array:
		ret = _matrix44ValuesSliced[slice].size();
		break;
	case pathTypeArray:
		ret = _stringValuesSliced[slice].size();
		break;
	case boolTypeArray:
		ret = _boolValuesSliced[slice].size();
		break;
	}
	std::cout << "PolyValue.sizeSlice: Done" << std::endl;
	return ret;
}

PolyValue::ValueType PolyValue::type(){
	return _type;
}

void PolyValue::setType(PolyValue::ValueType type){
	_type = type;
	_isArray = false;
	switch (_type){
	case stringType:
		_stringValuesSliced.resize(_slices);
		for(int i=0; i<_slices; ++i){
			_stringValuesSliced[i].resize(1);
		}
		break;
	case stringTypeArray:
		_isArray = true;
		_stringValuesSliced.resize(_slices);
		break;
	case pathType:
		_stringValuesSliced.resize(_slices);
		for(int i=0; i<_slices; ++i){
			_stringValuesSliced[i].resize(1);
		}
		break;
	case pathTypeArray:
		_isArray = true;
		_stringValuesSliced.resize(_slices);
		break;
	case numericTypeInt:
		_intValuesSliced.resize(_slices);
		for(int i=0; i<_slices; ++i){
			_intValuesSliced[i].resize(1);
		}
		break;
	case numericTypeIntArray:
		_isArray = true;
		_intValuesSliced.resize(_slices);
		break;
	case numericTypeFloat:
		_floatValuesSliced.resize(_slices);
		for(int i=0; i<_slices; ++i){
			_floatValuesSliced[i].resize(1);
		}
		break;
	case numericTypeFloatArray:
		_isArray = true;
		_floatValuesSliced.resize(_slices);
		break;
	case numericTypeVec3:
		_vec3ValuesSliced.resize(_slices);
		for (int i=0; i<_slices; ++i){
			_vec3ValuesSliced[i].resize(1);
		}
		break;
	case numericTypeVec3Array:
		_isArray = true;
		_vec3ValuesSliced.resize(_slices);
		break;
	case numericTypeCol4:
		_col4ValuesSliced.resize(_slices);
		for (int i=0; i<_slices; ++i){
			_col4ValuesSliced[i].resize(1);
		}
		break;
	case numericTypeCol4Array:
		_isArray = true;
		_col4ValuesSliced.resize(_slices);
		break;
	case numericTypeQuat:
		_quatValuesSliced.resize(_slices);
		for (int i=0; i<_slices; ++i){
			_quatValuesSliced[i].resize(1);
		}
		break;
	case numericTypeQuatArray:
		_isArray = true;
		_quatValuesSliced.resize(_slices);
		break;
	case numericTypeMatrix44:
		_matrix44ValuesSliced.resize(_slices);
		for (int i=0; i<_slices; ++i){
			_matrix44ValuesSliced[i].resize(1);
		}
		break;
	case numericTypeMatrix44Array:
		_isArray = true;
		_matrix44ValuesSliced.resize(_slices);
		break;
	case boolType:
		_boolValuesSliced.resize(_slices);
		for (int i=0; i<_slices; ++i){
			_boolValuesSliced[i].resize(1);
		}
		break;
	case boolTypeArray:
		_isArray = true;
		_boolValuesSliced.resize(_slices);
		break;
	}
}

void PolyValue::copy(const Value *other){
	const PolyValue *otherVal = dynamic_cast<const PolyValue*>(other);

	if(otherVal){
		_type = otherVal->_type;
		_isArray = otherVal->_isArray;

		_intValuesSliced = otherVal->_intValuesSliced;
		_floatValuesSliced = otherVal->_floatValuesSliced;
		_vec3ValuesSliced = otherVal->_vec3ValuesSliced;
		_quatValuesSliced = otherVal->_quatValuesSliced;
		_matrix44ValuesSliced = otherVal->_matrix44ValuesSliced;
		_col4ValuesSliced = otherVal->_col4ValuesSliced;
		_boolValuesSliced = otherVal-> _boolValuesSliced;
		_stringValuesSliced = otherVal->_stringValuesSliced;

	}
}

void PolyValue::resize(unsigned int newSize){
	resizeSlice(0, newSize);
}

void PolyValue::resizeSlice(unsigned int slice, unsigned int newSize){
	switch (_type){
	case stringType:
		for(int i = 0; i < _stringValuesSliced.size(); ++i){
			_stringValuesSliced[i].resize(newSize);
		}
		break;
	case stringTypeArray:
		for(int i = 0; i < _stringValuesSliced.size(); ++i){
			_stringValuesSliced[i].resize(newSize);
		}
		break;
	case pathType:
		for(int i = 0; i < _stringValuesSliced.size(); ++i){
			_stringValuesSliced[i].resize(newSize);
		}
		break;
	case pathTypeArray:
		for(int i = 0; i < _stringValuesSliced.size(); ++i){
			_intValuesSliced[i].resize(newSize);
		}
		break;
	case boolType:
		for(int i = 0; i < _boolValuesSliced.size(); ++i){
			_boolValuesSliced[i].resize(newSize);
		}
		break;
	case boolTypeArray:
		for(int i = 0; i < _boolValuesSliced.size(); ++i){
			_boolValuesSliced[i].resize(newSize);
		}
		break;
	case numericTypeInt:
		for(int i = 0; i < _intValuesSliced.size(); ++i){
			_intValuesSliced[i].resize(newSize);
		}
		break;
	case numericTypeIntArray:
		for(int i = 0; i < _intValuesSliced.size(); ++i){
			_intValuesSliced[i].resize(newSize);
		}
		break;
	case numericTypeFloat:
		for(int i = 0; i < _floatValuesSliced.size(); ++i){
			_floatValuesSliced[i].resize(newSize);
		}
		break;
	case numericTypeFloatArray:
		for(int i = 0; i < _floatValuesSliced.size(); ++i){
			_floatValuesSliced[i].resize(newSize);
		}
		break;
	case numericTypeVec3:
		for(int i = 0; i < _vec3ValuesSliced.size(); ++i){
			_intValuesSliced[i].resize(newSize);
		}
		break;
	case numericTypeVec3Array:
		for(int i = 0; i < _vec3ValuesSliced.size(); ++i){
			_vec3ValuesSliced[i].resize(newSize);
		}
		break;
	case numericTypeQuat:
		for(int i = 0; i < _quatValuesSliced.size(); ++i){
			_quatValuesSliced[i].resize(newSize);
		}
		break;
	case numericTypeMatrix44:
		for(int i = 0; i < _matrix44ValuesSliced.size(); ++i){
			_matrix44ValuesSliced[i].resize(newSize);
		}
		break;
	case numericTypeMatrix44Array:
		for(int i = 0; i < _matrix44ValuesSliced.size(); ++i){
			_matrix44ValuesSliced[i].resize(newSize);
		}
		break;
	case numericTypeCol4:
		for(int i = 0; i < _col4ValuesSliced.size(); ++i){
			_col4ValuesSliced[i].resize(newSize);
		}
		break;
	}
}

void PolyValue::resizeSlices(unsigned int slices){
	if (slices == 0){
		slices = 1;
	};

	if (slices != _slices){
		switch (_type){
		case stringType:
			_stringValuesSliced.resize(slices);
			for(int i = 0; i < slices; ++i){
				std::vector<std::string> &slicevec = _stringValuesSliced[i];
				if(!slicevec.size()){
					slicevec.resize(1);
				}
			}
			break;
		case stringTypeArray:
			_stringValuesSliced.resize(slices);
			break;
		case pathType:
			_stringValuesSliced.resize(slices);
			for(int i = 0; i < slices; ++i){
				std::vector<std::string> &slicevec = _stringValuesSliced[i];
				if(!slicevec.size()){
					slicevec.resize(1);
				}
			}
		case pathTypeArray:
			_stringValuesSliced.resize(slices);
			break;
		case boolType:
			_boolValuesSliced.resize(slices);
			for(int i = 0; i < slices; ++i){
				std::vector<bool> &slicevec = _boolValuesSliced[i];
				if(!slicevec.size()){
					slicevec.resize(1);
				}
			}
			break;
		case boolTypeArray:
			_boolValuesSliced.resize(slices);
			break;
		case numericTypeInt:
			_intValuesSliced.resize(slices);
			for(int i = 0; i < slices; ++i){
				std::vector<int> &slicevec = _intValuesSliced[i];
				if(!slicevec.size()){
					slicevec.resize(1);
				}
			}
		case numericTypeIntArray:
			_intValuesSliced.resize(slices);
			break;
		case numericTypeFloat:
			_floatValuesSliced.resize(slices);
			for(int i = 0; i < slices; ++i){
				std::vector<float> &slicevec = _floatValuesSliced[i];
				if(!slicevec.size()){
					slicevec.resize(1);
				}
			}
			break;
		case numericTypeFloatArray:
			_floatValuesSliced.resize(slices);
			break;
		case numericTypeVec3:
			_vec3ValuesSliced.resize(slices);
			for(int i = 0; i < slices; ++i){
				std::vector<Imath::V3f> &slicevec = _vec3ValuesSliced[i];
				if(!slicevec.size()){
					slicevec.resize(1);
				}
			}
			break;
		case numericTypeVec3Array:
			_vec3ValuesSliced.resize(slices);
			break;
		case numericTypeMatrix44:
			_matrix44ValuesSliced.resize(slices);
			for(int i = 0; i < slices; ++i){
				std::vector<Imath::M44f> &slicevec = _matrix44ValuesSliced[i];
				if(!slicevec.size()){
					slicevec.resize(1);
				}
			}
			break;
		case numericTypeMatrix44Array:
			_matrix44ValuesSliced.resize(slices);
			break;
		case numericTypeQuat:
			_quatValuesSliced.resize(slices);
			for(int i = 0; i < slices; ++i){
				std::vector<Imath::Quatf> &slicevec = _quatValuesSliced[i];
				if(!slicevec.size()){
					slicevec.resize(1);
				}
			}
			break;
		case numericTypeQuatArray:
			_quatValuesSliced.resize(slices);
			break;
		case numericTypeCol4:
			_col4ValuesSliced.resize(slices);
			for (int i=0; i<slices; ++i){
				std::vector<Imath::Color4f> &slicevec = _col4ValuesSliced[i];
				if (!slicevec.size()){
					slicevec.resize(1);
				}
			}
			break;
		case numericTypeCol4Array:
			_col4ValuesSliced.resize(slices);
	}
	_slices = slices;
	}
}

std::string PolyValue::boolSliceAsString(unsigned int slice){
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

std::string PolyValue::stringSliceAsString(unsigned int slice){
	std::string script;
	std::ostringstream stream;
	if (slice >= _slices){
		slice = _slices - 1;
	}
	for (int i=0; i<_stringValuesSliced[slice].size(); ++i){
		stream << stringUtils::replace(_stringValuesSliced[slice][i], "\n", "\\n");
		if (i < _stringValuesSliced[slice].size() - 1){
			stream << " ~ ";
		}
	}
	std::ostringstream type;
	type << _type;
	script = stream.str() + "\\ | /" + type.str();
	return script;
}

std::string PolyValue::intSliceAsString(unsigned int slice){
	std::string script;
	std::ostringstream stream;
	for(int i = 0; i < _intValuesSliced[slice].size(); ++i){
		stream << _intValuesSliced[slice][i];
		if(i < _intValuesSliced[slice].size() - 1){
			stream << ",";
		}
		if(i % 20 == 19)
			stream << "\n";
	}
	return script;
}

std::string PolyValue::floatSliceAsString(unsigned int slice){
	std::string script;
	std::ostringstream stream;
	for(int i = 0; i < _floatValuesSliced[slice].size(); ++i){
		stream << _floatValuesSliced[slice][i];
		if(i < _floatValuesSliced[slice].size() - 1){
			stream << ",";
		}
		if(i % 20 == 19)
			stream << "\n";
	}
	return script;
}

std::string PolyValue::vec3SliceAsString(unsigned int slice){
	std::string script;
	std::ostringstream stream;
	for(int i = 0; i < _vec3ValuesSliced[slice].size(); ++i){
		stream << "(";
		Imath::V3f *vec = &_vec3ValuesSliced[slice][i];

		stream << vec->x << ",";
		stream << vec->y << ",";
		stream << vec->z << ")";

		if(i < _vec3ValuesSliced[slice].size() - 1){
			stream << ",";
		}

		if(i % 20 == 19)
			stream << "\n";
	}
	return script;
}

std::string PolyValue::col4SliceAsString(unsigned int slice){
	std::string script;
	std::ostringstream stream;
	for(int i = 0; i < _col4ValuesSliced[slice].size(); ++i){
		stream << "(";
		Imath::Color4f *col = &_col4ValuesSliced[slice][i];

		stream << col->r << ",";
		stream << col->g << ",";
		stream << col->b << ",";
		stream << col->a << ")";

		if(i < _col4ValuesSliced[slice].size() - 1){
			stream << ",";
		}

		if(i % 20 == 19)
			stream << "\n";
	}
	return script;
}

std::string PolyValue::quatSliceAsString(unsigned int slice){
	std::string script;
	std::ostringstream stream;
	for(int i = 0; i < _quatValuesSliced[slice].size(); ++i){
		stream << "(";
		Imath::Quatf *quat = &_quatValuesSliced[slice][i];

		stream << quat->r << ",";
		stream << quat->v.x << ",";
		stream << quat->v.y << ",";
		stream << quat->v.z << ")";

		if(i < _quatValuesSliced[slice].size() - 1){
			stream << ",";
		}

		if(i % 20 == 19)
			stream << "\n";
	}
	return script;
}

std::string PolyValue::matrix44SliceAsString(unsigned int slice){
	std::string script;
	std::ostringstream stream;
	for(int i = 0; i < _matrix44ValuesSliced[slice].size(); ++i){
		stream << "(";
		Imath::M44f *mat = &_matrix44ValuesSliced[slice][i];

		stream << mat->x[0][0] << ",";
		stream << mat->x[0][1] << ",";
		stream << mat->x[0][2] << ",";
		stream << mat->x[0][3] << ",";
		stream << mat->x[1][0] << ",";
		stream << mat->x[1][1] << ",";
		stream << mat->x[1][2] << ",";
		stream << mat->x[1][3] << ",";
		stream << mat->x[2][0] << ",";
		stream << mat->x[2][1] << ",";
		stream << mat->x[2][2] << ",";
		stream << mat->x[2][3] << ",";
		stream << mat->x[3][0] << ",";
		stream << mat->x[3][1] << ",";
		stream << mat->x[3][2] << ",";
		stream << mat->x[3][3] << ")";

		if(i < _matrix44ValuesSliced.size() - 1){
			stream << ",";
		}

		if(i % 20 == 19)
			stream << "\n";
	}
	return script;
}

std::string PolyValue::sliceAsString(unsigned int slice){
	std::string script;
	if (_type != typeAny &&
		_type != stringTypeAny &&
		_type != numericTypeAny &&
		_type != boolTypeAny){
		std::ostringstream stream;
		if (slice >= _slices){
			slice = _slices - 1;
		}
		if (_type == stringType || _type == stringTypeArray ||
			_type == pathType || _type == pathTypeArray){
			stream << stringSliceAsString(slice);
		} else if (_type == boolType ||_type == boolTypeArray){
			stream << boolSliceAsString(slice);
		} else if (_type == numericTypeInt || _type == numericTypeIntArray){
			stream << intSliceAsString(slice);
		} else if (_type == numericTypeFloat || _type == numericTypeFloatArray){
			stream << floatSliceAsString(slice);
		} else if (_type == numericTypeVec3 || _type == numericTypeVec3Array){
			stream << vec3SliceAsString(slice);
		} else if (_type == numericTypeCol4 || _type == numericTypeCol4Array){
			stream << col4SliceAsString(slice);
		} else if (_type == numericTypeQuat || _type ==numericTypeQuatArray){
			stream << quatSliceAsString(slice);
		} else if (_type == numericTypeMatrix44 || _type == numericTypeMatrix44Array){
			stream << matrix44SliceAsString(slice);
		}
		std::ostringstream type;
		type << _type;
		script = "[" + stream.str() + "] " + type.str();
	}
	return script;
}

std::string PolyValue::asString(){
	return sliceAsString(0);
}

void PolyValue::setFromString(const std::string &value){
	std::cout << "PolyValue.setFromString" << std::endl;
	//@TODO determine the type first
	// numeric types
	std::string tmp = stringUtils::replace(value, "\n", "");
	std::vector<std::string> fields;
	stringUtils::split(tmp, fields, " ");
	if (fields.size() == 2){
		std::string valuesStr = stringUtils::strip(fields[0], "[]");
		PolyValue::ValueType type = PolyValue::ValueType(stringUtils::parseInt(fields.back()));
		switch (type){
		case stringType:
			stringSetFromString(value);
			break;
		case stringTypeArray:
			stringSetFromString(value);
			break;
		case pathType:
			stringSetFromString(value);
			break;
		case pathTypeArray:
			stringSetFromString(value);
			break;
		case boolType:
			boolSetFromString(value);
			break;
		case boolTypeArray:
			boolSetFromString(value);
			break;
		case numericTypeInt:
			intSetFromString(value);
			break;
		case numericTypeIntArray:
			intSetFromString(value);
			break;
		case numericTypeFloat:
			floatSetFromString(value);
			break;
		case numericTypeFloatArray:
			floatSetFromString(value);
			break;
		case numericTypeVec3:
			vec3SetFromString(value);
			break;
		case numericTypeVec3Array:
			vec3SetFromString(value);
			break;
		case numericTypeMatrix44:
			matrix44SetFromString(value);
			break;
		case numericTypeMatrix44Array:
			matrix44SetFromString(value);
			break;
		case numericTypeQuat:
			quatSetFromString(value);
			break;
		case numericTypeQuatArray:
			quatSetFromString(value);
			break;
		}
	}
	std::cout << "PolyValue.setFromString: Done" << std::endl;
}

void PolyValue::stringSetFromString(const std::string &value){
	std::string tmp = stringUtils::replace(value, "\\n", "\n");
	std::vector<std::string> fields;
	stringUtils::split(tmp, fields, "\\ | /");
	if (fields.size() == 2){
		std::string valuesStr = fields[0];
		PolyValue::ValueType type = PolyValue::ValueType(stringUtils::parseInt(fields[1]));
		_stringValuesSliced.resize(1);
		_stringValuesSliced[0].clear();
		std::vector<std::string> values;
		stringUtils::split(valuesStr, values, " ~ ");
		for (int i=0; i<values.size(); ++i){
			_stringValuesSliced[0].push_back(values[i]);
		}
		if (type == stringTypeArray || type == pathTypeArray){
			_isArray = true;
		}
	}

}

void PolyValue::boolSetFromString(const std::string &value){
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

void PolyValue::intSetFromString(const std::string &value){
	std::string tmp = stringUtils::replace(value, "\n", "");
	std::vector<std::string> fields;
	stringUtils::split(tmp, fields, " ");
	std::string valuesStr = stringUtils::strip(fields[0], "[]");
	_intValuesSliced.resize(1);
	_intValuesSliced[0].clear();

	std::vector<std::string> values;
	stringUtils::split(valuesStr, values, ",");
	for(int i = 0; i < values.size(); ++i){
		int value = stringUtils::parseInt(values[i]);
		_intValuesSliced[0].push_back(value);
	}
}

void PolyValue::floatSetFromString(const std::string &value){
	std::string tmp = stringUtils::replace(value, "\n", "");
	std::vector<std::string> fields;
	stringUtils::split(tmp, fields, " ");
	std::string valuesStr = stringUtils::strip(fields[0], "[]");
	_floatValuesSliced.resize(1);
	_floatValuesSliced[0].clear();

	std::vector<std::string> values;
	stringUtils::split(valuesStr, values, ",");
	for(int i = 0; i < values.size(); ++i){
		float value = stringUtils::parseFloat(values[i]);
		_floatValuesSliced[0].push_back(value);
	}
}

void PolyValue::vec3SetFromString(const std::string &value){
	std::string tmp = stringUtils::replace(value, "\n", "");
	std::vector<std::string> fields;
	stringUtils::split(tmp, fields, " ");
	std::string valuesStr = stringUtils::strip(fields[0], "[]");
	_vec3ValuesSliced.resize(1);
	_vec3ValuesSliced[0].clear();

	std::vector<std::string> values;
	stringUtils::split(valuesStr, values, "),(");

	for(int i = 0; i < values.size(); ++i){
		std::string numericValuesStr = stringUtils::strip(values[i], "()");
		std::vector<std::string> numericValues;
		stringUtils::split(numericValuesStr, numericValues, ",");

		if(numericValues.size() == 3){
			float x = stringUtils::parseFloat(numericValues[0]);
			float y = stringUtils::parseFloat(numericValues[1]);
			float z = stringUtils::parseFloat(numericValues[2]);

			Imath::V3f vec(x, y, z);
			_vec3ValuesSliced[0].push_back(vec);
		}
	}
}

void PolyValue::quatSetFromString(const std::string &value){
	std::string tmp = stringUtils::replace(value, "\n", "");
	std::vector<std::string> fields;
	stringUtils::split(tmp, fields, " ");
	std::string valuesStr = stringUtils::strip(fields[0], "[]");
	_quatValuesSliced.resize(1);
	_quatValuesSliced[0].clear();

	std::vector<std::string> values;
	stringUtils::split(valuesStr, values, "),(");

	for(int i = 0; i < values.size(); ++i){
		std::string numericValuesStr = stringUtils::strip(values[i], "()");
		std::vector<std::string> numericValues;
		stringUtils::split(numericValuesStr, numericValues, ",");

		if(numericValues.size() == 4){
			float r = stringUtils::parseFloat(numericValues[0]);
			float x = stringUtils::parseFloat(numericValues[1]);
			float y = stringUtils::parseFloat(numericValues[2]);
			float z = stringUtils::parseFloat(numericValues[3]);

			Imath::Quatf vec(r, x, y, z);
			_quatValuesSliced[0].push_back(vec);
		}
	}
}

void PolyValue::matrix44SetFromString(const std::string &value){
	std::string tmp = stringUtils::replace(value, "\n", "");
	std::vector<std::string> fields;
	stringUtils::split(tmp, fields, " ");
	std::string valuesStr = stringUtils::strip(fields[0], "[]");
	_matrix44ValuesSliced.resize(1);
	_matrix44ValuesSliced[0].clear();

	std::vector<std::string> values;
	stringUtils::split(valuesStr, values, "),(");

	for(int i = 0; i < values.size(); ++i){
		std::string numericValuesStr = stringUtils::strip(values[i], "()");
		std::vector<std::string> numericValues;
		stringUtils::split(numericValuesStr, numericValues, ",");

		if(numericValues.size() == 16){
			Imath::M44f matrix(
				stringUtils::parseFloat(numericValues[0]), stringUtils::parseFloat(numericValues[1]), stringUtils::parseFloat(numericValues[2]), stringUtils::parseFloat(numericValues[3]),
				stringUtils::parseFloat(numericValues[4]), stringUtils::parseFloat(numericValues[5]), stringUtils::parseFloat(numericValues[6]), stringUtils::parseFloat(numericValues[7]),
				stringUtils::parseFloat(numericValues[8]), stringUtils::parseFloat(numericValues[9]), stringUtils::parseFloat(numericValues[10]), stringUtils::parseFloat(numericValues[11]),
				stringUtils::parseFloat(numericValues[12]), stringUtils::parseFloat(numericValues[13]), stringUtils::parseFloat(numericValues[14]), stringUtils::parseFloat(numericValues[15]));

			_matrix44ValuesSliced[0].push_back(matrix);
		}
	}
}

void PolyValue::col4SetFromString(const std::string &value){
	std::string tmp = stringUtils::replace(value, "\n", "");
	std::vector<std::string> fields;
	stringUtils::split(tmp, fields, " ");
	std::string valuesStr = stringUtils::strip(fields[0], "[]");
	_col4ValuesSliced.resize(1);
	_col4ValuesSliced[0].clear();

	std::vector<std::string> values;
	stringUtils::split(valuesStr, values, "),(");

	for(int i = 0; i < values.size(); ++i){
		std::string numericValuesStr = stringUtils::strip(values[i], "()");
		std::vector<std::string> numericValues;
		stringUtils::split(numericValuesStr, numericValues, ",");

		if(numericValues.size() == 4){
			float r = stringUtils::parseFloat(numericValues[0]);
			float g = stringUtils::parseFloat(numericValues[1]);
			float b = stringUtils::parseFloat(numericValues[2]);
			float a = stringUtils::parseFloat(numericValues[3]);

			Imath::Color4f col(r, g, b, a);
			_col4ValuesSliced[0].push_back(col);
		}
	}
}

void PolyValue::setStringValueAtSlice(unsigned int slice, unsigned int id, std::string &value){
	if (slice < _stringValuesSliced.size()){
		std::vector<std::string> &slicevec = _stringValuesSliced[slice];
		if (id < slicevec.size()){
			slicevec[id] = value;
		}
	}
}

void PolyValue::setPathValueAtSlice(unsigned int slice, unsigned int id, std::string &value){
	setStringValueAtSlice(slice, id, value);
}

void PolyValue::setBoolValueAtSlice(unsigned int slice, unsigned int id, bool value){
	if (slice < _boolValuesSliced.size()){
		std::vector<bool> &slicevec = _boolValuesSliced[slice];
		if (id < slicevec.size()){
			slicevec[id] = value;
		}
	}
}

void PolyValue::setIntValueAtSlice(unsigned int slice, unsigned int id, int value){
	if (slice < _intValuesSliced.size()){
		std::vector<int> &slicevec = _intValuesSliced[slice];
		if (id < slicevec.size()){
			slicevec[id] = value;
		}
	}
}

void PolyValue::setFloatValueAtSlice(unsigned int slice, unsigned int id, float value){
	if (slice < _floatValuesSliced.size()){
		std::vector<float> &slicevec = _floatValuesSliced[slice];
		if (id < slicevec.size()){
			slicevec[id] = value;
		}
	}
}

void PolyValue::setMatrix44ValueAtSlice(unsigned int slice, unsigned int id, const Imath::M44f &value){
	if(slice < _matrix44ValuesSliced.size()){
		std::vector<Imath::M44f> &slicevec = _matrix44ValuesSliced[slice];
		if(id < slicevec.size()){
			slicevec[id] = value;
		}
	}
}

void PolyValue::setVec3ValueAtSlice(unsigned int slice, unsigned int id, const Imath::V3f &value){
	if (slice < _vec3ValuesSliced.size()){
		std::vector<Imath::V3f> &slicevec = _vec3ValuesSliced[slice];
		if (id < slicevec.size()){
			slicevec[id] = value;
		}
	}
}

void PolyValue::setCol4ValueAtSlice(unsigned int slice, unsigned int id, const Imath::Color4f &value){
	if (slice < _col4ValuesSliced.size()){
		std::vector<Imath::Color4f> &slicevec = _col4ValuesSliced[slice];
		if (id < _col4ValuesSliced.size()){
			slicevec[id] = value;
		}
	}
}

void PolyValue::setQuatValueAtSlice(unsigned int slice, unsigned int id, const Imath::Quatf &value){
	if(slice < _quatValuesSliced.size()){
		std::vector<Imath::Quatf> &slicevec = _quatValuesSliced[slice];
		if(id < slicevec.size()){
			slicevec[id] = value;
		}
	}
}

void PolyValue::setStringValueAt(unsigned int id, std::string value){
	setStringValueAtSlice(0, id, value);
}

void PolyValue::setPathValueAt(unsigned int id, std::string value){
	setStringValueAtSlice(0, id, value);
}

void PolyValue::setBoolValueAt(unsigned int id, bool value){
	setBoolValueAtSlice(0, id, value);
}

void PolyValue::setIntValueAt(unsigned int id, int value){
	setIntValueAtSlice(0, id, value);
}

void PolyValue::setFloatValueAt(unsigned int id, float value){
	setFloatValueAtSlice(0, id, value);
}

void PolyValue::setVec3ValueAt(unsigned int id, const Imath::V3f &value){
	setVec3ValueAtSlice(0, id, value);
}

void PolyValue::setCol4ValueAt(unsigned int id, const Imath::Color4f &value){
	setCol4ValueAtSlice(0, id, value);
}

void PolyValue::setQuatValueAt(unsigned int id, const Imath::Quatf &value){
	setQuatValueAtSlice(0, id, value);
}

void PolyValue::setMatrix44ValueAt(unsigned int id, const Imath::M44f &value){
	setMatrix44ValueAtSlice(0, id, value);
}

const std::vector<std::string> &PolyValue::stringValues(){
	return _stringValuesSliced[0];
}

const std::vector<std::string> &PolyValue::pathValues(){
	return stringValues();
}

const std::vector<bool> &PolyValue::boolValues(){
	return _boolValuesSliced[0];
}

const std::vector<int> &PolyValue::intValues(){
	return _intValuesSliced[0];
}

const std::vector<float> &PolyValue::floatValues(){
	return _floatValuesSliced[0];
}

const std::vector<Imath::V3f> &PolyValue::vec3Values(){
	return _vec3ValuesSliced[0];
}

const std::vector<Imath::Color4f> &PolyValue::col4Values(){
	return _col4ValuesSliced[0];
}

const std::vector<Imath::M44f> &PolyValue::matrix44Values(){
	return _matrix44ValuesSliced[0];
}

const std::vector<Imath::Quatf> &PolyValue::quatValues(){
	return _quatValuesSliced[0];
}

void PolyValue::setStringValues(const std::vector<std::string> &values){
	setStringValuesSlice(0, values);
}

void PolyValue::setPathValues(const std::vector<std::string> &values){
	setStringValuesSlice(0, values);
}

void PolyValue::setBoolValues(const std::vector<bool> &values){
	setBoolValuesSlice(0, values);
}

void PolyValue::setIntValues(const std::vector<int> &values){
	setIntValuesSlice(0, values);
}

void PolyValue::setFloatValues(const std::vector<float> &values){
	setFloatValuesSlice(0, values);
}

void PolyValue::setVec3Values(const std::vector<Imath::V3f> &values){
	setVec3ValuesSlice(0, values);
}

void PolyValue::setMatrix44Values(const std::vector<Imath::M44f> &values){
	setMatrix44ValuesSlice(0, values);
}

void PolyValue::setQuatValues(const std::vector<Imath::Quatf> &values){
	setQuatValuesSlice(0, values);
}

void PolyValue::setCol4Values(const std::vector<Imath::Color4f> &values){
	setCol4ValuesSlice(0, values);
}

void PolyValue::setStringValuesSlice(unsigned int slice, const std::vector<std::string> &values){
	if (slice < _stringValuesSliced.size()){
		_stringValuesSliced[slice] = values;
	}
}

void PolyValue::setPathValuesSlice(unsigned int slice, const std::vector<std::string> &values){
	if (slice < _stringValuesSliced.size()){
		_stringValuesSliced[slice] = values;
	}
}

void PolyValue::setBoolValuesSlice(unsigned int slice, const std::vector<bool> &values){
	if (slice < _boolValuesSliced.size()){
		_boolValuesSliced[slice] = values;
	}
}

void PolyValue::setIntValuesSlice(unsigned int slice, const std::vector<int> &values){
	if (slice < _intValuesSliced.size()){
		_intValuesSliced[slice] = values;
	}
}

void PolyValue::setFloatValuesSlice(unsigned int slice, const std::vector<float> &values){
	if (slice < _floatValuesSliced.size()){
		_floatValuesSliced[slice] = values;
	}
}

void PolyValue::setVec3ValuesSlice(unsigned int slice, const std::vector<Imath::V3f> &values){
	if (slice < _vec3ValuesSliced.size()){
		_vec3ValuesSliced[slice] = values;
	}
}

void PolyValue::setMatrix44ValuesSlice(unsigned int slice, const std::vector<Imath::M44f> &values){
	if (slice < _matrix44ValuesSliced.size()){
		_matrix44ValuesSliced[slice] = values;
	}
}

void PolyValue::setQuatValuesSlice(unsigned int slice, const std::vector<Imath::Quatf> &values){
	if (slice < _quatValuesSliced.size()){
		_quatValuesSliced[slice] = values;
	}
}

void PolyValue::setCol4ValuesSlice(unsigned int slice, const std::vector<Imath::Color4f> &values){
	if (slice < _col4ValuesSliced.size()){
		_col4ValuesSliced[slice] = values;
	}
}

const std::string PolyValue::stringValueAt(unsigned int id){
	return stringValueAtSlice(0, id);
}

const std::string PolyValue::pathValueAt(unsigned int id){
	return pathValueAtSlice(0, id);
}

const bool PolyValue::boolValueAt(unsigned int id){
	return boolValueAtSlice(0, id);
}

const int PolyValue::intValueAt(unsigned int id){
	return intValueAtSlice(0, id);
}

const float PolyValue::floatValueAt(unsigned int id){
	return floatValueAtSlice(0, id);
}

const Imath::V3f PolyValue::vec3ValueAt(unsigned int id){
	return vec3ValueAtSlice(0, id);
}

const Imath::Color4f PolyValue::col4ValueAt(unsigned int id){
	return col4ValueAtSlice(0, id);
}

const Imath::M44f PolyValue::matrix44ValueAt(unsigned int id){
	return matrix44ValueAtSlice(0, id);
}

const Imath::Quatf PolyValue::quatValueAt(unsigned int id){
	return quatValueAtSlice(0, id);
}


std::string PolyValue::stringValueAtSlice(unsigned int slice, unsigned int id){
	std::cout << "PolyValue.stringValueAtSlice" << std::endl;
	if(slice >= _stringValuesSliced.size()){
		slice = _stringValuesSliced.size() - 1;
	}
	std::vector<std::string> &slicevec = _stringValuesSliced[slice];
	int size = slicevec.size();
	if(id < size){
		return slicevec[id];
	}
	else if(size){
		return slicevec[size - 1];
	}
	return "";
}

std::string PolyValue::pathValueAtSlice(unsigned int slice, unsigned int id){
	if(slice >= _stringValuesSliced.size()){
		slice = _stringValuesSliced.size() - 1;
	}

	std::vector<std::string> &slicevec = _stringValuesSliced[slice];
	int size = slicevec.size();
	if(id < size){
		return slicevec[id];
	}
	else if(size){
		return slicevec[size - 1];
	}
	return "";
}

bool PolyValue::boolValueAtSlice(unsigned int slice, unsigned int id){
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
	return 0;
}

int PolyValue::intValueAtSlice(unsigned int slice, unsigned int id){
	if(slice >= _intValuesSliced.size()){
		slice = _intValuesSliced.size() - 1;
	}
	std::vector<int> &slicevec = _intValuesSliced[slice];
	int size = slicevec.size();
	if(id < size){
		return slicevec[id];
	}
	else if(size){
		return slicevec[size - 1];
	}
	return 0;
}

float PolyValue::floatValueAtSlice(unsigned int slice, unsigned int id){
	if(slice >= _floatValuesSliced.size()){
		slice = _floatValuesSliced.size() - 1;
	}
	std::vector<float> &slicevec = _floatValuesSliced[slice];
	int size = slicevec.size();
	if(id < size){
		return slicevec[id];
	}
	else if(size){
		return slicevec[size - 1];
	}
	return 0.0;
}

Imath::V3f PolyValue::vec3ValueAtSlice(unsigned int slice, unsigned int id){
	if(slice >= _vec3ValuesSliced.size()){
		slice = _vec3ValuesSliced.size() - 1;
	}
	std::vector<Imath::V3f> &slicevec = _vec3ValuesSliced[slice];
	int size = slicevec.size();
	if(id < size){
		return slicevec[id];
	}
	else if(size){
		return slicevec[size - 1];
	}
	return Imath::V3f(0.0, 0.0, 0.0);
}

Imath::Color4f PolyValue::col4ValueAtSlice(unsigned int slice, unsigned int id){
	if(slice >= _col4ValuesSliced.size()){
		slice = _col4ValuesSliced.size() - 1;
	}
	std::vector<Imath::Color4f> &slicevec = _col4ValuesSliced[slice];
	int size = slicevec.size();
	if(id < size){
		return slicevec[id];
	}
	else if(size){
		return slicevec[size - 1];
	}
	return Imath::Color4f(1.0, 1.0, 1.0, 1.0);
}

Imath::M44f PolyValue::matrix44ValueAtSlice(unsigned int slice, unsigned int id){
	if(slice >= _matrix44ValuesSliced.size()){
		slice = _matrix44ValuesSliced.size() - 1;
	}
	std::vector<Imath::M44f> &slicevec = _matrix44ValuesSliced[slice];
	int size = slicevec.size();
	if(id < size){
		return slicevec[id];
	}
	else if(size){
		return slicevec[size - 1];
	}
	return Imath::identity44f;
}

Imath::Quatf PolyValue::quatValueAtSlice(unsigned int slice, unsigned int id){
	if(slice >= _quatValuesSliced.size()){
		slice = _quatValuesSliced.size() - 1;
	}
	std::vector<Imath::Quatf> &slicevec = _quatValuesSliced[slice];
	int size = slicevec.size();
	if(id < size){
		return slicevec[id];
	}
	else if(size){
		return slicevec[size - 1];
	}
	return Imath::Quatf(0.0, 0.0, 0.0, 1.0);
}

