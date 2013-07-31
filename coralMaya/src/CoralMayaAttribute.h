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

#ifndef CORALMAYAATTRIBUTE_H
#define CORALMAYAATTRIBUTE_H

#include "coralMayaDefinitions.h"
#include <string>
#include <maya/MDataBlock.h>
#include <maya/MPlug.h>
#include <maya/MGlobal.h>
#include <maya/MDataHandle.h>

/* 
 This is the class to inherit from if you need to create a coral attribute that can exchange data with maya.
 Have a look at MayaFloatAttribute.h for a usage example.  
*/
class CORAL_MAYA_EXPORT CoralMayaAttribute{
public:
	CoralMayaAttribute();
	
	virtual void transferValueToMaya(MPlug &plug, MDataBlock &data){
	}
	
	virtual void transferValueFromMaya(MPlug &plug, MDataBlock &data){
	}
	
	void setMayaAttribute(const std::string &name){
		_mayaAttribute = name;
	}
	
	/*
	 The maya attribute associated with this coral attribute.
	*/
	std::string mayaAttribute(){
		return _mayaAttribute;
	}
	
	void setMayaNode(const std::string &name){
		_mayaNode = name;
	}
	
	/*
	 The maya node (of type CoralNetworkNode) containing this coral attribute.
	*/
	std::string mayaNode(){
		return _mayaNode;
	}
	
	/*
	 You have to call this method in the onDirtied() method of the class inheriting from this.
	*/
	void dirtyMayaAttribute();
	
	void setCanDirtyMayaAttribute(bool value){
		_canDirtyMayaAttribute = value;
	}

private:
	std::string _mayaAttribute;
	std::string _mayaNode;
	bool _canDirtyMayaAttribute;
};

#endif

