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

#include <ImathMatrixAlgo.h>

#include "Numeric.h"
#include "stringUtils.h"

using namespace coral;

Numeric::Numeric():
	_type(numericTypeAny),
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
}

void Numeric::copy(const Value *other){
	const Numeric *otherNum = dynamic_cast<const Numeric*>(other);
	
	if(otherNum){
		_type = otherNum->_type;
		_isArray = otherNum->_isArray;

		_intValuesSliced = otherNum->_intValuesSliced;
		_floatValuesSliced = otherNum->_floatValuesSliced;
		_vec3ValuesSliced = otherNum->_vec3ValuesSliced;
		_quatValuesSliced = otherNum->_quatValuesSliced;
		_matrix44ValuesSliced = otherNum->_matrix44ValuesSliced;
		_col4ValuesSliced = otherNum->_col4ValuesSliced;
	}
}

bool Numeric::isArray(){
	return _isArray;
}

unsigned int Numeric::size(){
	return sizeSlice(0);
}

unsigned int Numeric::sizeSlice(unsigned int slice){
	if(slice >= _slices){
		slice = _slices - 1;
	}

	if(_type == numericTypeAny){
		return 0;
	}
	else if(_type == numericTypeIntArray || _type == numericTypeInt){
		return _intValuesSliced[slice].size();
	}
	else if(_type == numericTypeFloatArray || _type == numericTypeFloat){
		return _floatValuesSliced[slice].size();
	}
	else if(_type == numericTypeVec3Array || _type == numericTypeVec3){
		return _vec3ValuesSliced[slice].size();
	}
	else if(_type == numericTypeQuatArray || _type == numericTypeQuat){
		return _quatValuesSliced[slice].size();
	}
	else if(_type == numericTypeMatrix44Array || _type == numericTypeMatrix44){
		return _matrix44ValuesSliced[slice].size();
	}
	else if(_type == numericTypeCol4Array || _type == numericTypeCol4){
		return _col4ValuesSliced[slice].size();
	}

	return 0;
}

Numeric::Type Numeric::type(){
	return _type;
}

void Numeric::setType(Numeric::Type type){
	_type = type;
	_isArray = false;
	
	if(type == numericTypeInt){
		_intValuesSliced.resize(_slices);
		for(int i = 0; i < _slices; ++i){
			_intValuesSliced[i].resize(1);
		}
	}
	else if(type == numericTypeIntArray){
		_intValuesSliced.resize(_slices);
		_isArray = true;
	}
	else if(type == numericTypeFloat){
		_floatValuesSliced.resize(_slices);
		for(int i = 0; i < _slices; ++i){
			_floatValuesSliced[i].resize(1);
		}
	}
	else if(type == numericTypeFloatArray){
		_floatValuesSliced.resize(_slices);
		_isArray = true;
	}
	else if(type == numericTypeVec3){
		_vec3ValuesSliced.resize(_slices);
		for(int i = 0; i < _slices; ++i){
			_vec3ValuesSliced[i].resize(1);
		}
	}
	else if(type == numericTypeVec3Array){
		_vec3ValuesSliced.resize(_slices);
		_isArray = true;
	}
	else if(type == numericTypeQuat){
		_quatValuesSliced.resize(_slices);
		for(int i = 0; i < _slices; ++i){
			_quatValuesSliced[i].resize(1);
		}
	}
	else if(type == numericTypeQuatArray){
		_quatValuesSliced.resize(_slices);
		_isArray = true;
	}
	else if(type == numericTypeMatrix44){
		_matrix44ValuesSliced.resize(_slices);
		for(int i = 0; i < _slices; ++i){
			_matrix44ValuesSliced[i].resize(1);
		}
	}
	else if(type == numericTypeMatrix44Array){
		_matrix44ValuesSliced.resize(_slices);
		_isArray = true;
	}
	else if(type == numericTypeCol4){
		_col4ValuesSliced.resize(_slices);
		for(int i = 0; i < _slices; ++i){
			_col4ValuesSliced[i].resize(1);
		}
	}
	else if(type == numericTypeCol4Array){
		_col4ValuesSliced.resize(_slices);
		_isArray = true;
	}
}

