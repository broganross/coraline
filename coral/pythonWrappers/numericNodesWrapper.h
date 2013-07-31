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


#ifndef TESTNODESWRAPPER_H
#define TESTNODESWRAPPER_H

#include "../src/Node.h"
#include "../builtinNodes/NumericNodes.h"
#include "../builtinNodes/LoopNodes.h"
#include "../src/NumericAttribute.h"
#include "../src/Numeric.h"
#include "../builtinNodes/ArithmeticNodes.h"
#include "../builtinNodes/MathNodes.h"
#include "../src/pythonWrapperUtils.h"

using namespace coral;

int numeric_numericTypeAny(){
	return int(Numeric::numericTypeAny);
}

int numeric_numericTypeInt(){
	return int(Numeric::numericTypeInt);
}

int numeric_numericTypeIntArray(){
	return int(Numeric::numericTypeIntArray);
}

int numeric_numericTypeFloat(){
	return int(Numeric::numericTypeFloat);
}

int numeric_numericTypeFloatArray(){
	return int(Numeric::numericTypeFloatArray);
}

int numeric_numericTypeVec3(){
	return int(Numeric::numericTypeVec3);
}

int numeric_numericTypeVec3Array(){
	return int(Numeric::numericTypeVec3Array);
}

int numeric_numericTypeCol4(){
	return int(Numeric::numericTypeCol4);
}

int numeric_numericTypeCol4Array(){
	return int(Numeric::numericTypeCol4Array);
}

int numeric_numericTypeQuat(){
	return int(Numeric::numericTypeQuat);
}

int numeric_numericTypeQuatArray(){
	return int(Numeric::numericTypeQuatArray);
}

int numeric_numericTypeMatrix44(){
	return int(Numeric::numericTypeMatrix44);
}

int numeric_numericTypeMatrix44Array(){
	return int(Numeric::numericTypeMatrix44Array);
}

int numeric_type(Numeric &self){
	return int(self.type());
}

std::vector<Imath::V3f> numeric_vec3Values(Numeric &self){
	return self.vec3Values();
}

std::vector<Imath::Color4f> numeric_col4Values(Numeric &self){
	return self.col4Values();
}

std::vector<Imath::Quatf> numeric_quatValues(Numeric &self){
	return self.quatValues();
}

std::vector<Imath::M44f> numeric_matrix44Values(Numeric &self){
	return self.matrix44Values();
}

std::vector<float> numeric_floatValues(Numeric &self){
	return self.floatValues();
}

void numeric_setFloatValues(Numeric &self, boost::python::list pyList){
	std::vector<float> convertedList;
	for(int i = 0; i < boost::python::len(pyList); ++i){
		float val = boost::python::extract<float>(pyList[i]);
		convertedList.push_back(val);
	}

	self.setFloatValues(convertedList);
}

std::vector<int> numeric_intValues(Numeric &self){
	return self.intValues();
}

