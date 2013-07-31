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

#ifndef CORALDRAWLINENODE_H
#define CORALDRAWLINENODE_H

#include <cstdio>
#include <string.h>

#include <coral/src/Node.h>
#include <coral/src/NumericAttribute.h>
#include "DrawNode.h"
#include "coralUiDefinitions.h"

#include <GL/glew.h>

namespace coralUi{

class CORALUI_EXPORT DrawLineNode : public DrawNode{
public:
	DrawLineNode(const std::string &name, coral::Node *parent);
	~DrawLineNode();
	void drawSlice(unsigned int slice);
	void initGL();
	void initShader();

private:
	coral::NumericAttribute *_points;
	coral::NumericAttribute *_thickness;
	coral::NumericAttribute *_colors;

	void updatePointValues(unsigned int slice);
	void updateColorValues(unsigned int slice);
	void drawLines(unsigned int slice);

	// OpenGL
	GLuint _pointBuffer;
	GLuint _colorBuffer;

	GLuint _shaderProgram;
	GLuint _pointIndexAttr;
	GLuint _colorIndexAttr;
};

}

#endif
