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


#ifndef CORAL_BOOLATTRIBUTEWRAPPER_H
#define CORAL_BOOLATTRIBUTEWRAPPER_H

#include <boost/python.hpp>
#include "../src/BoolAttribute.h"
#include "../builtinNodes/BoolNode.h"
#include "../src/pythonWrapperUtils.h"

using namespace coral;

void boolWrapper(){
	boost::python::class_<Bool, boost::shared_ptr<Bool>, boost::python::bases<Value>, boost::noncopyable>("Bool", boost::python::no_init)
		.def("__init__", pythonWrapperUtils::__init__<Bool>)
		.def("setBoolValueAt", &Bool::setBoolValueAt)
		.def("boolValueAt", &Bool::boolValueAt)
		.def("size", &Bool::size)
		.def("isArray", &Bool::isArray)
		.def("resize", &Bool::resize)
		.def("createUnwrapped", pythonWrapperUtils::createUnwrapped<Bool>)
		.staticmethod("createUnwrapped")
	;
	
	pythonWrapperUtils::pythonWrapper<BoolAttribute, Attribute>("BoolAttribute");
	pythonWrapperUtils::pythonWrapper<BoolNode, Node>("BoolNode");
}

#endif
