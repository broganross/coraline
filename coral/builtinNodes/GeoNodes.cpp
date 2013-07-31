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

#include "GeoNodes.h"
#include "../src/Numeric.h"
#include "../src/containerUtils.h"

using namespace coral;

void GetGeoElements::contextChanged(Node *parentNode, Enum *enum_){
	GetGeoElements* self = (GetGeoElements*)parentNode;
	int id = enum_->currentIndex();
	if(id == 0){
		self->_contextualUpdate = &GetGeoElements::updateVertices;
	}
	else if(id == 1){
		self->_contextualUpdate = &GetGeoElements::updateEdges;
	}
	else if(id == 2){
		self->_contextualUpdate = &GetGeoElements::updateFaces;
	}
}

GetGeoElements::GetGeoElements(const std::string &name, Node *parent): 
Node(name, parent),
_contextualUpdate(0){
	_context = new EnumAttribute("context", this);
	_geo = new GeoAttribute("geo", this);
	_elements = new NumericAttribute("elements", this);
	
	addInputAttribute(_context);
	addInputAttribute(_geo);
	addOutputAttribute(_elements);
	
	setAttributeAffect(_context, _elements);
	setAttributeAffect(_geo, _elements);
	
	setAttributeAllowedSpecialization(_elements, "IntArray");
	
	Enum *context = _context->outValue();
	context->addEntry(0, "vertices");
	context->addEntry(1, "edges");
	context->addEntry(2, "faces");
	
	context->setCurrentIndexChangedCallback(this, GetGeoElements::contextChanged);
	context->setCurrentIndex(0);
}

void GetGeoElements::updateVertices(Geo *geo, std::vector<int> &elements){
	const std::vector<Vertex*> &vertices = geo->vertices();
	int size = vertices.size();
	elements.resize(size);
	for(int i = 0; i < size; ++i){
		elements[i] = vertices[i]->id();
	}
}

void GetGeoElements::updateEdges(Geo *geo, std::vector<int> &elements){
	const std::vector<Edge*> &edges = geo->edges();
	int size = edges.size();
	elements.resize(size);
	
	for(int i = 0; i < size; ++i){
		elements[i] = edges[i]->id();
	}
}

void GetGeoElements::updateFaces(Geo *geo, std::vector<int> &elements){
	const std::vector<Face*> &faces = geo->faces();
	int size = faces.size();
	elements.resize(size);
	
	for(int i = 0; i < size; ++i){
		elements[i] = faces[i]->id();
	}
}

void GetGeoElements::updateSlice(Attribute *attribute, unsigned int slice){
	if(_contextualUpdate){
		Geo *geo = _geo->value();
		std::vector<int> elements;
		(this->*_contextualUpdate)(geo, elements);
		
		_elements->outValue()->setIntValuesSlice(slice, elements);
	}
}

void GetGeoSubElements::contextChanged(Node *parentNode, Enum *enum_){
	GetGeoSubElements* self = (GetGeoSubElements*)parentNode;
	int id = enum_->currentIndex();
	if(id == 0){
		self->_contextualUpdate = &GetGeoSubElements::updateVertexNeighbours;
	}
	else if(id == 1){
		self->_contextualUpdate = &GetGeoSubElements::updateEdgeVertices;
	}
	else if(id == 2){
		self->_contextualUpdate = &GetGeoSubElements::updateFaceVertices;
	}
}

GetGeoSubElements::GetGeoSubElements(const std::string &name, Node *parent): 
Node(name, parent),
_contextualUpdate(0){
	_context = new EnumAttribute("context", this);
	_geo = new GeoAttribute("geo", this);
	_index = new NumericAttribute("index", this);
	_subElements = new NumericAttribute("subElements", this);
	
	addInputAttribute(_context);
	addInputAttribute(_geo);
	addInputAttribute(_index);
	addOutputAttribute(_subElements);

	setAttributeAffect(_context, _subElements);
	setAttributeAffect(_geo, _subElements);
	setAttributeAffect(_index, _subElements);
	
	std::vector<std::string> indexSpecs;
	indexSpecs.push_back("Int");
	indexSpecs.push_back("IntArray");
	setAttributeAllowedSpecializations(_index, indexSpecs);
	setAttributeAllowedSpecialization(_subElements, "IntArray");
	
	Enum *context = _context->outValue();
	context->addEntry(0, "vertex neighbours");
	context->addEntry(1, "edge vertices");
	context->addEntry(2, "face vertices");

	context->setCurrentIndexChangedCallback(this, GetGeoSubElements::contextChanged);
	context->setCurrentIndex(0);
}

void GetGeoSubElements::updateVertexNeighbours(Geo *geo, const std::vector<int> &index, std::vector<int> &subElements){
	const std::vector<Vertex*> &vertices = geo->vertices();
	int verticesSize = vertices.size();

	for(int i = 0; i < index.size(); ++i){
		int vertexId = index[i];

		if(vertexId >= 0 && vertexId < verticesSize){
			const std::vector<Vertex*> &neighbours = vertices[vertexId]->neighbourVertices();
			for(int j = 0; j < neighbours.size(); ++j){
				subElements.push_back(neighbours[j]->id());
			}
		}
	}
}

void GetGeoSubElements::updateEdgeVertices(Geo *geo, const std::vector<int> &index, std::vector<int> &subElements){
	const std::vector<Edge*> &edges = geo->edges();
	int edgesSize = edges.size();

	for(int i = 0; i < index.size(); ++i){
		int edgeId = index[i];

		if(edgeId >= 0 && edgeId < edgesSize){
			const std::vector<Vertex*> &vertices = edges[edgeId]->vertices();
			subElements.push_back(vertices[0]->id());
			subElements.push_back(vertices[1]->id());
		}
	}
}