void Numeric::resize(unsigned int newSize){
	resizeSlice(0, newSize);
}

void Numeric::resizeSlice(unsigned int slice, unsigned int newSize){
	if(_type != numericTypeAny){
		if(_type == numericTypeInt || _type == numericTypeIntArray){
			for(int i = 0; i < _intValuesSliced.size(); ++i){
				_intValuesSliced[i].resize(newSize);
			}
		}
		else if(_type == numericTypeFloat || _type == numericTypeFloatArray){
			for(int i = 0; i < _floatValuesSliced.size(); ++i){
				_floatValuesSliced[i].resize(newSize);
			}
		}
		else if(_type == numericTypeVec3 || _type == numericTypeVec3Array){
			for(int i = 0; i < _vec3ValuesSliced.size(); ++i){
				_vec3ValuesSliced[i].resize(newSize);
			}
		}
		else if(_type == numericTypeQuat || _type == numericTypeQuatArray){
			for(int i = 0; i < _quatValuesSliced.size(); ++i){
				_quatValuesSliced[i].resize(newSize);
			}
		}
		else if(_type == numericTypeMatrix44 || _type == numericTypeMatrix44Array){
			for(int i = 0; i < _matrix44ValuesSliced.size(); ++i){
				_matrix44ValuesSliced[i].resize(newSize);
			}
		}
		else if(_type == numericTypeCol4 || _type == numericTypeCol4Array){
			for(int i = 0; i < _col4ValuesSliced.size(); ++i){
				_col4ValuesSliced[i].resize(newSize);
			}
		}
	}
}

// TODO: Really Useful? Never used...
bool Numeric::isArrayType(Numeric::Type type){
	bool arrayType = false;
	if(
		type == numericTypeIntArray ||
		type == numericTypeFloatArray ||
		type == numericTypeVec3Array ||
		type == numericTypeQuatArray ||
		type == numericTypeMatrix44Array ||
		type == numericTypeCol4Array){
		
		arrayType = true;
	}
	
	return arrayType;
}

void Numeric::setIntValueAt(unsigned int id, int value){
	setIntValueAtSlice(0, id, value);
}

void Numeric::setFloatValueAt(unsigned int id, float value){
	setFloatValueAtSlice(0, id, value);
}

void Numeric::setVec3ValueAt(unsigned int id, const Imath::V3f &value){
	setVec3ValueAtSlice(0, id, value);
}

void Numeric::setCol4ValueAt(unsigned int id, const Imath::Color4f &value){
	setCol4ValueAtSlice(0, id, value);
}

void Numeric::setQuatValueAt(unsigned int id, const Imath::Quatf &value){
	setQuatValueAtSlice(0, id, value);
}

void Numeric::setMatrix44ValueAt(unsigned int id, const Imath::M44f &value){
	setMatrix44ValueAtSlice(0, id, value);
}

const std::vector<int> &Numeric::intValues(){
	return _intValuesSliced[0];
}

const std::vector<float> &Numeric::floatValues(){
	return _floatValuesSliced[0];
}

const std::vector<Imath::V3f> &Numeric::vec3Values(){
	return _vec3ValuesSliced[0];
}

const std::vector<Imath::Color4f> &Numeric::col4Values(){
	return _col4ValuesSliced[0];
}

const std::vector<Imath::Quatf> &Numeric::quatValues(){
	return _quatValuesSliced[0];
}

const std::vector<Imath::M44f> &Numeric::matrix44Values(){
	return _matrix44ValuesSliced[0];
}

int Numeric::intValueAt(unsigned int id){
	return intValueAtSlice(0, id);
}

float Numeric::floatValueAt(unsigned int id){
	return floatValueAtSlice(0, id);
}

Imath::V3f Numeric::vec3ValueAt(unsigned int id){
	return vec3ValueAtSlice(0, id);
}

Imath::Color4f Numeric::col4ValueAt(unsigned int id){
	return col4ValueAtSlice(0, id);
}

Imath::Quatf Numeric::quatValueAt(unsigned int id){
	return quatValueAtSlice(0, id);
}

