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

#include <GL/glew.h>

#include <Python.h>
#include "CoralDrawNode.h"

#include <coralUi/src/MainDrawRoutine.h>

// TODO: provide proper mayaId
MTypeId CoralDrawNode::id(0x08102B);

void CoralDrawNode::draw(M3dView & view, const MDagPath & path, M3dView::DisplayStyle style, M3dView::DisplayStatus status){
	view.beginGL();
	
	glPushAttrib( GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_ENABLE_BIT | GL_PIXEL_MODE_BIT ); 
	
	PyGILState_STATE state = PyGILState_Ensure(); // ensure python's threads won't fuck up maya

	coralUi::MainDrawRoutine::drawAll();
	
	PyGILState_Release(state);

	glPopAttrib();
	
	view.endGL();
}

void coralMaya_refreshViewport(){
	M3dView::active3dView().refresh(false, true);
}

void *CoralDrawNode::creator(){
	//coral::MainDrawRoutine::_viewportRefreshCallback = coralMaya_refreshViewport;
	
	return new CoralDrawNode();
}

MStatus CoralDrawNode::initialize(){
	PyGILState_STATE state = PyGILState_Ensure();

	coralUi::MainDrawRoutine::init();

	PyGILState_Release(state);

	return MS::kSuccess;
}
