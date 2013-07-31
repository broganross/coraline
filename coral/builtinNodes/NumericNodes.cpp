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

#include <map>
#include <cmath>

#ifdef CORAL_PARALLEL_TBB
	#include <tbb/mutex.h>
#endif

#include "NumericNodes.h"
#include "../src/Numeric.h"
#include "../src/containerUtils.h"
#include "../src/mathUtils.h"
#include "../src/stringUtils.h"
#include "../src/Command.h"

#include <ImathEuler.h>

using namespace coral;

namespace {

int findMinorNumericSize(NumericAttribute *attrs[], int numAttrs, unsigned int slice){
	int minorSize = -1;
	for(int i = 0; i < numAttrs; ++i){
		if(attrs[i]->input()){
			Numeric *num = attrs[i]->value();
			int size = num->sizeSlice(slice);
			if(size < minorSize || minorSize == -1){
				minorSize = size;
			}
		}
	}
	if(minorSize == -1){
		minorSize = 0;
	}
	
	return minorSize;
}

std::map<std::string, Numeric> _globalNumericStorage;

#ifdef CORAL_PARALLEL_TBB
	tbb::mutex _globalMutex;
#endif
}


IntNode::IntNode(const std::string &name, Node* parent): Node(name, parent){
	setSliceable(true);

	_out = new NumericAttribute("out", this);
	addOutputAttribute(_out);
	
	setAttributeAllowedSpecialization(_out, "Int");
	
	_out->outValue()->resize(1);
	_out->outValue()->setFloatValueAtSlice(0, 0, 0.0);
}

FloatNode::FloatNode(const std::string &name, Node* parent): Node(name, parent){
	setSliceable(true);

	_out = new NumericAttribute("out", this);
	addOutputAttribute(_out);
	
	setAttributeAllowedSpecialization(_out, "Float");
	
	_out->outValue()->resize(1);
	_out->outValue()->setIntValueAtSlice(0, 0, 0.0);
}

Vec3Node::Vec3Node(const std::string &name, Node* parent): Node(name, parent){
	setSliceable(true);

	_x = new NumericAttribute("x", this);
	_y = new NumericAttribute("y", this);
	_z = new NumericAttribute("z", this);
	_vector = new NumericAttribute("vector", this);
	
	addInputAttribute(_x);
	addInputAttribute(_y);
	addInputAttribute(_z);
	addOutputAttribute(_vector);
	
	setAttributeAffect(_x, _vector);
	setAttributeAffect(_y, _vector);
	setAttributeAffect(_z, _vector);
	
	std::vector<std::string> xyzSpecializations;
	xyzSpecializations.push_back("Float");
	xyzSpecializations.push_back("FloatArray");
	
	std::vector<std::string> vectorSpecializations;
	vectorSpecializations.push_back("Vec3");
	vectorSpecializations.push_back("Vec3Array");
	
	setAttributeAllowedSpecializations(_x, xyzSpecializations);
	setAttributeAllowedSpecializations(_y, xyzSpecializations);
	setAttributeAllowedSpecializations(_z, xyzSpecializations);
	setAttributeAllowedSpecializations(_vector, vectorSpecializations);
	
	addAttributeSpecializationLink(_x, _vector);
	addAttributeSpecializationLink(_x, _y);
	addAttributeSpecializationLink(_y, _z);
	
	setSpecializationPreset("single", _x, "Float");
	setSpecializationPreset("single", _y, "Float");
	setSpecializationPreset("single", _z, "Float");
	setSpecializationPreset("single", _vector, "Vec3");
	
	setSpecializationPreset("array", _x, "FloatArray");
	setSpecializationPreset("array", _y, "FloatArray");
	setSpecializationPreset("array", _z, "FloatArray");
	setSpecializationPreset("array", _vector, "Vec3Array");
	enableSpecializationPreset("single");
}

void Vec3Node::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(attributeA == _x && attributeB == _vector){
		std::vector<std::string> vectorSpecializations = _vector->allowedSpecialization();
		std::vector<std::string> xSpecializations = _x->allowedSpecialization();
	
		if(specializationA.size() == 1){
			specializationB.clear();
			
			if(specializationA[0] == "Float"){
				specializationB.push_back(vectorSpecializations[0]);
			}
			else if(specializationA[0] == "FloatArray"){
				specializationB.push_back(vectorSpecializations[1]);
			}
		}
		else if(specializationB.size() == 1){
			specializationA.clear();
			
			if(specializationB[0] == "Vec3"){
				specializationA.push_back(xSpecializations[0]);
			}
			else if(specializationB[0] == "Vec3Array"){
				specializationA.push_back(xSpecializations[1]);
			}
		}
	}
	else{
		Node::updateSpecializationLink(attributeA, attributeB, specializationA, specializationB);
	}
}

void Vec3Node::updateSlice(Attribute *attribute, unsigned int slice){
	Numeric *x = _x->value();
	Numeric *y = _y->value();
	Numeric *z = _z->value();
	
	NumericAttribute *attrs[] = {_x, _y, _z};
	int minorSize = findMinorNumericSize(attrs, 3, slice);
	if(minorSize < 1)
		minorSize = 1;
	
	std::vector<Imath::V3f> outArray(minorSize);

	for(int i = 0; i < minorSize; ++i){
		Imath::V3f *vec = &outArray[i];
		vec->x = x->floatValueAtSlice(slice, i);
		vec->y = y->floatValueAtSlice(slice, i);
		vec->z = z->floatValueAtSlice(slice, i);
	}
	
	_vector->outValue()->setVec3ValuesSlice(slice, outArray);
}

Vec3ToFloats::Vec3ToFloats(const std::string &name, Node* parent): Node(name, parent){
	setSliceable(true);

	_vector = new NumericAttribute("vector", this);
	_x = new NumericAttribute("x", this);
	_y = new NumericAttribute("y", this);
	_z = new NumericAttribute("z", this);
	
	addInputAttribute(_vector);
	addOutputAttribute(_x);
	addOutputAttribute(_y);
	addOutputAttribute(_z);
	
	setAttributeAffect(_vector, _x);
	setAttributeAffect(_vector, _y);
	setAttributeAffect(_vector, _z);
	
	std::vector<std::string> vectorSpecializations;
	vectorSpecializations.push_back("Vec3");
	vectorSpecializations.push_back("Vec3Array");
	
	std::vector<std::string> xyzSpecializations;
	xyzSpecializations.push_back("Float");
	xyzSpecializations.push_back("FloatArray");
	
	setAttributeAllowedSpecializations(_vector, vectorSpecializations);
	setAttributeAllowedSpecializations(_x, xyzSpecializations);
	setAttributeAllowedSpecializations(_y, xyzSpecializations);
	setAttributeAllowedSpecializations(_z, xyzSpecializations);
	
	addAttributeSpecializationLink(_vector, _x);
	addAttributeSpecializationLink(_vector, _y);
	addAttributeSpecializationLink(_vector, _z);
}

void Vec3ToFloats::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	std::vector<std::string> vectorSpecializations = attributeA->allowedSpecialization();
	std::vector<std::string> xSpecializations = attributeB->allowedSpecialization();

	if(specializationA.size() == 1){
		specializationB.clear();

		if(specializationA[0] == "Vec3"){
			specializationB.push_back(xSpecializations[0]);
		}
		else if(specializationA[0] == "Vec3Array"){
			specializationB.push_back(xSpecializations[1]);
		}
	}
	else if(specializationB.size() == 1){
		specializationA.clear();

		if(specializationB[0] == "Float"){
			specializationA.push_back(vectorSpecializations[0]);
		}
		else if(specializationB[0] == "FloatArray"){
			specializationA.push_back(vectorSpecializations[1]);
		}
	}
}

void Vec3ToFloats::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<Imath::V3f> &vec3Values = _vector->value()->vec3ValuesSlice(slice);
	int size = vec3Values.size();
	
	std::vector<float> xValues(size);
	std::vector<float> yValues(size);
	std::vector<float> zValues(size);
	
	for(int i = 0; i < size; ++i){
		const Imath::V3f &vec = vec3Values[i];
		xValues[i] = vec.x;
		yValues[i] = vec.y;
		zValues[i] = vec.z;
	}
	
	_x->outValue()->setFloatValuesSlice(slice, xValues);
	_y->outValue()->setFloatValuesSlice(slice, yValues);
	_z->outValue()->setFloatValuesSlice(slice, zValues);
	
	setAttributeIsClean(_x, true);
	setAttributeIsClean(_y, true);
	setAttributeIsClean(_z, true);
}

Col4Node::Col4Node(const std::string &name, Node* parent): Node(name, parent){
	setSliceable(true);

	_r = new NumericAttribute("r", this);
	_g = new NumericAttribute("g", this);
	_b = new NumericAttribute("b", this);
	_a = new NumericAttribute("a", this);
	_color = new NumericAttribute("color", this);

	addInputAttribute(_r);
	addInputAttribute(_g);
	addInputAttribute(_b);
	addInputAttribute(_a);
	addOutputAttribute(_color);

	setAttributeAffect(_r, _color);
	setAttributeAffect(_g, _color);
	setAttributeAffect(_b, _color);
	setAttributeAffect(_a, _color);

	std::vector<std::string> rgbaSpecializations;
	rgbaSpecializations.push_back("Float");
	rgbaSpecializations.push_back("FloatArray");

	std::vector<std::string> colorSpecializations;
	colorSpecializations.push_back("Col4");
	colorSpecializations.push_back("Col4Array");

	setAttributeAllowedSpecializations(_r, rgbaSpecializations);
	setAttributeAllowedSpecializations(_g, rgbaSpecializations);
	setAttributeAllowedSpecializations(_b, rgbaSpecializations);
	setAttributeAllowedSpecializations(_color, colorSpecializations);

	addAttributeSpecializationLink(_r, _color);
	addAttributeSpecializationLink(_r, _g);
	addAttributeSpecializationLink(_g, _b);
	addAttributeSpecializationLink(_b, _a);

	setSpecializationPreset("single", _r, "Float");
	setSpecializationPreset("single", _g, "Float");
	setSpecializationPreset("single", _b, "Float");
	setSpecializationPreset("single", _a, "Float");
	setSpecializationPreset("single", _color, "Col4");

	setSpecializationPreset("array", _r, "FloatArray");
	setSpecializationPreset("array", _g, "FloatArray");
	setSpecializationPreset("array", _b, "FloatArray");
	setSpecializationPreset("array", _a, "FloatArray");
	setSpecializationPreset("array", _color, "Col4Array");
	enableSpecializationPreset("single");

	_r->outValue()->setFloatValueAtSlice(0, 0, 0.5);
	_g->outValue()->setFloatValueAtSlice(0, 0, 0.5);
	_b->outValue()->setFloatValueAtSlice(0, 0, 0.5);
	_a->outValue()->setFloatValueAtSlice(0, 0, 1.0);
}

void Col4Node::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(attributeA == _r && attributeB == _color){
		std::vector<std::string> colorSpecializations = _color->allowedSpecialization();
		std::vector<std::string> rSpecializations = _r->allowedSpecialization();

		if(specializationA.size() == 1){
			specializationB.clear();

			if(specializationA[0] == "Float"){
				specializationB.push_back(colorSpecializations[0]);
			}
			else if(specializationA[0] == "FloatArray"){
				specializationB.push_back(colorSpecializations[1]);
			}
		}
		else if(specializationB.size() == 1){
			specializationA.clear();

			if(specializationB[0] == "Col4"){
				specializationA.push_back(rSpecializations[0]);
			}
			else if(specializationB[0] == "Col4Array"){
				specializationA.push_back(rSpecializations[1]);
			}
		}
	}
	else{
		Node::updateSpecializationLink(attributeA, attributeB, specializationA, specializationB);
	}
}

void Col4Node::updateSlice(Attribute *attribute, unsigned int slice){
	Numeric *r = _r->value();
	Numeric *g = _g->value();
	Numeric *b = _b->value();
	Numeric *a = _a->value();

	NumericAttribute *attrs[] = {_r, _g, _b, _a};
	int minorSize = findMinorNumericSize(attrs, 4, slice);
	if(minorSize < 1)
		minorSize = 1;

	std::vector<Imath::Color4f> outArray(minorSize);

	for(int i = 0; i < minorSize; ++i){
		Imath::Color4f *col = &outArray[i];
		col->r = r->floatValueAtSlice(slice, i);
		col->g = g->floatValueAtSlice(slice, i);
		col->b = b->floatValueAtSlice(slice, i);
		col->a = a->floatValueAtSlice(slice, i);
	}

	_color->outValue()->setCol4ValuesSlice(slice, outArray);
}

