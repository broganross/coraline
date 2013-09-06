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
#ifdef CORAL_PARALLEL_TBB
	#include <tbb/mutex.h>
#endif

#include <coral/src/NetworkManager.h>
#include <coral/src/Attribute.h>
#include <coral/src/containerUtils.h>
#include "MainDrawRoutine.h"
#include "DrawNode.h"

using namespace coral;
using namespace coralUi;

namespace {
	#ifdef CORAL_PARALLEL_TBB
		tbb::mutex _globalMutex;
	#endif

	std::vector<DrawNode*> _drawNodes;
	bool _renderScheduled = false;
	bool _initialized = false;
}

void(*MainDrawRoutine::_viewportRefreshCallback)(void) = 0;

void MainDrawRoutine::init(){
	if(!_initialized){
		glewExperimental = GL_TRUE;
		GLenum glewInitResult = glewInit();
		if(glewInitResult != GLEW_OK){
		    std::cout << "Impossible to init GLEW: " << glewGetErrorString(glewInitResult) << std::endl;
		    return;
		}

		_initialized = true;

		for(std::vector<DrawNode*>::const_iterator itDrawNode = _drawNodes.begin(); itDrawNode != _drawNodes.end(); ++itDrawNode){
			(*itDrawNode)->initGL();
		}
	}
}

bool MainDrawRoutine::initialized(){
	return _initialized;
}

void MainDrawRoutine::addDrawNode(DrawNode *drawNode){
	if(!containerUtils::elementInContainer(drawNode, _drawNodes)){
		_drawNodes.push_back(drawNode);
	}
}

void MainDrawRoutine::removeDrawNode(DrawNode *drawNode){
	containerUtils::eraseElementInContainer(drawNode, _drawNodes);
	
	//scheduleRender();
}

void MainDrawRoutine::drawAll(){
	_renderScheduled = false;
	if(_initialized){
		for(std::vector<DrawNode*>::const_iterator itDrawNode = _drawNodes.begin(); itDrawNode != _drawNodes.end(); ++itDrawNode){
			(*itDrawNode)->draw();
		}
	}
}

void MainDrawRoutine::dirtyingDoneCallback(Attribute *attribute){
	if(_viewportRefreshCallback){
		_renderScheduled = false;
		_viewportRefreshCallback();
	}
}

void MainDrawRoutine::scheduleRender(){
	if(_initialized){
		if(!_renderScheduled){
			#ifdef CORAL_PARALLEL_TBB
				tbb::mutex::scoped_lock lock(_globalMutex);
			#endif
			_renderScheduled = true;
			
			Attribute::queueDirtyingDoneCallback(&MainDrawRoutine::dirtyingDoneCallback);
		}
	}
}

