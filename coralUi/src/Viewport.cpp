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

#include <vector>
#include "Viewport.h"
#include <coral/src/NetworkManager.h>
#include <coral/src/containerUtils.h>
#include "MainDrawRoutine.h"

#include <GL/glew.h>

using namespace coralUi;

#define PI 3.14159265
#define DEGTORAD(x) ((x) * 0.01745329251994)		// (x * PI ) / 180.0
#define RADTODEG(x) ((x) / 0.01745329251994)		// (x * 180.0) / PI

GLfloat lightPosition[] = { 20.f, 50.f, 0.f, 1.f };

GLfloat lightAmbient[] = { .4f, .4f, .4f };
GLfloat lightDiffuse[] = { .8f, .8f, .8f };
GLfloat lightSpecular[] = { 1.f, 1.f, 1.f, 1.f };
GLint defaultShininess = 96;

GLfloat grayAmbient[] = { .3f, .3f, .3f, 1.f };
GLfloat grayDiffuse[] = { .8f, .8f, .8f, 1.f };
GLfloat whiteSpecular[] = { 1.f, 1.f, 1.f, 1.f };

bool _shouldDrawWireframe = false;
bool _shouldDrawFlat = false;
bool _shouldDrawSmooth = true;
bool _shouldDrawPoints = false;
bool _shouldDrawNormals = false;

Viewport::Viewport() :
_initialized(false),
_width(0), _height(0),
_fov(60.0), _roll(0.0), _zNear(0.01), _zFar(1000.0),
_isProjDirty(true),
_gridVisible(true)
{
	_modelMatrix.setTranslation(Imath::V3f(0.f, 0.f, 10.f));
	_target.setValue(0.f, 0.f, 0.f);
	
	orbit(-40, 45);
	dolly(240);
}

Viewport::~Viewport(){
}

void Viewport::initializeGL(){
	MainDrawRoutine::init();

	glClearColor(.3f, .3f, .3f, 1.f);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_TRUE);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA);
	
	// lighting
	glEnable(GL_LIGHT0);
	
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
}

void Viewport::resizeGL(int width, int height){
	if(height == 0)
		height = 1;
	
	_width = width;
	_height = height;
	
	_isProjDirty = true;
}

void Viewport::setupProjection(){
	glMatrixMode(GL_PROJECTION);
	
	if(_isProjDirty){
	        glLoadIdentity();
	
	        float height = _zNear * tan(_fov * PI / 360.0);
	        float width = height;
	
	        float aspect = _width / (float)_height;
	        if(aspect > 1.0)
	            height /= aspect;
	        else
	            width = height * aspect;
	
	        glFrustum(-width, width, -height, height, _zNear, _zFar);
	        glGetFloatv(GL_PROJECTION_MATRIX, _projectionMatrix);
	        _isProjDirty = false;
	}
	    else
		glLoadMatrixf(_projectionMatrix);
}

void Viewport::prepareForDrawing(){
	setupProjection();
	glViewport(0, 0, _width, _height);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(_modelMatrix.inverse().getValue());
	
	// gray-shaded
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, grayAmbient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, grayDiffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, whiteSpecular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, defaultShininess);
}

void Viewport::setGridVisible(bool value){
	_gridVisible = value;
}

void Viewport::draw(){
	bool bHeadLight = true;
	
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	prepareForDrawing();
	
	glDisable(GL_LIGHTING);
	glShadeModel(GL_FLAT);
	
	if(_gridVisible)
		drawGrid();
	
	drawAxis();
	
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	
	if(bHeadLight){
		// constrain light to camera
		lightPosition[0] = _modelMatrix.x[3][0];
		lightPosition[1] = _modelMatrix.x[3][1] + 100.0;
		lightPosition[2] = _modelMatrix.x[3][2];
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	}
	
	prepareForDrawing();
	MainDrawRoutine::drawAll();
}

void Viewport::orbit(int deltaSide, int deltaUp){
	Imath::V3f cam(_modelMatrix.translation());
	_modelMatrix.x[3][0] = _modelMatrix.x[3][1] = _modelMatrix.x[3][2] = 0.f;
	
	Imath::M44f mUp, mSide, mRoll, mUnroll;
	mUp.setAxisAngle(Imath::V3f(1.f, 0.f, 0.f), (float)DEGTORAD((float)-deltaUp * 0.5f));	// setRotationX(deltaSide)
	mSide.setAxisAngle(Imath::V3f(0.f, 1.f, 0.f), (float)DEGTORAD((float)-deltaSide * 0.5f));// setRotationY(..)
	mRoll.setAxisAngle(Imath::V3f(0.f, 0.f, 1.f), (float)DEGTORAD(_roll));
	mUnroll.setAxisAngle(Imath::V3f(0.f, 0.f, -1.f), (float)DEGTORAD(_roll));
	
	_modelMatrix.setValue(mRoll *((mUp * mUnroll * _modelMatrix) * mSide));
	
	// create a new cam (0,0,cam-target), mul it by cam matrix, add target
	cam.setValue(0.f, 0.f, (cam - _target).length());
	cam *= _modelMatrix;
	cam += _target;
	
	_modelMatrix.x[3][0] = cam.x;
	_modelMatrix.x[3][1] = cam.y;
	_modelMatrix.x[3][2] = cam.z;
}