Col4ToFloats::Col4ToFloats(const std::string &name, Node* parent): Node(name, parent){
	setSliceable(true);

	_color = new NumericAttribute("color", this);
	_r = new NumericAttribute("r", this);
	_g = new NumericAttribute("g", this);
	_b = new NumericAttribute("b", this);
	_a = new NumericAttribute("a", this);

	addInputAttribute(_color);
	addOutputAttribute(_r);
	addOutputAttribute(_g);
	addOutputAttribute(_b);
	addOutputAttribute(_a);

	setAttributeAffect(_color, _r);
	setAttributeAffect(_color, _g);
	setAttributeAffect(_color, _b);
	setAttributeAffect(_color, _a);

	std::vector<std::string> colorSpecializations;
	colorSpecializations.push_back("Col4");
	colorSpecializations.push_back("Col4Array");

	std::vector<std::string> rgbaSpecializations;
	rgbaSpecializations.push_back("Float");
	rgbaSpecializations.push_back("FloatArray");

	setAttributeAllowedSpecializations(_color, colorSpecializations);
	setAttributeAllowedSpecializations(_r, rgbaSpecializations);
	setAttributeAllowedSpecializations(_g, rgbaSpecializations);
	setAttributeAllowedSpecializations(_b, rgbaSpecializations);
	setAttributeAllowedSpecializations(_a, rgbaSpecializations);

	addAttributeSpecializationLink(_color, _r);	//TODO: Could you just check this Andreas?
	addAttributeSpecializationLink(_r, _g);
	addAttributeSpecializationLink(_g, _b);
	addAttributeSpecializationLink(_b, _a);
}

void Col4ToFloats::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(attributeA == _color && attributeB == _r){
		std::vector<std::string> colorSpecializations = _color->allowedSpecialization();
		std::vector<std::string> rSpecializations = _r->allowedSpecialization();

		if(specializationA.size() == 1){
			specializationB.clear();

			if(specializationA[0] == "Col4"){
				specializationB.push_back(rSpecializations[0]);
			}
			else if(specializationA[0] == "Col4Array"){
				specializationB.push_back(rSpecializations[1]);
			}
		}
		else if(specializationB.size() == 1){
			specializationA.clear();

			if(specializationB[0] == "Float"){
				specializationA.push_back(colorSpecializations[0]);
			}
			else if(specializationB[0] == "FloatArray"){
				specializationA.push_back(colorSpecializations[1]);
			}
		}
	}
	else{
		Node::updateSpecializationLink(attributeA, attributeB, specializationA, specializationB);
	}
}

void Col4ToFloats::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<Imath::Color4f> &col4Values = _color->value()->col4ValuesSlice(slice);
	int size = col4Values.size();

	std::vector<float> rValues(size);
	std::vector<float> gValues(size);
	std::vector<float> bValues(size);
	std::vector<float> aValues(size);

	for(int i = 0; i < size; ++i){
		const Imath::Color4f &col = col4Values[i];
		rValues[i] = col.r;
		gValues[i] = col.g;
		bValues[i] = col.b;
		aValues[i] = col.a;
	}

	_r->outValue()->setFloatValuesSlice(slice, rValues);
	_g->outValue()->setFloatValuesSlice(slice, gValues);
	_b->outValue()->setFloatValuesSlice(slice, bValues);
	_a->outValue()->setFloatValuesSlice(slice, aValues);

	setAttributeIsClean(_r, true);
	setAttributeIsClean(_g, true);
	setAttributeIsClean(_b, true);
	setAttributeIsClean(_a, true);
}

Col4Reverse::Col4Reverse(const std::string &name, Node* parent): Node(name, parent){
	setSliceable(true);

	_inColor = new NumericAttribute("in", this);
	_outColor = new NumericAttribute("out", this);

	addInputAttribute(_inColor);
	addOutputAttribute(_outColor);

	setAttributeAffect(_inColor, _outColor);

	std::vector<std::string> colorSpecializations;
	colorSpecializations.push_back("Col4");
	colorSpecializations.push_back("Col4Array");

	setAttributeAllowedSpecializations(_inColor, colorSpecializations);
	setAttributeAllowedSpecializations(_outColor, colorSpecializations);

	addAttributeSpecializationLink(_inColor, _outColor);
}

void Col4Reverse::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(attributeA == _inColor && attributeB == _outColor){
		std::vector<std::string> inColorSpecializations = _inColor->allowedSpecialization();
		std::vector<std::string> outColorSpecializations = _outColor->allowedSpecialization();

		if(specializationA.size() == 1){
			specializationB.clear();

			if(specializationA[0] == "Col4"){
				specializationB.push_back(outColorSpecializations[0]);
			}
			else if(specializationA[0] == "Col4Array"){
				specializationB.push_back(outColorSpecializations[1]);
			}
		}
		else if(specializationB.size() == 1){
			specializationA.clear();

			if(specializationB[0] == "Col4"){
				specializationA.push_back(inColorSpecializations[0]);
			}
			else if(specializationB[0] == "Col4Array"){
				specializationA.push_back(inColorSpecializations[1]);
			}
		}
	}
	else{
		Node::updateSpecializationLink(attributeA, attributeB, specializationA, specializationB);
	}
}

void Col4Reverse::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<Imath::Color4f> &inCol4Values = _inColor->value()->col4ValuesSlice(slice);
	int size = inCol4Values.size();

	std::vector<Imath::Color4f> outCol4Values(size);

	for(int i = 0; i < size; ++i){
		const Imath::Color4f &inCol4 = inCol4Values[i];

		outCol4Values[i].r = 1-inCol4.r;
		outCol4Values[i].g = 1-inCol4.g;
		outCol4Values[i].b = 1-inCol4.b;
		outCol4Values[i].a = inCol4.a;
	}

	_outColor->outValue()->setCol4ValuesSlice(slice, outCol4Values);

	setAttributeIsClean(_outColor, true);
}

QuatNode::QuatNode(const std::string &name, Node* parent): Node(name, parent){
	setSliceable(true);

	_r = new NumericAttribute("r", this);
	_x = new NumericAttribute("x", this);
	_y = new NumericAttribute("y", this);
	_z = new NumericAttribute("z", this);
	_quat = new NumericAttribute("quaternion", this);

	addInputAttribute(_r);
	addInputAttribute(_x);
	addInputAttribute(_y);
	addInputAttribute(_z);
	addOutputAttribute(_quat);

	setAttributeAffect(_r, _quat);
	setAttributeAffect(_x, _quat);
	setAttributeAffect(_y, _quat);
	setAttributeAffect(_z, _quat);

	std::vector<std::string> rxyzSpecializations;
	rxyzSpecializations.push_back("Float");
	rxyzSpecializations.push_back("FloatArray");

	std::vector<std::string> vectorSpecializations;
	vectorSpecializations.push_back("Quat");
	vectorSpecializations.push_back("QuatArray");


	setAttributeAllowedSpecializations(_r, rxyzSpecializations);
	setAttributeAllowedSpecializations(_x, rxyzSpecializations);
	setAttributeAllowedSpecializations(_y, rxyzSpecializations);
	setAttributeAllowedSpecializations(_z, rxyzSpecializations);
	setAttributeAllowedSpecializations(_quat, vectorSpecializations);

	addAttributeSpecializationLink(_r ,_quat);
	addAttributeSpecializationLink(_r, _x);
	addAttributeSpecializationLink(_x, _y);
	addAttributeSpecializationLink(_y, _z);
}

void QuatNode::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(attributeA == _r && attributeB == _quat){
		std::vector<std::string> vectorSpecializations = _quat->allowedSpecialization();
		std::vector<std::string> xSpecializations = _r->allowedSpecialization();

		if(specializationA.size() == 1){
			specializationB.clear();

			if(specializationA[0] == "Float"){
				specializationB.push_back(vectorSpecializations[0]);
			}
			else if(specializationA[0] == "FloatArray"){
				specializationB.push_back(vectorSpecializations[1]);
			}
		}
		else if(specializationB.size() == 1){
			specializationA.clear();

			if(specializationB[0] == "Quat"){
				specializationA.push_back(xSpecializations[0]);
			}
			else if(specializationB[0] == "QuatArray"){
				specializationA.push_back(xSpecializations[1]);
			}
		}
	}
	else{
		Node::updateSpecializationLink(attributeA, attributeB, specializationA, specializationB);
	}
}

void QuatNode::updateSlice(Attribute *attribute, unsigned int slice){
	Numeric *r = _r->value();
	Numeric *x = _x->value();
	Numeric *y = _y->value();
	Numeric *z = _z->value();

	NumericAttribute *attrs[] = {_r,_x, _y, _z};
	int minorSize = findMinorNumericSize(attrs, 4, slice);
	if(minorSize < 1)
		minorSize = 1;

	std::vector<Imath::Quatf> outArray(minorSize);

	for(int i = 0; i < minorSize; ++i){
		Imath::Quatf *vec = &outArray[i];
		vec->r = r->floatValueAtSlice(slice, i);
		vec->v.x = x->floatValueAtSlice(slice, i);
		vec->v.y = y->floatValueAtSlice(slice, i);
		vec->v.z = z->floatValueAtSlice(slice, i);
	}

	_quat->outValue()->setQuatValuesSlice(slice, outArray);
}

QuatToFloats::QuatToFloats(const std::string &name, Node* parent): Node(name, parent){
	setSliceable(true);

	_quat = new NumericAttribute("quaternion", this);
	_r = new NumericAttribute("r", this);
	_x = new NumericAttribute("x", this);
	_y = new NumericAttribute("y", this);
	_z = new NumericAttribute("z", this);

	addInputAttribute(_quat);
	addOutputAttribute(_r);
	addOutputAttribute(_x);
	addOutputAttribute(_y);
	addOutputAttribute(_z);

	setAttributeAffect(_quat, _r);
	setAttributeAffect(_quat, _x);
	setAttributeAffect(_quat, _y);
	setAttributeAffect(_quat, _z);

	std::vector<std::string> vectorSpecializations;
	vectorSpecializations.push_back("Quat");
	vectorSpecializations.push_back("QuatArray");

	std::vector<std::string> xyzSpecializations;
	xyzSpecializations.push_back("Float");
	xyzSpecializations.push_back("FloatArray");

	setAttributeAllowedSpecializations(_quat, vectorSpecializations);

	setAttributeAllowedSpecializations(_r, xyzSpecializations);
	setAttributeAllowedSpecializations(_x, xyzSpecializations);
	setAttributeAllowedSpecializations(_y, xyzSpecializations);
	setAttributeAllowedSpecializations(_z, xyzSpecializations);

	addAttributeSpecializationLink(_quat, _r);
	addAttributeSpecializationLink(_r, _x);
	addAttributeSpecializationLink(_x, _y);
	addAttributeSpecializationLink(_y, _z);

}

void QuatToFloats::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(attributeA == _quat && attributeB == _r){
		std::vector<std::string> vectorSpecializations = _quat->allowedSpecialization();
		std::vector<std::string> xSpecializations = _r->allowedSpecialization();

		if(specializationA.size() == 1){
			specializationB.clear();

			if(specializationA[0] == "Quat"){
				specializationB.push_back(xSpecializations[0]);
			}
			else if(specializationA[0] == "QuatArray"){
				specializationB.push_back(xSpecializations[1]);
			}
		}
		else if(specializationB.size() == 1){
			specializationA.clear();

			if(specializationB[0] == "Float"){
				specializationA.push_back(vectorSpecializations[0]);
			}
			else if(specializationB[0] == "FloatArray"){
				specializationA.push_back(vectorSpecializations[1]);
			}
		}
	}
	else{
		Node::updateSpecializationLink(attributeA, attributeB, specializationA, specializationB);
	}
}

void QuatToFloats::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<Imath::Quatf> &quatValues = _quat->value()->quatValuesSlice(slice);
	int size = quatValues.size();

	std::vector<float> rValues(size);
	std::vector<float> xValues(size);
	std::vector<float> yValues(size);
	std::vector<float> zValues(size);

	for(int i = 0; i < size; ++i){
		const Imath::Quatf &vec = quatValues[i];
		rValues[i] = vec.r;
		xValues[i] = vec.v.x;
		yValues[i] = vec.v.y;
		zValues[i] = vec.v.z;
	}

	_r->outValue()->setFloatValuesSlice(slice, rValues);
	_x->outValue()->setFloatValuesSlice(slice, xValues);
	_y->outValue()->setFloatValuesSlice(slice, yValues);
	_z->outValue()->setFloatValuesSlice(slice, zValues);

	setAttributeIsClean(_r, true);
	setAttributeIsClean(_x, true);
	setAttributeIsClean(_y, true);
	setAttributeIsClean(_z, true);
}