void numericNodesWrapper(){

	boost::python::to_python_converter<std::vector<int>, pythonWrapperUtils::stdVectorToPythonList<int> >();
	boost::python::to_python_converter<std::vector<float>, pythonWrapperUtils::stdVectorToPythonList<float> >();

	boost::python::class_<Numeric, boost::shared_ptr<Numeric>, boost::python::bases<Value>, boost::noncopyable>("Numeric", boost::python::no_init)
		.def("__init__", pythonWrapperUtils::__init__<Numeric>)
		.def("copy", &Numeric::copy)
		.def("type", numeric_type)
		.def("isArray", &Numeric::isArray)
		.def("size", &Numeric::size)
		.def("resize", &Numeric::resize)
		.def("setIntValueAt", &Numeric::setIntValueAt)
		.def("setFloatValueAt", &Numeric::setFloatValueAt)
		.def("setVec3ValueAt", &Numeric::setVec3ValueAt)
		.def("setCol4ValueAt", &Numeric::setCol4ValueAt)
		.def("setQuatValueAt", &Numeric::setQuatValueAt)
		.def("setMatrix44ValueAt", &Numeric::setMatrix44ValueAt)
		.def("intValues", numeric_intValues)
		.def("floatValues", numeric_floatValues)
		.def("vec3Values", numeric_vec3Values)
		.def("col4Values", numeric_col4Values)
		.def("quatValues", numeric_quatValues)
		.def("matrix44Values", numeric_matrix44Values)
		.def("intValueAt", &Numeric::intValueAt)
		.def("floatValueAt", &Numeric::floatValueAt)
		.def("vec3ValueAt", &Numeric::vec3ValueAt)
		.def("col4ValueAt", &Numeric::col4ValueAt)
		.def("quatValueAt", &Numeric::quatValueAt)
		.def("matrix44ValueAt", &Numeric::matrix44ValueAt)
		.def("setIntValues", &Numeric::setIntValues)
		.def("setFloatValues", numeric_setFloatValues)
		.def("setVec3Values", &Numeric::setVec3Values)
		.def("setCol4Values", &Numeric::setCol4Values)
		.def("setMatrix44Values", &Numeric::setMatrix44Values)
		.add_static_property("numericTypeAny", numeric_numericTypeAny)
		.add_static_property("numericTypeInt", numeric_numericTypeInt)
		.add_static_property("numericTypeIntArray", numeric_numericTypeIntArray)
		.add_static_property("numericTypeFloat", numeric_numericTypeFloat)
		.add_static_property("numericTypeFloatArray", numeric_numericTypeFloatArray)
		.add_static_property("numericTypeVec3", numeric_numericTypeVec3)
		.add_static_property("numericTypeVec3Array", numeric_numericTypeVec3Array)
		.add_static_property("numericTypeCol4", numeric_numericTypeCol4)
		.add_static_property("numericTypeCol4Array", numeric_numericTypeCol4Array)
		.add_static_property("numericTypeQuat", numeric_numericTypeQuat)
		.add_static_property("numericTypeQuatArray", numeric_numericTypeQuatArray)
		.add_static_property("numericTypeMatrix44", numeric_numericTypeMatrix44)
		.add_static_property("numericTypeMatrix44Array", numeric_numericTypeMatrix44Array)
		.def("createUnwrapped", pythonWrapperUtils::createUnwrapped<Numeric>)
		.staticmethod("createUnwrapped")
	;
	
	pythonWrapperUtils::pythonWrapper<NumericAttribute, Attribute>("NumericAttribute");
	pythonWrapperUtils::pythonWrapper<IntNode, Node>("IntNode");
	pythonWrapperUtils::pythonWrapper<FloatNode, Node>("FloatNode");
	pythonWrapperUtils::pythonWrapper<ArithmeticNode, Node>("ArithmeticNode");
	pythonWrapperUtils::pythonWrapper<AddNode, ArithmeticNode>("AddNode");
	pythonWrapperUtils::pythonWrapper<SubNode, ArithmeticNode>("SubNode");
	pythonWrapperUtils::pythonWrapper<MulNode, ArithmeticNode>("MulNode");
	pythonWrapperUtils::pythonWrapper<DivNode, ArithmeticNode>("DivNode");
	pythonWrapperUtils::pythonWrapper<Vec3Node, Node>("Vec3Node");
	pythonWrapperUtils::pythonWrapper<Vec3ToFloats, Node>("Vec3ToFloats");
	pythonWrapperUtils::pythonWrapper<Col4Node, Node>("Col4Node");
	pythonWrapperUtils::pythonWrapper<Col4ToFloats, Node>("Col4ToFloats");
	pythonWrapperUtils::pythonWrapper<Col4Reverse, Node>("Col4Reverse");
	pythonWrapperUtils::pythonWrapper<QuatNode, Node>("QuatNode");
	pythonWrapperUtils::pythonWrapper<QuatToFloats, Node>("QuatToFloats");
	pythonWrapperUtils::pythonWrapper<QuatToAxisAngle, Node>("QuatToAxisAngle");
	pythonWrapperUtils::pythonWrapper<QuatToEulerRotation, Node>("QuatToEulerRotation");
	pythonWrapperUtils::pythonWrapper<QuatToMatrix44, Node>("QuatToMatrix44");
	pythonWrapperUtils::pythonWrapper<Matrix44Node, Node>("Matrix44Node");
	pythonWrapperUtils::pythonWrapper<ConstantArray, Node>("ConstantArray");
	pythonWrapperUtils::pythonWrapper<ArraySize, Node>("ArraySize");
	pythonWrapperUtils::pythonWrapper<BuildArray, Node>("BuildArray")
		.def("addNumericAttribute", &BuildArray::addNumericAttribute);
	pythonWrapperUtils::pythonWrapper<RangeArray, Node>("RangeArray");
	pythonWrapperUtils::pythonWrapper<Matrix44Translation, Node>("Matrix44Translation");
	pythonWrapperUtils::pythonWrapper<Matrix44RotationAxis, Node>("Matrix44RotationAxis");
	pythonWrapperUtils::pythonWrapper<Matrix44FromVectors, Node>("Matrix44FromVectors");
	pythonWrapperUtils::pythonWrapper<Matrix44EulerRotation, Node>("Matrix44EulerRotation");
	pythonWrapperUtils::pythonWrapper<Matrix44ToQuat, Node>("Matrix44ToQuat");
	pythonWrapperUtils::pythonWrapper<RangeLoop, Node>("RangeLoop");
	pythonWrapperUtils::pythonWrapper<RandomNumber, Node>("RandomNumber");
	pythonWrapperUtils::pythonWrapper<ArrayIndices, Node>("ArrayIndices");
	pythonWrapperUtils::pythonWrapper<GetArrayElement, Node>("GetArrayElement");
	pythonWrapperUtils::pythonWrapper<SetArrayElement, Node>("SetArrayElement");
	pythonWrapperUtils::pythonWrapper<GetSimulationStep, Node>("GetSimulationStep");
	pythonWrapperUtils::pythonWrapper<SetSimulationStep, Node>("SetSimulationStep");
}

#endif
