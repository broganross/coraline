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


#ifndef CORALOBJECTWRAPPER_H
#define CORALOBJECTWRAPPER_H

#include <string>
#include <boost/python.hpp>

#include "../src/Object.h"
#include "../src/PythonDataCollector.h"
#include "../src/pythonWrapperUtils.h"

void object_addReferenceCallback(Object *object){
	boost::python::object pyObject = PythonDataCollector::findPyObject(object->id());
	
	if(!pyObject){
		// if no object was found it means the pointer was created from c++ and not from python
		boost::shared_ptr<Object> newObject(object);
		
		pyObject = boost::python::object(newObject);
		PythonDataCollector::storePyObject(object->id(), boost::python::object(boost::python::handle<>(pyObject.ptr())));
	}
	
	// now we own the object and python can't delete it
	boost::python::incref(pyObject.ptr());
}

void object_removeReferenceCallback(Object *object){
	PyObject *pyObj = PythonDataCollector::findPyObjectPtr(object->id());
	
	if(pyObj){
		if(pyObj->ob_refcnt == 1 && PythonDataCollector::isBorrowedObject(object->id()) == false){
			// if this object was created from c++ (not borrowed from python) and we own the last copy, then remove it from the collector
			PythonDataCollector::removePyObject(object->id());
		}
		else{
			boost::python::decref(pyObj);
		}
	}
}

void objectWrapper(){
	boost::python::class_<Object, boost::shared_ptr<Object>, boost::noncopyable>("Object")
		.def("__init__", pythonWrapperUtils::__init__<Object>)
		.def("id", &Object::id)
		;
	
	Object::_addReferenceCallback = object_addReferenceCallback;
	Object::_removeReferenceCallback = object_removeReferenceCallback;
}

#endif
