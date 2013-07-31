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
#include "DrawMatrixNode.h"
#include <coral/src/Attribute.h>
#include <coral/src/Numeric.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))
#define OPENGL_CHECK_ERRORS { const GLenum errcode = glGetError(); if (errcode != GL_NO_ERROR) fprintf(stderr, "OpenGL Error code %i in '%s' line %i\n", errcode, __FILE__, __LINE__-1); }


using namespace coral;
using namespace coralUi;

DrawMatrixNode::DrawMatrixNode(const std::string &name, Node *parent):
DrawNode(name, parent),
_gizmoBuffer(0),
_matrixBuffer(0),
_shaderProgram(0),
_pointAttrLoc(-1),
_colorAttrLoc(-1),
_matrixAttrLoc(-1){
	setSliceable(true);
	
	_matrix = new NumericAttribute("matrix", this);
	_size = new NumericAttribute("size mult", this);

	addInputAttribute(_matrix);
	addInputAttribute(_size);

	std::vector<std::string> pointSpecializations;
	pointSpecializations.push_back("Matrix44");
	pointSpecializations.push_back("Matrix44Array");
	setAttributeAllowedSpecializations(_matrix, pointSpecializations);

	setAttributeAllowedSpecialization(_size, "Float");

	_size->outValue()->setFloatValueAt(0, 1.0);

	if(glContextExists()){
		initGL();
	}
}

DrawMatrixNode::~DrawMatrixNode(){
	if(glContextExists()){
		glDeleteBuffers(1, &_gizmoBuffer);
		glDeleteBuffers(1, &_matrixBuffer);
	}
}

void DrawMatrixNode::initGL(){
	DrawNode::initGL();
	
	glGenBuffers(1, &_gizmoBuffer);
	glGenBuffers(1, &_matrixBuffer);

	initShader();
}

void DrawMatrixNode::initShader(){

	std::string vertexShaderSource =
		"#version 120\n\
		attribute vec3 in_Position;\n\
		attribute vec3 in_Color;\n\
		attribute mat4 gizmoMatrixAttr;\n\
		void main() {\n\
			gl_FrontColor = vec4(in_Color, 1.0);\n\
			gl_Position = gl_ModelViewProjectionMatrix * gizmoMatrixAttr * vec4(in_Position, 1.0);\n\
		}";

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	GLchar *glVertexShaderSource = (GLchar*)vertexShaderSource.data();
	glShaderSource(vertexShader, 1, (const GLchar**) &glVertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// check compilation status
	int shaderStatus;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shaderStatus);

	// output compilation error if one
	if(!shaderStatus){
		std::cout << "error while compiling vertex shader" << std::endl;

		GLsizei logSize = 0;
		glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &logSize);

		char *log = new char[logSize];
		if(log == NULL)
		{
			std::cout << "impossible to allocate memory for shader compilation log" << std::endl;
		}

		memset(log, '\0', logSize + 1);

		glGetShaderInfoLog(vertexShader, logSize, &logSize, log);
		std::cout << "shader compilation log:" << std::endl << log << std::endl;
	}

	_shaderProgram = glCreateProgram();
	glAttachShader(_shaderProgram, vertexShader);

	glLinkProgram(_shaderProgram);

	// check link status
	GLint linkStatus;
	glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &linkStatus);
	if(linkStatus == GL_FALSE){
		std::cout << "error while linking shader program" << std::endl;
	}

	// get attribute location
	_pointAttrLoc = glGetAttribLocation(_shaderProgram, "in_Position");
	_colorAttrLoc = glGetAttribLocation(_shaderProgram, "in_Color");
	_matrixAttrLoc = glGetAttribLocation(_shaderProgram, "gizmoMatrixAttr");
}

void DrawMatrixNode::updateMat44Values(unsigned int slice, const std::vector<Imath::M44f> &matrix){
	glBindBuffer(GL_ARRAY_BUFFER, _matrixBuffer);
	glBufferData(GL_ARRAY_BUFFER, 16*sizeof(GLfloat)*matrix.size(), (GLvoid*)&matrix[0].x, GL_STATIC_DRAW);
}

