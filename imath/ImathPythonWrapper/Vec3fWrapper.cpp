// // <license>
// // Copyright (C) 2011 Andrea Interguglielmi, All rights reserved.
// // This file is part of the coral repository downloaded from http://code.google.com/p/coral-repo.
// // 
// // Redistribution and use in source and binary forms, with or without
// // modification, are permitted provided that the following conditions are
// // met:
// // 
// //    * Redistributions of source code must retain the above copyright
// //      notice, this list of conditions and the following disclaimer.
// // 
// //    * Redistributions in binary form must reproduce the above copyright
// //      notice, this list of conditions and the following disclaimer in the
// //      documentation and/or other materials provided with the distribution.
// // 
// // THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
// // IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// // THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// // PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// // CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// // EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// // PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// // PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// // LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// // NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// // SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// // </license>

#include <pythonWrapperUtils.h>
#include "Vec3fWrapper.h"

#include <vector>

namespace{
	boost::shared_ptr<Imath::V3f> vec3_defaultInit()
	{
		return boost::shared_ptr<Imath::V3f>(new Imath::V3f(0.f, 0.f, 0.f));
	}

	boost::python::tuple getValue(Imath::V3f &self)
	{
		return boost::python::make_tuple(self.x, self.y, self.z);
	}

	void setValue(Imath::V3f &self, float a, float b, float c)
	{
		self.setValue(a, b, c);
	}

	Imath::V3f normalize(Imath::V3f &self)
	{
		self.normalize();
		return self;
	}
}

void vec3fWrapper(){
	boost::python::to_python_converter<std::vector<Imath::V3f >, coral::pythonWrapperUtils::stdVectorToPythonList<Imath::V3f > >();

	boost::python::class_<Imath::V3f>("Vec3f")
		.def("__init__", boost::python::make_constructor(vec3_defaultInit))
		.def(boost::python::init<float, float, float>())
		.def(boost::python::init<Imath::V3f>())
		.def_readwrite("x", &Imath::V3f::x)
		.def_readwrite("y", &Imath::V3f::y)
		.def_readwrite("z", &Imath::V3f::z)
		.def(boost::python::self + boost::python::self)
		.def(boost::python::self += boost::python::self)
		.def(boost::python::self - boost::python::self)
		.def(boost::python::self -= boost::python::self)
		.def(boost::python::self * boost::python::self)
		.def(boost::python::self *= boost::python::self)
		.def(boost::python::self / boost::python::self)
		.def(boost::python::self /= boost::python::self)
		.def(boost::python::self * boost::python::other<float>())
		.def(boost::python::self *= boost::python::other<float>())
		.def(- boost::python::self)
		.def("getValue", getValue)
		.def("setValue", setValue)
		.def("dot", &Imath::V3f::dot)
		.def("cross", &Imath::V3f::cross)
		.def("negate", &Imath::V3f::negate, boost::python::return_internal_reference<1>())
		.def("normalize", &Imath::V3f::normalize, boost::python::return_internal_reference<1>())
		.def("normalized", &Imath::V3f::normalized)
		.def("length", &Imath::V3f::length);
}
