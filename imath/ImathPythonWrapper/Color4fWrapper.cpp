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
#include <vector>

#include <ImathColor.h>
#include <pythonWrapperUtils.h>
#include "Color4fWrapper.h"

namespace{
	boost::shared_ptr<Imath::Color4f> col4_defaultInit()
	{
		return boost::shared_ptr<Imath::Color4f>(new Imath::Color4f(1.f, 1.f, 1.f, 1.f));
	}
}

void color4fWrapper()
{
	boost::python::to_python_converter<std::vector<Imath::Color4f >, coral::pythonWrapperUtils::stdVectorToPythonList<Imath::Color4f > >();

	boost::python::class_<Imath::Color4f>("Color4f")
		.def("__init__", boost::python::make_constructor(col4_defaultInit))
		.def(boost::python::init<float, float, float, float>())
		.def(boost::python::init<Imath::Color4f>())
		.def_readwrite("r", &Imath::Color4f::r)
		.def_readwrite("g", &Imath::Color4f::g)
		.def_readwrite("b", &Imath::Color4f::b)
		.def_readwrite("a", &Imath::Color4f::a)
	;
}