Imath::M44f Numeric::matrix44ValueAt(unsigned int id){
	return matrix44ValueAtSlice(0, id);
}

void Numeric::setIntValues(const std::vector<int> &values){
	setIntValuesSlice(0, values);
}

void Numeric::setFloatValues(const std::vector<float> &values){
	setFloatValuesSlice(0, values);
}

void Numeric::setVec3Values(const std::vector<Imath::V3f> &values){
	setVec3ValuesSlice(0, values);
}

void Numeric::setCol4Values(const std::vector<Imath::Color4f> &values){
	setCol4ValuesSlice(0, values);
}

void Numeric::setQuatValues(const std::vector<Imath::Quatf> &values){
	setQuatValuesSlice(0, values);
}

void Numeric::setMatrix44Values(const std::vector<Imath::M44f> &values){
	setMatrix44ValuesSlice(0, values);
}

std::string Numeric::sliceAsString(unsigned int slice){
	std::string script;

	if(_type != numericTypeAny){
		std::ostringstream stream;
		
		if(slice >= _slices){
			slice = _slices - 1;
		}

		if(_type == numericTypeInt || _type == numericTypeIntArray){
			for(int i = 0; i < _intValuesSliced[slice].size(); ++i){
				stream << _intValuesSliced[slice][i];
				
				if(i < _intValuesSliced[slice].size() - 1){
					stream << ",";
				}
				
				if(i % 20 == 19)
					stream << "\n";
			}
		}
		else if(_type == numericTypeFloat || _type == numericTypeFloatArray){
			for(int i = 0; i < _floatValuesSliced[slice].size(); ++i){
				stream << _floatValuesSliced[slice][i];
				
				if(i < _floatValuesSliced[slice].size() - 1){
					stream << ",";
				}
				
				if(i % 20 == 19)
					stream << "\n";
			}
		}
		else if(_type == numericTypeVec3 || _type == numericTypeVec3Array){
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
		}
		else if(_type == numericTypeCol4 || _type == numericTypeCol4Array){
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
		}
		else if(_type == numericTypeQuat || _type == numericTypeQuatArray){
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
		}
		else if(_type == numericTypeMatrix44 || _type == numericTypeMatrix44Array){
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
		}
		
		std::ostringstream type;
		type << _type;
		
		script = "[" + stream.str() + "] " + type.str();
	}
	
	return script;
}

std::string Numeric::asString(){
	return sliceAsString(0);
}

