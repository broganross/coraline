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
#include "MayaGeoAttribute.h"

void MayaGeoAttribute::transferValueToMaya(MPlug &plug, MDataBlock &data){
	coral::Geo *coralGeo = value();
	const std::vector<Imath::V3f> &coralPoints = coralGeo->points();
	
	// transfer points
	MFloatPointArray mayaPoints;
	for(int i = 0; i < coralPoints.size(); ++i){
		const Imath::V3f *coralPoint = &coralPoints[i];
		mayaPoints.append(MFloatPoint(coralPoint->x, coralPoint->y, coralPoint->z));
	}
	
	// transfer faces
	MIntArray mayaFaceCount;
	MIntArray mayaFaceVertices;
	
	const std::vector<std::vector<int> > coralFaces = coralGeo->rawFaces();
	for(int polyId = 0; polyId < coralFaces.size(); ++polyId){
		const std::vector<int> *coralFace = &coralFaces[polyId];
		int faceVertexCount = coralFace->size();
		mayaFaceCount.append(faceVertexCount);
		
		for(int i = 0; i < faceVertexCount; ++i){
			mayaFaceVertices.append(coralFace->at(i));
		}
	}
	
	// create maya mesh
	MDataHandle dataHandle = data.outputValue(plug);
	
	MFnMeshData dataCreator;
	MObject newOutputData = dataCreator.create();
	
	MFnMesh newMesh;
	newMesh.create(mayaPoints.length(), coralFaces.size(), mayaPoints, mayaFaceCount, mayaFaceVertices, newOutputData);
	dataHandle.set(newOutputData);
}

void MayaGeoAttribute::transferValueFromMaya(MPlug &plug, MDataBlock &data){
	MDataHandle dataHandle = data.inputValue(plug);
	MFnMesh meshFn(dataHandle.asMesh());
	
	MFloatPointArray mayaPoints;
	meshFn.getPoints(mayaPoints);
	
	// collect points
	std::vector<Imath::V3f> coralPoints;
	for(int i = 0; i < mayaPoints.length(); ++i){
		MFloatPoint* mayaPoint = &mayaPoints[i];
		coralPoints.push_back(Imath::V3f(mayaPoint->x, mayaPoint->y, mayaPoint->z));
	}
	
	// collect faces
	int numPolys =  meshFn.numPolygons();
	std::vector<std::vector<int> > coralFaces(numPolys);
	for(int polyId = 0; polyId < numPolys; ++polyId){
		MIntArray mayaVertexList;
		meshFn.getPolygonVertices(polyId, mayaVertexList);
		
		int polyPoints = mayaVertexList.length();
		std::vector<int> coralFace(polyPoints);
		for(int i = 0; i < polyPoints; ++i){
			int pointId = mayaVertexList[i];
			coralFace[i] = pointId;
		}
		
		coralFaces[polyId] = coralFace;
	}
	
	// create coral geo
	coral::Geo *coralGeo = outValue();
	
	if(coralGeo->hasSameTopology(coralFaces)){
		coralGeo->setPoints(coralPoints);
	}
	else{
		coralGeo->build(coralPoints, coralFaces);
	}
	
	valueChanged();
}

void MayaGeoAttribute::onDirtied(){
	CoralMayaAttribute::dirtyMayaAttribute();
}
