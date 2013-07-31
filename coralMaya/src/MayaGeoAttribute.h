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

#ifndef MAYAGEOATTRIBUTE_H
#define MAYAGEOATTRIBUTE_H

#include <vector>

#include <maya/MGlobal.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFnMesh.h>
#include <maya/MFnMeshData.h>
#include <maya/MDataHandle.h>
#include <maya/MDataBlock.h>

#include <ImathVec.h>

#include "coral/src/Node.h"
#include "coral/src/Numeric.h"
#include "coral/src/GeoAttribute.h"

#include "CoralMayaAttribute.h"

class CORAL_MAYA_EXPORT MayaGeoAttribute: public coral::GeoAttribute, public CoralMayaAttribute{
public:
	MayaGeoAttribute(const std::string &name, coral::Node *parent): 
		coral::GeoAttribute(name, parent),
		CoralMayaAttribute(){
		setClassName("MayaGeoAttribute");
	}
	
	virtual void onDirtied();
	virtual void transferValueToMaya(MPlug &plug, MDataBlock &data);	
	virtual void transferValueFromMaya(MPlug &plug, MDataBlock &data);
};

#endif
