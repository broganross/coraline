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
#include "MayaMatrixAttribute.h"
#include <maya/MMatrix.h>

namespace {

MMatrix imathToMayaMatrix(const Imath::M44f &imathMatrix){
	MMatrix mayaMatrix(imathMatrix.x);
	
	return mayaMatrix;
}

Imath::M44f mayaToImathMatrix(const MMatrix &mayaMatrix){
	Imath::M44f imathMatrix(
		mayaMatrix(0, 0), mayaMatrix(0, 1), mayaMatrix(0, 2), 0.0,
		mayaMatrix(1, 0), mayaMatrix(1, 1), mayaMatrix(1, 2), 0.0,
		mayaMatrix(2, 0), mayaMatrix(2, 1), mayaMatrix(2, 2), 0.0,
		mayaMatrix(3, 0), mayaMatrix(3, 1), mayaMatrix(3, 2), 1.0);

	return imathMatrix;
}

}

void MayaMatrixAttribute::transferValueToMaya(MPlug &plug, MDataBlock &data){
	MDataHandle dataHandle = data.outputValue(plug);
	dataHandle.setMMatrix(imathToMayaMatrix(value()->matrix44ValueAt(0)));
}

void MayaMatrixAttribute::transferValueFromMaya(MPlug &plug, MDataBlock &data){
	MDataHandle dataHandle = data.inputValue(plug);
	outValue()->setMatrix44ValueAt(0, mayaToImathMatrix(dataHandle.asMatrix()));
	valueChanged();
}

void MayaMatrixArrayAttribute::transferValueToMaya(MPlug &plug, MDataBlock &data){
	MArrayDataHandle arrayHandle = data.outputArrayValue(plug);
	const std::vector<Imath::M44f> &values = value()->matrix44Values();
	int minCount = arrayHandle.elementCount();
	if(values.size() < minCount){
		minCount = values.size();
	}
	
	for(int i = 0; i < minCount; ++i){
		arrayHandle.jumpToElement(i);
		MDataHandle valuesHnd = arrayHandle.outputValue();
		valuesHnd.setMMatrix(imathToMayaMatrix(values[i]));
	}
	
	arrayHandle.setAllClean();
}

void MayaMatrixArrayAttribute::transferValueFromMaya(MPlug &plug, MDataBlock &data){
	MArrayDataHandle arrayHandle = data.outputArrayValue(plug);
	
	int elements = arrayHandle.elementCount();
	std::vector<Imath::M44f> values(elements);
	
	for(int i = 0; i < elements; ++i){
		arrayHandle.jumpToElement(i);
		MDataHandle valuesHnd = arrayHandle.inputValue();
		values[i] = mayaToImathMatrix(valuesHnd.asMatrix());
	}
	
	outValue()->setMatrix44Values(values);
	valueChanged();
}
