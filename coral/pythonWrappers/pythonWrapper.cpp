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

#include <boost/python.hpp>

#include <map>
#include <vector>

#include "../src/Node.h"
#include "../src/NetworkManager.h"
#include "../src/pythonWrapperUtils.h"
#include "../src/PythonDataCollector.h"
#include "../tests/coralTests.h"

#include "networkManagerWrapper.h"
#include "nodeWrapper.h"
#include "objectWrapper.h"
#include "nestedObjectWrapper.h"
#include "attributeWrapper.h"
#include "numericNodesWrapper.h"
#include "passThroughAttributeWrapper.h"
#include "valueWrapper.h"
#include "imageNodeWrapper.h"
#include "geoWrapper.h"
#include "commandWrapper.h"
#include "objImporterNodeWrapper.h"
#include "stringWrapper.h"
#include "geoGridNodeWrapper.h"
#include "geoSphereNodeWrapper.h"
#include "geoCubeNodeWrapper.h"
#include "errorObjectWrapper.h"
#include "boolWrapper.h"
#include "conditionalNodesWrapper.h"
#include "mathNodesWrapper.h"
#include "splineNodesWrapper.h"
#include "coralIOWrapper.h"
#include "loopNodesWrapper.h"
#include "enumWrapper.h"
#include "processSimulationNodeWrapper.h"
#include "deformerNodesWrapper.h"
#include "../builtinNodes/KdNodes.h"

using namespace coral;

template <class T>
struct ObjectVectorToPythonList
{
	static PyObject* convert(std::vector<T*> const& vectorToConvert)
	{
		boost::python::list convertedList;
		for(unsigned int i = 0; i < vectorToConvert.size(); ++i)
		{
			boost::python::object object = PythonDataCollector::findPyObject(vectorToConvert[i]->id());
			
			if(object){
				convertedList.append(object);
			}
		}

		return boost::python::incref(convertedList.ptr());
	}
};

void coral_setCallback(const std::string &message, boost::python::object callback){
	PythonDataCollector::storeCallback(message, callback);
}

BOOST_PYTHON_MODULE(_coral)
{
	boost::python::def("setCallback", coral_setCallback);
	boost::python::def("runTests", coralTests::run);
	
	objectWrapper();
	valueWrapper();
	nestedObjectWrapper();
	attributeWrapper();
	nodeWrapper();
	networkManagerWrapper();
	loopNodesWrapper();
	numericNodesWrapper();
	mathNodesWrapper();
	passThroughAttributeWrapper();
	imageNodeWrapper();
	geoWrapper();
	commandWrapper();
	objImporterNodeWrapper();
	stringWrapper();
	geoGridNodeWrapper();
	geoSphereNodeWrapper();
	geoCubeNodeWrapper();
	errorObjectWrapper();
	boolWrapper();
	conditionalNodesWrapper();
	splineNodesWrapper();
	coralIOWrapper();
	enumWrapper();
	processSimulationNodeWrapper();
	deformerNodesWrapper();
	pythonWrapperUtils::pythonWrapper<FindPointsInRange, Node>("FindPointsInRange");
	
	boost::python::to_python_converter<std::vector<std::string>, pythonWrapperUtils::stdVectorToPythonList<std::string> >();
	boost::python::to_python_converter<std::vector<Node*>, ObjectVectorToPythonList<Node> >();
	boost::python::to_python_converter<std::vector<Attribute*>, ObjectVectorToPythonList<Attribute> >();
	boost::python::to_python_converter<std::vector<NestedObject*>, ObjectVectorToPythonList<NestedObject> >();
}
