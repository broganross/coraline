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


#ifndef NESTEDOBJECTWRAPPER_H
#define NESTEDOBJECTWRAPPER_H

#include <boost/python.hpp>

#include "../src/NestedObject.h"
#include "../src/PythonDataCollector.h"
#include "../src/pythonWrapperUtils.h"

// TODO: change name of this file and other outdated wrappers

void nestedObject_setNameCallback(NestedObject *self, const std::string &name){
	if(PythonDataCollector::hasCallback("nestedObject_setName")){
		if(PythonDataCollector::hasPyObject(self->id()))
			PythonDataCollector::findCallback("nestedObject_setName")(PythonDataCollector::findPyObject(self->id()), name);
	}
}

boost::python::object nestedObject_findObject(NestedObject &self, const std::string &name){
	NestedObject *object = self.findObject(name);
	boost::python::object pyObject;
	
	if(object){
		pyObject = PythonDataCollector::findPyObject(object->id());
	}
	
	return pyObject;
}

void nestedObjectWrapper(){
	boost::python::class_<NestedObject, boost::shared_ptr<NestedObject>, boost::python::bases<Object>, boost::noncopyable>("NestedObject", boost::python::init<const std::string &, NestedObject *>())
		.def("__init__", pythonWrapperUtils::__init__<NestedObject, const std::string, NestedObject*>)
		.def("name", &NestedObject::name)
		.def("setName", &NestedObject::setName)
		.def("setClassName", &NestedObject::setClassName)
		.def("className", &NestedObject::className)
		.def("classNames", &NestedObject::classNames)
		.def("hasClassName", &NestedObject::hasClassName)
		.def("findObject", nestedObject_findObject)
		.def("fullName", &NestedObject::fullName)
		.def("objects", &NestedObject::objects)
		.def("createUnwrapped", pythonWrapperUtils::createUnwrapped2<NestedObject, const std::string, Node*>)
		.staticmethod("createUnwrapped")
		;
	
	NestedObject::_setNameCallback = nestedObject_setNameCallback;
}

#endif
