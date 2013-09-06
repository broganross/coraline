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

#include "../src/PythonDataCollector.h"

using namespace coral;

std::map<int, boost::python::object> pyObjects;
std::map<int, PyObject*> borrowedPyObjects;
std::map<std::string, boost::python::object> callbacks;

void PythonDataCollector::storePyObject(int id, boost::python::object object){
	pyObjects[id] = object;
}

void PythonDataCollector::removePyObject(int id){
	if(pyObjects.find(id) != pyObjects.end()){
		pyObjects.erase(id);
	}
}

void PythonDataCollector::storeBorrowedPyObject(int id, PyObject *object){
	borrowedPyObjects[id] = object;
}

bool PythonDataCollector::hasPyObject(int id){
	bool found = false;
	if(pyObjects.find(id) != pyObjects.end() || borrowedPyObjects.find(id) != borrowedPyObjects.end()){
		found = true;
	}
	
	return found;
}

boost::python::object PythonDataCollector::findPyObject(int id){
	boost::python::object pyobj;
	if(pyObjects.find(id) != pyObjects.end()){
		pyobj = pyObjects[id];
	}
	else if(borrowedPyObjects.find(id) != borrowedPyObjects.end()){
		pyobj = boost::python::object(boost::python::handle<>(boost::python::borrowed(borrowedPyObjects[id])));
	}
	
	return pyobj;
}

PyObject * PythonDataCollector::findPyObjectPtr(int id){
	PyObject *pyobj = 0;
	
	if(pyObjects.find(id) != pyObjects.end()){
		pyobj = pyObjects[id].ptr();
	}
	else if(borrowedPyObjects.find(id) != borrowedPyObjects.end()){
		pyobj = borrowedPyObjects[id];
	}
	
	return pyobj;
}

bool PythonDataCollector::isBorrowedObject(int id){
	bool isBorrowed = false;
	
	if(borrowedPyObjects.find(id) != borrowedPyObjects.end()){
		isBorrowed = true;
	}
	
	return isBorrowed;
}

void PythonDataCollector::storeCallback(const std::string &message, boost::python::object callback){
	callbacks[message] = callback;
}

bool PythonDataCollector::hasCallback(const std::string &message){
	bool found = false;
	if(callbacks.find(message) != callbacks.end()){
		if(callbacks[message]){
			found = true;
		}
	}
	
	return found;
}

boost::python::object PythonDataCollector::findCallback(const std::string &message){
	return callbacks[message];
}
