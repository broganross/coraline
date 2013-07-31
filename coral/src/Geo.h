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

#ifndef GEO_H
#define GEO_H

#ifdef CORAL_PARALLEL_TBB
	#include <tbb/mutex.h>
#endif

#include <map>
#include <vector>
#include <ImathVec.h>

#include "Value.h"

namespace coral{
class Geo;
class Face;
class Edge;
class Vertex;

class Face{
public:
	Face(): _id(0), _geo(0){
	}

	int id(){
		return _id;
	}
	
	Geo *geo(){
		return _geo;
	}
	
	const std::vector<Edge*> &edges(){
		return _edges;
	}
	
	const std::vector<Vertex*> &vertices(){
		return _vertices;
	}
	
	std::vector<Imath::V3f> points(){
		int size = _points.size();
		std::vector<Imath::V3f> points(size);
		for(int i = 0; i < size; ++i){
			points[i] = *_points[i];
		}
		
		return points;
	}
	
private:
	friend class Geo;
	
	int _id;
	Geo *_geo;
	std::vector<Edge*> _edges;
	std::vector<Vertex*> _vertices;
	std::vector<Imath::V3f*> _points;
};

class Edge{
public:
	Edge(): _id(0), _geo(0), _vertices(2), _points(2){
	}

	int id(){
		return _id;
	}
	
	const std::vector<Face*> &rawFaces() const{
		return _faces;
	}
	
	const std::vector<Vertex*> &vertices() const{
		return _vertices;
	}
	
	std::vector<Imath::V3f> points(){
		int size = _points.size();
		std::vector<Imath::V3f> points(size);
		for(int i = 0; i < size; ++i){
			points[i] = *_points[i];
		}
		
		return points;
	}

private:
	friend class Geo;
	
	int _id;
	Geo *_geo;
	std::vector<Face*> _faces;
	std::vector<Vertex*> _vertices;
	std::vector<Imath::V3f*> _points;
};

class Vertex{
public:
	Vertex(): _id(0), _geo(0), _point(0){
	}
	
	int id(){
		return _id;
	}
	
	Imath::V3f point(){
		return *_point;
	}
	
	const std::vector<Face*> &neighbourFaces() const{
		return _neighbourFaces;
	}
	
	const std::vector<Edge*> &neighbourEdges() const{
		return _neighbourEdges;
	}
	
	const std::vector<Vertex*> &neighbourVertices() const{
		return _neighbourVertices;
	}
	
	std::vector<Imath::V3f> neighbourPoints() const{
		int size = _neighbourPoints.size();
		std::vector<Imath::V3f> points(size);
		for(int i = 0; i < size; ++i){
			points[i] = *_neighbourPoints[i];
		}
		
		return points;
	}

private:
	friend class Geo;

	int _id;
	Geo *_geo;
	Imath::V3f* _point;
	std::vector<Face*> _neighbourFaces;
	std::vector<Edge*> _neighbourEdges;
	std::vector<Vertex*> _neighbourVertices;
	std::vector<Imath::V3f*> _neighbourPoints;
};

//! A class to handle Geometry, used by GeoAttribute. 
class CORAL_EXPORT Geo: public Value{ 
public:
	Geo();
	
	void copy(const Geo *other);
	void build(const std::vector<Imath::V3f> &points, const std::vector<std::vector<int> > &faces);
	void build(const std::vector<Imath::V3f> &points, const std::vector<std::vector<int> > &faces, const std::vector<Imath::V2f> &uvs);
	const std::vector<Imath::V3f> &points();
	int pointsCount() const;
	const std::vector<Imath::V2f> &rawUvs();
	const std::vector<std::vector<int> > &rawFaces();

	/*! Return a pointer to an array of packaged indices: {0,1,2,3, 1,4,5,2, 4,6,7,5, etc...}.
	 * Should be used with rawIndexCounts()
	 * \return A pointer to an array of indices
	 */
	const std::vector<int> &rawIndices();

	/*! Return a pointer to an array of vertex counts for each polygon: {4,4,4,4,3,4,4,4,5,4,4, etc...}.
	 * Should be used with rawIndices().
	 * \return A pointer to an array of vertex counts for each polygon
	 */
	const std::vector<int> &rawIndexCounts();
	int facesCount() const;
	const std::vector<Imath::V3f> &faceNormals();
	const std::vector<Imath::V3f> &verticesNormals();
	void setVerticesNormals(const std::vector<Imath::V3f> &normals);
	void setPoints(const std::vector<Imath::V3f> &points);
	void displacePoints(const std::vector<Imath::V3f> &displacedPoints);
	bool hasSameTopology(const std::vector<std::vector<int> > &faces) const;
	void clear();
	const std::vector<Vertex*> &vertices();
	const std::vector<Edge*> &edges();
	const std::vector<Face*> &faces();

private:
	void computeVertexPerFaceNormals(std::vector<Imath::V3f> &vertexPerFaceNormals);
	void cacheTopologyStructures();
	void cacheFaceNormals();
	void cacheAlignmentData();

	bool _topologyStructuresDirty;
	bool _faceNormalsDirty;
	bool _verticesNormalsDirty;
	bool _alignmentDataDirty;
	bool _overrideVerticesNormals;

	std::vector<std::vector<int> > _rawFaces;
	std::vector<Face> _faces;
	std::vector<Face*> _facesPtr;
	std::vector<Vertex> _vertices;
	std::vector<Vertex*> _verticesPtr;
	std::vector<Edge*> _edges;
	std::map<std::pair<int, int>, Edge> _edgesMap;
	
	std::vector<Imath::V3f> _points;
	std::vector<Imath::V3f> _faceNormals;
	std::vector<Imath::V3f> _verticesNormals;
	std::vector<Imath::V2f> _rawUvs;
	
	// alignement data
	std::vector<int> _rawIndices;
	std::vector<int> _rawIndexCounts;
	std::vector<int> _vertexIdOffset;
	std::vector<std::vector<int> > _vertexFaces;
	
	#ifdef CORAL_PARALLEL_TBB
		tbb::mutex _localMutex;
	#endif
};

}

#endif
