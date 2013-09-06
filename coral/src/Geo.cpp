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



#include "Geo.h"
#include <assert.h>
#include "containerUtils.h"

using namespace coral;
using namespace containerUtils;

Geo::Geo():
_faceNormalsDirty(true),
_verticesNormalsDirty(true),
_topologyStructuresDirty(true),
_alignmentDataDirty(true),
_overrideVerticesNormals(false){
}

void Geo::copy(const Geo *other){
	clear();
	
	_points = other->_points;
	_rawFaces = other->_rawFaces;
	if(other->_overrideVerticesNormals){
		_verticesNormals = other->_verticesNormals;
		_overrideVerticesNormals = true;
	}
}

void Geo::setVerticesNormals(const std::vector<Imath::V3f> &normals){
	_verticesNormals = normals;
	_overrideVerticesNormals = true;
}

const std::vector<Imath::V3f> &Geo::points(){
	return _points;
}

int Geo::pointsCount() const{
	return (int)_points.size();
}

const std::vector<Imath::V2f> &Geo::rawUvs(){
	return _rawUvs;
}

const std::vector<std::vector<int> > &Geo::rawFaces(){
	return _rawFaces;
}

const std::vector<int> &Geo::rawIndices(){
	#ifdef CORAL_PARALLEL_TBB
		tbb::mutex::scoped_lock lock(_localMutex);
	#endif

	if(_alignmentDataDirty){
		cacheAlignmentData();
	}

	return _rawIndices;
}

const std::vector<int> &Geo::rawIndexCounts(){
	#ifdef CORAL_PARALLEL_TBB
		tbb::mutex::scoped_lock lock(_localMutex);
	#endif
	
	if(_alignmentDataDirty){
		cacheAlignmentData();
	}
	
	return _rawIndexCounts;
}

int Geo::facesCount() const{
	return (int)_rawFaces.size();
}

bool Geo::hasSameTopology(const std::vector<std::vector<int> > &faces) const{
	return faces == _rawFaces;
}

// assign new vertices coordinates IF arrays match
void Geo::setPoints(const std::vector<Imath::V3f> &points){
	if(_points.size() == points.size()){

		_points = points;

		_faceNormalsDirty = true;
		_verticesNormalsDirty = true;
		_overrideVerticesNormals = false;
	}
}

// Will displace the points of this geo without modifying the size of the array.
void Geo::displacePoints(const std::vector<Imath::V3f> &displacedPoints){
	int displacedPointsSize = displacedPoints.size();
	int pointsSize = _points.size();
	int minSize;
	
	if(displacedPointsSize >= pointsSize){
		minSize = pointsSize;
	}
	else{
		minSize = displacedPointsSize;
	}
	
	for(int i = 0; i < minSize; ++i){
		_points[i] = displacedPoints[i];
	}

	_faceNormalsDirty = true;
	_verticesNormalsDirty = true;
	_overrideVerticesNormals = false;
}

void Geo::clear(){
	_points.clear();
	_rawFaces.clear();
	_rawUvs.clear();
	_rawIndices.clear();
	_rawIndexCounts.clear();
	_faces.clear();
	_vertices.clear();
	
	_vertexIdOffset.clear();
	_faceNormals.clear();
	_vertexFaces.clear();

	_faceNormalsDirty = true;
	_verticesNormalsDirty = true;
	_topologyStructuresDirty = true;
	_alignmentDataDirty = true;
}

void Geo::build(const std::vector<Imath::V3f> &points, const std::vector<std::vector<int> > &faces){
	clear();
	
	_points = points;
	_rawFaces = faces;
}

void Geo::build(const std::vector<Imath::V3f> &points, const std::vector<std::vector<int> > &faces, const std::vector<Imath::V2f> &uvs){
	clear();

	_points = points;
	_rawFaces = faces;
	_rawUvs = uvs;
}

void Geo::computeVertexPerFaceNormals(std::vector<Imath::V3f> &vertexPerFaceNormals){
	int nFaces = _rawFaces.size();
	if(nFaces){
		int lastFaceID = nFaces - 1;
		std::vector<int> &lastFace = _rawFaces[lastFaceID];
	
		int size = _vertexIdOffset[lastFaceID] + lastFace.size();
		vertexPerFaceNormals.resize(size);
    
		int counter = 0;
		for(unsigned int faceID = 0; faceID < _rawFaces.size(); ++faceID){
			std::vector<int> &face = _rawFaces[faceID];
    
			int faceVerticesCount = (int)face.size();
    
			for(int i = 0; i < faceVerticesCount; i++){
				// for each triplet of points in this polygon, cross the 2 adjacent points of each point
				// es: {last,0,1}, {0,1,2}, {1,2,3}, {2,3,last}, {3,last,0}
    
				const Imath::V3f& v0 = _points[face[i == 0 ? faceVerticesCount-1 : i-1]];
				const Imath::V3f& v1 = _points[face[i]];
				const Imath::V3f& v2 = _points[face[i == faceVerticesCount-1 ? 0 : i+1]];
    
				vertexPerFaceNormals[counter].setValue((v1-v0).cross(v2-v0).normalized());
				++counter;
			}
		}
	}
}

