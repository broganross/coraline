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
#include "GeoCube.h"
#include "../src/Geo.h"

using namespace coral;

GeoCube::GeoCube(const std::string &name, Node *parent): Node(name, parent){
	_width = new NumericAttribute("width", this);
	_height = new NumericAttribute("height", this);
	_depth = new NumericAttribute("depth", this);
	_widthSubdivisions = new NumericAttribute("widthSubdivisions", this);
	_heightSubdivisions = new NumericAttribute("heightSubdivisions", this);
	_depthSubdivisions = new NumericAttribute("depthSubdivisions", this);
	_out = new GeoAttribute("out", this);
	
	addInputAttribute(_width);
	addInputAttribute(_height);
	addInputAttribute(_depth);
	addInputAttribute(_widthSubdivisions);
	addInputAttribute(_heightSubdivisions);
	addInputAttribute(_depthSubdivisions);
	addOutputAttribute(_out);
	
	setAttributeAffect(_width, _out);
	setAttributeAffect(_height, _out);
	setAttributeAffect(_depth, _out);
	setAttributeAffect(_widthSubdivisions, _out);
	setAttributeAffect(_heightSubdivisions, _out);
	setAttributeAffect(_depthSubdivisions, _out);
	
	setAttributeAllowedSpecialization(_width, "Float");
	setAttributeAllowedSpecialization(_height, "Float");
	setAttributeAllowedSpecialization(_depth, "Float");
	setAttributeAllowedSpecialization(_widthSubdivisions, "Int");
	setAttributeAllowedSpecialization(_heightSubdivisions, "Int");
	setAttributeAllowedSpecialization(_depthSubdivisions, "Int");
	
	_width->outValue()->setFloatValueAt(0, 15.0);
	_height->outValue()->setFloatValueAt(0, 20.0);
	_depth->outValue()->setFloatValueAt(0, 10.0);
	_widthSubdivisions->outValue()->setIntValueAt(0, 3);
	_heightSubdivisions->outValue()->setIntValueAt(0, 3);
	_depthSubdivisions->outValue()->setIntValueAt(0, 3);
}

void GeoCube::assignFacePoints(int point0, int point1, int point2, int point3, bool clockWise, std::vector<int> &faceVertices){
	if(clockWise){
		faceVertices[0] = point3;
		faceVertices[1] = point2;
		faceVertices[2] = point1;
		faceVertices[3] = point0;
	}
	else{
		faceVertices[0] = point0;
		faceVertices[1] = point1;
		faceVertices[2] = point2;
		faceVertices[3] = point3;
	}
}

