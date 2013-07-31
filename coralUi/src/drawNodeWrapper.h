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


#ifndef DRAWNODEWRAPPER_H
#define DRAWNODEWRAPPER_H

#include <boost/python.hpp>
#include "DrawNode.h"
#include <coral/src/pythonWrapperUtils.h>
#include <coral/src/PythonDataCollector.h>

using namespace coralUi;
using namespace coral;

class DrawNodeWrapper: public DrawNode, public boost::python::wrapper<DrawNode>{
public:
	DrawNodeWrapper(const std::string &name, Node *parent) : DrawNode(name, parent){
	}

	void draw(){
		// return boost::python::call<void>(get_override("draw").ptr());
		boost::python::object self = PythonDataCollector::findPyObject(id());

		return boost::python::call_method<void>(self.ptr(), "draw");
	}

	void draw_default(){
		DrawNode::draw();
	}
};

void drawNodeWrapper(){
	boost::python::class_<DrawNodeWrapper, boost::shared_ptr<DrawNodeWrapper>, boost::python::bases<coral::Node>, boost::noncopyable>("DrawNode", boost::python::no_init)
		.def("__init__", pythonWrapperUtils::__init__<DrawNodeWrapper, const std::string, coral::Node*>)
		.def("draw", &DrawNode::draw, &DrawNodeWrapper::draw_default)
		.def("createUnwrapped", pythonWrapperUtils::createUnwrapped2<DrawNodeWrapper, const std::string, coral::Node*>)
		.staticmethod("createUnwrapped")
	;
}

#endif