void Geo::cacheFaceNormals(){
	if(_alignmentDataDirty){
		cacheAlignmentData();
	}

	std::vector<Imath::V3f> vertexPerFaceNormals;
	computeVertexPerFaceNormals(vertexPerFaceNormals);
	
	int facesCount = (int)_rawFaces.size();
	_faceNormals.resize(facesCount);

	int counter = 0;
	for(int f = 0; f < facesCount; ++f){
		std::vector<int> &face = _rawFaces[f];

		Imath::V3f faceNormal(0.f, 0.f, 0.f);

		for(unsigned int v = 0; v < face.size(); ++v){
			faceNormal += vertexPerFaceNormals[counter];
			++counter;
		}

		faceNormal /= face.size();
		faceNormal.normalize();

		_faceNormals[f].setValue(faceNormal);
	}

	_faceNormalsDirty = false;
}

const std::vector<Imath::V3f> &Geo::faceNormals(){
	#ifdef CORAL_PARALLEL_TBB
		tbb::mutex::scoped_lock lock(_localMutex);
	#endif
	
	if(_faceNormalsDirty){
		cacheFaceNormals();
	}
	
	return _faceNormals;
}

const std::vector<Imath::V3f> &Geo::verticesNormals(){
	if(_overrideVerticesNormals){
		return _verticesNormals;
	}
	else{
		#ifdef CORAL_PARALLEL_TBB
			tbb::mutex::scoped_lock lock(_localMutex);
		#endif
		
		if(_verticesNormalsDirty){
			if(_faceNormalsDirty){
				cacheFaceNormals();
			}
			
			int verticesCount = (int)_points.size();
			_verticesNormals.resize(verticesCount);

			for(int vertexID = 0; vertexID < verticesCount; ++vertexID){
				std::vector<int> &faces = _vertexFaces[vertexID];

				Imath::V3f vertexNormal(0.f, 0.f, 0.f);
				for(unsigned int index = 0; index < faces.size(); ++index){
					int faceID = faces[index];

					vertexNormal += _faceNormals[faceID];
				}

				vertexNormal.normalize();

				_verticesNormals[vertexID].setValue(vertexNormal);
			}

			_verticesNormalsDirty = false;
		}
	}
	return _verticesNormals;
}

void Geo::cacheAlignmentData(){
	int faceCount = _rawFaces.size();
	_vertexIdOffset.resize(faceCount);
	_rawIndexCounts.reserve(faceCount);

	int vertexCount = _points.size();
	_vertexFaces.resize(vertexCount);
	
	int vertexIdOffset = 0;

	// count the total number of index element and reserve the vector size to avoid reallocation
	int idxCount = 0;
	for(int i = 0; i < faceCount; ++i){
		std::vector<int> &rawVerticesPerFace = _rawFaces[i];
		idxCount += rawVerticesPerFace.size();	// count 4+3+4+4+4+4+4+3+4+5+etc...
	}

	// reserve a size for the array and feed it of index
	_rawIndices.reserve(idxCount);
	
	for(int i = 0; i < faceCount; ++i){
		std::vector<int> &rawVerticesPerFace = _rawFaces[i];
		
		int verticesPerFaceCount = rawVerticesPerFace.size();
		
		_vertexIdOffset[i] = vertexIdOffset;

		_rawIndexCounts.push_back(verticesPerFaceCount);	// {4,4,4,4,3,4,4,4,4,5, etc...}
		_vertexIdOffset[i] = vertexIdOffset;
		
		for(int j = 0; j < verticesPerFaceCount; ++j){
			int vertexId = rawVerticesPerFace[j];
			
			_rawIndices.push_back(vertexId);	// {0,1,2,3, 1,4,5,2 4,6,7,5, etc...}.
			_vertexFaces[vertexId].push_back(i);
			
			vertexIdOffset += verticesPerFaceCount;
		}
	}

	_alignmentDataDirty = false;
}

