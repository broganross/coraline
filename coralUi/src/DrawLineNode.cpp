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
#include "DrawLineNode.h"
#include <coral/src/Attribute.h>
#include <coral/src/Numeric.h>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))


using namespace coral;
using namespace coralUi;

DrawLineNode::DrawLineNode(const std::string &name, Node *parent):
DrawNode(name, parent),
_pointIndexAttr(0),
_colorIndexAttr(1){
	setSliceable(true);
	
	_points = new NumericAttribute("points", this);
	_thickness = new NumericAttribute("thickness", this);
	_colors = new NumericAttribute("colors", this);

	addInputAttribute(_points);
	addInputAttribute(_thickness);
	addInputAttribute(_colors);

	setAttributeAllowedSpecialization(_points, "Vec3Array");
	setAttributeAllowedSpecialization(_thickness, "Float");

	std::vector<std::string> colorSpecializations;
	colorSpecializations.push_back("Col4");
	colorSpecializations.push_back("Col4Array");
	setAttributeAllowedSpecializations(_colors, colorSpecializations);

	_thickness->outValue()->setFloatValueAt(0, 1.0);

	setSpecializationPreset("single", _colors, "Col4");
	setSpecializationPreset("array", _colors, "Col4Array");
	enableSpecializationPreset("single");

	if(glContextExists()){
		initGL();
	}
}

void DrawLineNode::initGL(){
	DrawNode::initGL();
	
	glGenBuffers(1, &_pointBuffer);
	glGenBuffers(1, &_colorBuffer);

	initShader();
}

DrawLineNode::~DrawLineNode(){
	if(glContextExists()){
		glDeleteBuffers(1, &_pointBuffer);
		glDeleteBuffers(1, &_colorBuffer);
	}
}

void DrawLineNode::initShader(){

	// TODO gl_ModelViewProjectionMatrix is deprecated. We should send our own matrix: http://www.opengl.org/discussion_boards/ubbthreads.php?ubb=showflat&Number=283405
	// and http://stackoverflow.com/questions/4202456/how-do-you-get-the-modelview-and-projection-matrices-in-opengl
	std::string vertexShaderSource =
		"#version 120\n\
		uniform bool un_useSingleColor;\n\
		uniform vec4 un_singleColor;\n\
		attribute vec3 in_Position;\n\
		attribute vec4 in_Color;\n\
		void main() {\n\
			if(un_useSingleColor){\n\
				gl_FrontColor = un_singleColor;\n\
			} else {\n\
				gl_FrontColor = in_Color;\n\
			}\n\
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

	// prepare varing for future binding to VBOs
	glBindAttribLocation(_shaderProgram, _pointIndexAttr, "in_Position");
	glBindAttribLocation(_shaderProgram, _colorIndexAttr, "in_Color");

	glLinkProgram(_shaderProgram);

	// check link status
	GLint linkStatus;
	glGetProgramiv(_shaderProgram, GL_LINK_STATUS, &linkStatus);
	if(linkStatus == GL_FALSE){
		std::cout << "error while linking shader program" << std::endl;
	}
}

