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

#include "SplineNodes.h"
#include "../src/Numeric.h"
#include "../src/stringUtils.h"

using namespace coral;

SplinePoint::SplinePoint(const std::string &name, Node *parent): 
Node(name, parent),
_selectedOperation(0),
_degree(4),
_oldCvsSize(0){
	_curveType = new EnumAttribute("curveType", this);
	_param = new NumericAttribute("param", this);
	_controlPoints = new NumericAttribute("controlPoints", this);
	_pointOnCurve = new NumericAttribute("pointsOnCurve", this);
	
	addInputAttribute(_curveType);
	addInputAttribute(_param);
	addInputAttribute(_controlPoints);
	addOutputAttribute(_pointOnCurve);
	
	std::vector<std::string> paramSpecialization;
	paramSpecialization.push_back("Float");
	paramSpecialization.push_back("FloatArray");
	
	std::vector<std::string> pointsOnCurveSpecialization;
	pointsOnCurveSpecialization.push_back("Vec3");
	pointsOnCurveSpecialization.push_back("Vec3Array");
	
	setAttributeAllowedSpecializations(_param, paramSpecialization);
	setAttributeAllowedSpecialization(_controlPoints, "Vec3Array");
	setAttributeAllowedSpecializations(_pointOnCurve, pointsOnCurveSpecialization);
	
	addAttributeSpecializationLink(_param, _pointOnCurve);
	
	setAttributeAffect(_curveType, _pointOnCurve);
	setAttributeAffect(_param, _pointOnCurve);
	setAttributeAffect(_controlPoints, _pointOnCurve);

	Enum *curveType = _curveType->outValue();
	curveType->addEntry(0, "bezier");
	curveType->addEntry(1, "catmull-rom");
	curveType->setCurrentIndex(0);

	setSpecializationPreset("single", _curveType, "Enum");
	setSpecializationPreset("single", _param, "Float");
	setSpecializationPreset("single", _controlPoints, "Vec3Array");
	setSpecializationPreset("single", _pointOnCurve, "Vec3");

	setSpecializationPreset("array", _curveType, "Enum");
	setSpecializationPreset("array", _param, "FloatArray");
	setSpecializationPreset("array", _controlPoints, "Vec3Array");
	setSpecializationPreset("array", _pointOnCurve, "Vec3Array");
}

void SplinePoint::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(attributeA == _param && attributeB == _pointOnCurve){
		if(specializationA.size() == 1){
			specializationB.resize(1);
			if(specializationA[0] == "Float"){
				specializationB[0] = "Vec3";
			}
			else if(specializationA[0] == "FloatArray"){
				specializationB[0] = "Vec3Array";
			}
		}
		else if(specializationB.size() == 1){
			specializationA.resize(1);
			if(specializationB[0] == "Vec3"){
				specializationA[0] = "Float";
			}
			else if(specializationB[0] == "Vec3Array"){
				specializationA[0] = "FloatArray";
			}
		}
	}
}

void SplinePoint::attributeSpecializationChanged(Attribute *attribute){
	_selectedOperation = 0;
	
	std::vector<std::string> specialization = attribute->specialization();
	if(specialization.size() == 1){
		if(stringUtils::endswith(specialization[0], "Array")){
			_selectedOperation = &SplinePoint::updateArray;
		}
		else{
			_selectedOperation = &SplinePoint::updateSingle;
		}
	}
}

float SplinePoint::basis(int i, int degree, float param, const std::vector<float> &knots){
	float ret = 0.0;
	
	if(degree > 0){
		float n1 = (param - knots[i]) * basis(i, degree - 1, param, knots);
		
		float d1 = knots[i + degree] - knots[i];
		float n2 = (knots[i + degree + 1] - param) * basis(i + 1, degree - 1, param, knots);
		float d2 = knots[i + degree + 1] - knots[i + 1];
		
		float a = 0.0;
		if(d1 > 0.0001 || d1 < - 0.0001){
			a = n1 / d1;
		}
		
		float b = 0.0;
		if(d2 > 0.0001 || d2 < - 0.0001){
			b = n2 / d2;
		}
		
		ret = a + b;
	}
	else{
		if(knots[0] <= param && param <= knots[i + 1]){
			ret = 1.0;
		}
	}
	
	return ret;
}

void SplinePoint::pointOnBezier(float param, int degree, const std::vector<Imath::V3f> &cvs, const std::vector<float> &knots, Imath::V3f &outPoint){
	if(param <= 0.0){
		outPoint = cvs[0];
		return;
	}
	else if(param >= 1.0){
		outPoint = cvs[cvs.size() - 1];
		return;
	}
	
	float normalizedParam = param * abs(knots[0] - knots[knots.size() - 1]);
	
	outPoint.x = 0.0;
	outPoint.y = 0.0;
	outPoint.z = 0.0;

	float rational = 0.0;
	
	for(int i = 0; i < cvs.size(); ++i){
		float b = basis(i, degree, normalizedParam, knots);
		outPoint += (cvs[i] * b);
		rational += b;
	}
	
	outPoint *= (1.0 / (rational + 0.00001));
}