void Viewport::pan(int deltaSide, int deltaUp){
	float lenDist = (_modelMatrix.translation() - _target).length();
	
	float factorX = lenDist * 0.001f * -deltaSide;
	float factorY = lenDist * 0.001f * deltaUp;
	
	Imath::V3f panVec(factorX, factorY, 0.f);
	_target += _modelMatrix.translate(panVec).translation() - _modelMatrix.translation();
}

void Viewport::dolly(int deltaSide){
	if(deltaSide != 0){
	
		Imath::V3f dollyVec(_modelMatrix.x[2][0], _modelMatrix.x[2][1], _modelMatrix.x[2][2]);	// cam does dolly on its Z local axis
	
		dollyVec *= (deltaSide * (_modelMatrix.translation() - _target).length() * 0.01f);	// moltiplicator (based on cam-target)
	
		_modelMatrix.x[3][0] += dollyVec.x;
		_modelMatrix.x[3][1] += dollyVec.y;
		_modelMatrix.x[3][2] += dollyVec.z;
	}
}

void Viewport::zoom(int deltaSide){
	float newFov = _fov - (float)deltaSide * 0.5;

	if(newFov > 0){
		_fov = newFov;
		_isProjDirty = true;
	}
}

void Viewport::roll(int deltaSide){
	if(deltaSide != 0){
	
		_roll += deltaSide;
	
		Imath::M44f mRoll;
		mRoll.setAxisAngle(Imath::V3f(0.f, 0.f, 1.f), (float)DEGTORAD(deltaSide));
	
		_modelMatrix.setValue(mRoll * _modelMatrix);
	}
}

void Viewport::drawGrid(){
	int count = 20;
	GLfloat scale = 1.0f;
	
	glColor3f(0.45f, 0.45f, 0.45f);
	glLineWidth(1.f);
	glBegin(GL_LINES);
	{
		for(int w=0; w<=count; w++){
			glVertex3f(-count/2*scale,			0,	(w-count/2)*scale);
			glVertex3f((count-count/2)*scale,	0,	(w-count/2)*scale);
		}
		for(int w=0; w<=count; w++){
			glVertex3f((w-count/2)*scale,		0,	-count/2*scale );
			glVertex3f((w-count/2)*scale,		0,	(count-count/2)*scale);
		}
	}
	glEnd();
	
	// draw 2 middle lines again
	glColor3f (0.9f, 0.2f, 0.2f);
	glLineWidth(1.f);
	glBegin(GL_LINES);
	{
		glVertex3f(-count/2*scale,			0,	(count/2-count/2)*scale);
		glVertex3f((count-(count/2))*scale,	0,	(count/2-count/2)*scale);
	
		glVertex3f((count/2-count/2)*scale,	0,	-count/2*scale);
		glVertex3f((count/2-count/2)*scale,	0,	(count-count/2)*scale);
	}
	glEnd();
}

void Viewport::drawAxis(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	glOrtho(-1, 1, -1, 1, -1, 1);
	glViewport(0, 0, 50, 50);
	
	GLfloat mvm[16];
	glMatrixMode(GL_MODELVIEW);
	glGetFloatv(GL_MODELVIEW_MATRIX, mvm);
	mvm[12] = mvm[13] = mvm[14] = 0.f;
	glLoadMatrixf(mvm);
	
	glLineWidth(2.f);
	glBegin(GL_LINES);
	{
		// X
		glColor3f(1.f, 0.f, 0.f);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(1.f, 0.f, 0.f);
		// Y
		glColor3f(0.f, 1.f, 0.f);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(0.f, 1.f, 0.f);
		// Z
		glColor3f(0.f, 0.f, 1.f);
		glVertex3f(0.f, 0.f, 0.f);
		glVertex3f(0.f, 0.f, 1.f);
	}
	glEnd();
}

Imath::M44f Viewport::modelMatrix(){
	return _modelMatrix;
}

float Viewport::fov(){
	return _fov;
}

float Viewport::zNear(){
	return _zNear;
}

float Viewport::zFar(){
	return _zFar;
}
