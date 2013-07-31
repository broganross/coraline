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

#include <pythonWrapperUtils.h>
#include "QuatfWrapper.h"

float QuatIndexer::get(const Imath::Quatf &x, int i)
{
	// Do we want to handle backward indexing?
	if ( i >= 0 && i < 4 )
	{
		return x[i];
	}
	else
	{
		throw std::out_of_range("");
	}

}

void QuatIndexer::set(Imath::Quatf  &x, int i, const float &v)
{
	if ( i >= 0 && i < 4 )
	{
		x[i] = v;
	}
	else
	{
		throw std::out_of_range("");
	}
}

namespace {
	std::string reprQuatf( Imath::Quatf &x )
	{
		std::stringstream s;
		s << "Imath.Quatf" << "( ";
		for( unsigned i=0; i<4; i++ )
		{
			s << x[i];
			if( i!=3 )
			{
				s << ", ";
			}
		}
		s << " )";
		return s.str();
	}

	std::string strQuatf( Imath::Quatf &x )
	{
		std::stringstream s;
		for( unsigned i=0; i<4; i++ )
		{
			s << x[i];
			if( i!=3 )
			{
				s << " ";
			}
		}
		return s.str();
	}

	boost::shared_ptr<Imath::Quatf> defaultQuatfInit()	{
		return boost::shared_ptr<Imath::Quatf>(new Imath::Quatf());
	}
}

void quatfWrapper()
{
	boost::python::to_python_converter<std::vector<Imath::Quatf >, coral::pythonWrapperUtils::stdVectorToPythonList<Imath::Quatf > >();

	boost::python::class_<Imath::Quatf>("Quatf")
		.def(boost::python::init<>())
		.def(boost::python::init<Imath::Quatf >())
		.def(boost::python::init<float, float, float, float>())
		.def(boost::python::init<float, Imath::V3f >())
		.def_readwrite("r", &Imath::Quatf::r)
		.def_readwrite("v", &Imath::Quatf::v)

		.def("__getitem__", &QuatIndexer::get)
		.def("__setitem__", &QuatIndexer::set)

		.def(boost::python::self ^ boost::python::self)
		.def(boost::python::self *= boost::python::self)
		.def(boost::python::self *= boost::python::other<float>())
		.def(boost::python::self /= boost::python::self)
		.def(boost::python::self /= boost::python::other<float>())
		.def(boost::python::self += boost::python::self)
		.def(boost::python::self -= boost::python::self)
		.def(boost::python::self == boost::python::self)
		.def(boost::python::self != boost::python::self)
		.def( boost::python::self * boost::python::self )
		.def( ~boost::python::self )

		.def("identity", &Imath::Quatf::identity).staticmethod("identity")
		.def("invert", &Imath::Quatf::invert, boost::python::return_self<>())
		.def("inverse", &Imath::Quatf::inverse)
		.def("normalize", &Imath::Quatf::normalize, boost::python::return_self<>())
		.def("normalized", &Imath::Quatf::normalized)
		.def("length", &Imath::Quatf::length)
		.def("setAxisAngle", &Imath::Quatf::setAxisAngle, boost::python::return_self<>())
		.def("setRotation", &Imath::Quatf::setRotation, boost::python::return_self<>())
		.def("angle", &Imath::Quatf::angle)
		.def("axis", &Imath::Quatf::axis)
		.def("toMatrix44", &Imath::Quatf::toMatrix44)
		.def("log", &Imath::Quatf::log)
		.def("exp", &Imath::Quatf::exp)

		.def("__str__", &strQuatf)
		.def("__repr__", &reprQuatf)
	;

	boost::python::def("squad", Imath::squad<float>);
	boost::python::def("spline", Imath::spline<float>);
	boost::python::def("slerp", Imath::slerp<float>);
}

