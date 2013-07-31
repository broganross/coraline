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

#ifndef MAYAINTATTRIBUTE_H
#define MAYAINTATTRIBUTE_H

#include <coral/src/Node.h>
#include <coral/src/NumericAttribute.h>
#include <coral/src/Numeric.h>

#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MString.h>
#include <maya/MDataHandle.h>
#include <maya/MDataBlock.h>

#include "CoralMayaAttribute.h"

class CORAL_MAYA_EXPORT MayaIntAttribute: public coral::NumericAttribute, public CoralMayaAttribute{
public:
	MayaIntAttribute(const std::string &name, coral::Node *parent): 
		coral::NumericAttribute(name, parent),
		CoralMayaAttribute(){
		setClassName("MayaIntAttribute");
	}
	
	virtual void onDirtied(){
		CoralMayaAttribute::dirtyMayaAttribute();
	}
	
	virtual void transferValueToMaya(MPlug &plug, MDataBlock &data){
		MDataHandle dataHandle = data.outputValue(plug);
		dataHandle.setInt(value()->intValueAt(0));
	}
	
	virtual void transferValueFromMaya(MPlug &plug, MDataBlock &data){
		MDataHandle dataHandle = data.inputValue(plug);
		outValue()->setIntValueAt(0, dataHandle.asInt());
		valueChanged();
	}
};


#endif