Matrix44Node::Matrix44Node(const std::string &name, Node* parent): Node(name, parent){
	setSliceable(true);

	_translateX = new NumericAttribute("translateX", this);
	_translateY = new NumericAttribute("translateY", this);
	_translateZ = new NumericAttribute("translateZ", this);
	_eulerX = new NumericAttribute("eulerX", this);
	_eulerY = new NumericAttribute("eulerY", this);
	_eulerZ = new NumericAttribute("eulerZ", this);
	_scaleX = new NumericAttribute("scaleX", this);
	_scaleY = new NumericAttribute("scaleY", this);
	_scaleZ = new NumericAttribute("scaleZ", this);
	_matrix = new NumericAttribute("matrix", this);
	
	addInputAttribute(_translateX);
	addInputAttribute(_translateY);
	addInputAttribute(_translateZ);
	addInputAttribute(_eulerX);
	addInputAttribute(_eulerY);
	addInputAttribute(_eulerZ);
	addInputAttribute(_scaleX);
	addInputAttribute(_scaleY);
	addInputAttribute(_scaleZ);
	addOutputAttribute(_matrix);
	
	setAttributeAffect(_translateX, _matrix);
	setAttributeAffect(_translateY, _matrix);
	setAttributeAffect(_translateZ, _matrix);
	setAttributeAffect(_eulerX, _matrix);
	setAttributeAffect(_eulerY, _matrix);
	setAttributeAffect(_eulerZ, _matrix);
	setAttributeAffect(_scaleX, _matrix);
	setAttributeAffect(_scaleY, _matrix);
	setAttributeAffect(_scaleZ, _matrix);
	
	std::vector<std::string> floatSpecialization;
	floatSpecialization.push_back("Float");
	floatSpecialization.push_back("FloatArray");
	
	std::vector<std::string> matrixSpecialization;
	matrixSpecialization.push_back("Matrix44");
	matrixSpecialization.push_back("Matrix44Array");
	
	setAttributeAllowedSpecializations(_translateX, floatSpecialization);
	setAttributeAllowedSpecializations(_translateY, floatSpecialization);
	setAttributeAllowedSpecializations(_translateZ, floatSpecialization);
	setAttributeAllowedSpecializations(_eulerX, floatSpecialization);
	setAttributeAllowedSpecializations(_eulerY, floatSpecialization);
	setAttributeAllowedSpecializations(_eulerZ, floatSpecialization);
	setAttributeAllowedSpecializations(_scaleX, floatSpecialization);
	setAttributeAllowedSpecializations(_scaleY, floatSpecialization);
	setAttributeAllowedSpecializations(_scaleZ, floatSpecialization);
	setAttributeAllowedSpecializations(_matrix, matrixSpecialization);
	
	addAttributeSpecializationLink(_translateX, _matrix);
	addAttributeSpecializationLink(_translateY, _matrix);
	addAttributeSpecializationLink(_translateZ, _matrix);
	addAttributeSpecializationLink(_eulerX, _matrix);
	addAttributeSpecializationLink(_eulerY, _matrix);
	addAttributeSpecializationLink(_eulerZ, _matrix);
	addAttributeSpecializationLink(_scaleX, _matrix);
	addAttributeSpecializationLink(_scaleY, _matrix);
	addAttributeSpecializationLink(_scaleZ, _matrix);
	
	setSpecializationPreset("single", _translateX, "Float");
	setSpecializationPreset("single", _translateY, "Float");
	setSpecializationPreset("single", _translateZ, "Float");
	setSpecializationPreset("single", _eulerX, "Float");
	setSpecializationPreset("single", _eulerY, "Float");
	setSpecializationPreset("single", _eulerZ, "Float");
	setSpecializationPreset("single", _scaleX, "Float");
	setSpecializationPreset("single", _scaleY, "Float");
	setSpecializationPreset("single", _scaleZ, "Float");
	setSpecializationPreset("single", _matrix, "Matrix44");
	
	setSpecializationPreset("array", _translateX, "FloatArray");
	setSpecializationPreset("array", _translateY, "FloatArray");
	setSpecializationPreset("array", _translateZ, "FloatArray");
	setSpecializationPreset("array", _eulerX, "FloatArray");
	setSpecializationPreset("array", _eulerY, "FloatArray");
	setSpecializationPreset("array", _eulerZ, "FloatArray");
	setSpecializationPreset("array", _scaleX, "FloatArray");
	setSpecializationPreset("array", _scaleY, "FloatArray");
	setSpecializationPreset("array", _scaleZ, "FloatArray");
	setSpecializationPreset("array", _matrix, "Matrix44Array");
	
	enableSpecializationPreset("single");
	
	_scaleX->outValue()->setFloatValueAtSlice(0, 0, 1.0);
	_scaleY->outValue()->setFloatValueAtSlice(0, 0, 1.0);
	_scaleZ->outValue()->setFloatValueAtSlice(0, 0, 1.0);
}

void Matrix44Node::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(specializationA.size() == 1){
		specializationB.clear();
		
		if(specializationA[0] == "Float"){
			specializationB.push_back("Matrix44");
		}
		else if(specializationA[0] == "FloatArray"){
			specializationB.push_back("Matrix44Array");
		}
	}
	else if(specializationB.size() == 1){
		specializationA.clear();

		if(specializationB[0] == "Matrix44"){
			specializationA.push_back("Float");
		}
		else if(specializationB[0] == "Matrix44Array"){
			specializationA.push_back("FloatArray");
		}
	}
}

void Matrix44Node::updateSlice(Attribute *attribute, unsigned int slice){
	Numeric *translateX = _translateX->value();
	Numeric *translateY = _translateY->value();
	Numeric *translateZ = _translateZ->value();
	Numeric *eulerX = _eulerX->value();
	Numeric *eulerY = _eulerY->value();
	Numeric *eulerZ = _eulerZ->value();
	Numeric *scaleX = _scaleX->value();
	Numeric *scaleY = _scaleY->value();
	Numeric *scaleZ = _scaleZ->value();
	
	NumericAttribute *attrs[] = {
			_translateX,
			_translateY,
			_translateZ,
			_eulerX,
			_eulerY,
			_eulerZ,
			_scaleX,
			_scaleY,
			_scaleZ};
			
	int minorSize = findMinorNumericSize(attrs, 9, slice);
	if(minorSize < 1)
		minorSize = 1;
	
	std::vector<Imath::M44f> newMatrixValues(minorSize);
	float radian = (M_PI / 180);
	for(int i = 0; i < minorSize; ++i){
		Imath::M44f matrix;
		Imath::V3f vec(translateX->floatValueAtSlice(slice, i), translateY->floatValueAtSlice(slice, i), translateZ->floatValueAtSlice(slice, i));
		matrix.translate(vec);
		
		vec.setValue(radian * eulerX->floatValueAtSlice(slice, i), radian * eulerY->floatValueAtSlice(slice, i), radian * eulerZ->floatValueAtSlice(slice, i));
		matrix.rotate(vec);
		
		vec.setValue(scaleX->floatValueAtSlice(slice, i), scaleY->floatValueAtSlice(slice, i), scaleZ->floatValueAtSlice(slice, i));
		matrix.scale(vec);
		
		newMatrixValues[i] = matrix;
	}
	
	_matrix->outValue()->setMatrix44ValuesSlice(slice, newMatrixValues);
}

ConstantArray::ConstantArray(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);

	_size = new NumericAttribute("size", this);
	_constant = new NumericAttribute("constant", this);
	_array = new NumericAttribute("array", this);
	
	addInputAttribute(_size);
	addInputAttribute(_constant);
	addOutputAttribute(_array);
	
	setAttributeAffect(_size, _array);
	setAttributeAffect(_constant, _array);
	
	std::vector<std::string> constantSpecializations;
	constantSpecializations.push_back("Int");
	constantSpecializations.push_back("Float");
	constantSpecializations.push_back("Vec3");
	constantSpecializations.push_back("Col4");
	constantSpecializations.push_back("Matrix44");
	
	std::vector<std::string> arraySpecializations;
	arraySpecializations.push_back("IntArray");
	arraySpecializations.push_back("FloatArray");
	arraySpecializations.push_back("Vec3Array");
	arraySpecializations.push_back("Col4Array");
	arraySpecializations.push_back("Matrix44Array");
	
	setAttributeAllowedSpecialization(_size, "Int");
	setAttributeAllowedSpecializations(_constant, constantSpecializations);
	setAttributeAllowedSpecializations(_array, arraySpecializations);
	
	addAttributeSpecializationLink(_constant, _array);
}

void ConstantArray::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(specializationA.size() < specializationB.size()){
		std::vector<std::string> newSpecialization;
		
		for(int i = 0; i < specializationA.size(); ++i){
			newSpecialization.push_back(specializationA[i] + "Array");
		}
		
		specializationB = newSpecialization;
	}
	else{
		std::vector<std::string> newSpecialization;
		
		for(int i = 0; i < specializationB.size(); ++i){
			newSpecialization.push_back(stringUtils::replace(specializationB[i], "Array", ""));
		}
		
		specializationA = newSpecialization;
	}
}

void ConstantArray::updateSlice(Attribute *attribute, unsigned int slice){
	Numeric *constant = _constant->value();
	if(constant->type() != Numeric::numericTypeAny){
		int size = _size->value()->intValueAtSlice(slice, 0);
		if(size < 0){
			size = 0;
			_size->outValue()->setIntValueAtSlice(slice, 0, 0);
		}
		
		Numeric *array = _array->outValue();
		if(constant->type() == Numeric::numericTypeInt){
			int constantValue = constant->intValueAtSlice(slice, 0);
			std::vector<int> values(size);
			for(int i = 0; i < size; ++i){
				values[i] = constantValue;
			}
			array->setIntValuesSlice(slice, values);
		}
		else if(constant->type() == Numeric::numericTypeFloat){
			float constantValue = constant->floatValueAtSlice(slice, 0);
			std::vector<float> values(size);
			for(int i = 0; i < size; ++i){
				values[i] = constantValue;
			}
			array->setFloatValuesSlice(slice, values);
		}
		else if(constant->type() == Numeric::numericTypeVec3){
			Imath::V3f constantValue = constant->vec3ValueAtSlice(slice, 0);
			std::vector<Imath::V3f> values(size);
			for(int i = 0; i < size; ++i){
				values[i] = constantValue;
			}
			array->setVec3ValuesSlice(slice, values);
		}
		else if(constant->type() == Numeric::numericTypeCol4){
			Imath::Color4f constantValue = constant->col4ValueAtSlice(slice, 0);
			std::vector<Imath::Color4f> values(size);
			for(int i = 0; i < size; ++i){
				values[i] = constantValue;
			}
			array->setCol4ValuesSlice(slice, values);
		}
		else if(constant->type() == Numeric::numericTypeMatrix44){
			Imath::M44f constantValue = constant->matrix44ValueAtSlice(slice, 0);
			std::vector<Imath::M44f> values(size);
			for(int i = 0; i < size; ++i){
				values[i] = constantValue;
			}
			array->setMatrix44ValuesSlice(slice, values);
		}
	}
	else{
		setAttributeIsClean(_array, false);
	}
}

ArraySize::ArraySize(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);

	_array = new NumericAttribute("array", this);
	_size = new NumericAttribute("size", this);
	
	addInputAttribute(_array);
	addOutputAttribute(_size);
	
	setAttributeAffect(_array, _size);
	
	std::vector<std::string> arraySpecializations;
	arraySpecializations.push_back("IntArray");
	arraySpecializations.push_back("FloatArray");
	arraySpecializations.push_back("Vec3Array");
	arraySpecializations.push_back("Col4Array");
	arraySpecializations.push_back("Matrix44Array");
	
	setAttributeAllowedSpecializations(_array, arraySpecializations);
	setAttributeAllowedSpecialization(_size, "Int");
}

void ArraySize::updateSlice(Attribute *attribute, unsigned int slice){
	Numeric *array = _array->value();
	if(array->type() != Numeric::numericTypeAny){
		_size->outValue()->setIntValueAtSlice(slice, 0, array->sizeSlice(slice));
	}
	else{
		setAttributeIsClean(_size, false);
	}
}


BuildArray::BuildArray(const std::string &name, Node *parent): 
	Node(name, parent),
	_selectedOperation(0){
	setSliceable(true);

	setAllowDynamicAttributes(true);
	
	_array = new NumericAttribute("array", this);
	addOutputAttribute(_array);
}