void GeoCube::buildDepthForHeightFaces(int widthSubdivisions, int depthSubdivisions, int heightSubdivisions, int totalPoints, bool otherSide, std::vector<std::vector<int> > &faces){
	std::vector<int> faceVertices(4);
	
	int sideOffset = 0;
	if(otherSide){
		sideOffset = widthSubdivisions;
	}
	
	if(heightSubdivisions == 1){
		for(int faceId = 0; faceId < depthSubdivisions * heightSubdivisions; ++faceId){
			int col = faceId % depthSubdivisions;
			
			int point0 = ((widthSubdivisions + 1) * col) + sideOffset;
			int point1 = (((widthSubdivisions + 1) * (col + 1))) + sideOffset;
			int point2 = (((widthSubdivisions + 1) * ((depthSubdivisions * 2) + heightSubdivisions - 1 - col))) + sideOffset;
			int point3 = (((widthSubdivisions + 1) * ((depthSubdivisions * 2) + heightSubdivisions - col))) + sideOffset;
			
			assignFacePoints(point0, point1, point2, point3, otherSide, faceVertices);
			faces.push_back(faceVertices);
		}
		return;
	}
	
	if(depthSubdivisions == 1){
		for(int faceId = 0; faceId < depthSubdivisions * heightSubdivisions; ++faceId){
			int row = faceId / depthSubdivisions;
			int col = faceId % depthSubdivisions;
			
			int point0 = (((widthSubdivisions + 1) * ((depthSubdivisions * 2) + (heightSubdivisions * 2) - row)) % totalPoints) + sideOffset;
			int point1 = ((widthSubdivisions + 1) * (depthSubdivisions + row)) + sideOffset;
			int point2 = ((widthSubdivisions + 1) * (depthSubdivisions + (row + 1))) + sideOffset;
			int point3 = ((widthSubdivisions + 1) * ((depthSubdivisions * 2) + (heightSubdivisions * 2) - (row + 1))) + sideOffset;
			
			assignFacePoints(point0, point1, point2, point3, otherSide, faceVertices);
			faces.push_back(faceVertices);
		}
		return;
	}
	
	int localFaceId = 0;
	for(int faceId = 0; faceId < depthSubdivisions * heightSubdivisions; ++faceId){
		int row = faceId / depthSubdivisions;
		int col = faceId % depthSubdivisions;
		
		if(row > 0 && row < (heightSubdivisions - 1) && col > 0 && col < (depthSubdivisions - 1)){
			int localWidthSubdivisions = depthSubdivisions - 2;
			int localRow = row - 1;
		
			int point0 = localFaceId + localRow + totalPoints;
			int point1 = point0 + 1;
			int point2 = point0 + localWidthSubdivisions + 2;
			int point3 = point0 + localWidthSubdivisions + 1;
			
			assignFacePoints(point0, point1, point2, point3, otherSide, faceVertices);
			faces.push_back(faceVertices);
			localFaceId++;
		}
		else{
			if(row == 0){
				if(col == 0){
					int point0 = sideOffset;
					int point1 = (widthSubdivisions + 1) + sideOffset;
					int point2 = totalPoints;
					int point3 = ((widthSubdivisions + 1) * (((depthSubdivisions * 2) + (heightSubdivisions * 2)) - 1)) + sideOffset;
					
					assignFacePoints(point0, point1, point2, point3, otherSide, faceVertices);
				}
				else if(col > 0 && col < (depthSubdivisions - 1)){
					int point0 = (faceId * (widthSubdivisions + 1)) + sideOffset;
					int point1 = ((faceId * (widthSubdivisions + 1)) + (widthSubdivisions + 1)) + sideOffset;
					int point2 = (totalPoints + faceId);
					int point3 = (totalPoints + faceId - 1);
					
					assignFacePoints(point0, point1, point2, point3, otherSide, faceVertices);
				}
				else if(col == (depthSubdivisions - 1)){
					int point0 = ((((widthSubdivisions + 1) * (depthSubdivisions + 1)) - (widthSubdivisions + 1)) - (widthSubdivisions + 1)) + sideOffset;
					int point1 = (((widthSubdivisions + 1) * (depthSubdivisions + 1)) - (widthSubdivisions + 1)) + sideOffset;
					int point2 = ((widthSubdivisions + 1) * (depthSubdivisions + 1)) + sideOffset;
					int point3 = (totalPoints + faceId - 1);
					
					assignFacePoints(point0, point1, point2, point3, otherSide, faceVertices);
				}
				
				faces.push_back(faceVertices);
			}
			else if(row > 0 && row < (heightSubdivisions - 1)){
				if(col == 0){
					int point0 = ((widthSubdivisions + 1) * ((depthSubdivisions * 2) + (heightSubdivisions * 2) - row)) + sideOffset;
					int point1 = totalPoints + ((depthSubdivisions - 1) * (row - 1));
					int point2 = totalPoints + ((depthSubdivisions - 1) * row);
					int point3 = (widthSubdivisions + 1) * ((depthSubdivisions * 2) + (heightSubdivisions * 2) - (row + 1)) + sideOffset;
					
					assignFacePoints(point0, point1, point2, point3, otherSide, faceVertices);
				}
				else if(col == (depthSubdivisions - 1)){
					int point0 = totalPoints + ((depthSubdivisions - 1) * (row - 1)) + (col - 1);
					int point1 = ((widthSubdivisions + 1) * (depthSubdivisions + row)) + sideOffset;
					int point2 = ((widthSubdivisions + 1) * (depthSubdivisions + row + 1)) + sideOffset;
					int point3 = totalPoints + ((depthSubdivisions - 1) * (row)) + (col - 1);
					
					assignFacePoints(point0, point1, point2, point3, otherSide, faceVertices);
				}
				
				faces.push_back(faceVertices);
			}
			else if(row == (heightSubdivisions - 1)){
				if(col == 0){
					int point0 = ((widthSubdivisions + 1) * ((depthSubdivisions * 2) + (heightSubdivisions * 2) - row)) + sideOffset;
					int point1 = totalPoints + ((depthSubdivisions - 1) * (heightSubdivisions - 2));
					int point2 = (((widthSubdivisions + 1) * ((depthSubdivisions * 2) + heightSubdivisions)) - (widthSubdivisions + 1)) + sideOffset;
					int point3 = ((widthSubdivisions + 1) * ((depthSubdivisions * 2) + heightSubdivisions)) + sideOffset;
					
					assignFacePoints(point0, point1, point2, point3, otherSide, faceVertices);
				}
				else if(col > 0 && col < (depthSubdivisions - 1)){
					int point0 = totalPoints + ((depthSubdivisions - 1) * (heightSubdivisions - 2)) + (col - 1);
					int point1 = totalPoints + ((depthSubdivisions - 1) * (heightSubdivisions - 2)) + (col);
					int point2 = (((widthSubdivisions + 1) * ((depthSubdivisions * 2) + heightSubdivisions - col)) - (widthSubdivisions + 1)) + sideOffset;
					int point3 =  ((widthSubdivisions + 1) * ((depthSubdivisions * 2) + heightSubdivisions - col)) + sideOffset ;
					
					assignFacePoints(point0, point1, point2, point3, otherSide, faceVertices);
				}
				else if(col == (depthSubdivisions - 1)){
					int point0 = totalPoints + ((depthSubdivisions - 1) * (heightSubdivisions - 2)) + (col - 1);
					int point1 = ((((widthSubdivisions + 1) * (depthSubdivisions + heightSubdivisions + 1)) - (widthSubdivisions + 1)) - (widthSubdivisions + 1)) + sideOffset;
					int point2 = (((widthSubdivisions + 1) * (depthSubdivisions + heightSubdivisions + 1)) - (widthSubdivisions + 1)) + sideOffset;
					int point3 = ((widthSubdivisions + 1) * (depthSubdivisions + heightSubdivisions + 1)) + sideOffset;
					
					assignFacePoints(point0, point1, point2, point3, otherSide, faceVertices);
				}
				
				faces.push_back(faceVertices);
			}
		}
	}
}

