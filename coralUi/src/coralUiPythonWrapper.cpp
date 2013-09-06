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
#include "viewportWrapper.h"
// #include "viewportOutputAttributeWrapper.h"
#include "mainDrawRoutineWrapper.h"
#include "drawNodeWrapper.h"
#include "geoDrawNodeWrapper.h"
#include "DrawPointNode.h"
#include "DrawLineNode.h"
#include "DrawMatrixNode.h"
#include "DrawGeoInstance.h"
#include "ShaderNode.h"

using namespace coralUi;

BOOST_PYTHON_MODULE(_coralUi){
	// viewportOutputAttributeWrapper();
	drawNodeWrapper();
	geoDrawNodeWrapper();	// TODO put every function that have only have one line here
	mainDrawRoutineWrapper();
	viewportWrapper();

	coral::pythonWrapperUtils::pythonWrapper<DrawPointNode, DrawNode>("DrawPointNode");
	coral::pythonWrapperUtils::pythonWrapper<DrawLineNode, DrawNode>("DrawLineNode");
	coral::pythonWrapperUtils::pythonWrapper<DrawMatrixNode, DrawNode>("DrawMatrixNode");
	coral::pythonWrapperUtils::pythonWrapper<DrawGeoInstance, DrawNode>("DrawGeoInstance");
	coral::pythonWrapperUtils::pythonWrapper<ShaderNode, DrawNode>("ShaderNode")
		.def("recompileShader", &ShaderNode::recompileShader)
		.def("recompileShaderLog", &ShaderNode::recompileShaderLog);
}