void BuildArray::addNumericAttribute(){
	std::string numStr = stringUtils::intToString(inputAttributes().size());
	NumericAttribute *attr = new NumericAttribute("in" + numStr, this);
	addInputAttribute(attr);
	setAttributeAffect(attr, _array);
	
	std::vector<std::string> specialization;
	specialization.push_back("Int");
	specialization.push_back("Float");
	specialization.push_back("Vec3");
	specialization.push_back("Col4");
	specialization.push_back("Matrix44");
	
	setAttributeAllowedSpecializations(attr, specialization);
	
	addAttributeSpecializationLink(attr, _array);
	
	addDynamicAttribute(attr);
	updateAttributeSpecialization(attr);
}

void BuildArray::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(specializationA.size() <= specializationB.size()){
		specializationB.resize(specializationA.size());
		for(int i = 0; i < specializationA.size(); ++i){
			specializationB[i] = specializationA[i] + "Array";
		}
	}
	else{
		specializationA.resize(specializationB.size());
		for(int i = 0; i < specializationB.size(); ++i){
			specializationA[i] = stringUtils::strip(specializationB[i], "Array");
		}
	}
}

void BuildArray::attributeSpecializationChanged(Attribute *attribute){
	if(attribute == _array){
		_selectedOperation = 0;
		
		Numeric::Type type = _array->outValue()->type();
		if(type != Numeric::numericTypeAny){
			if(type == Numeric::numericTypeIntArray){
				_selectedOperation = &BuildArray::updateInt;
			}
			else if(type == Numeric::numericTypeFloatArray){
				_selectedOperation = &BuildArray::updateFloat;
			}
			else if(type == Numeric::numericTypeVec3Array){
				_selectedOperation = &BuildArray::updateVec3;
			}
			else if(type == Numeric::numericTypeCol4Array){
				_selectedOperation = &BuildArray::updateCol4;
			}
			else if(type == Numeric::numericTypeMatrix44Array){
				_selectedOperation = &BuildArray::updateMatrix44;
			}
		}
	}
}

void BuildArray::updateInt(const std::vector<Attribute*> &inAttrs, int arraySize, Numeric *array, unsigned int slice){
	std::vector<int> arrayValues(arraySize);
	for(int i = 0; i < arraySize; ++i){
		Numeric *inNum = (Numeric*)inAttrs[i]->value();
		arrayValues[i] = inNum->intValueAtSlice(slice, 0);
	}
	
	array->setIntValuesSlice(slice, arrayValues);
}

void BuildArray::updateFloat(const std::vector<Attribute*> &inAttrs, int arraySize, Numeric *array, unsigned int slice){
	std::vector<float> arrayValues(arraySize);
	for(int i = 0; i < arraySize; ++i){
		Numeric *inNum = (Numeric*)inAttrs[i]->value();
		arrayValues[i] = inNum->floatValueAtSlice(slice, 0);
	}
	
	array->setFloatValuesSlice(slice, arrayValues);
}

void BuildArray::updateVec3(const std::vector<Attribute*> &inAttrs, int arraySize, Numeric *array, unsigned int slice){
	std::vector<Imath::V3f> arrayValues(arraySize);
	for(int i = 0; i < arraySize; ++i){
		Numeric *inNum = (Numeric*)inAttrs[i]->value();
		arrayValues[i] = inNum->vec3ValueAtSlice(slice, 0);
	}
	
	array->setVec3ValuesSlice(slice, arrayValues);
}

void BuildArray::updateCol4(const std::vector<Attribute*> &inAttrs, int arraySize, Numeric *array, unsigned int slice){
	std::vector<Imath::Color4f> arrayValues(arraySize);
	for(int i = 0; i < arraySize; ++i){
		Numeric *inNum = (Numeric*)inAttrs[i]->value();
		arrayValues[i] = inNum->col4ValueAtSlice(slice, 0);
	}

	array->setCol4ValuesSlice(slice, arrayValues);
}

void BuildArray::updateMatrix44(const std::vector<Attribute*> &inAttrs, int arraySize, Numeric *array, unsigned int slice){
	std::vector<Imath::M44f> arrayValues(arraySize);
	for(int i = 0; i < arraySize; ++i){
		Numeric *inNum = (Numeric*)inAttrs[i]->value();
		arrayValues[i] = inNum->matrix44ValueAtSlice(slice, 0);
	}
	
	array->setMatrix44ValuesSlice(slice, arrayValues);
}

void BuildArray::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		std::vector<Attribute*> inAttrs = inputAttributes();
		int arraySize = inAttrs.size();
		Numeric *array = _array->outValue();
		
		(this->*_selectedOperation)(inAttrs, arraySize, array, slice);
	}
}


RangeArray::RangeArray(const std::string &name, Node* parent):
Node(name, parent),
_selectedOperation(0){	
	setSliceable(true);

	_start = new NumericAttribute("start", this);
	_end = new NumericAttribute("end", this);
	_steps = new NumericAttribute("steps", this);
	_array = new NumericAttribute("array", this);
	
	addInputAttribute(_start);
	addInputAttribute(_end);
	addInputAttribute(_steps);
	addOutputAttribute(_array);
	
	setAttributeAffect(_start, _array);
	setAttributeAffect(_end, _array);
	setAttributeAffect(_steps, _array);
	
	std::vector<std::string> inSpecializations;
	inSpecializations.push_back("Int");
	inSpecializations.push_back("Float");
	
	std::vector<std::string> outSpecializations;
	outSpecializations.push_back("IntArray");
	outSpecializations.push_back("FloatArray");
	
	setAttributeAllowedSpecializations(_start, inSpecializations);
	setAttributeAllowedSpecializations(_end, inSpecializations);
	setAttributeAllowedSpecialization(_steps, "Int");
	setAttributeAllowedSpecializations(_array, outSpecializations);
	
	addAttributeSpecializationLink(_start, _array);
	addAttributeSpecializationLink(_end, _array);
}

void RangeArray::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(specializationA.size() <= specializationB.size()){
		specializationB.resize(specializationA.size());
		for(int i = 0; i < specializationA.size(); ++i){
			specializationB[i] = specializationA[i] + "Array";
		}
	}
	else{
		specializationA.resize(specializationB.size());
		for(int i = 0; i < specializationB.size(); ++i){
			specializationA[i] = stringUtils::strip(specializationB[i], "Array");
		}
	}
}

void RangeArray::attributeSpecializationChanged(Attribute *attribute){
	if(attribute == _array){
		_selectedOperation = 0;
		
		Numeric::Type type = _array->outValue()->type();
		if(type != Numeric::numericTypeAny){
			if(type == Numeric::numericTypeIntArray){
				_selectedOperation = &RangeArray::updateInt;
			}
			else if(type == Numeric::numericTypeFloatArray){
				_selectedOperation = &RangeArray::updateFloat;
			}
		}
	}
}

void RangeArray::updateInt(Numeric *start, Numeric *end, int steps, Numeric *array, unsigned int slice){
	int startValue = start->intValueAtSlice(slice, 0);
	int endValue = end->intValueAtSlice(slice, 0);
	
	float totalRange = endValue - startValue;
	int incrStep = totalRange / steps;
	int currentStep = startValue;
	std::vector<int> arrayValues(steps + 1);
	
	for(int i = 0; i < steps + 1; ++i){
		arrayValues[i] = currentStep;
		currentStep += incrStep;
	}
	
	array->setIntValuesSlice(slice, arrayValues);
}

void RangeArray::updateFloat(Numeric *start, Numeric *end, int steps, Numeric *array, unsigned int slice){
	float startValue = start->floatValueAtSlice(slice, 0);
	float endValue = end->floatValueAtSlice(slice, 0);
	
	float totalRange = endValue - startValue;
	float incrStep = totalRange / float(steps);
	float currentStep = startValue;
	std::vector<float> arrayValues(steps + 1);
	
	for(int i = 0; i < steps + 1; ++i){
		arrayValues[i] = currentStep;
		
		currentStep += incrStep;
	}
	
	array->setFloatValuesSlice(slice, arrayValues);
}

void RangeArray::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		Numeric *start = _start->value();
		Numeric *end = _end->value();
		int steps = _steps->value()->intValueAtSlice(slice, 0);
		if(steps < 0){
			_steps->outValue()->setIntValueAtSlice(slice, 0, 0);
			steps = 0;
		}
		
		Numeric *array = _array->outValue();
		
		(this->*_selectedOperation)(start, end, steps, array, slice);
	}
}

Matrix44Translation::Matrix44Translation(const std::string &name, Node* parent): Node(name, parent){	
	setSliceable(true);

	_matrix = new NumericAttribute("matrix", this);
	_translation = new NumericAttribute("translation", this);
	
	std::vector<std::string> matrixSpec;
	matrixSpec.push_back("Matrix44");
	matrixSpec.push_back("Matrix44Array");
	setAttributeAllowedSpecializations(_matrix, matrixSpec);
	
	std::vector<std::string> translationSpec;
	translationSpec.push_back("Vec3");
	translationSpec.push_back("Vec3Array");
	setAttributeAllowedSpecializations(_translation, translationSpec);
	
	addInputAttribute(_matrix);
	addOutputAttribute(_translation);
	
	setAttributeAffect(_matrix, _translation);
	
	addAttributeSpecializationLink(_matrix, _translation);
}

void Matrix44Translation::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(specializationA.size() < specializationB.size()){
		specializationB.resize(1);
		std::string suffix = stringUtils::replace(specializationA[0], "Matrix44", "");
		specializationB[0] = "Vec3" + suffix;
	}
	else if(specializationB.size() < specializationA.size()){
		specializationA.resize(1);
		std::string suffix = stringUtils::replace(specializationB[0], "Vec3", "");
		specializationA[0] = "Matrix44" + suffix;
	}
}

void Matrix44Translation::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<Imath::M44f> &matrix = _matrix->value()->matrix44ValuesSlice(slice);
	int size = matrix.size();
	std::vector<Imath::V3f> translationValues(size);
	
	for(int i = 0; i < size; ++i){
		Imath::V3f &translationValue = translationValues[i];
		const Imath::M44f &m = matrix[i];
		
		translationValue.x = m[3][0];
		translationValue.y = m[3][1];
		translationValue.z = m[3][2];
	}
	
	_translation->outValue()->setVec3ValuesSlice(slice, translationValues);
}

Matrix44RotationAxis::Matrix44RotationAxis(const std::string &name, Node* parent): Node(name, parent){	
	setSliceable(true);

	_matrix = new NumericAttribute("matrix", this);
	_axisX = new NumericAttribute("axisX", this);
	_axisY = new NumericAttribute("axisY", this);
	_axisZ = new NumericAttribute("axisZ", this);
	
	addInputAttribute(_matrix);
	addOutputAttribute(_axisX);
	addOutputAttribute(_axisY);
	addOutputAttribute(_axisZ);
	
	std::vector<std::string> matrixSpec;
	matrixSpec.push_back("Matrix44");
	matrixSpec.push_back("Matrix44Array");
	setAttributeAllowedSpecializations(_matrix, matrixSpec);
	
	std::vector<std::string> axisSpec;
	axisSpec.push_back("Vec3");
	axisSpec.push_back("Vec3Array");
	setAttributeAllowedSpecializations(_axisX, axisSpec);
	setAttributeAllowedSpecializations(_axisY, axisSpec);
	setAttributeAllowedSpecializations(_axisZ, axisSpec);
	
	setAttributeAffect(_matrix, _axisX);
	setAttributeAffect(_matrix, _axisY);
	setAttributeAffect(_matrix, _axisZ);
	
	addAttributeSpecializationLink(_matrix, _axisX);
	addAttributeSpecializationLink(_matrix, _axisY);
	addAttributeSpecializationLink(_matrix, _axisZ);
}

void Matrix44RotationAxis::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(specializationA.size() < specializationB.size()){
		specializationB.resize(1);
		std::string suffix = stringUtils::replace(specializationA[0], "Matrix44", "");
		specializationB[0] = "Vec3" + suffix;
	}
	else if(specializationB.size() < specializationA.size()){
		specializationA.resize(1);
		std::string suffix = stringUtils::replace(specializationB[0], "Vec3", "");
		specializationA[0] = "Matrix44" + suffix;
	}
}

void Matrix44RotationAxis::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<Imath::M44f> &matrix = _matrix->value()->matrix44ValuesSlice(slice);
	int size = matrix.size();
	std::vector<Imath::V3f> axisXValues(size);
	std::vector<Imath::V3f> axisYValues(size);
	std::vector<Imath::V3f> axisZValues(size);
	
	for(int i = 0; i < size; ++i){
		const Imath::M44f &mat = matrix[i];
		axisXValues[i] = Imath::V3f(mat[0][0], mat[0][1], mat[0][2]);
		axisYValues[i] = Imath::V3f(mat[1][0], mat[1][1], mat[1][2]);
		axisZValues[i] = Imath::V3f(mat[2][0], mat[2][1], mat[2][2]);
	}
	
	_axisX->outValue()->setVec3ValuesSlice(slice, axisXValues);
	_axisY->outValue()->setVec3ValuesSlice(slice, axisYValues);
	_axisZ->outValue()->setVec3ValuesSlice(slice, axisZValues);
	
	setAttributeIsClean(_axisX, true);
	setAttributeIsClean(_axisY, true);
	setAttributeIsClean(_axisZ, true);
}