void GeoCube::updateSlice(Attribute *attribute, unsigned int slice){
	float width = _width->value()->floatValueAt(0);
	float height = _height->value()->floatValueAt(0);
	float depth = _depth->value()->floatValueAt(0);
	int widthSubdivisions = _widthSubdivisions->value()->intValueAt(0);
	int heightSubdivisions = _heightSubdivisions->value()->intValueAt(0);
	int depthSubdivisions = _depthSubdivisions->value()->intValueAt(0);
	
	if(widthSubdivisions < 1){
		widthSubdivisions = 1;
		_widthSubdivisions->outValue()->setIntValueAt(0, 1);
	}
	
	if(heightSubdivisions < 1){
		heightSubdivisions = 1;
		_heightSubdivisions->outValue()->setIntValueAt(0, 1);
	}
	
	if(depthSubdivisions < 1){
		depthSubdivisions = 1;
		_depthSubdivisions->outValue()->setIntValueAt(0, 1);
	}
	
	int totalFaces0 = widthSubdivisions * depthSubdivisions;
	int totalFaces1 = widthSubdivisions * heightSubdivisions;
	int totalFaces2 = widthSubdivisions * depthSubdivisions;
	int totalFaces3 = widthSubdivisions * heightSubdivisions;

	std::vector<Imath::V3f> points;
	std::vector<std::vector<int> > faces;
	std::vector<int> faceVertices(4);
	
	float widthStep = width / widthSubdivisions;
	float depthStep = depth / depthSubdivisions;
	float heightStep = height / heightSubdivisions;
	
	float halfWidth = width / 2.0;
	float halfDepth = depth / 2.0;
	float halfHeight = height / 2.0;
	
	// we populate each face of the cube following this order
	//    [0]
	//    [1]
	// [4][2][5]
	//    [3]
	
	// face points 0
	for(int row = 0; row < depthSubdivisions; ++row){
		for(int col = 0; col <= widthSubdivisions; ++col){
			float currentWidth = (widthStep * col);
			float currentHeight = (depthStep * row);
			
			points.push_back(Imath::V3f((- halfWidth) + currentWidth, - halfHeight, (- halfDepth) + currentHeight));
		}
	}
	
	// face points 1
	for(int row = 0; row < heightSubdivisions; ++row){
		for(int col = 0; col <= widthSubdivisions; ++col){
			float currentWidth = (widthStep * col);
			float currentHeight = (heightStep * row);
			
			points.push_back(Imath::V3f((- halfWidth) + currentWidth, currentHeight - halfHeight, halfDepth));
		}
	}
	
	// face points 2
	for(int row = 0; row < depthSubdivisions; ++row){
		for(int col = 0; col <= widthSubdivisions; ++col){
			float currentWidth = (widthStep * col);
			float currentHeight = (depthStep * row);
			
			points.push_back(Imath::V3f((- halfWidth) + currentWidth, halfHeight, halfDepth - currentHeight));
		}
	}
	
	// face points 3
	for(int row = 0; row < heightSubdivisions; ++row){
		for(int col = 0; col <= widthSubdivisions; ++col){
			float currentWidth = (widthStep * col);
			float currentHeight = (heightStep * row);
			
			points.push_back(Imath::V3f((- halfWidth) + currentWidth, halfHeight - currentHeight, - halfDepth));
		}
	}
	
	int totalPoints = points.size();
	
	// face points 4
	for(int row = 1; row < heightSubdivisions; ++row){
		for(int col = 1; col < depthSubdivisions; ++col){
			float currentWidth = (depthStep * col);
			float currentHeight = (heightStep * row);
			
			points.push_back(Imath::V3f(- halfWidth, currentHeight - halfHeight, (- halfDepth) + currentWidth));
		}
	}
	
	// face points 5
	for(int row = 1; row < heightSubdivisions; ++row){
		for(int col = 1; col < depthSubdivisions; ++col){
			float currentWidth = (depthStep * col);
			float currentHeight = (heightStep * row);
			
			points.push_back(Imath::V3f(halfWidth, currentHeight - halfHeight, (- halfDepth) + currentWidth));
		}
	}
	
	//// build faces 0, 1, 2, 3
	int totalFaces = totalFaces0 + totalFaces1 + totalFaces2 + totalFaces3;
	int totalRows = (depthSubdivisions * 2) + (heightSubdivisions * 2);
	
	for(int faceId = 0; faceId < totalFaces; ++faceId){
		int row = faceId / widthSubdivisions;
		
		faceVertices[0] = faceId + row;
		faceVertices[1] = faceId + row + 1;
		faceVertices[2] = (faceId + row + widthSubdivisions + 2) % totalPoints;
		faceVertices[3] = (faceId + row + widthSubdivisions + 1) % totalPoints;
		
		faces.push_back(faceVertices);
	}
	
	// build faces 4, 5
	bool otherSide = false;
	buildDepthForHeightFaces(widthSubdivisions, depthSubdivisions, heightSubdivisions, totalPoints, otherSide, faces);
	
	totalPoints += (depthSubdivisions - 1) * (heightSubdivisions - 1);
	otherSide = true;
	buildDepthForHeightFaces(widthSubdivisions, depthSubdivisions, heightSubdivisions, totalPoints, otherSide, faces);
	
	_out->outValue()->build(points, faces);
}
