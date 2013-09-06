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
#include "GeoSphere.h"
#include "../src/Geo.h"

using namespace coral;

GeoSphere::GeoSphere(const std::string &name, Node *parent): Node(name, parent){	
	_radius = new NumericAttribute("radius", this);
	_rings = new NumericAttribute("rings", this);
	_sectors = new NumericAttribute("sectors", this);
	_out = new GeoAttribute("out", this);
	
	addInputAttribute(_radius);
	addInputAttribute(_rings);
	addInputAttribute(_sectors);
	addOutputAttribute(_out);
	
	setAttributeAffect(_radius, _out);
	setAttributeAffect(_rings, _out);
	setAttributeAffect(_sectors, _out);
	
	setAttributeAllowedSpecialization(_radius, "Float");
	setAttributeAllowedSpecialization(_rings, "Int");
	setAttributeAllowedSpecialization(_sectors, "Int");
	
	_radius->outValue()->setFloatValueAt(0, 10.0);
	_rings->outValue()->setIntValueAt(0, 10);
	_sectors->outValue()->setIntValueAt(0, 10);
}

void GeoSphere::updateSlice(Attribute *attribute, unsigned int slice){
	float radius = _radius->value()->floatValueAt(0);
	int rings = _rings->value()->intValueAt(0);
	int sectors = _sectors->value()->intValueAt(0);

	if(rings < 2){
		rings = 2;
		_rings->outValue()->setIntValueAt(0, 2);
	}
	if(sectors < 3){
		sectors = 3;
		_sectors->outValue()->setIntValueAt(0, 3);
	}

	int totalFaces = (rings - 2) * sectors;
	
	float ringStep = 1.0 / rings;
	float sectorStep = 1.0 / sectors;
	
	int i = 0;
	std::vector<Imath::V3f> points;
	
	points.push_back(Imath::V3f(0.0, -radius, 0.0));

	for(int ring = 1; ring < rings; ring++){
		for(int sector = 0; sector < sectors; sector++){
			float y = sin( -M_PI_2 + M_PI * (float(ring)) * ringStep) * radius;
			float x = cos(2*M_PI * (float(sector)) * sectorStep) * sin( M_PI * (float(ring)) * ringStep ) * radius;
			float z = sin(2*M_PI * (float(sector)) * sectorStep) * sin( M_PI * (float(ring)) * ringStep ) * radius;
			
			points.push_back(Imath::V3f(x, y, z));
			i++;
		}
	}

	points.push_back(Imath::V3f(0.0, radius, 0.0));

	std::vector<std::vector<int> > faces;
	std::vector<int> faceVertices(3);
	
	for(int i = 0; i < sectors; ++i){
		faceVertices[0] = 0;
		faceVertices[1] = i + 1;
		faceVertices[2] = i + 2;

		if(i == sectors - 1){
			faceVertices[2] = 1;
		}

		faces.push_back(faceVertices);
	}

	faceVertices.resize(4);
	for(int i = 1; i < rings - 1; ++i){
		for(int j = 0; j < sectors; ++j){
			faceVertices[0] = ((sectors * i) - sectors) + j + 1;
			faceVertices[1] = (sectors * i) + j + 1;
			faceVertices[2] = (sectors * i) + j + 2;
			faceVertices[3] = ((sectors * i) - sectors) + j + 2;

			if(j == sectors - 1){
				faceVertices[2] -= sectors;
				faceVertices[3] -= sectors;
			}

			faces.push_back(faceVertices);
		}
	}

	int lastPoint = points.size() - 1;

	faceVertices.resize(3);
	for(int i = 0; i < sectors; ++i){
		faceVertices[0] = lastPoint;
		faceVertices[2] = ((sectors * (rings - 1)) - sectors) + i + 1;
		faceVertices[1] = ((sectors * (rings - 1)) - sectors) + i + 2;

		if(i == sectors - 1){
			faceVertices[1] -= sectors;
		}

		faces.push_back(faceVertices);
	}

	std::vector<Imath::V2f> uvs;
	
	for(int ring = 0; ring < rings; ring++){
		for(int sector = 0; sector < sectors; sector++){
			uvs.push_back(Imath::V2f(ringStep * ring, sectorStep * sector));
		}
	}
	
	_out->outValue()->build(points, faces, uvs);
}
