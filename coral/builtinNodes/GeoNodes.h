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

#ifndef CORAL_GEONODES_H
#define CORAL_GEONODES_H

#include <ImathVec.h>

#include "../src/Node.h"
#include "../src/GeoAttribute.h"
#include "../src/Numeric.h"
#include "../src/NumericAttribute.h"
#include "../src/EnumAttribute.h"

namespace coral
{

class GetGeoElements: public Node{
public:
	GetGeoElements(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);
	
private:
	EnumAttribute *_context;
	GeoAttribute *_geo;
	NumericAttribute *_elements;
	
	std::string _currentContext;
	std::vector<std::string> _contexts;
	
	void(GetGeoElements::*_contextualUpdate)(Geo *, std::vector<int>&);
	
	void updateVertices(Geo *geo, std::vector<int> &elements);
	void updateEdges(Geo *geo, std::vector<int> &elements);
	void updateFaces(Geo *geo, std::vector<int> &elements);
	static void contextChanged(Node *parentNode, Enum *enum_);
};

class GetGeoSubElements: public Node{
public:
	GetGeoSubElements(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	EnumAttribute *_context;
	GeoAttribute *_geo;
	NumericAttribute *_index;
	NumericAttribute *_subElements;

	void(GetGeoSubElements::*_contextualUpdate)(Geo *, const std::vector<int> &, std::vector<int>&);

	void updateVertexNeighbours(Geo *geo, const std::vector<int> &index, std::vector<int> &subElements);
	void updateEdgeVertices(Geo *geo, const std::vector<int> &index, std::vector<int> &subElements);
	void updateFaceVertices(Geo *geo, const std::vector<int> &index, std::vector<int> &subElements);

	static void contextChanged(Node *parentNode, Enum *enum_);
};

class GetGeoPoints: public Node{
public:
	GetGeoPoints(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);
	
private:
	GeoAttribute *_geo;
	NumericAttribute *_points;
};

class SetGeoPoints: public Node{
public:
	SetGeoPoints(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	GeoAttribute *_inGeo;
	GeoAttribute *_outGeo;
	NumericAttribute *_points;
};

class GetGeoNormals: public Node{
public:
	GetGeoNormals(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);
	
private:
	GeoAttribute *_geo;
	NumericAttribute *_normals;
};

class GeoNeighbourPoints: public Node{
public:
	GeoNeighbourPoints(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);
	
private:
	GeoAttribute *_geo;
	NumericAttribute *_vertex;
	NumericAttribute *_neighbourPoints;
	NumericAttribute *_neighbourVertices;
};

}

#endif
