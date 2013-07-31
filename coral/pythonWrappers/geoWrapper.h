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

#ifndef CORAL_GEOWRAPPER_H
#define CORAL_GEOWRAPPER_H

#include "../src/Geo.h"
#include "../src/GeoAttribute.h"
#include "../builtinNodes/GeoNodes.h"
#include "../src/pythonWrapperUtils.h"
#include "../src/GeoInstanceArray.h"
#include "../src/GeoInstanceArrayAttribute.h"
#include "../builtinNodes/GeoArrayInstanceNodes.h"

void geoWrapper(){
	boost::python::class_<Geo, boost::shared_ptr<Geo>, boost::python::bases<Value>, boost::noncopyable>("Geo", boost::python::no_init)
		.def("__init__", pythonWrapperUtils::__init__<Geo>)
		.def("createUnwrapped", pythonWrapperUtils::createUnwrapped<Geo>)
		.staticmethod("createUnwrapped")
	;

	pythonWrapperUtils::pythonWrapper<GeoAttribute, Attribute>("GeoAttribute");
	pythonWrapperUtils::pythonWrapper<GetGeoPoints, Node>("GetGeoPoints");
	pythonWrapperUtils::pythonWrapper<SetGeoPoints, Node>("SetGeoPoints");
	pythonWrapperUtils::pythonWrapper<GetGeoNormals, Node>("GetGeoNormals");
	pythonWrapperUtils::pythonWrapper<GeoNeighbourPoints, Node>("GeoNeighbourPoints");
	
	pythonWrapperUtils::pythonWrapper<GetGeoElements, Node>("GetGeoElements");
	pythonWrapperUtils::pythonWrapper<GetGeoSubElements, Node>("GetGeoSubElements");

	boost::python::class_<GeoInstanceArray, boost::shared_ptr<GeoInstanceArray>, boost::python::bases<Value>, boost::noncopyable>("GeoInstanceArray", boost::python::no_init)
		.def("__init__", pythonWrapperUtils::__init__<GeoInstanceArray>)
		.def("createUnwrapped", pythonWrapperUtils::createUnwrapped<GeoInstanceArray>)
		.staticmethod("createUnwrapped")
	;

	pythonWrapperUtils::pythonWrapper<GeoInstanceArrayAttribute, Attribute>("GeoInstanceArrayAttribute");
	pythonWrapperUtils::pythonWrapper<GeoInstanceGenerator, Node>("GeoInstanceGenerator")
		.def("addInputGeo", &GeoInstanceGenerator::addInputGeo);
}

#endif