void Geo::cacheTopologyStructures(){
	int faceCount = _rawFaces.size();
	_faces.resize(faceCount);
	_facesPtr.resize(faceCount);

	int vertexCount = _points.size();
	_vertices.resize(vertexCount);
	_verticesPtr.resize(vertexCount);
	
	_edges.clear();
	_edgesMap.clear();
	
	for(int i = 0; i < faceCount; ++i){
		std::vector<int> &rawVerticesPerFace = _rawFaces[i];
		
		Face &face = _faces[i];
		_facesPtr[i] = &face;
		
		face._id = i;
		face._geo = this;
		
		int verticesPerFaceCount = rawVerticesPerFace.size();
		
		face._vertices.resize(verticesPerFaceCount);
		face._edges.resize(verticesPerFaceCount);
		face._points.resize(verticesPerFaceCount);
		
		for(int j = 0; j < verticesPerFaceCount; ++j){
			int vertexId = rawVerticesPerFace[j];
			
			Imath::V3f &point = _points[vertexId];
			
			face._points[j] = &point;
			
			// vertex
			Vertex &vertex = _vertices[vertexId];
			
			_verticesPtr[vertexId] = &vertex;
			vertex._id = vertexId;
			vertex._geo = this;
			vertex._point = &point;
			
			face._vertices[j] = &vertex;
			if(!containerUtils::elementInContainer<Face*>(&face, vertex._neighbourFaces)){
				vertex._neighbourFaces.push_back(&face);
			}
			
			// edge
			int edgeVertexId1 = rawVerticesPerFace[(verticesPerFaceCount + j - 1) % verticesPerFaceCount];
			int edgeVertexId2 = rawVerticesPerFace[j];
			
			if(edgeVertexId2 < edgeVertexId1){
				int tmp = edgeVertexId2;
				edgeVertexId2 = edgeVertexId1;
				edgeVertexId1 = tmp;
			}
			
			Edge &edge = _edgesMap[std::pair<int, int>(edgeVertexId1, edgeVertexId2)];
			
			face._edges[j] = &edge;
			edge._faces.push_back(&face);
			
			Vertex &vertex1 = _vertices[edgeVertexId1];
			Vertex &vertex2 = _vertices[edgeVertexId2];
		
			Imath::V3f &point1 = _points[edgeVertexId1];
			Imath::V3f &point2 = _points[edgeVertexId2];
			
			if(!edge._geo){
				_edges.push_back(&edge);
				
				edge._geo = this;
				edge._id = _edges.size() - 1;
				edge._vertices[0] = &vertex1;
				edge._vertices[1] = &vertex2;
				edge._points[0] = &point1;
				edge._points[1] = &point2;
			}
			
			// vertex neighbours
			if(!containerUtils::elementInContainer<Vertex*>(&vertex1, vertex2._neighbourVertices)){
				vertex2._neighbourVertices.push_back(&vertex1);
				vertex2._neighbourPoints.push_back(&point1);
			}
			if(!containerUtils::elementInContainer<Vertex*>(&vertex2, vertex1._neighbourVertices)){
				vertex1._neighbourVertices.push_back(&vertex2);
				vertex1._neighbourPoints.push_back(&point2);
			}
			if(!containerUtils::elementInContainer<Edge*>(&edge, vertex1._neighbourEdges)){
				vertex1._neighbourEdges.push_back(&edge);
			}
			if(!containerUtils::elementInContainer<Edge*>(&edge, vertex2._neighbourEdges)){
				vertex2._neighbourEdges.push_back(&edge);
			}
		}
	}
	
	_topologyStructuresDirty = false;
}

const std::vector<Vertex*> &Geo::vertices(){
	#ifdef CORAL_PARALLEL_TBB
		tbb::mutex::scoped_lock lock(_localMutex);
	#endif
	
	if(_topologyStructuresDirty){
		cacheTopologyStructures();
	}

	return _verticesPtr;
}

const std::vector<Edge*> &Geo::edges(){
	#ifdef CORAL_PARALLEL_TBB
		tbb::mutex::scoped_lock lock(_localMutex);
	#endif
	
	if(_topologyStructuresDirty){
		cacheTopologyStructures();
	}
	
	return _edges;
}

const std::vector<Face*> &Geo::faces(){
	#ifdef CORAL_PARALLEL_TBB
		tbb::mutex::scoped_lock lock(_localMutex);
	#endif
	
	if(_topologyStructuresDirty){
		cacheTopologyStructures();
	}
	
	return _facesPtr;
}