void SplinePoint::evalCatmull(const Imath::V3f &point0, const Imath::V3f &point1, const Imath::V3f &point2, const Imath::V3f &point3, float u, Imath::V3f &result){
	float u3 = u * u * u;
	float u2 = u * u;
	float f1 = -0.5 * u3 + u2 - 0.5 * u;
	float f2 =  1.5 * u3 - 2.5 * u2 + 1.0;
	float f3 = -1.5 * u3 + 2.0 * u2 + 0.5 * u;
	float f4 =  0.5 * u3 - 0.5 * u2;

	result.x = point0.x * f1 + point1.x * f2 + point2.x * f3 + point3.x * f4;
	result.y = point0.y * f1 + point1.y * f2 + point2.y * f3 + point3.y * f4;
	result.z = point0.z * f1 + point1.z * f2 + point2.z * f3 + point3.z * f4;
}

void SplinePoint::pointOnCatmull(float param, const std::vector<Imath::V3f> &cvs, const Imath::V3f &firstPoint, const Imath::V3f &lastPoint, Imath::V3f &result){
	if(param <= 0.0){
		result = cvs[0];
		return;
	}
	else if(param >= 1.0){
		result = cvs[cvs.size() - 1];
		return;
	}
	else{
		int cvsSize = cvs.size(); 
		float step = 1.0 / float(cvsSize - 1);
		int id = param / step;

		float u = fmod(param / step, 1.0f);
		if(id <= 0){
			evalCatmull(firstPoint, cvs[0], cvs[1], cvs[2], u, result);
		}
		else if(id >= cvsSize - 2){
			evalCatmull(cvs[cvsSize - 3], cvs[cvsSize - 2], cvs[cvsSize - 1], lastPoint, u, result);
		}
		else{
			evalCatmull(cvs[id - 1], cvs[id], cvs[id + 1], cvs[id + 2], u, result);
		}
	}
}

void SplinePoint::updateArray(){
	const std::vector<float> &params = _param->value()->floatValues();
	const std::vector<Imath::V3f> &cvs = _controlPoints->value()->vec3Values();
	
	int paramsSize = params.size();
	int cvsSize = cvs.size();
	
	std::vector<Imath::V3f> pointsOnCurve(paramsSize);
	
	if(cvsSize){
		int curveType = _curveType->value()->currentIndex();
		if(curveType == 0){ // bezier
			if(cvsSize != _oldCvsSize){
				updateKnots();
				_oldCvsSize = cvsSize;
			}

			for(int i = 0; i < paramsSize; ++i){
				pointOnBezier(params[i], _degree, cvs, _knots, pointsOnCurve[i]);
			}
		}
		else{ // catmull
			Imath::V3f firstPoint = cvs[0] + (-(cvs[1] - cvs[0]));
			Imath::V3f lastPoint = cvs[cvsSize - 1] + (cvs[cvsSize - 2] - cvs[cvsSize - 1]);

			for(int i = 0; i < paramsSize; ++i){
				pointOnCatmull(params[i], cvs, firstPoint, lastPoint, pointsOnCurve[i]);
			}
		}
	}

	_pointOnCurve->outValue()->setVec3Values(pointsOnCurve);
}

void SplinePoint::updateSingle(){
	int curveType = _curveType->value()->currentIndex();
	const std::vector<Imath::V3f> &cvs = _controlPoints->value()->vec3Values();
	float param = _param->value()->floatValueAt(0);

	int cvsSize = cvs.size();
	
	Imath::V3f pointOnCurve;
	if(cvsSize){
		if(curveType == 0){ // bezier
			if(cvsSize != _oldCvsSize){
				updateKnots();
				_oldCvsSize = cvsSize;
			}
			pointOnBezier(param, _degree, cvs, _knots, pointOnCurve);
		}
		else{ // catmull-rom
			
			Imath::V3f firstPoint = cvs[0] + (-(cvs[1] - cvs[0]));
			Imath::V3f lastPoint = cvs[cvsSize - 1] + (cvs[cvsSize - 2] - cvs[cvsSize - 1]);

			pointOnCatmull(param, cvs, firstPoint, lastPoint, pointOnCurve);
		}
	}

	_pointOnCurve->outValue()->setVec3ValueAt(0, pointOnCurve);
}

void SplinePoint::updateKnots(){
	int ncvs = _controlPoints->value()->size();
	_degree = ncvs - 1;
	int nknots = ncvs + (_degree * 2) -2;
	_knots.clear();
	
	for(int i = 0; i < _degree; ++i){
		_knots.push_back(0.0);
	}
	
	float n = 0.0;
	int nspans = ncvs - _degree;
	for(int i = 0; i < nspans; ++i){
		_knots.push_back(n);
		n += 1.0;
	}
	
	for(int i = 0; i < (_degree + 1); ++i){
		_knots.push_back(n);
	}
}

void SplinePoint::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		(this->*_selectedOperation)();
	}
}