void Numeric::setFromString(const std::string &value){
	std::string tmp = stringUtils::replace(value, "\n", "");
	std::vector<std::string> fields;
	stringUtils::split(tmp, fields, " ");
	
	if(fields.size() == 2){
		std::string valuesStr = stringUtils::strip(fields[0], "[]");
		Numeric::Type type = Numeric::Type(stringUtils::parseInt(fields[1]));
		
		if(type == Numeric::numericTypeInt || type == Numeric::numericTypeIntArray){
			_intValuesSliced.resize(1);
			_intValuesSliced[0].clear();

			std::vector<std::string> values;
			stringUtils::split(valuesStr, values, ",");
			for(int i = 0; i < values.size(); ++i){
				int value = stringUtils::parseInt(values[i]);
				_intValuesSliced[0].push_back(value);
			}
		}
		else if(type == Numeric::numericTypeFloat || type == Numeric::numericTypeFloatArray){
			_floatValuesSliced.resize(1);
			_floatValuesSliced[0].clear();

			std::vector<std::string> values;
			stringUtils::split(valuesStr, values, ",");
			for(int i = 0; i < values.size(); ++i){
				float value = stringUtils::parseFloat(values[i]);
				_floatValuesSliced[0].push_back(value);
			}
		}
		else if(type == Numeric::numericTypeVec3 || type == Numeric::numericTypeVec3Array){
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
		else if(type == Numeric::numericTypeQuat || type == Numeric::numericTypeQuatArray){
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
		else if(type == Numeric::numericTypeMatrix44 || type == Numeric::numericTypeMatrix44Array){
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
		else if(type == Numeric::numericTypeCol4 || type == Numeric::numericTypeCol4Array){
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
	}
}

void Numeric::setIntValueAtSlice(unsigned int slice, unsigned int id, int value){
	if(slice < _intValuesSliced.size()){
		std::vector<int> &slicevec = _intValuesSliced[slice];
		if(id < slicevec.size()){
			slicevec[id] = value;
		}
	}
}

void Numeric::setFloatValueAtSlice(unsigned int slice, unsigned int id, float value){
	if(slice < _floatValuesSliced.size()){
		std::vector<float> &slicevec = _floatValuesSliced[slice];
		if(id < slicevec.size()){
			slicevec[id] = value;
		}
	}
}

void Numeric::setVec3ValueAtSlice(unsigned int slice, unsigned int id, const Imath::V3f &value){
	if(slice < _vec3ValuesSliced.size()){
		std::vector<Imath::V3f> &slicevec = _vec3ValuesSliced[slice];
		if(id < slicevec.size()){
			slicevec[id] = value;
		}
	}
}

void Numeric::setMatrix44ValueAtSlice(unsigned int slice, unsigned int id, const Imath::M44f &value){
	if(slice < _matrix44ValuesSliced.size()){
		std::vector<Imath::M44f> &slicevec = _matrix44ValuesSliced[slice];
		if(id < slicevec.size()){
			slicevec[id] = value;
		}
	}
}

void Numeric::setCol4ValueAtSlice(unsigned int slice, unsigned int id, const Imath::Color4f &value){
	if(slice < _col4ValuesSliced.size()){
		std::vector<Imath::Color4f> &slicevec = _col4ValuesSliced[slice];
		if(id < slicevec.size()){
			slicevec[id] = value;
		}
	}
}

void Numeric::setQuatValueAtSlice(unsigned int slice, unsigned int id, const Imath::Quatf &value){
	if(slice < _quatValuesSliced.size()){
		std::vector<Imath::Quatf> &slicevec = _quatValuesSliced[slice];
		if(id < slicevec.size()){
			slicevec[id] = value;
		}
	}
}

int Numeric::intValueAtSlice(unsigned int slice, unsigned int id){
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

float Numeric::floatValueAtSlice(unsigned int slice, unsigned int id){
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

Imath::V3f Numeric::vec3ValueAtSlice(unsigned int slice, unsigned int id){
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

Imath::Color4f Numeric::col4ValueAtSlice(unsigned int slice, unsigned int id){
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

Imath::Quatf Numeric::quatValueAtSlice(unsigned int slice, unsigned int id){
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

Imath::M44f Numeric::matrix44ValueAtSlice(unsigned int slice, unsigned int id){
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

void Numeric::setIntValuesSlice(unsigned int slice, const std::vector<int> &values){
	if(slice < _intValuesSliced.size()){
		_intValuesSliced[slice] = values;
	}
}

void Numeric::setFloatValuesSlice(unsigned int slice, const std::vector<float> &values){
	if(slice < _floatValuesSliced.size()){
		_floatValuesSliced[slice] = values;
	}
}

void Numeric::setVec3ValuesSlice(unsigned int slice, const std::vector<Imath::V3f> &values){
	if(slice < _vec3ValuesSliced.size()){
		_vec3ValuesSliced[slice] = values;
	}
}

void Numeric::setQuatValuesSlice(unsigned int slice, const std::vector<Imath::Quatf> &values){
	if(slice < _quatValuesSliced.size()){
		_quatValuesSliced[slice] = values;
	}
}

void Numeric::setCol4ValuesSlice(unsigned int slice, const std::vector<Imath::Color4f> &values){
	if(slice < _col4ValuesSliced.size()){
		_col4ValuesSliced[slice] = values;
	}
}

void Numeric::setMatrix44ValuesSlice(unsigned int slice, const std::vector<Imath::M44f> &values){
	if(slice < _matrix44ValuesSliced.size()){
		_matrix44ValuesSliced[slice] = values;
	}
}

const std::vector<int> &Numeric::intValuesSlice(unsigned int slice){
	if(slice >= _intValuesSliced.size()){
		slice = _intValuesSliced.size() - 1;
	}

	return _intValuesSliced[slice];
}

const std::vector<float> &Numeric::floatValuesSlice(unsigned int slice){
	if(slice >= _floatValuesSliced.size()){
		slice = _floatValuesSliced.size() - 1;
	}

	return _floatValuesSliced[slice];
}

const std::vector<Imath::V3f> &Numeric::vec3ValuesSlice(unsigned int slice){
	if(slice >= _vec3ValuesSliced.size()){
		slice = _vec3ValuesSliced.size() - 1;
	}

	return _vec3ValuesSliced[slice];
}

const std::vector<Imath::Color4f> &Numeric::col4ValuesSlice(unsigned int slice){
	if(slice >= _col4ValuesSliced.size()){
		slice = _col4ValuesSliced.size() - 1;
	}

	return _col4ValuesSliced[slice];
}

const std::vector<Imath::Quatf> &Numeric::quatValuesSlice(unsigned int slice){
	if(slice >= _quatValuesSliced.size()){
		slice = _quatValuesSliced.size() - 1;
	}

	return _quatValuesSliced[slice];
}

const std::vector<Imath::M44f> &Numeric::matrix44ValuesSlice(unsigned int slice){
	if(slice >= _matrix44ValuesSliced.size()){
		slice = _matrix44ValuesSliced.size() - 1;
	}

	return _matrix44ValuesSliced[slice];
}

void Numeric::resizeSlices(unsigned int slices){
	if(slices == 0){
		slices = 1;
	}

	if(slices != _slices && _type != numericTypeAny){
		if(_type == numericTypeInt){
			_intValuesSliced.resize(slices);
			for(int i = 0; i < slices; ++i){
				std::vector<int> &slicevec = _intValuesSliced[i];
				if(!slicevec.size()){
					slicevec.resize(1);
				}
			}
		}
		else if(_type == numericTypeIntArray){
			_intValuesSliced.resize(slices);
		}
		else if(_type == numericTypeFloat){
			_floatValuesSliced.resize(slices);
			for(int i = 0; i < slices; ++i){
				std::vector<float> &slicevec = _floatValuesSliced[i];
				if(!slicevec.size()){
					slicevec.resize(1);
				}
			}
		}
		else if(_type == numericTypeFloatArray){
			_floatValuesSliced.resize(slices);
		}
		else if(_type == numericTypeVec3){
			_vec3ValuesSliced.resize(slices);
			for(int i = 0; i < slices; ++i){
				std::vector<Imath::V3f> &slicevec = _vec3ValuesSliced[i];
				if(!slicevec.size()){
					slicevec.resize(1);
				}
			}
		}
		else if(_type == numericTypeVec3Array){
			_vec3ValuesSliced.resize(slices);
		}
		else if(_type == numericTypeQuat){
			_quatValuesSliced.resize(slices);
			for(int i = 0; i < slices; ++i){
				std::vector<Imath::Quatf> &slicevec = _quatValuesSliced[i];
				if(!slicevec.size()){
					slicevec.resize(1);
				}
			}
		}
		else if(_type == numericTypeQuatArray){
			_quatValuesSliced.resize(slices);
		}
		else if(_type == numericTypeMatrix44){
			_matrix44ValuesSliced.resize(slices);
			for(int i = 0; i < slices; ++i){
				std::vector<Imath::M44f> &slicevec = _matrix44ValuesSliced[i];
				if(!slicevec.size()){
					slicevec.resize(1);
				}
			}
		}
		else if(_type == numericTypeMatrix44Array){
			_matrix44ValuesSliced.resize(slices);
		}
		else if(_type == numericTypeCol4){
			_col4ValuesSliced.resize(slices);
			for(int i = 0; i < slices; ++i){
				std::vector<Imath::Color4f> &slicevec = _col4ValuesSliced[i];
				if(!slicevec.size()){
					slicevec.resize(1);
				}
			}
		}
		else if(_type == numericTypeCol4Array){
			_col4ValuesSliced.resize(slices);
		}

		_slices = slices;
	}
}
