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


#ifndef ATTRIBUTEWRAPPER_H
#define ATTRIBUTEWRAPPER_H

#include <boost/python.hpp>

#include "../src/Value.h"
#include "../src/Attribute.h"
#include "../src/AttributeAccessor.h"
#include "../src/ObjectAccessor.h"
#include "../src/pythonWrapperUtils.h"

class AttributeWrapper: public Attribute, public boost::python::wrapper<Attribute>{
public:
	AttributeWrapper(const std::string &name, Node *parent): Attribute(name, parent){
	}
};

boost::python::object attribute_input(Attribute &self){
	boost::python::object input;
	
	if(self.input()){
		input = PythonDataCollector::findPyObject(self.input()->id());
	}
	
	return input;
}

boost::python::object attribute_connectedNonPassThrough(Attribute &self){
	boost::python::object attrObj;

	Attribute *attr = self.connectedNonPassThrough();
	if(attr){
		attrObj = PythonDataCollector::findPyObject(attr->id());
	}

	return attrObj;
}

boost::python::object attribute_parent(Attribute &self){
	boost::python::object parent;
	
	if(self.parent()){
		parent = PythonDataCollector::findPyObject(self.parent()->id());
	}
	
	return parent;
}

std::vector<Attribute*> attribute_outputs(Attribute &self){
	return self.outputs();
}

std::vector<Attribute*> attribute_affectedBy(Attribute &self){
	return self.affectedBy();
}

boost::python::object attribute_value(Attribute &self){
	PyThreadState *state = 0;
	if(!pythonWrapperUtils::pyGILEnsured){
		state = PyEval_SaveThread();
	}

	boost::python::object valueObj;
	
	Value *value = self.value();
	if(value){
		valueObj = PythonDataCollector::findPyObject(value->id());
	}

	if(state){
		PyEval_RestoreThread(state);
	}

	return valueObj;
}

boost::python::object attributeoutValue(Attribute &self){
	boost::python::object valueObj;
	
	Value *value = AttributeAccessor::outValue(self);
	if(value){
		valueObj = PythonDataCollector::findPyObject(value->id());
	}
	
	return valueObj;
}

void attribute_connectToCallback(Attribute *self, Attribute *other){
	if(PythonDataCollector::hasCallback("attribute_connectTo")){
		if(PythonDataCollector::hasPyObject(self->id()) && PythonDataCollector::hasPyObject(other->id()))
			PythonDataCollector::findCallback("attribute_connectTo")(PythonDataCollector::findPyObject(self->id()), PythonDataCollector::findPyObject(other->id()));
	}
}

void attribute_disconnectInputCallback(Attribute *self){
	if(PythonDataCollector::hasCallback("attribute_disconnectInput")){
		if(PythonDataCollector::hasPyObject(self->id()))
			PythonDataCollector::findCallback("attribute_disconnectInput")(PythonDataCollector::findPyObject(self->id()));
	}
}

void attribute_disconnectOutputCallback(Attribute *self, Attribute *other){
	if(PythonDataCollector::hasCallback("attribute_disconnectOutput")){
		if(PythonDataCollector::hasPyObject(self->id()) && PythonDataCollector::hasPyObject(other->id()))
			PythonDataCollector::findCallback("attribute_disconnectOutput")(PythonDataCollector::findPyObject(self->id()), PythonDataCollector::findPyObject(other->id()));
	}
}

void attribute_specializationCallBack(Attribute *self){
	if(PythonDataCollector::hasCallback("attribute_specialization")){
		if(PythonDataCollector::hasPyObject(self->id()))
			PythonDataCollector::findCallback("attribute_specialization")(PythonDataCollector::findPyObject(self->id()));
	}
}

void attribute__del__(Attribute &self){
	if(!self.isDeleted()){
		ObjectAccessor::_setIsDeleted(self, true);
		self.deleteIt();
	}
}

void attribute_deleteItCallback(Attribute *self){
	if(PythonDataCollector::hasCallback("attribute_deleteIt")){
		if(PythonDataCollector::hasPyObject(self->id()))
			PythonDataCollector::findCallback("attribute_deleteIt")(PythonDataCollector::findPyObject(self->id()));
	}
}

