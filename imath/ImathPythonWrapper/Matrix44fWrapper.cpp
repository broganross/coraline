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
#include "Matrix44fWrapper.h"

namespace{
	boost::python::tuple getX(Imath::M44f &self)
	{
		boost::python::tuple x =
			boost::python::make_tuple(
									  boost::python::make_tuple(self.x[0][0],
																self.x[0][1],
																self.x[0][2],
																self.x[0][3]),
									  boost::python::make_tuple(self.x[1][0],
																self.x[1][1],
																self.x[1][2],
																self.x[1][3]),
									  boost::python::make_tuple(self.x[2][0],
																self.x[2][1],
																self.x[2][2],
																self.x[2][3]),
									  boost::python::make_tuple(self.x[3][0],
																self.x[3][1],
																self.x[3][2],
																self.x[3][3]));

		return x;
	}

	void setX(Imath::M44f &self, const boost::python::object &x)
	{
		boost::python::tuple tupleX = boost::python::tuple(x);

		for(int i = 0; i < 4; i++)
		{
			boost::python::tuple row =
				boost::python::extract<boost::python::tuple>(tupleX[i]);
			for(int j = 0; j < 4; j++)
			{
				self.x[i][j] = boost::python::extract<float>(row[j]);
			}
		}
	}

	void setXElement(Imath::M44f &self, int row, int column, float value)
	{
		self.x[row][column] = value;
	}

	float getXElement(Imath::M44f &self, int row, int column)
	{
		return self.x[row][column];
	}


	void invert(Imath::M44f &self)
	{
		bool singExc = false;
		self.invert(singExc);
	}

	Imath::M44f inverse(Imath::M44f &self)
	{
		bool singExc = false;
		return self.inverse(singExc);
	}

	void setEulerAngles(Imath::M44f &self, const Imath::V3f &r)
	{
		self.setEulerAngles(r);
	}

	void setAxisAngle(Imath::M44f &self, const Imath::V3f &ax, float ang)
	{
		self.setAxisAngle(ax, ang);
	}

	void setTranslation(Imath::M44f &self, const Imath::V3f &t)
	{
		self.setTranslation(t);
	}
}

void matrix44fWrapper()
{
	boost::python::to_python_converter<std::vector<Imath::M44f >,
		coral::pythonWrapperUtils::stdVectorToPythonList<Imath::M44f > >();

	boost::python::class_<Imath::M44f>("Matrix44f")
		.def(boost::python::init<Imath::M44f>())
		.add_property("x", getX, setX)
		.def("setX", setXElement)
		.def("getX", getXElement)
		.def(boost::python::self * boost::python::self)
		.def(boost::python::self *= boost::python::self)
		.def(boost::python::other<Imath::V3f>() * boost::python::self)
		// .def("getValue", getValue)
		.def("invert", invert)
		.def("inverse", inverse)
		.def("transpose", &Imath::M44f::transpose,
			 boost::python::return_internal_reference<1>())
		.def("transposed", &Imath::M44f::transposed)
		.def("setAxisAngle", setAxisAngle)
		.def("setEulerAngles", setEulerAngles)
		.def("translation", &Imath::M44f::translation)
		.def("setTranslation", setTranslation)
		;
}