void GetGeoSubElements::updateFaceVertices(Geo *geo, const std::vector<int> &index, std::vector<int> &subElements){
	const std::vector<Face*> &faces = geo->faces();
	int facesSize = faces.size();

	for(int i = 0; i < index.size(); ++i){
		int faceId = index[i];

		if(faceId >= 0 && faceId < facesSize){
			const std::vector<Vertex*> &vertices = faces[faceId]->vertices();
			for(int j = 0; j < vertices.size(); ++j){
				subElements.push_back(vertices[j]->id());
			}
		}
	}
}

void GetGeoSubElements::updateSlice(Attribute *attribute, unsigned int slice){
	if(_contextualUpdate){
		Geo *geo = _geo->value();
		const std::vector<int> &index = _index->value()->intValuesSlice(slice);

		std::vector<int> subElements;
		(this->*_contextualUpdate)(geo, index, subElements);
		
		_subElements->outValue()->setIntValuesSlice(slice, subElements);
	}
}


GetGeoPoints::GetGeoPoints(const std::string &name, Node *parent): Node(name, parent){
	_geo = new GeoAttribute("geo", this);
	_points = new NumericAttribute("points", this);
	
	addInputAttribute(_geo);
	addOutputAttribute(_points);
	
	setAttributeAffect(_geo, _points);
	
	setAttributeAllowedSpecialization(_points, "Vec3Array");
}

void GetGeoPoints::updateSlice(Attribute *attribute, unsigned int slice){
	_points->outValue()->setVec3ValuesSlice(slice, _geo->value()->points());
}

SetGeoPoints::SetGeoPoints(const std::string &name, Node *parent): Node(name, parent){		
	_inGeo = new GeoAttribute("inGeo", this);
	_points = new NumericAttribute("points", this);
	_outGeo = new GeoAttribute("outGeo", this);
	
	addInputAttribute(_inGeo);
	addInputAttribute(_points);
	addOutputAttribute(_outGeo);
	
	setAttributeAffect(_inGeo, _outGeo);
	setAttributeAffect(_points, _outGeo);
	
	setAttributeAllowedSpecialization(_points, "Vec3Array");
}

void SetGeoPoints::updateSlice(Attribute *attribute, unsigned int slice){
	Geo *outGeoValue = _outGeo->outValue();
	
	outGeoValue->copy(_inGeo->value());
	outGeoValue->displacePoints(_points->value()->vec3ValuesSlice(slice));
}

GetGeoNormals::GetGeoNormals(const std::string &name, Node *parent): Node(name, parent){
	_geo = new GeoAttribute("geo", this);
	_normals = new NumericAttribute("normals", this);
	
	addInputAttribute(_geo);
	addOutputAttribute(_normals);
	
	setAttributeAffect(_geo, _normals);
	
	setAttributeAllowedSpecialization(_normals, "Vec3Array");
}

void GetGeoNormals::updateSlice(Attribute *attribute, unsigned int slice){
	_normals->outValue()->setVec3ValuesSlice(slice, _geo->value()->verticesNormals());
}


GeoNeighbourPoints::GeoNeighbourPoints(const std::string &name, Node *parent): Node(name, parent){
	_geo = new GeoAttribute("geo", this);	
	_vertex = new NumericAttribute("vertex", this);
	_neighbourPoints = new NumericAttribute("points", this);
	_neighbourVertices = new NumericAttribute("vertices", this);
	
	addInputAttribute(_geo);
	addInputAttribute(_vertex);
	addOutputAttribute(_neighbourPoints);
	addOutputAttribute(_neighbourVertices);
	
	setAttributeAffect(_geo, _neighbourPoints);
	setAttributeAffect(_vertex, _neighbourPoints);
	setAttributeAffect(_geo, _neighbourVertices);
	setAttributeAffect(_vertex, _neighbourVertices);

	setAttributeAllowedSpecialization(_vertex, "Int");
	setAttributeAllowedSpecialization(_neighbourPoints, "Vec3Array");
	setAttributeAllowedSpecialization(_neighbourVertices, "IntArray");
}

void GeoNeighbourPoints::updateSlice(Attribute *attribute, unsigned int slice){
	Geo *geo = _geo->value();
	int vertexId = _vertex->value()->intValueAtSlice(slice, 0);

	const std::vector<Vertex*> &vertices = geo->vertices();
	
	if(vertexId < 0 || vertexId >= vertices.size()){
		if(attribute == _neighbourPoints){
			_neighbourPoints->outValue()->setVec3ValuesSlice(slice, std::vector<Imath::V3f>());
		}
		else{
			_neighbourVertices->outValue()->setIntValuesSlice(slice, std::vector<int>());
		}
	}
	else{
		if(attribute == _neighbourPoints){
			_neighbourPoints->outValue()->setVec3ValuesSlice(slice, vertices[vertexId]->neighbourPoints());
		}
		else{
			const std::vector<Vertex*> &neighbourVertices = vertices[vertexId]->neighbourVertices();
			int neighboursSize = neighbourVertices.size();
			
			std::vector<int> neighbourIds(neighboursSize);
			for(int i = 0; i < neighboursSize; ++i){
				neighbourIds[i] = neighbourVertices[i]->id();
			}
			_neighbourVertices->outValue()->setIntValuesSlice(slice, neighbourIds);
		}
	}	
}