Matrix44FromVectors::Matrix44FromVectors(const std::string &name, Node* parent): Node(name, parent){	
	setSliceable(true);

	_translation = new NumericAttribute("translation", this);
	_axisX = new NumericAttribute("axisX", this);
	_axisY = new NumericAttribute("axisY", this);
	_axisZ = new NumericAttribute("axisZ", this);
	_scale = new NumericAttribute("scale", this);
	_matrix = new NumericAttribute("matrix", this);
	
	addInputAttribute(_translation);
	addInputAttribute(_axisX);
	addInputAttribute(_axisY);
	addInputAttribute(_axisZ);
	addInputAttribute(_scale);
	addOutputAttribute(_matrix);
	
	setAttributeAffect(_translation, _matrix);
	setAttributeAffect(_axisX, _matrix);
	setAttributeAffect(_axisY, _matrix);
	setAttributeAffect(_axisZ, _matrix);
	setAttributeAffect(_scale, _matrix);
	
	std::vector<std::string> vecSpec;
	vecSpec.push_back("Vec3");
	vecSpec.push_back("Vec3Array");
	setAttributeAllowedSpecializations(_translation, vecSpec);
	setAttributeAllowedSpecializations(_axisX, vecSpec);
	setAttributeAllowedSpecializations(_axisY, vecSpec);
	setAttributeAllowedSpecializations(_axisZ, vecSpec);
	setAttributeAllowedSpecializations(_scale, vecSpec);
	
	std::vector<std::string> matrixSpec;
	matrixSpec.push_back("Matrix44");
	matrixSpec.push_back("Matrix44Array");
	setAttributeAllowedSpecializations(_matrix, matrixSpec);
	
	addAttributeSpecializationLink(_translation, _matrix);
	addAttributeSpecializationLink(_axisX, _matrix);
	addAttributeSpecializationLink(_axisY, _matrix);
	addAttributeSpecializationLink(_axisZ, _matrix);
	addAttributeSpecializationLink(_scale, _matrix);
}

void Matrix44FromVectors::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(specializationA.size() < specializationB.size()){
		specializationB.resize(1);
		std::string suffix = stringUtils::replace(specializationA[0], "Vec3", "");
		specializationB[0] = "Matrix44" + suffix;
	}
	else if(specializationB.size() < specializationA.size()){
		specializationA.resize(1);
		std::string suffix = stringUtils::replace(specializationB[0], "Matrix44", "");
		specializationA[0] = "Vec3" + suffix;
	}
}

void Matrix44FromVectors::updateSlice(Attribute *attribute, unsigned int slice){
	Numeric *translation = _translation->value();
	Numeric *axisX = _axisX->value();
	Numeric *axisY = _axisY->value();
	Numeric *axisZ = _axisZ->value();
	Numeric *scale = _scale->value();
	
	NumericAttribute *attrs[] = {
			_translation,
			_axisX,
			_axisY,
			_axisZ,
			_scale};
			
	int minorSize = findMinorNumericSize(attrs, 5, slice);
	if(minorSize < 1)
		minorSize = 1;
		
	std::vector<Imath::M44f> matrixValues(minorSize);
	for(int i = 0; i < minorSize; ++i){
		Imath::V3f translationValue = translation->vec3ValueAtSlice(slice, i);
		Imath::V3f scaleValue = scale->vec3ValueAtSlice(slice, i);
		
		Imath::V3f axisXValue = axisX->vec3ValueAtSlice(slice, i).normalized() * scaleValue.x;
		Imath::V3f axisYValue = axisY->vec3ValueAtSlice(slice, i).normalized() * scaleValue.y;
		Imath::V3f axisZValue = axisZ->vec3ValueAtSlice(slice, i).normalized() * scaleValue.z;
		
		matrixValues[i] = Imath::M44f(
			axisXValue.x, axisXValue.y, axisXValue.z, 0.0,
			axisYValue.x, axisYValue.y, axisYValue.z, 0.0,
			axisZValue.x, axisZValue.y, axisZValue.z, 0.0,
			translationValue.x, translationValue.y, translationValue.z, 1.0);
	}
	
	_matrix->outValue()->setMatrix44ValuesSlice(slice, matrixValues);
}

Matrix44EulerRotation::Matrix44EulerRotation(const std::string &name, Node* parent): Node(name, parent){	
	setSliceable(true);

	_matrix = new NumericAttribute("matrix", this);
	_eulerAngles = new NumericAttribute("eulerAngles", this);
	
	addInputAttribute(_matrix);
	addOutputAttribute(_eulerAngles);
	
	setAttributeAffect(_matrix, _eulerAngles);
	
	std::vector<std::string> matrixSpec;
	matrixSpec.push_back("Matrix44");
	matrixSpec.push_back("Matrix44Array");
	
	setAttributeAllowedSpecializations(_matrix, matrixSpec);
	
	std::vector<std::string> eulerSpec;
	eulerSpec.push_back("Vec3");
	eulerSpec.push_back("Vec3Array");
	
	setAttributeAllowedSpecializations(_eulerAngles, eulerSpec);
	
	addAttributeSpecializationLink(_matrix, _eulerAngles);
}

void Matrix44EulerRotation::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(specializationA.size() < specializationB.size()){
		specializationB.resize(1);
		std::string suffix = stringUtils::replace(specializationA[0], "Matrix44", "");
		specializationB[0] = "Vec3" + suffix;
	}
	else if(specializationB.size() < specializationA.size()){
		specializationA.resize(1);
		std::string suffix = stringUtils::replace(specializationB[0], "Vec3", "");
		specializationA[0] = "Matrix44" + suffix;
	}
}

void Matrix44EulerRotation::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<Imath::M44f> &matrix = _matrix->value()->matrix44ValuesSlice(slice);
	int size = matrix.size();
	
	std::vector<Imath::V3f> eulerAngles(size);
	
	Imath::Eulerf euler;
	float degree = (180 / M_PI);
	for(int i = 0; i < size; ++i){
		euler.extract(matrix[i]);
		Imath::V3f &currentValue = eulerAngles[i];
		
		currentValue.x = euler.x * degree;
		currentValue.y = euler.y * degree;
		currentValue.z = euler.z * degree;
	}
	
	_eulerAngles->outValue()->setVec3ValuesSlice(slice, eulerAngles);
}

RangeLoop::RangeLoop(const std::string &name, Node* parent): 
	Node(name, parent),
	_selectedOperation(0){	
	setSliceable(true);

	_start = new NumericAttribute("start", this);
	_end = new NumericAttribute("end", this);
	_step = new NumericAttribute("step", this);
	_valueInRange = new NumericAttribute("valueInRange", this);
	
	addInputAttribute(_start);
	addInputAttribute(_end);
	addInputAttribute(_step);
	addOutputAttribute(_valueInRange);
	
	setAttributeAffect(_start, _valueInRange);
	setAttributeAffect(_end, _valueInRange);
	setAttributeAffect(_step, _valueInRange);
	
	std::vector<std::string> specialization;
	specialization.push_back("Float");
	specialization.push_back("FloatArray");
	specialization.push_back("Int");
	specialization.push_back("IntArray");
	
	setAttributeAllowedSpecializations(_start, specialization);
	setAttributeAllowedSpecializations(_end, specialization);
	setAttributeAllowedSpecializations(_step, specialization);
	setAttributeAllowedSpecializations(_valueInRange, specialization);
	
	addAttributeSpecializationLink(_start, _valueInRange);
	addAttributeSpecializationLink(_end, _valueInRange);
	addAttributeSpecializationLink(_step, _valueInRange);
}

void RangeLoop::updateFloat(Numeric *start, Numeric *end, Numeric *step, Numeric *out, unsigned int slice){
	float startVal = start->floatValueAtSlice(slice, 0);
	float endVal = end->floatValueAtSlice(slice, 0);
	float stepVal = step->floatValueAtSlice(slice, 0);
	
	float outVal = fmod((stepVal + startVal), endVal);
	out->setFloatValueAtSlice(slice, 0, outVal);
}

void RangeLoop::updateFloatArray(Numeric *start, Numeric *end, Numeric *step, Numeric *out, unsigned int slice){
	NumericAttribute *attrs[] = {_start, _end, _step};
			
	int minorSize = findMinorNumericSize(attrs, 3, slice);
	if(minorSize < 1)
		minorSize = 1;
	
	std::vector<float> outVals(minorSize);
	for(int i = 0; i < minorSize; ++i){
		float startVal = start->floatValueAtSlice(slice, i);
		float endVal = end->floatValueAtSlice(slice, i);
		float stepVal = step->floatValueAtSlice(slice, i);

		outVals[i] = fmod((stepVal + startVal), endVal);
	}
	
	out->setFloatValuesSlice(slice, outVals);
}

void RangeLoop::updateInt(Numeric *start, Numeric *end, Numeric *step, Numeric *out, unsigned int slice){
	int startVal = start->intValueAtSlice(slice, 0);
	int endVal = end->intValueAtSlice(slice, 0);
	int stepVal = step->intValueAtSlice(slice, 0);

	int outVal = 0;
	if(endVal > 0){
		outVal = (stepVal + startVal) % endVal;
	}

	out->setIntValueAtSlice(slice, 0, outVal);
}

void RangeLoop::updateIntArray(Numeric *start, Numeric *end, Numeric *step, Numeric *out, unsigned int slice){
	NumericAttribute *attrs[] = {_start, _end, _step};
			
	int minorSize = findMinorNumericSize(attrs, 3, slice);
	if(minorSize < 1)
		minorSize = 1;
	
	std::vector<int> outVals(minorSize);
	for(int i = 0; i < minorSize; ++i){
		int startVal = start->intValueAtSlice(slice, i);
		int endVal = end->intValueAtSlice(slice, i);
		int stepVal = step->intValueAtSlice(slice, i);

		int outVal = 0;
		if(endVal > 0){
			outVal = (stepVal + startVal) % endVal;
		}

		outVals[i] = outVal;
	}
	
	out->setIntValuesSlice(slice, outVals);
}

void RangeLoop::attributeSpecializationChanged(Attribute *attribute){
	_selectedOperation = 0;
	
	std::vector<std::string> outSpec = _valueInRange->specialization();
	if(outSpec.size() == 1){
		std::string spec = outSpec[0];
		
		if(spec == "Float"){
			_selectedOperation = &RangeLoop::updateFloat;
		}
		else if(spec == "FloatArray"){
			_selectedOperation = &RangeLoop::updateFloatArray;
		}
		else if(spec == "Int"){
			_selectedOperation = &RangeLoop::updateInt;
		}
		else if(spec == "IntArray"){
			_selectedOperation = &RangeLoop::updateIntArray;
		}
	}
}

void RangeLoop::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		Numeric *start = _start->value();
		Numeric *end = _end->value();
		Numeric *step = _step->value();
		Numeric *out = _valueInRange->outValue();
		
		(this->*_selectedOperation)(start, end, step, out, slice);
	}
}

RandomNumber::RandomNumber(const std::string &name, Node* parent): 
	Node(name, parent),
	_selectedOperation(0){	
	setSliceable(true);

	_min = new NumericAttribute("min", this);
	_max = new NumericAttribute("max", this);
	_seed = new NumericAttribute("seed", this);
	_out = new NumericAttribute("out", this);
	
	addInputAttribute(_min);
	addInputAttribute(_max);
	addInputAttribute(_seed);
	addOutputAttribute(_out);
	
	setAttributeAffect(_min, _out);
	setAttributeAffect(_max, _out);
	setAttributeAffect(_seed, _out);
	
	std::vector<std::string> specialization;
	specialization.push_back("Float");
	specialization.push_back("FloatArray");
	specialization.push_back("Int");
	specialization.push_back("IntArray");
	
	setAttributeAllowedSpecializations(_min, specialization);
	setAttributeAllowedSpecializations(_max, specialization);
	setAttributeAllowedSpecialization(_seed, "Int");
	setAttributeAllowedSpecializations(_out, specialization);
	
	addAttributeSpecializationLink(_min, _out);
	addAttributeSpecializationLink(_max, _out);
}

