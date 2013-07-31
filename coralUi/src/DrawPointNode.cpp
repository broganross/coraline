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
#include "DrawPointNode.h"
#include <coral/src/Attribute.h>
#include <coral/src/Numeric.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))


using namespace coral;
using namespace coralUi;

DrawPointNode::DrawPointNode(const std::string &name, Node *parent):
DrawNode(name, parent),
_pointIndexAttr(-1),
_colorIndexAttr(-1),
_sizeIndexAttr(-1){
	setSliceable(true);
	
	_points = new NumericAttribute("points", this);
	_sizes = new NumericAttribute("sizes", this);
	_colors = new NumericAttribute("colors", this);
	
	addInputAttribute(_points);
	addInputAttribute(_sizes);
	addInputAttribute(_colors);
	
	std::vector<std::string> pointSpecializations;
	pointSpecializations.push_back("Vec3");
	pointSpecializations.push_back("Vec3Array");
	setAttributeAllowedSpecializations(_points, pointSpecializations);

	std::vector<std::string> sizeSpecializations;
	sizeSpecializations.push_back("Float");
	sizeSpecializations.push_back("FloatArray");
	setAttributeAllowedSpecializations(_sizes, sizeSpecializations);

	std::vector<std::string> colorSpecializations;
	colorSpecializations.push_back("Col4");
	colorSpecializations.push_back("Col4Array");
	setAttributeAllowedSpecializations(_colors, colorSpecializations);

	setSpecializationPreset("single size and color", _sizes, "Float");
	setSpecializationPreset("single size and color", _colors, "Col4");

	enableSpecializationPreset("single size and color");

	_sizes->outValue()->setFloatValueAt(0, 3.0);

	if(glContextExists()){
		initGL();
	}
}

void DrawPointNode::initGL(){
	DrawNode::initGL();

	// generate OpenGL buffers
	glGenBuffers(1, &_pointBuffer);
	glGenBuffers(1, &_sizeBuffer);
	glGenBuffers(1, &_colorBuffer);

	initShader();
}

DrawPointNode::~DrawPointNode(){
	if(glContextExists()){
		glDeleteBuffers(1, &_pointBuffer);
		glDeleteBuffers(1, &_sizeBuffer);
		glDeleteBuffers(1, &_colorBuffer);
	}
}

void DrawPointNode::initShader(){

	// this permit to use per vertex point size in GLSL (gl_PointSize)
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

	// TODO gl_ModelViewProjectionMatrix is deprecated. We should send our own matrix: http://www.opengl.org/discussion_boards/ubbthreads.php?ubb=showflat&Number=283405
	// and http://stackoverflow.com/questions/4202456/how-do-you-get-the-modelview-and-projection-matrices-in-opengl
	std::string vertexShaderSource =
		"#version 120\n\
		attribute vec3 in_Position;\n\
		attribute float in_Size;\n\
		attribute vec4 in_Color;\n\
		void main() {\n\
			gl_FrontColor = in_Color;\n\
			gl_PointSize = in_Size;\n\
			gl_Position = gl_ModelViewProjectionMatrix * vec4(in_Position,1.0);\n\
		}";

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	GLchar *glVertexShaderSource = (GLchar*)vertexShaderSource.data();
	glShaderSource(vertexShader, 1, (const GLchar**) &glVertexShaderSource, NULL);
	glCompileShader(vertexShader);

	// check compilation status
	int shaderStatus;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shaderStatus);

	// output compilation error if one
	if(shaderStatus == GL_FALSE){
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
	_pointIndexAttr = glGetAttribLocation(_shaderProgram, "in_Position");
	_sizeIndexAttr = glGetAttribLocation(_shaderProgram, "in_Size");
	_colorIndexAttr = glGetAttribLocation(_shaderProgram, "in_Color");
}

