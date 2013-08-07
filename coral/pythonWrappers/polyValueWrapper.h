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


#ifndef POLYVALUEWRAPPER_H
#define POLYVALUEWRAPPER_H

#include <boost/python.hpp>

#include "../src/PolyValue.h"
#include "../src/pythonWrapperUtils.h"

using namespace coral;

class PolyValueWrapper: public PolyValue, public boost::python::wrapper<PolyValue>{
public:
	PolyValueWrapper(): PolyValue(){
	}
};

int value_typeAny(){
	return int(PolyValue::typeAny);
}

int value_numericTypeAny(){
	return int(PolyValue::numericTypeAny);
}

int value_numericTypeInt(){
	return int(PolyValue::numericTypeInt);
}

int value_numericTypeIntArray(){
	return int(PolyValue::numericTypeIntArray);
}

int value_numericTypeFloat(){
	return int(PolyValue::numericTypeFloat);
}

int value_numericTypeFloatArray(){
	return int(PolyValue::numericTypeFloatArray);
}

int value_numericTypeVec3(){
	return int(PolyValue::numericTypeVec3);
}

int value_numericTypeVec3Array(){
	return int(PolyValue::numericTypeVec3Array);
}

int value_numericTypeCol4(){
	return int(PolyValue::numericTypeCol4);
}

int value_numericTypeCol4Array(){
	return int(PolyValue::numericTypeCol4Array);
}

int value_numericTypeQuat(){
	return int(PolyValue::numericTypeQuat);
}

int value_numericTypeQuatArray(){
	return int(PolyValue::numericTypeQuatArray);
}

int value_numericTypeMatrix44(){
	return int(PolyValue::numericTypeMatrix44);
}

int value_numericTypeMatrix44Array(){
	return int(PolyValue::numericTypeMatrix44Array);
}

int value_stringTypeAny(){
	return int(PolyValue::stringTypeAny);
}

int value_stringType(){
	return int(PolyValue::stringType);
}

int value_stringTypeArray(){
	return int(PolyValue::stringTypeArray);
}

int value_pathType(){
	return int(PolyValue::pathType);
}

int value_pathTypeArray(){
	return int(PolyValue::pathTypeArray);
}

int value_boolTypeAny(){
	return int(PolyValue::boolTypeAny);
}

int value_boolType(){
	return int(PolyValue::boolType);
}

int value_boolTypeArray(){
	return int(PolyValue::boolTypeArray);
}

int value_type(PolyValue &self){
	return int(self.type());

}

std::vector<std::string> value_stringValues(PolyValue &self){
	return self.stringValues();
}

std::vector<std::string> value_pathValues(PolyValue &self){
	return self.pathValues();
}

std::vector<bool> value_boolValues(PolyValue &self){
	return self.boolValues();
}

std::vector<int> value_intValues(PolyValue &self){
	return self.intValues();
}

std::vector<float> value_floatValues(PolyValue &self){
	return self.floatValues();
}

std::vector<Imath::V3f> value_vec3Values(PolyValue &self){
	return self.vec3Values();
}

std::vector<Imath::Color4f> value_col4Values(PolyValue &self){
	return self.col4Values();
}

std::vector<Imath::M44f> value_matrix44Values(PolyValue &self){
	return self.matrix44Values();
}

std::vector<Imath::Quatf> value_quatValues(PolyValue &self){
	return self.quatValues();
}