void RandomNumber::attributeSpecializationChanged(Attribute *attribute){
	_selectedOperation = 0;
	
	std::vector<std::string> outSpec = _out->specialization();
	if(outSpec.size() == 1){
		std::string spec = outSpec[0];
		
		if(spec == "Float"){
			_selectedOperation = &RandomNumber::updateFloat;
		}
		else if(spec == "FloatArray"){
			_selectedOperation = &RandomNumber::updateFloatArray;
		}
		else if(spec == "Int"){
			_selectedOperation = &RandomNumber::updateInt;
		}
		else if(spec == "IntArray"){
			_selectedOperation = &RandomNumber::updateIntArray;
		}
	}
	
}

void RandomNumber::updateFloat(Numeric *min, Numeric *max, Numeric *out, unsigned int slice){
	float minVal = min->floatValueAtSlice(slice, 0);
	float maxVal = max->floatValueAtSlice(slice, 0);

	srand(_seed->value()->intValueAtSlice(slice, 0));
	for(float i = minVal; i < maxVal; ++i){
		rand();
	}
	
	float outVal = ((float(rand()) / float(RAND_MAX)) * (maxVal - minVal)) + minVal;

	out->setFloatValueAtSlice(slice, 0, outVal);
}

void RandomNumber::updateFloatArray(Numeric *min, Numeric *max, Numeric *out, unsigned int slice){
	NumericAttribute *attrs[] = {_min, _max};
	
	int minorSize = findMinorNumericSize(attrs, 2, slice);
	if(minorSize < 1)
		minorSize = 1;
	
	srand(_seed->value()->intValueAtSlice(slice, 0));
	std::vector<float> outVals(minorSize);
	for(int i = 0; i < minorSize; ++i){
		float minVal = min->floatValueAtSlice(slice, i);
		float maxVal = max->floatValueAtSlice(slice, i);
		
		outVals[i] =  ((float(rand()) / float(RAND_MAX)) * (maxVal - minVal)) + minVal;
	}
	
	out->setFloatValuesSlice(slice, outVals);
}

void RandomNumber::updateInt(Numeric *min, Numeric *max, Numeric *out, unsigned int slice){
	int minVal = min->intValueAtSlice(slice, 0);
	int maxVal = max->intValueAtSlice(slice, 0);

	srand(_seed->value()->intValueAtSlice(slice, 0));
	for(int i = minVal; i < maxVal; ++i){
		rand();
	}

	int outVal = minVal + (int) (maxVal - minVal + 1)*(rand() / (RAND_MAX + 1.0));

	out->setIntValueAtSlice(slice, 0, outVal);
}

void RandomNumber::updateIntArray(Numeric *min, Numeric *max, Numeric *out, unsigned int slice){
	NumericAttribute *attrs[] = {_min, _max};
			
	int minorSize = findMinorNumericSize(attrs, 2, slice);
	if(minorSize < 1)
		minorSize = 1;
	
	srand(_seed->value()->intValueAtSlice(slice, 0));
	std::vector<int> outVals(minorSize);
	for(int i = 0; i < minorSize; ++i){
		int minVal = min->intValueAtSlice(slice, i);
		int maxVal = max->intValueAtSlice(slice, i);
		
		outVals[i] = minVal + (int) (maxVal - minVal + 1)*(rand() / (RAND_MAX + 1.0));
	}
	
	out->setIntValuesSlice(slice, outVals);
}

void RandomNumber::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		Numeric *min = _min->value();
		Numeric *max = _max->value();
		Numeric *out = _out->outValue();
		
		(this->*_selectedOperation)(min, max, out, slice);
	}
}

ArrayIndices::ArrayIndices(const std::string &name, Node* parent): Node(name, parent){
	setSliceable(true);

	_array = new NumericAttribute("array", this);
	_indices = new NumericAttribute("indices", this);
	
	addInputAttribute(_array);
	addOutputAttribute(_indices);
	
	setAttributeAffect(_array, _indices);
	
	std::vector<std::string> arraySpec;
	arraySpec.push_back("IntArray");
	arraySpec.push_back("FloatArray");
	arraySpec.push_back("Vec3Array");
	arraySpec.push_back("Col4Array");
	arraySpec.push_back("Matrix44Array");
	
	setAttributeAllowedSpecializations(_array, arraySpec);
	setAttributeAllowedSpecialization(_indices, "IntArray");
}

void ArrayIndices::updateSlice(Attribute *attribute, unsigned int slice){
	int arraySize = _array->value()->sizeSlice(slice);
	std::vector<int> indices(arraySize);
	
	for(int i = 0; i < arraySize; ++i){
		indices[i] = i;
	}
	
	_indices->outValue()->setIntValuesSlice(slice, indices);
}

GetArrayElement::GetArrayElement(const std::string &name, Node *parent): 
	Node(name, parent),
	_selectedOperation(0){
	setSliceable(true);

	_array = new NumericAttribute("array", this);
	_index = new NumericAttribute("index", this);
	_element = new NumericAttribute("element", this);
	
	addInputAttribute(_array);
	addInputAttribute(_index);
	addOutputAttribute(_element);
	
	setAttributeAffect(_array, _element);
	setAttributeAffect(_index, _element);
	
	std::vector<std::string> arraySpec;
	arraySpec.push_back("IntArray");
	arraySpec.push_back("FloatArray");
	arraySpec.push_back("Vec3Array");
	arraySpec.push_back("Col4Array");
	arraySpec.push_back("Matrix44Array");
	
	std::vector<std::string> elementSpec;
	elementSpec.push_back("Int");
	elementSpec.push_back("IntArray");
	elementSpec.push_back("Float");
	elementSpec.push_back("FloatArray");
	elementSpec.push_back("Vec3");
	elementSpec.push_back("Vec3Array");
	elementSpec.push_back("Col4");
	elementSpec.push_back("Col4Array");
	elementSpec.push_back("Matrix44");
	elementSpec.push_back("Matrix44Array");
	
	std::vector<std::string> indexSpec;
	indexSpec.push_back("Int");
	indexSpec.push_back("IntArray");
	
	setAttributeAllowedSpecializations(_array, arraySpec);
	setAttributeAllowedSpecializations(_index, indexSpec);
	setAttributeAllowedSpecializations(_element, elementSpec);
	
	addAttributeSpecializationLink(_array, _element);
	addAttributeSpecializationLink(_index, _element);
}

void GetArrayElement::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(attributeA == _array){
		std::vector<std::string> newSpecA;
		for(int i = 0; i < specializationA.size(); ++i){
			std::string &specA = specializationA[i];
			std::string typeA = stringUtils::strip(specA, "Array");
			for(int j = 0; j < specializationB.size(); ++j){
				std::string typeB = stringUtils::strip(specializationB[j], "Array");
				if(typeA == typeB){
					newSpecA.push_back(specA);
					break;
				}
			}
		}
		
		std::vector<std::string> newSpecB;
		for(int i = 0; i < specializationB.size(); ++i){
			std::string &specB = specializationB[i];
			std::string typeB = stringUtils::strip(specB, "Array");
			for(int j = 0; j < specializationA.size(); ++j){
				std::string typeA = stringUtils::strip(specializationA[j], "Array");
				if(typeB == typeA){
					newSpecB.push_back(specB);
					break;
				}
			}
		}
		
		specializationA = newSpecA;
		specializationB = newSpecB;
	}
	else{
		if(specializationA.size() == 1){
			std::string specASuffix = "";
			if(stringUtils::endswith(specializationA[0], "Array")){
				specASuffix = "Array";
			}
			
			std::vector<std::string> newSpecB;
			for(int i = 0; i < specializationB.size(); ++i){
				std::string &specB = specializationB[i];
				
				std::string specBSuffix = "";
				if(stringUtils::endswith(specB, "Array")){
					specBSuffix = "Array";
				}
				
				if(specASuffix == specBSuffix){
					newSpecB.push_back(specB);
				}
			}
			
			specializationB = newSpecB;
		}
		else{
			std::vector<std::string> newSpecA;
			for(int i = 0; i < specializationB.size(); ++i){
				std::string &specB = specializationB[i];
				if(!stringUtils::endswith(specB, "Array")){
					newSpecA.push_back("Int");
					break;
				}
			}
			
			for(int i = 0; i < specializationB.size(); ++i){
				std::string &specB = specializationB[i];
				if(stringUtils::endswith(specB, "Array")){
					newSpecA.push_back("IntArray");
					break;
				}
			}
			
			specializationA = newSpecA;
		}
	}
}

void GetArrayElement::attributeSpecializationChanged(Attribute *attribute){
	if(attribute == _array){
		_selectedOperation = 0;
		
		Numeric::Type type = _array->outValue()->type();
		if(type != Numeric::numericTypeAny){
			if(type == Numeric::numericTypeIntArray){
				_selectedOperation = &GetArrayElement::updateInt;
			}
			else if(type == Numeric::numericTypeFloatArray){
				_selectedOperation = &GetArrayElement::updateFloat;
			}
			else if(type == Numeric::numericTypeVec3Array){
				_selectedOperation = &GetArrayElement::updateVec3;
			}
			else if(type == Numeric::numericTypeCol4Array){
				_selectedOperation = &GetArrayElement::updateCol4;
			}
			else if(type == Numeric::numericTypeMatrix44Array){
				_selectedOperation = &GetArrayElement::updateMatrix44;
			}
		}
	}
}

void GetArrayElement::updateInt(Numeric *array, const std::vector<int> &index, Numeric *element, unsigned int slice){
	int size = index.size();
	element->resize(size);
	for(int i = 0; i < size; ++i){
		element->setIntValueAtSlice(slice, i, array->intValueAtSlice(slice, index[i]));
	}
}

void GetArrayElement::updateFloat(Numeric *array,  const std::vector<int> &index, Numeric *element, unsigned int slice){
	int size = index.size();
	element->resize(size);
	for(int i = 0; i < size; ++i){
		element->setFloatValueAtSlice(slice, i, array->floatValueAtSlice(slice, index[i]));
	}
}

void GetArrayElement::updateVec3(Numeric *array,  const std::vector<int> &index, Numeric *element, unsigned int slice){
	int size = index.size();
	element->resize(size);
	for(int i = 0; i < size; ++i){
		element->setVec3ValueAtSlice(slice, i, array->vec3ValueAtSlice(slice, index[i]));
	}
}

void GetArrayElement::updateCol4(Numeric *array, const std::vector<int> &index, Numeric *element, unsigned int slice){
	int size = index.size();
	element->resize(size);
	for(int i = 0; i < size; ++i){
		element->setCol4ValueAtSlice(slice, i, array->col4ValueAtSlice(slice, index[i]));
	}
}

void GetArrayElement::updateMatrix44(Numeric *array,  const std::vector<int> &index, Numeric *element, unsigned int slice){
	int size = index.size();
	element->resize(size);
	for(int i = 0; i < size; ++i){
		element->setMatrix44ValueAtSlice(slice, i, array->matrix44ValueAtSlice(slice, index[i]));
	}
}

void GetArrayElement::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		Numeric *array = _array->value();
		std::vector<int> index = _index->value()->intValuesSlice(slice);
		Numeric *element = _element->outValue();
		
		(this->*_selectedOperation)(array, index, element, slice);
	}
}

SetArrayElement::SetArrayElement(const std::string &name, Node *parent):
Node(name, parent),
_selectedOperation(0){
	setSliceable(true);

	_array = new NumericAttribute("array", this);
	_index = new NumericAttribute("index", this);
	_element = new NumericAttribute("element", this);
	_outArray = new NumericAttribute("outArray", this);
	
	addInputAttribute(_array);
	addInputAttribute(_index);
	addInputAttribute(_element);
	addOutputAttribute(_outArray);
	
	setAttributeAffect(_array, _outArray);
	setAttributeAffect(_element, _outArray);
	setAttributeAffect(_index, _outArray);
	
	std::vector<std::string> elementSpecs;
	elementSpecs.push_back("Int");
	elementSpecs.push_back("IntArray");
	elementSpecs.push_back("Float");
	elementSpecs.push_back("FloatArray");
	elementSpecs.push_back("Vec3");
	elementSpecs.push_back("Vec3Array");
	elementSpecs.push_back("Col4");
	elementSpecs.push_back("Col4Array");
	elementSpecs.push_back("Matrix44");
	elementSpecs.push_back("Matrix44Array");
	
	std::vector<std::string> arraySpecs;
	arraySpecs.push_back("IntArray");
	arraySpecs.push_back("FloatArray");
	arraySpecs.push_back("Vec3Array");
	arraySpecs.push_back("Col4Array");
	arraySpecs.push_back("Matrix44Array");
	
	std::vector<std::string> indexSpec;
	indexSpec.push_back("Int");
	indexSpec.push_back("IntArray");

	setAttributeAllowedSpecializations(_array, arraySpecs);
	setAttributeAllowedSpecializations(_element, elementSpecs);
	setAttributeAllowedSpecializations(_index, indexSpec);
	setAttributeAllowedSpecializations(_outArray, arraySpecs);
	
	addAttributeSpecializationLink(_array, _outArray);
	addAttributeSpecializationLink(_element, _outArray);
}