void DrawPointNode::updatePointValues(unsigned int slice, const std::vector<Imath::V3f> &points){
	glBindBuffer(GL_ARRAY_BUFFER, _pointBuffer);
	glBufferData(GL_ARRAY_BUFFER, 3*sizeof(GLfloat)*points.size(), (GLvoid*)&points[0].x, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DrawPointNode::updateSizeValues(unsigned int slice, const std::vector<Imath::V3f> &points, const std::vector<float> &sizes){
	int sizeCount = sizes.size();
	int pointCount = points.size();

	glBindBuffer(GL_ARRAY_BUFFER, _sizeBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*pointCount, (GLvoid*)&sizes[0], GL_STATIC_DRAW);

	if(sizeCount < pointCount){
		GLfloat defaultSize = 3.0;
		if(sizeCount > 0){
			defaultSize = sizes[sizeCount - 1];
		}

		int emptySizeCount = pointCount - sizeCount;	// get the number of empty color to create in the buffer to match the number of vertex

		// create an array to feed
		std::vector<GLfloat> emptySizeArray;
		emptySizeArray.resize(emptySizeCount, defaultSize);

		GLintptr offset = sizeof(GLfloat)*sizeCount;
		GLsizeiptr size = sizeof(GLfloat)*emptySizeCount;
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, (GLvoid*)&emptySizeArray[0]);
	}

	// clean OpenGL state
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DrawPointNode::updateColorValues(unsigned int slice, const std::vector<Imath::V3f> &points, const std::vector<Imath::Color4f> &colors){
	int colorCount = colors.size();
	int pointCount = points.size();

	glBindBuffer(GL_ARRAY_BUFFER, _colorBuffer);
	glBufferData(GL_ARRAY_BUFFER, 4*sizeof(GLfloat)*pointCount, (GLvoid*)&colors[0].r, GL_STATIC_DRAW);
	
	if(colorCount < pointCount){
		Imath::Color4f defaultColor(1.0, 1.0, 1.0, 1.0);
		if(colorCount > 0){
			defaultColor = colors[colorCount - 1];
		}

		int emptyColCount = pointCount - colorCount;	// get the number of empty color to create in the buffer to match the number of vertex

		// create an array and feed it with default color
		std::vector<Imath::Color4f> emptyColArray;
		emptyColArray.resize(emptyColCount, defaultColor);

		GLintptr offset = 4*sizeof(GLfloat)*colorCount;
		GLsizeiptr size = 4*sizeof(GLfloat)*emptyColCount;
		glBufferSubData(GL_ARRAY_BUFFER, offset, size, (GLvoid*)&emptyColArray[0].r);
	}

	// clean OpenGL state
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DrawPointNode::drawPoints(unsigned int slice, const std::vector<Imath::V3f> &points){
	int pointCount = points.size();

	glUseProgram(_shaderProgram);

	// set VBOs
	glBindBuffer(GL_ARRAY_BUFFER, _pointBuffer);
	glVertexAttribPointer(_pointIndexAttr, 3, GL_FLOAT, 0, 0, NULL);
	glEnableVertexAttribArray(_pointIndexAttr);

	glBindBuffer(GL_ARRAY_BUFFER, _sizeBuffer);
	glVertexAttribPointer(_sizeIndexAttr, 1, GL_FLOAT, 0, 0, NULL);
	glEnableVertexAttribArray(_sizeIndexAttr);

	glBindBuffer(GL_ARRAY_BUFFER, _colorBuffer);
	glVertexAttribPointer(_colorIndexAttr, 4, GL_FLOAT, 0, 0, NULL);
	glEnableVertexAttribArray(_colorIndexAttr);

	// render
	glDrawArrays(GL_POINTS, 0, pointCount);

	// clean OpenGL statement
	glDisableVertexAttribArray(_pointIndexAttr);
	glDisableVertexAttribArray(_sizeIndexAttr);
	glDisableVertexAttribArray(_colorIndexAttr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(0);
}

void DrawPointNode::drawSlice(unsigned int slice){
	const std::vector<Imath::V3f> &points = _points->value()->vec3ValuesSlice(slice);
	unsigned int pointsCount = points.size();
	if(pointsCount == 0)
		return;

	const std::vector<Imath::Color4f> &colors = _colors->value()->col4ValuesSlice(slice);
	const std::vector<float> &sizes = _sizes->value()->floatValuesSlice(slice);

	updatePointValues(slice, points);
	updateSizeValues(slice, points, sizes);
	updateColorValues(slice, points, colors);
	drawPoints(slice, points);
}
