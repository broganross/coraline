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


#ifndef CORAL_ENUMWRAPPER_H
#define CORAL_ENUMWRAPPER_H

#include <boost/python.hpp>
#include "../src/EnumAttribute.h"
#include "../src/pythonWrapperUtils.h"

using namespace coral;

void enumWrapper(){
	boost::python::class_<Enum, boost::shared_ptr<Enum>, boost::python::bases<Value>, boost::noncopyable>("Enum", boost::python::no_init)
		.def("__init__", pythonWrapperUtils::__init__<Enum>)
		.def("createUnwrapped", pythonWrapperUtils::createUnwrapped<Enum>)
		.staticmethod("createUnwrapped")
		.def("addEntry", &Enum::addEntry)
		.def("indices", &Enum::indices)
		.def("entries", &Enum::entries)
		.def("setCurrentIndex", &Enum::setCurrentIndex)
		.def("currentIndex", &Enum::currentIndex)
	;
	
	pythonWrapperUtils::pythonWrapper<EnumAttribute, Attribute>("EnumAttribute");
}

#endif