void SetArrayElement::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(attributeA == _array){
		Node::updateSpecializationLink(attributeA, attributeB, specializationA, specializationB);
	}
	else{
		std::vector<std::string> newSpecA;
		std::vector<std::string> newSpecB;
		for(int i = 0; i < specializationA.size(); ++i){
			std::string &specA = specializationA[i];
			std::string typeA = stringUtils::strip(specA, "Array");
			for(int j = 0; j < specializationB.size(); ++j){
				std::string &specB = specializationB[j];
				std::string typeB = stringUtils::strip(specB, "Array");
				if(typeA == typeB){
					if(!containerUtils::elementInContainer(specB, newSpecB)){
						newSpecB.push_back(specB);
					}
					if(!containerUtils::elementInContainer(specA, newSpecA)){
						newSpecA.push_back(specA);
					}
				}
			}
		}

		specializationA = newSpecA;
		specializationB = newSpecB;
	}
}

void SetArrayElement::attributeSpecializationChanged(Attribute *attribute){
	if(attribute == _array){
		_selectedOperation = 0;
		
		Numeric::Type type = _array->outValue()->type();
		if(type != Numeric::numericTypeAny){
			if(type == Numeric::numericTypeIntArray){
				_selectedOperation = &SetArrayElement::updateInt;
			}
			else if(type == Numeric::numericTypeFloatArray){
				_selectedOperation = &SetArrayElement::updateFloat;
			}
			else if(type == Numeric::numericTypeVec3Array){
				_selectedOperation = &SetArrayElement::updateVec3;
			}
			else if(type == Numeric::numericTypeCol4Array){
				_selectedOperation = &SetArrayElement::updateCol4;
			}
			else if(type == Numeric::numericTypeMatrix44Array){
				_selectedOperation = &SetArrayElement::updateMatrix44;
			}
		}
	}
}

void SetArrayElement::updateInt(Numeric *array, const std::vector<int> &index, Numeric *element, Numeric *outArray, unsigned int slice){
	std::vector<int> values = array->intValuesSlice(slice);
	int valuesSize = array->sizeSlice(slice);
	for(int i = 0; i < index.size(); ++i){
		int currentIndex = index[i];
		if(currentIndex >= 0 && currentIndex < valuesSize){
			values[currentIndex] = element->intValueAtSlice(slice, i);
		}
	}

	outArray->setIntValuesSlice(slice, values);
}

void SetArrayElement::updateFloat(Numeric *array, const std::vector<int> &index, Numeric *element, Numeric *outArray, unsigned int slice){
	std::vector<float> values = array->floatValuesSlice(slice);
	int valuesSize = array->sizeSlice(slice);
	for(int i = 0; i < index.size(); ++i){
		int currentIndex = index[i];
		if(currentIndex >= 0 && currentIndex < valuesSize){
			values[currentIndex] = element->floatValueAtSlice(slice, i);
		}
	}

	outArray->setFloatValuesSlice(slice, values);
}

void SetArrayElement::updateVec3(Numeric *array, const std::vector<int> &index, Numeric *element, Numeric *outArray, unsigned int slice){
	std::vector<Imath::V3f> values = array->vec3ValuesSlice(slice);
	int valuesSize = array->sizeSlice(slice);
	for(int i = 0; i < index.size(); ++i){
		int currentIndex = index[i];
		if(currentIndex >= 0 && currentIndex < valuesSize){
			values[currentIndex] = element->vec3ValueAtSlice(slice, i);
		}
	}

	outArray->setVec3ValuesSlice(slice, values);
}

void SetArrayElement::updateCol4(Numeric *array, const std::vector<int> &index, Numeric *element, Numeric *outArray, unsigned int slice){
	std::vector<Imath::Color4f> values = array->col4ValuesSlice(slice);
	int valuesSize = array->sizeSlice(slice);
	for(int i = 0; i < index.size(); ++i){
		int currentIndex = index[i];
		if(currentIndex >= 0 && currentIndex < valuesSize){
			values[currentIndex] = element->col4ValueAtSlice(slice, i);
		}
	}

	outArray->setCol4ValuesSlice(slice, values);
}

void SetArrayElement::updateMatrix44(Numeric *array, const std::vector<int> &index, Numeric *element, Numeric *outArray, unsigned int slice){
	std::vector<Imath::M44f> values = array->matrix44ValuesSlice(slice);
	int valuesSize = array->sizeSlice(slice);
	for(int i = 0; i < index.size(); ++i){
		int currentIndex = index[i];
		if(currentIndex >= 0 && currentIndex < valuesSize){
			values[currentIndex] = element->matrix44ValueAtSlice(slice, i);
		}
	}

	outArray->setMatrix44ValuesSlice(slice, values);
}

void SetArrayElement::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		Numeric *array = _array->value();
		const std::vector<int> &index = _index->value()->intValuesSlice(slice);
		Numeric *element = _element->value();
		Numeric *outArray = _outArray->outValue();
		
		(this->*_selectedOperation)(array, index, element, outArray, slice);
	}
}

SetSimulationStep::SetSimulationStep(const std::string &name, Node *parent): 
Node(name, parent),
_selectedOperation(0){
	setSliceable(true);

	_storageKey = new StringAttribute("storageKey", this);
	_data = new NumericAttribute("data", this);
	_result = new NumericAttribute("result", this);
	
	addInputAttribute(_storageKey);
	addInputAttribute(_data);
	addOutputAttribute(_result);
	
	setAttributeAffect(_storageKey, _result);
	setAttributeAffect(_data, _result);
	
	addAttributeSpecializationLink(_data, _result);
}

void SetSimulationStep::attributeSpecializationChanged(Attribute *attribute){
	if(attribute == _data){
		_selectedOperation = 0;
		
		Numeric::Type type = _data->outValue()->type();
		if(type != Numeric::numericTypeAny){
			if(type == Numeric::numericTypeInt || type == Numeric::numericTypeIntArray){
				_selectedOperation = &SetSimulationStep::updateInt;
			}
			else if(type == Numeric::numericTypeFloat || type == Numeric::numericTypeFloatArray){
				_selectedOperation = &SetSimulationStep::updateFloat;
			}
			else if(type == Numeric::numericTypeVec3 || type == Numeric::numericTypeVec3Array){
				_selectedOperation = &SetSimulationStep::updateVec3;
			}
			else if(type == Numeric::numericTypeCol4 || type == Numeric::numericTypeCol4Array){
				_selectedOperation = &SetSimulationStep::updateCol4;
			}
			else if(type == Numeric::numericTypeMatrix44 || type == Numeric::numericTypeMatrix44Array){
				_selectedOperation = &SetSimulationStep::updateMatrix44;
			}
			else if(type == Numeric::numericTypeQuat || type == Numeric::numericTypeQuatArray){
				_selectedOperation = &SetSimulationStep::updateQuat;
			}
		}
	}
}

void SetSimulationStep::updateInt(const std::string &storageKey, Numeric *data, Numeric *result, unsigned int slice){
	_globalNumericStorage[storageKey].setIntValuesSlice(slice, data->intValuesSlice(slice));
	result->setIntValuesSlice(slice, data->intValuesSlice(slice));
}

void SetSimulationStep::updateFloat(const std::string &storageKey, Numeric *data, Numeric *result, unsigned int slice){
	_globalNumericStorage[storageKey].setFloatValuesSlice(slice, data->floatValuesSlice(slice));
	result->setFloatValuesSlice(slice, data->floatValuesSlice(slice));
}

void SetSimulationStep::updateVec3(const std::string &storageKey, Numeric *data, Numeric *result, unsigned int slice){
	_globalNumericStorage[storageKey].setVec3ValuesSlice(slice, data->vec3ValuesSlice(slice));
	result->setVec3ValuesSlice(slice, data->vec3ValuesSlice(slice));
}

void SetSimulationStep::updateCol4(const std::string &storageKey, Numeric *data, Numeric *result, unsigned int slice){
	_globalNumericStorage[storageKey].setCol4ValuesSlice(slice, data->col4ValuesSlice(slice));
	result->setCol4ValuesSlice(slice, data->col4ValuesSlice(slice));
}

void SetSimulationStep::updateMatrix44(const std::string &storageKey, Numeric *data, Numeric *result, unsigned int slice){
	_globalNumericStorage[storageKey].setMatrix44ValuesSlice(slice, data->matrix44ValuesSlice(slice));
	result->setMatrix44ValuesSlice(slice, data->matrix44ValuesSlice(slice));
}

void SetSimulationStep::updateQuat(const std::string &storageKey, Numeric *data, Numeric *result, unsigned int slice){
	_globalNumericStorage[storageKey].setQuatValuesSlice(slice, data->quatValuesSlice(slice));
	result->setQuatValuesSlice(slice, data->quatValuesSlice(slice));
}

void SetSimulationStep::resizedSlices(unsigned int slices){
	_globalNumericStorage[_storageKey->value()->stringValue()].resizeSlices(slices);
}

void SetSimulationStep::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		#ifdef CORAL_PARALLEL_TBB
			tbb::mutex::scoped_lock lock(_globalMutex); // block setting _globalNumericStorage from two different threads
		#endif

		(this->*_selectedOperation)(_storageKey->value()->stringValue(), _data->value(), _result->outValue(), slice);
	}
}

GetSimulationStep::GetSimulationStep(const std::string &name, Node *parent): 
Node(name, parent),
_selectedOperation(0){
	setSliceable(true);

	_storageKey = new StringAttribute("storageKey", this);
	_source = new NumericAttribute("source", this);
	_step = new NumericAttribute("step", this);
	_data = new NumericAttribute("data", this);
	
	addInputAttribute(_storageKey);
	addInputAttribute(_source);
	addInputAttribute(_step);
	addOutputAttribute(_data);
	
	setAttributeAffect(_storageKey, _data);
	setAttributeAffect(_source, _data);
	setAttributeAffect(_step, _data);
	
	std::vector<std::string> spec;
	spec.push_back("Int");
	spec.push_back("Float");
	setAttributeAllowedSpecializations(_step, spec);
	
	addAttributeSpecializationLink(_source, _data);
}

void GetSimulationStep::attributeSpecializationChanged(Attribute *attribute){
	if(attribute == _source){
		_selectedOperation = 0;
		
		Numeric::Type type = _source->outValue()->type();
		if(type != Numeric::numericTypeAny){
			if(type == Numeric::numericTypeInt || type == Numeric::numericTypeIntArray){
				_selectedOperation = &GetSimulationStep::updateInt;
			}
			else if(type == Numeric::numericTypeFloat || type == Numeric::numericTypeFloatArray){
				_selectedOperation = &GetSimulationStep::updateFloat;
			}
			else if(type == Numeric::numericTypeVec3 || type == Numeric::numericTypeVec3Array){
				_selectedOperation = &GetSimulationStep::updateVec3;
			}
			else if(type == Numeric::numericTypeCol4 || type == Numeric::numericTypeCol4Array){
				_selectedOperation = &GetSimulationStep::updateCol4;
			}
			else if(type == Numeric::numericTypeMatrix44 || type == Numeric::numericTypeMatrix44Array){
				_selectedOperation = &GetSimulationStep::updateMatrix44;
			}
			else if(type == Numeric::numericTypeQuat || type == Numeric::numericTypeQuatArray){
				_selectedOperation = &GetSimulationStep::updateQuat;
			}
		}
	}
}

void GetSimulationStep::updateInt(const std::string &storageKey, int step, Numeric *source, Numeric *data, unsigned int slice){
	if(step <= 0 || _globalNumericStorage.find(storageKey) == _globalNumericStorage.end()){
		data->setIntValuesSlice(slice, source->intValuesSlice(slice));
	}
	else{
		data->setIntValuesSlice(slice, _globalNumericStorage[storageKey].intValuesSlice(slice));
	}
}

void GetSimulationStep::updateFloat(const std::string &storageKey, int step, Numeric *source, Numeric *data, unsigned int slice){
	if(step <= 0 || _globalNumericStorage.find(storageKey) == _globalNumericStorage.end()){
		data->setFloatValuesSlice(slice, source->floatValuesSlice(slice));
	}
	else{
		data->setFloatValuesSlice(slice, _globalNumericStorage[storageKey].floatValuesSlice(slice));
	}
}

