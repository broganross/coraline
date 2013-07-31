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

#ifndef CORALDRAWPOINTNODE_H
#define CORALDRAWPOINTNODE_H

#include <cstdio>
#include <string.h>

#include <coral/src/Node.h>
#include <coral/src/NumericAttribute.h>
#include "DrawNode.h"
#include "coralUiDefinitions.h"

#include <GL/glew.h>

namespace coralUi{

class CORALUI_EXPORT DrawPointNode : public DrawNode{
public:
	DrawPointNode(const std::string &name, coral::Node *parent);
	~DrawPointNode();
	void drawSlice(unsigned int slice);
	void initGL();
	void initShader();

private:
	coral::NumericAttribute *_points;
	coral::NumericAttribute *_sizes;
	coral::NumericAttribute *_colors;

	void updatePointValues(unsigned int slice, const std::vector<Imath::V3f> &points);
	void updateSizeValues(unsigned int slice, const std::vector<Imath::V3f> &points, const std::vector<float> &sizes);
	void updateColorValues(unsigned int slice, const std::vector<Imath::V3f> &points, const std::vector<Imath::Color4f> &colors);
	void drawPoints(unsigned int slice, const std::vector<Imath::V3f> &points);

	// OpenGL
	GLuint _pointBuffer;	// buffer of vertices: {0.35, 0.76, 0.48, 0.56, 0.37, etc...}
	GLuint _sizeBuffer;		// buffer of sizes: {0.35, 0.76, 0.48, 0.56, 0.37, etc...}
	GLuint _colorBuffer;	// buffer of color4: {0.5, 0.5, 0.5, 1.0, 0.5, 0.5, 0.5, 1.0, etc...}

	GLuint _shaderProgram;
	GLint _pointIndexAttr;
	GLint _sizeIndexAttr;
	GLint _colorIndexAttr;
};

}

#endif