void polyValueWrapper(){
/*	boost::python::enum_<Value::ValueType>("ValueType")
		.value("typeAny", Value::typeAny)
		.value("numericTypeAny", Value::numericTypeAny)
		.value("numericTypeInt", Value::numericTypeInt)
		.value("numericTypeIntArray", Value::numericTypeIntArray)
		.value("numericTypeFloat", Value::numericTypeFloat)
		.value("numericTypeFloatArray", Value::numericTypeFloatArray)
		.value("numericTypeVec3", Value::numericTypeVec3)
		.value("numericTypeVec3Array", Value::numericTypeVec3Array)
		.value("numericTypeCol4", Value::numericTypeCol4)
		.value("numericTypeCol4Array", Value::numericTypeCol4Array)
		.value("numericTypeQuat", Value::numericTypeQuat)
		.value("numericTypeQuatArray", Value::numericTypeQuatArray)
		.value("numericTypeMatrix44", Value::numericTypeMatrix44)
		.value("numericTypeMatrix44Array", Value::numericTypeMatrix44Array)
		.value("stringTypeAny", Value::stringTypeAny)
		.value("stringType", Value::stringType)
		.value("stringTypeArray", Value::stringTypeArray)
		.value("pathType", Value::pathType)
		.value("pathTypeArray", Value::pathTypeArray)
		.value("boolTypeAny", Value::boolTypeAny)
		.value("boolType", Value::boolType)
		.value("boolTypeArray", Value::boolTypeArray)
//		.value("geoTypeAny", Value::geoTypeAny)
		.value("geoType", Value::geoType)
//		.value("geoTypeArray", Value::geoTypeArray)
//		.value("imageTypeAny", Value::imageTypeAny)
		.value("imageType", Value::imageType)
//		.value("imageTypeArray", Value::imageTypeArray)
	;
*/
	boost::python::register_ptr_to_python<boost::shared_ptr<PolyValue> >();

	boost::python::class_<PolyValueWrapper, boost::shared_ptr<PolyValueWrapper>, boost::python::bases<Object>, boost::noncopyable>("PolyValue", boost::python::no_init)
		.def("__init__", pythonWrapperUtils::__init__<PolyValueWrapper>)
		.def("createUnwrapped", pythonWrapperUtils::createUnwrapped<PolyValue>)
		.def("type", value_type)
		.def("setType", &PolyValue::setType)
		.def("isArray", &PolyValue::isArray)
		.def("asString", &PolyValue::asString)
		.def("setFromString", &PolyValue::setFromString)
		.def("size", &PolyValue::size)
		.def("resize", &PolyValue::resize)

		.def("stringValues", value_stringValues)
		.def("pathValues", value_pathValues)
		.def("boolValues", value_boolValues)
		.def("intVaues", value_intValues)
		.def("floatValues", value_floatValues)
		.def("vec3Values", value_vec3Values)
		.def("matrix44Values", value_matrix44Values)
		.def("quatValues", value_quatValues)
		.def("col4Values", value_col4Values)

		.def("stringValueAt", &PolyValue::stringValueAt)
		.def("pathValueAt", &PolyValue::pathValueAt)
		.def("boolValueAt", &PolyValue::boolValueAt)
		.def("intValueAt", &PolyValue::intValueAt)
		.def("floatValueAt", &PolyValue::floatValueAt)
		.def("vec3ValueAt", &PolyValue::vec3ValueAt)
		.def("col4ValueAt", &PolyValue::col4ValueAt)
		.def("matrix44ValueAt", &PolyValue::matrix44ValueAt)
		.def("quatValueAt", &PolyValue::quatValueAt)

		.def("setStringValueAt", &PolyValue::setStringValueAt)
		.def("setPathValueAt", &PolyValue::setPathValueAt)
		.def("setBoolValueAt", &PolyValue::setBoolValueAt)
		.def("setIntValueAt", &PolyValue::setIntValueAt)
		.def("setFloatValueAt", &PolyValue::setFloatValueAt)
		.def("setCol4ValueAt", &PolyValue::setCol4ValueAt)
		.def("setVec3ValueAt", &PolyValue::setVec3ValueAt)
		.def("setMatrix44ValueAt", &PolyValue::setMatrix44ValueAt)
		.def("setQuatValueAt", &PolyValue::setQuatValueAt)

		.def("setStringValues", &PolyValue::setStringValues)
		.def("setPathValues", &PolyValue::setPathValues)
		.def("setBoolValues", &PolyValue::setBoolValues)
		.def("setIntValues", &PolyValue::setIntValues)
		.def("setFloatValues", &PolyValue::setFloatValues)
		.def("setVec3Values", &PolyValue::setVec3Values)
		.def("setCol4Values", &PolyValue::setCol4Values)
		.def("setMatrix44Values", &PolyValue::setMatrix44Values)
		.def("setQuatValues", &PolyValue::setQuatValues)

		.staticmethod("createUnwrapped")

		.add_static_property("typeAny", value_typeAny)
		.add_static_property("stringTypeAny", value_stringTypeAny)
//		.add_static_property("stringType", value_stringType)
//		.add_static_property("stringTypeArray", value_stringTypeArray)
//		.add_static_property("pathType", value_pathType)
//		.add_static_property("pathTypeArray", value_pathTypeArray)
//		.add_static_property("boolTypeAny", value_boolTypeAny)
//		.add_static_property("boolType", value_boolType)
//		.add_static_property("boolTypeArray", value_boolTypeArray)
//		.add_static_property("numericTypeAny", value_numericTypeAny)
//		.add_static_property("numericTypeInt", value_numericTypeInt)
//		.add_static_property("numericTypeIntArray", value_numericTypeIntArray)
//		.add_static_property("numericTypeFloat", value_numericTypeFloat)
//		.add_static_property("numericTypeFloatArray", value_numericTypeFloatArray)
//		.add_static_property("numericTypeVec3", value_numericTypeVec3)
//		.add_static_property("numericTypeVec3Array", value_numericTypeVec3Array)
//		.add_static_property("numericTypeCol4", value_numericTypeCol4)
//		.add_static_property("numericTypeCol4Array", value_numericTypeCol4Array)
//		.add_static_property("numericTypeQuat", value_numericTypeQuat)
//		.add_static_property("numericTypeQuatArray", value_numericTypeQuatArray)
//		.add_static_property("numericTypeMatrix44", value_numericTypeMatrix44)
//		.add_static_property("numericTypeMatrix44Array", value_numericTypeMatrix44Array)
	;

	pythonWrapperUtils::pythonWrapper<PolyAttribute, Attribute>("PolyAttribute");
}

#endif