void GetSimulationStep::updateVec3(const std::string &storageKey, int step, Numeric *source, Numeric *data, unsigned int slice){
	if(step <= 0 || _globalNumericStorage.find(storageKey) == _globalNumericStorage.end()){
		data->setVec3ValuesSlice(slice, source->vec3ValuesSlice(slice));
	}
	else{
		data->setVec3ValuesSlice(slice, _globalNumericStorage[storageKey].vec3ValuesSlice(slice));
	}
}

void GetSimulationStep::updateCol4(const std::string &storageKey, int step, Numeric *source, Numeric *data, unsigned int slice){
	if(step <= 0 || _globalNumericStorage.find(storageKey) == _globalNumericStorage.end()){
		data->setCol4ValuesSlice(slice, source->col4ValuesSlice(slice));
	}
	else{
		data->setCol4ValuesSlice(slice, _globalNumericStorage[storageKey].col4ValuesSlice(slice));
	}
}

void GetSimulationStep::updateMatrix44(const std::string &storageKey, int step, Numeric *source, Numeric *data, unsigned int slice){
	if(step <= 0 || _globalNumericStorage.find(storageKey) == _globalNumericStorage.end()){
		data->setMatrix44ValuesSlice(slice, source->matrix44ValuesSlice(slice));
	}
	else{
		data->setMatrix44ValuesSlice(slice, _globalNumericStorage[storageKey].matrix44ValuesSlice(slice));
	}
}

void GetSimulationStep::updateQuat(const std::string &storageKey, int step, Numeric *source, Numeric *data, unsigned int slice){
	if(step <= 0 || _globalNumericStorage.find(storageKey) == _globalNumericStorage.end()){
		data->setQuatValuesSlice(slice, source->quatValuesSlice(slice));
	}
	else{
		data->setQuatValuesSlice(slice, _globalNumericStorage[storageKey].quatValuesSlice(slice));
	}
}

void GetSimulationStep::updateSlice(Attribute *attribute, unsigned int slice){
	if(_selectedOperation){
		Numeric *step = _step->value();

		int stepVal;
		if(step->type() == Numeric::numericTypeFloat){
			stepVal = int(step->floatValueAtSlice(slice, 0));
		}
		else if(step->type() == Numeric::numericTypeInt){
			stepVal = step->intValueAtSlice(slice, 0);
		}

		(this->*_selectedOperation)(_storageKey->value()->stringValue(), stepVal, _source->value(), _data->outValue(), slice);
	}
}

QuatToAxisAngle::QuatToAxisAngle(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);

	_quat = new NumericAttribute("quat",this);
	_axis = new NumericAttribute("axis",this);
	_angle = new NumericAttribute("angle",this);

	addInputAttribute(_quat);
	addOutputAttribute(_axis);
	addOutputAttribute(_angle);

	setAttributeAffect(_quat, _axis);
	setAttributeAffect(_quat, _angle);

	std::vector<std::string> quatSpecializations;
	quatSpecializations.push_back("Quat");
	quatSpecializations.push_back("QuatArray");

	std::vector<std::string> angleSpecializations;
	angleSpecializations.push_back("Float");
	angleSpecializations.push_back("FloatArray");

	std::vector<std::string> axisSpecializations;
	axisSpecializations.push_back("Vec3");
	axisSpecializations.push_back("Vec3Array");

	setAttributeAllowedSpecializations(_quat, quatSpecializations);
	setAttributeAllowedSpecializations(_axis, axisSpecializations);
	setAttributeAllowedSpecializations(_angle, angleSpecializations);

	addAttributeSpecializationLink(_quat, _axis);
	addAttributeSpecializationLink(_axis, _angle);
}

void QuatToAxisAngle::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB)
{
	if(specializationA.size() < specializationB.size()){
		std::string specB = specializationB[0];
		specializationB.resize(1);
		if(stringUtils::endswith(specializationA[0], "Array")){
			specializationB[0] = specB + "Array";
		}
		else{
			specializationB[0] = specB;
		}
	}
	else if(specializationB.size() < specializationA.size()){
		std::string specA = specializationA[0];
		specializationA.resize(1);
		
		if(stringUtils::endswith(specializationB[0], "Array")){
			specializationA[0] = specA + "Array";
		}
		else{
			specializationA[0] = specA;
		}
	}
}

void QuatToAxisAngle::updateSlice(Attribute *attribute, unsigned int slice)
{
	const std::vector<Imath::Quatf> &quatValues = _quat->value()->quatValuesSlice(slice);
	int size = quatValues.size();

	std::vector<Imath::V3f> axisValues(size);
	std::vector<float> angleValues(size);

	for(int i = 0; i < size; ++i){
		const Imath::Quatf &quat = quatValues[i];
		axisValues[i] = quat.axis();
		angleValues[i] =quat.angle();
	}

	_axis->outValue()->setVec3ValuesSlice(slice, axisValues);
	_angle->outValue()->setFloatValuesSlice(slice, angleValues);

	setAttributeIsClean(_axis, true);
	setAttributeIsClean(_angle, true);
}

QuatToEulerRotation::QuatToEulerRotation(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);

	_quat = new NumericAttribute("quat",this);
	_euler = new NumericAttribute("euler",this);

	addInputAttribute(_quat);
	addOutputAttribute(_euler);

	setAttributeAffect(_quat, _euler);

	std::vector<std::string> quatSpecializations;
	quatSpecializations.push_back("Quat");
	quatSpecializations.push_back("QuatArray");

	std::vector<std::string> eulerSpecializations;
	eulerSpecializations.push_back("Vec3");
	eulerSpecializations.push_back("Vec3Array");

	setAttributeAllowedSpecializations(_quat, quatSpecializations);
	setAttributeAllowedSpecializations(_euler, eulerSpecializations);

	addAttributeSpecializationLink(_quat, _euler);
}

void QuatToEulerRotation::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB)
{
	if(specializationA.size() < specializationB.size()){
		std::string specB = specializationB[0];
		specializationB.resize(1);
		if(stringUtils::endswith(specializationA[0], "Array")){
			specializationB[0] = specB + "Array";
		}
		else{
			specializationB[0] = specB;
		}
	}
	else if(specializationB.size() < specializationA.size()){
		std::string specA = specializationA[0];
		specializationA.resize(1);

		if(stringUtils::endswith(specializationB[0], "Array")){
			specializationA[0] = specA + "Array";
		}
		else{
			specializationA[0] = specA;
		}
	}
}

void QuatToEulerRotation::updateSlice(Attribute *attribute, unsigned int slice)
{
	const std::vector<Imath::Quatf> &quatValues = _quat->value()->quatValuesSlice(slice);
	int size = quatValues.size();

	std::vector<Imath::V3f> eulerValues(size);

	float degree = (180 / M_PI);
	Imath::Eulerf euler;
	Imath::M44f mtx;

	for(int i = 0; i < size; ++i){
		Imath::Quatf quat = quatValues[i];
		quat.normalize();
		mtx = quat.toMatrix44();

		euler.extract(mtx);
		eulerValues[i].x = euler.x * degree;
		eulerValues[i].y = euler.y * degree;
		eulerValues[i].z = euler.z * degree;
	}

	_euler->outValue()->setVec3ValuesSlice(slice, eulerValues);

	setAttributeIsClean(_euler, true);
}

QuatToMatrix44::QuatToMatrix44(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);

	_quat = new NumericAttribute("quat",this);
	_matrix = new NumericAttribute("matrix",this);

	addInputAttribute(_quat);
	addOutputAttribute(_matrix);

	setAttributeAffect(_quat, _matrix);

	std::vector<std::string> quatSpecializations;
	quatSpecializations.push_back("Quat");
	quatSpecializations.push_back("QuatArray");

	std::vector<std::string> matrixSpecializations;
	matrixSpecializations.push_back("Matrix44");
	matrixSpecializations.push_back("Matrix44Array");

	setAttributeAllowedSpecializations(_quat, quatSpecializations);
	setAttributeAllowedSpecializations(_matrix, matrixSpecializations);

	addAttributeSpecializationLink(_quat, _matrix);
}

void QuatToMatrix44::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(specializationA.size() < specializationB.size()){
		std::string specB = specializationB[0];
		specializationB.resize(1);
		if(stringUtils::endswith(specializationA[0], "Array")){
			specializationB[0] = specB + "Array";
		}
		else{
			specializationB[0] = specB;
		}
	}
	else if(specializationB.size() < specializationA.size()){
		std::string specA = specializationA[0];
		specializationA.resize(1);

		if(stringUtils::endswith(specializationB[0], "Array")){
			specializationA[0] = specA + "Array";
		}
		else{
			specializationA[0] = specA;
		}
	}
}

void QuatToMatrix44::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<Imath::Quatf> &quatValues = _quat->value()->quatValuesSlice(slice);
	int size = quatValues.size();

	std::vector<Imath::M44f> matrixValues(size);

	for(int i = 0; i < size; ++i){
		Imath::Quatf quat = quatValues[i];
		quat.normalize();
		matrixValues[i] = quat.toMatrix44();
	}

	_matrix->outValue()->setMatrix44ValuesSlice(slice, matrixValues);

	setAttributeIsClean(_matrix, true);
}

Matrix44ToQuat::Matrix44ToQuat(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);

	_quat = new NumericAttribute("quat",this);
	_matrix = new NumericAttribute("matrix",this);

	addInputAttribute(_matrix);
	addOutputAttribute(_quat);

	setAttributeAffect(_matrix,_quat);

	std::vector<std::string> quatSpecializations;
	quatSpecializations.push_back("Quat");
	quatSpecializations.push_back("QuatArray");

	std::vector<std::string> matrixSpecializations;
	matrixSpecializations.push_back("Matrix44");
	matrixSpecializations.push_back("Matrix44Array");

	setAttributeAllowedSpecializations(_quat, quatSpecializations);
	setAttributeAllowedSpecializations(_matrix, matrixSpecializations);

	addAttributeSpecializationLink(_matrix,_quat);
}

void Matrix44ToQuat::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(specializationA.size() < specializationB.size()){
		std::string specB = specializationB[0];
		specializationB.resize(1);
		if(stringUtils::endswith(specializationA[0], "Array")){
			specializationB[0] = specB + "Array";
		}
		else{
			specializationB[0] = specB;
		}
	}
	else if(specializationB.size() < specializationA.size()){
		std::string specA = specializationA[0];
		specializationA.resize(1);

		if(stringUtils::endswith(specializationB[0], "Array")){
			specializationA[0] = specA + "Array";
		}
		else{
			specializationA[0] = specA;
		}
	}
}

void Matrix44ToQuat::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<Imath::M44f> &mtxValues = _matrix->value()->matrix44ValuesSlice(slice);
	int size = mtxValues.size();

	std::vector<Imath::Quatf> quatValues(size);

	for(int i = 0; i < size; ++i){
		const Imath::M44f& kRot = mtxValues[i];

		float fTrace = kRot[0][0]+kRot[1][1]+kRot[2][2];
		float fRoot;

		if ( fTrace > 0.0 )
		{
			// |w| > 1/2, may as well choose w > 1/2
			fRoot = sqrt(fTrace + 1.0);  // 2w
			quatValues[i].r = 0.5*fRoot;
			fRoot = 0.5/fRoot;  // 1/(4w)
			quatValues[i].v.x = -(kRot[2][1]-kRot[1][2])*fRoot;
			quatValues[i].v.y = -(kRot[0][2]-kRot[2][0])*fRoot;
			quatValues[i].v.z = -(kRot[1][0]-kRot[0][1])*fRoot;
		}
		else
		{
			// |w| <= 1/2
			static unsigned int s_iNext[3] = { 1, 2, 0 };
			unsigned int ii = 0;
			if ( kRot[1][1] > kRot[0][0] )
				ii = 1;
			if ( kRot[2][2] > kRot[ii][ii] )
				ii = 2;
			unsigned int j = s_iNext[ii];
			unsigned int k = s_iNext[j];

			fRoot = sqrt(kRot[ii][ii]-kRot[j][j]-kRot[k][k] + 1.0);
			float* apkQuat[3] = { &(quatValues[i].v.x), &(quatValues[i].v.y), &(quatValues[i].v.z) };
			*apkQuat[ii] = -0.5*fRoot;
			fRoot = 0.5/fRoot;
			quatValues[i].r = (kRot[k][j]-kRot[j][k])*fRoot;
			*apkQuat[j] = -(kRot[j][ii]+kRot[ii][j])*fRoot;
			*apkQuat[k] = -(kRot[k][ii]+kRot[ii][k])*fRoot;
		}
	}

	_quat->outValue()->setQuatValuesSlice(slice, quatValues);

	setAttributeIsClean(_quat, true);
}