void attribute_setValuePtr(Attribute &self, Value *value){
	AttributeAccessor::_setValuePtr(self, value);
}

void atribute_setIsInput(Attribute &self, bool value){
	AttributeAccessor::_setIsInput(self, value);
}

void atribute_setIsOutput(Attribute &self, bool value){
	AttributeAccessor::_setIsOutput(self, value);
}

void attribute_postInit(Attribute &self){
	if(PythonDataCollector::hasCallback("attribute_created")){
		PythonDataCollector::findCallback("attribute_created")(PythonDataCollector::findPyObject(self.id()));
	}
}

void attribute_setAllowedSpecialization(Attribute &self, boost::python::list stringList){
	std::vector<std::string> specialization;
	for(int i = 0; i < boost::python::len(stringList); ++i){
		std::string spec = boost::python::extract<std::string>(stringList[i]);
		if(!spec.empty()){
			specialization.push_back(spec);
		}
	}
	
	if(specialization.size()){
		AttributeAccessor::_setAllowedSpecialization(self, specialization);
	}
}

void attributeWrapper(){
	boost::python::register_ptr_to_python<boost::shared_ptr<Attribute> >();
	
	boost::python::class_<AttributeWrapper, boost::shared_ptr<AttributeWrapper>, boost::python::bases<NestedObject>, boost::noncopyable>("Attribute", boost::python::no_init)
		.def("__init__", pythonWrapperUtils::__init__<AttributeWrapper, const std::string, Node*>)
		.def("_postInit", attribute_postInit)
		.def("__del__", attribute__del__)
		.def("createUnwrapped", pythonWrapperUtils::createUnwrapped2<Attribute, const std::string, Node*>)
		.staticmethod("createUnwrapped")
		.def("input", attribute_input)
		.def("isClean", &Attribute::isClean)
		.def("disconnectOutput", &Attribute::disconnectOutput)
		.def("disconnectInput", &Attribute::disconnectInput)
		.def("isConnectedTo", &Attribute::isConnectedTo)
		.def("parent", attribute_parent)
		.def("outputs", attribute_outputs)
		.def("value", attribute_value)
		.def("deleteIt", &Attribute::deleteIt)
		.def("isOutput", &Attribute::isOutput)
		.def("isInput", &Attribute::isInput)
		.def("valueChanged", &Attribute::valueChanged)
		.def("_setValuePtr", attribute_setValuePtr)
		.def("outValue", attributeoutValue) 
		.def("_setIsInput", atribute_setIsInput)
		.def("_setIsOutput", atribute_setIsOutput)
		.def("specialization", &Attribute::specialization)
		.def("isPassThrough", &Attribute::isPassThrough)
		.def("allowedSpecialization", &Attribute::allowedSpecialization)
		.def("affectedBy", attribute_affectedBy)
		.def("specializationLinkedTo", &Attribute::specializationLinkedTo)
		.def("specializationLinkedBy", &Attribute::specializationLinkedBy)
		.def("isAffectedBy", &Attribute::isAffectedBy)
		.def("_setAllowedSpecialization", attribute_setAllowedSpecialization)
		.def("setSpecializationOverride", &Attribute::setSpecializationOverride)
		.def("removeSpecializationOverride", &Attribute::removeSpecializationOverride)
		.def("forceSpecializationUpdate", &Attribute::forceSpecializationUpdate)
		.def("specializationOverride", &Attribute::specializationOverride)
		.def("forceDirty", &Attribute::forceDirty)
		.def("connectedNonPassThrough", &attribute_connectedNonPassThrough)
		.def("shortDebugInfo", &Attribute::shortDebugInfo)
		;
	
	Attribute::_connectToCallback = attribute_connectToCallback;
	Attribute::_disconnectInputCallback = attribute_disconnectInputCallback;
	Attribute::_disconnectOutputCallback = attribute_disconnectOutputCallback;
	Attribute::_deleteItCallback = attribute_deleteItCallback;
	Attribute::_specializationCallBack = attribute_specializationCallBack;
}

#endif
