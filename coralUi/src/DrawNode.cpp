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
#include "DrawNode.h"
#include "MainDrawRoutine.h"
#include "Viewport.h"
//#include "ViewportOutputAttribute.h"

using namespace coral;
using namespace coralUi;

DrawNode::DrawNode(const std::string &name, Node *parent) : Node(name, parent){	
	setUpdateEnabled(false);
	
	MainDrawRoutine::addDrawNode(this);
}

void DrawNode::deleteIt(){
	MainDrawRoutine::removeDrawNode(this);
	Node::deleteIt();
	
	MainDrawRoutine::_viewportRefreshCallback();
}

void DrawNode::draw(){
	Node *slicer_ = slicer();
	if(slicer_){
		unsigned int slices_ = slicer_->computeSlices();

		if(slices_ != slices()){
			resizedSlices(slices_);
		}

		for(int i = 0; i < slices_; ++i){
			drawSlice(i);
		}
	}
	else{
		drawSlice(0);
	}
}

void DrawNode::drawSlice(unsigned int slice){
}

bool DrawNode::glContextExists(){
	return MainDrawRoutine::initialized();
}

void DrawNode::initGL(){
	const std::vector<Attribute*> inputs = inputAttributes();
	for(int i = 0; i < inputs.size(); ++i){
		catchAttributeDirtied(inputs[i]);
	}
}

void DrawNode::attributeDirtied(Attribute *attribute){
	MainDrawRoutine::scheduleRender();
}

void DrawNode::attributeConnectionChanged(Attribute *attribute){
	MainDrawRoutine::_viewportRefreshCallback();
}

