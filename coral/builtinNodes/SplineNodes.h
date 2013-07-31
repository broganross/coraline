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

#ifndef CORAL_SPLINENODES_H
#define CORAL_SPLINENODES_H

#include <cstdlib>
#include <vector>
#include "../src/Node.h"
#include "../src/NumericAttribute.h"
#include "../src/EnumAttribute.h"

namespace coral{

class SplinePoint: public Node{
public:
	SplinePoint(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);
	void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB);
	void attributeSpecializationChanged(Attribute *attribute);
	
private:
	EnumAttribute *_curveType;
	NumericAttribute *_param;
	NumericAttribute *_controlPoints;
	NumericAttribute *_pointOnCurve;
	void(SplinePoint::*_selectedOperation)(void);
	int _degree;
	int _oldCvsSize;
	std::vector<float> _knots;
	
	void updateArray();
	void updateSingle();
	void updateKnots();
	void pointOnBezier(float param, int degree, const std::vector<Imath::V3f> &cvs, const std::vector<float> &knots, Imath::V3f &outPoint);
	void pointOnCatmull(float param, const std::vector<Imath::V3f> &cvs, const Imath::V3f &firstPoint, const Imath::V3f &lastPoint, Imath::V3f &result);
	void evalCatmull(const Imath::V3f &point0, const Imath::V3f &point1, const Imath::V3f &point2, const Imath::V3f &point3, float u, Imath::V3f &result);
	float basis(int i, int degree, float param, const std::vector<float> &knots);
};

}

#endif