void DrawMatrixNode::updateMatrixGizmo(unsigned int slice){
	GLfloat size = _size->value()->floatValueAtSlice(slice, 0);

	// generate and store the main gizmo array (which will be multiplied by user matrix in the shader, later)
	GLfloat gizmoArray[36] = {0.0, 0.0, 0.0,	1.0, 0.0, 0.0,	// position*3, color*3
					size, 0.0, 0.0,	1.0, 0.0, 0.0,
					0.0, 0.0, 0.0,	0.0, 1.0, 0.0,
					0.0, size, 0.0,	0.0, 1.0, 0.0,
					0.0, 0.0, 0.0,	0.0, 0.0, 1.0,
					0.0, 0.0, size,	0.0, 0.0, 1.0 };

	glBindBuffer(GL_ARRAY_BUFFER, _gizmoBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(gizmoArray), (GLvoid*)&gizmoArray, GL_STATIC_DRAW);
}

void DrawMatrixNode::drawMatrix(unsigned int slice, const std::vector<Imath::M44f> &matrix){
	glLineWidth(2.0);

	glUseProgram(_shaderProgram);

	// GLSL mat4 are actually 4 vec4 so we need to set them separately
	glBindBuffer(GL_ARRAY_BUFFER, _matrixBuffer);
	glEnableVertexAttribArray(_matrixAttrLoc);
	glEnableVertexAttribArray(_matrixAttrLoc+1);
	glEnableVertexAttribArray(_matrixAttrLoc+2);
	glEnableVertexAttribArray(_matrixAttrLoc+3);

	glVertexAttribPointer(_matrixAttrLoc, 4, GL_FLOAT, GL_FALSE, sizeof(Imath::M44f), NULL);
	glVertexAttribPointer(_matrixAttrLoc+1, 4, GL_FLOAT, GL_FALSE, sizeof(Imath::M44f), BUFFER_OFFSET(sizeof(GLfloat)*4));
	glVertexAttribPointer(_matrixAttrLoc+2, 4, GL_FLOAT, GL_FALSE, sizeof(Imath::M44f), BUFFER_OFFSET(sizeof(GLfloat)*8));
	glVertexAttribPointer(_matrixAttrLoc+3, 4, GL_FLOAT, GL_FALSE, sizeof(Imath::M44f), BUFFER_OFFSET(sizeof(GLfloat)*12));

	glVertexAttribDivisorARB(_matrixAttrLoc, 1);
	glVertexAttribDivisorARB(_matrixAttrLoc+1, 1);
	glVertexAttribDivisorARB(_matrixAttrLoc+2, 1);
	glVertexAttribDivisorARB(_matrixAttrLoc+3, 1);

	// set geometry and color (which is just an pointer offset). A stride is used: (3 float) * 2
	glBindBuffer(GL_ARRAY_BUFFER, _gizmoBuffer);
	glVertexAttribPointer(_pointAttrLoc, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*6, NULL);
	glEnableVertexAttribArray(_pointAttrLoc);

	glVertexAttribPointer(_colorAttrLoc, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*6, BUFFER_OFFSET(sizeof(GLfloat)*3));
	glEnableVertexAttribArray(_colorAttrLoc);

	// render
	glDrawArraysInstancedARB(GL_LINES, 0, 6, matrix.size());

	// clean OpenGL state
	glDisableVertexAttribArray(_matrixAttrLoc);
	glDisableVertexAttribArray(_matrixAttrLoc+1);
	glDisableVertexAttribArray(_matrixAttrLoc+2);
	glDisableVertexAttribArray(_matrixAttrLoc+3);

	glDisableVertexAttribArray(_pointAttrLoc);
	glDisableVertexAttribArray(_colorAttrLoc);

	glVertexAttribDivisorARB(_matrixAttrLoc, 0);
	glVertexAttribDivisorARB(_matrixAttrLoc + 1, 0);
	glVertexAttribDivisorARB(_matrixAttrLoc + 2, 0);
	glVertexAttribDivisorARB(_matrixAttrLoc + 3, 0);


	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(0);

	glLineWidth(1.0);
}

void DrawMatrixNode::drawSlice(unsigned int slice){
	const std::vector<Imath::M44f> &matrix = _matrix->value()->matrix44ValuesSlice(slice);

	if(matrix.size() == 0){
		return;
	}

	updateMatrixGizmo(slice);
	updateMat44Values(slice, matrix);
	drawMatrix(slice, matrix);
}
