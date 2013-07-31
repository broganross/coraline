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
#include <coral/src/pythonWrapperUtils.h>

#include "MayaFloatAttribute.h"
#include "MayaGeoAttribute.h"
#include "MayaIntAttribute.h"
#include "MayaMatrixAttribute.h"
#include "MayaAngleAttribute.h"

BOOST_PYTHON_MODULE(_coralMaya){
	boost::python::class_<MayaIntAttribute, boost::shared_ptr<MayaIntAttribute>, boost::python::bases<coral::NumericAttribute>, boost::noncopyable>("MayaIntAttribute", boost::python::no_init)
		.def("__init__", coral::pythonWrapperUtils::__init__<MayaIntAttribute, const std::string, coral::Node*>)
		.def("setMayaNode", &MayaFloatAttribute::setMayaNode)
		.def("setMayaAttribute", &MayaFloatAttribute::setMayaAttribute)
		.def("mayaAttribute", &MayaFloatAttribute::mayaAttribute)
		.def("mayaNode", &MayaFloatAttribute::mayaNode)
	;
	
	boost::python::class_<MayaFloatAttribute, boost::shared_ptr<MayaFloatAttribute>, boost::python::bases<coral::NumericAttribute>, boost::noncopyable>("MayaFloatAttribute", boost::python::no_init)
		.def("__init__", coral::pythonWrapperUtils::__init__<MayaFloatAttribute, const std::string, coral::Node*>)
		.def("setMayaNode", &MayaFloatAttribute::setMayaNode)
		.def("setMayaAttribute", &MayaFloatAttribute::setMayaAttribute)
		.def("mayaAttribute", &MayaFloatAttribute::mayaAttribute)
		.def("mayaNode", &MayaFloatAttribute::mayaNode)
	;
	
	boost::python::class_<MayaFloat3ArrayAttribute, boost::shared_ptr<MayaFloat3ArrayAttribute>, boost::python::bases<coral::NumericAttribute>, boost::noncopyable>("MayaFloat3ArrayAttribute", boost::python::no_init)
		.def("__init__", coral::pythonWrapperUtils::__init__<MayaFloat3ArrayAttribute, const std::string, coral::Node*>)
		.def("setMayaNode", &MayaFloat3ArrayAttribute::setMayaNode)
		.def("setMayaAttribute", &MayaFloat3ArrayAttribute::setMayaAttribute)
		.def("mayaAttribute", &MayaFloat3ArrayAttribute::mayaAttribute)
		.def("mayaNode", &MayaFloat3ArrayAttribute::mayaNode)
	;
	
	boost::python::class_<MayaGeoAttribute, boost::shared_ptr<MayaGeoAttribute>, boost::python::bases<coral::GeoAttribute>, boost::noncopyable>("MayaGeoAttribute", boost::python::no_init)
		.def("__init__", coral::pythonWrapperUtils::__init__<MayaGeoAttribute, const std::string, coral::Node*>)
		.def("setMayaNode", &MayaGeoAttribute::setMayaNode)
		.def("setMayaAttribute", &MayaGeoAttribute::setMayaAttribute)
		.def("mayaAttribute", &MayaGeoAttribute::mayaAttribute)
		.def("mayaNode", &MayaGeoAttribute::mayaNode)
	;
	
	boost::python::class_<MayaMatrixAttribute, boost::shared_ptr<MayaMatrixAttribute>, boost::python::bases<coral::NumericAttribute>, boost::noncopyable>("MayaMatrixAttribute", boost::python::no_init)
		.def("__init__", coral::pythonWrapperUtils::__init__<MayaMatrixAttribute, const std::string, coral::Node*>)
		.def("setMayaNode", &MayaMatrixAttribute::setMayaNode)
		.def("setMayaAttribute", &MayaMatrixAttribute::setMayaAttribute)
		.def("mayaAttribute", &MayaMatrixAttribute::mayaAttribute)
		.def("mayaNode", &MayaMatrixAttribute::mayaNode)
	;
	
	boost::python::class_<MayaMatrixArrayAttribute, boost::shared_ptr<MayaMatrixArrayAttribute>, boost::python::bases<coral::NumericAttribute>, boost::noncopyable>("MayaMatrixArrayAttribute", boost::python::no_init)
		.def("__init__", coral::pythonWrapperUtils::__init__<MayaMatrixArrayAttribute, const std::string, coral::Node*>)
		.def("setMayaNode", &MayaMatrixArrayAttribute::setMayaNode)
		.def("setMayaAttribute", &MayaMatrixArrayAttribute::setMayaAttribute)
		.def("mayaAttribute", &MayaMatrixArrayAttribute::mayaAttribute)
		.def("mayaNode", &MayaMatrixArrayAttribute::mayaNode)
	;
	
	boost::python::class_<MayaAngleAttribute, boost::shared_ptr<MayaAngleAttribute>, boost::python::bases<coral::NumericAttribute>, boost::noncopyable>("MayaAngleAttribute", boost::python::no_init)
		.def("__init__", coral::pythonWrapperUtils::__init__<MayaAngleAttribute, const std::string, coral::Node*>)
		.def("setMayaNode", &MayaAngleAttribute::setMayaNode)
		.def("setMayaAttribute", &MayaAngleAttribute::setMayaAttribute)
		.def("mayaAttribute", &MayaAngleAttribute::mayaAttribute)
		.def("mayaNode", &MayaAngleAttribute::mayaNode)
	;
	
	boost::python::class_<MayaAngle3ArrayAttribute, boost::shared_ptr<MayaAngle3ArrayAttribute>, boost::python::bases<coral::NumericAttribute>, boost::noncopyable>("MayaAngle3ArrayAttribute", boost::python::no_init)
		.def("__init__", coral::pythonWrapperUtils::__init__<MayaAngle3ArrayAttribute, const std::string, coral::Node*>)
		.def("setMayaNode", &MayaAngle3ArrayAttribute::setMayaNode)
		.def("setMayaAttribute", &MayaAngle3ArrayAttribute::setMayaAttribute)
		.def("mayaAttribute", &MayaAngle3ArrayAttribute::mayaAttribute)
		.def("mayaNode", &MayaAngle3ArrayAttribute::mayaNode)
	;
}
