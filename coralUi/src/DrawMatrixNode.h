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

#ifndef CORALDRAWMATRIXNODE_H
#define CORALDRAWMATRIXNODE_H

#include <cstdio>
#include <string.h>

#include <coral/src/Node.h>
#include <coral/src/NumericAttribute.h>
#include "DrawNode.h"
#include "coralUiDefinitions.h"

#include <GL/glew.h>

namespace coralUi{

class CORALUI_EXPORT DrawMatrixNode : public DrawNode{
public:
	DrawMatrixNode(const std::string &name, coral::Node *parent);
	~DrawMatrixNode();
	void drawSlice(unsigned int slice);
	void initGL();
	void initShader();

private:
	coral::NumericAttribute *_matrix;
	coral::NumericAttribute *_size;

	void updateMat44Values(unsigned int slice, const std::vector<Imath::M44f> &matrix);
	void updateMatrixGizmo(unsigned int slice);
	void drawMatrix(unsigned int slice, const std::vector<Imath::M44f> &matrix);

	// OpenGL
	GLuint _gizmoBuffer;	// the gizmo geometry + color
	GLuint _matrixBuffer;	// the matrix buffer

	GLuint _shaderProgram;	// the main shader program (only a vertex shader actually
	GLint _pointAttrLoc;	// point and color attribut location
	GLint _colorAttrLoc;
	GLint _matrixAttrLoc;
};

}

#endif
