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

#ifndef CORAL_MAYAANGLEATTRIBUTE_H
#define CORAL_MAYAANGLEATTRIBUTE_H

#include <coral/src/Node.h>
#include <coral/src/NumericAttribute.h>
#include <coral/src/Numeric.h>

#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MString.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MDataBlock.h>
#include <maya/MAngle.h>

#include "CoralMayaAttribute.h"

class CORAL_MAYA_EXPORT MayaAngleAttribute: public coral::NumericAttribute, public CoralMayaAttribute{
public:
	MayaAngleAttribute(const std::string &name, coral::Node *parent): coral::NumericAttribute(name, parent),CoralMayaAttribute(){
		setClassName("MayaAngleAttribute");
		std::vector<std::string> specialization;
		specialization.push_back("Float");
		setAllowedSpecialization(specialization);
	}
	
	virtual void onDirtied(){
		CoralMayaAttribute::dirtyMayaAttribute();
	}
	
	virtual void transferValueToMaya(MPlug &plug, MDataBlock &data){
		MDataHandle dataHandle = data.outputValue(plug);
		dataHandle.setMAngle(MAngle(value()->floatValueAt(0), MAngle::kDegrees));
	}
	
	virtual void transferValueFromMaya(MPlug &plug, MDataBlock &data){
		MDataHandle dataHandle = data.inputValue(plug);
		outValue()->setFloatValueAt(0, dataHandle.asAngle().asDegrees());
		valueChanged();
	}
};

class CORAL_MAYA_EXPORT MayaAngle3ArrayAttribute: public coral::NumericAttribute, public CoralMayaAttribute{
public:
	MayaAngle3ArrayAttribute(const std::string &name, coral::Node *parent): coral::NumericAttribute(name, parent),CoralMayaAttribute(){
		setClassName("MayaAngle3ArrayAttribute");
		std::vector<std::string> specialization;
		specialization.push_back("Vec3Array");
		setAllowedSpecialization(specialization);
	}
	
	virtual void onDirtied(){
		CoralMayaAttribute::dirtyMayaAttribute();
	}
	
	virtual void transferValueToMaya(MPlug &plug, MDataBlock &data){
		MArrayDataHandle arrayHandle = data.outputArrayValue(plug);
		const std::vector<Imath::V3f> &values = value()->vec3Values();
		int minCount = arrayHandle.elementCount();
		if(values.size() < minCount){
			minCount = values.size();
		}
		
		for(int i = 0; i < minCount; ++i){
			arrayHandle.jumpToElement(i);
			MDataHandle valuesHnd = arrayHandle.outputValue();
			MPlug currentPlugElement = plug.elementByPhysicalIndex(i);
			if(currentPlugElement.numChildren() == 3){
				MDataHandle valueXHandle = valuesHnd.child(currentPlugElement.child(0));
				MDataHandle valueYHandle = valuesHnd.child(currentPlugElement.child(1));
				MDataHandle valueZHandle = valuesHnd.child(currentPlugElement.child(2));
			
				valueXHandle.setMAngle(MAngle(values[i].x, MAngle::kDegrees));
				valueYHandle.setMAngle(MAngle(values[i].y, MAngle::kDegrees));
				valueZHandle.setMAngle(MAngle(values[i].z, MAngle::kDegrees));
			}
			else{ // plugs not yet ready, wait for next cleanup
				break;
			}
		}
		
		arrayHandle.setAllClean();
	}
	
	virtual void transferValueFromMaya(MPlug &plug, MDataBlock &data){
		MArrayDataHandle arrayHandle = data.outputArrayValue(plug);
		
		int elements = arrayHandle.elementCount();
		std::vector<Imath::V3f> values(elements);
		
		for(int i = 0; i < elements; ++i){
			arrayHandle.jumpToElement(i);
			MDataHandle valuesHnd = arrayHandle.inputValue();
			MPlug currentPlugElement = plug.elementByPhysicalIndex(i);
			if(currentPlugElement.numChildren() == 3){
				MDataHandle valueXHandle = valuesHnd.child(currentPlugElement.child(0));
				MDataHandle valueYHandle = valuesHnd.child(currentPlugElement.child(1));
				MDataHandle valueZHandle = valuesHnd.child(currentPlugElement.child(2));
			
				values[i].x = valueXHandle.asAngle().asDegrees();
				values[i].y = valueYHandle.asAngle().asDegrees();
				values[i].z = valueZHandle.asAngle().asDegrees();
			}
			else{
				break;
			}
		}
		
		outValue()->setVec3Values(values);
		valueChanged();
	}
};

#endif