void DrawLineNode::updatePointValues(unsigned int slice){
	Numeric *vec3Numeric = _points->value();
	const std::vector<Imath::V3f> &vec3Values = vec3Numeric->vec3ValuesSlice(slice);

	// vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, _pointBuffer);
	glBufferData(GL_ARRAY_BUFFER, 3*sizeof(GLfloat)*vec3Values.size(), (GLvoid*)&vec3Values[0].x, GL_STATIC_DRAW);
	
	// clean OpenGL states
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DrawLineNode::updateColorValues(unsigned int slice){
	Numeric *col4Numeric = _colors->value();
	const std::vector<Imath::Color4f> &col4Values = col4Numeric->col4ValuesSlice(slice);

	// color buffer
	if(col4Numeric->isArray() && col4Values.size() > 0){
		// avoid empty color (and maybe crashs)
		Numeric *vec3Numeric = _points->value();
		const std::vector<Imath::V3f> &vec3Values = vec3Numeric->vec3ValuesSlice(slice);
		int pointCount = (int)vec3Values.size();
		int colorCount = (int)col4Values.size();

		glBindBuffer(GL_ARRAY_BUFFER, _colorBuffer);
		glBufferData(GL_ARRAY_BUFFER, 4*sizeof(GLfloat)*pointCount, (GLvoid*)&col4Values[0].r, GL_STATIC_DRAW);

		if(colorCount < pointCount){
			int emptyColCount = pointCount - colorCount;	// get the number of empty color to create in the buffer to match the number of vertex

			// create an array to feed of green color
			std::vector<Imath::Color4f> emptyColArray;
			emptyColArray.resize(emptyColCount, Imath::Color4f(1.0, 1.0, 1.0, 1.0));

			GLintptr offset = 4*sizeof(GLfloat)*colorCount;
			GLsizeiptr size = 4*sizeof(GLfloat)*emptyColCount;
			glBufferSubData(GL_ARRAY_BUFFER, offset, size, (GLvoid*)&emptyColArray[0].r);
		}

		// clean OpenGL state
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

void DrawLineNode::drawLines(unsigned int slice){
	glDisable(GL_LINE_SMOOTH);

	// get line size
	Numeric *thicknessNumeric = _thickness->value();
	GLfloat lineWith = 1.0;
	if(thicknessNumeric->type() == Numeric::numericTypeInt){
		const std::vector<int> &intValues = thicknessNumeric->intValuesSlice(slice);
		lineWith = (GLfloat) intValues[0];
	}
	else if (thicknessNumeric->type() == Numeric::numericTypeFloat){
		const std::vector<float> &floatValues = thicknessNumeric->floatValuesSlice(slice);
		lineWith = (GLfloat) floatValues[0];
	}

	Numeric *colors = _colors->value();
	bool useSingleColor = !colors->isArray();

	// set line size
	glLineWidth(lineWith);

	glUseProgram(_shaderProgram);

	// set uniform values (can only be done after glUseProgram)
	GLint locUseSingleColor = glGetUniformLocation(_shaderProgram, "un_useSingleColor");
	glUniform1i(locUseSingleColor, useSingleColor);

	// set VBOs
	glBindBuffer(GL_ARRAY_BUFFER, _pointBuffer);
	glVertexAttribPointer(_pointIndexAttr, 3, GL_FLOAT, 0, 0, NULL);
	glEnableVertexAttribArray(_pointIndexAttr);

	// do not set pointer if it use a single value. The value is already send via VBO
	if(useSingleColor){
		const Imath::Color4f &color = colors->col4ValuesSlice(slice)[0];
		GLint locSingleColor = glGetUniformLocation(_shaderProgram, "un_singleColor");
		glUniform4f(locSingleColor, color.r, color.g, color.b, color.a);
	}
	else{
		glBindBuffer(GL_ARRAY_BUFFER, _colorBuffer);
		glVertexAttribPointer(_colorIndexAttr, 4, GL_FLOAT, 0, 0, NULL);
		glEnableVertexAttribArray(_colorIndexAttr);
	}

	glEnableClientState(GL_VERTEX_ARRAY);

	// render
	const std::vector<Imath::V3f> &points = _points->value()->vec3ValuesSlice(slice);
	glDrawArrays(GL_LINE_STRIP, 0, points.size());
	
	// clean OpenGL statement
	glDisableVertexAttribArray(_pointIndexAttr);
	glDisableVertexAttribArray(_colorIndexAttr);

	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(0);
}

void DrawLineNode::drawSlice(unsigned int slice){
	Numeric *points = _points->value();
	const std::vector<Imath::V3f> &vec3Values = points->vec3ValuesSlice(slice);

	if(vec3Values.size() == 0)
		return;
	
	updatePointValues(slice);
	updateColorValues(slice);
	drawLines(slice);
}
