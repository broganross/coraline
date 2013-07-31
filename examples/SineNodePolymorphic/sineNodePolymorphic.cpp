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
#ifndef EXAMPLE_JITTERNODE_H
#define EXAMPLE_JITTERNODE_H

#include <math.h>
#include <boost/python.hpp>
#include <coral/Node.h>
#include <coral/NumericAttribute.h>
#include <coral/mathUtils.h>
#include <coral/pythonWrapperUtils.h>
#include <ImathVec.h>

using namespace coral;

/*
This example show how one node can work with different data type.
This node accepts one float as input and computes a sine wave for one point, 
but it also accepts an array of floats as input and in that case the output is an array of points.

This is particularly handy when you want to code a single node that can deal with as many contexts as required.
*/

class SineNodePolymorphic: public Node{
public:
	SineNodePolymorphic(const std::string &name, Node *parent): Node(name, parent){		
		_time = new NumericAttribute("time", this);
		_outPoint = new NumericAttribute("outPoints", this);
		
		addInputAttribute(_time);
		addOutputAttribute(_outPoint);
		
		setAttributeAffect(_time, _outPoint);
		
		// here we tell each attribute what types it mutate into.
		std::vector<std::string> timeSpecialization;
		timeSpecialization.push_back("Float");
		timeSpecialization.push_back("FloatArray");
		
		std::vector<std::string> outPointSpecialization;
		outPointSpecialization.push_back("Vec3");
		outPointSpecialization.push_back("Vec3Array");
		
		setAttributeAllowedSpecializations(_time, timeSpecialization);
		setAttributeAllowedSpecializations(_outPoint, outPointSpecialization);
		
		// this establishes a link between the two attributes, this link will be resolved by updateSpecializationLink.
		addAttributeSpecializationLink(_time, _outPoint);
	}
	
	void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
		// This overloaded method is called when the specialization of one of the two linked attributes changes.
		// In this case if one of the two attributes becomes a single value or an array, 
		// the other must also become single or array accordingly.
		
		if(specializationA.size() == 1){
			std::vector<std::string> newSpecialization;
			if(specializationA[0] == "Float"){
				newSpecialization.push_back("Vec3");
			}
			else{
				newSpecialization.push_back("Vec3Array");
			}
			specializationB = newSpecialization;
		}
		else if(specializationB.size() == 1){
			std::vector<std::string> newSpecialization;
			if(specializationB[0] == "Vec3"){
				newSpecialization.push_back("Float");
			}
			else{
				newSpecialization.push_back("FloatArray");
			}
			specializationA = newSpecialization;
		}
	}
	
	void update(Attribute *attribute){
		// Numeric stores its values in arrays even when it contains only one value, 
		// so we can safely run a for loop regardless if this node is working with singles or arrays. 
		
		Numeric *time = _time->value();
		
		int size = time->size();
		std::vector<Imath::V3f> outPoints(size);
		for(int i = 0; i < size; ++i){
			float timeValue = time->floatValueAt(i);
			float sinValue = sin(timeValue);
			
			outPoints[i] = Imath::V3f(sinValue, float(i), 0.0);
		}
		
		_outPoint->outValue()->setVec3Values(outPoints);
	}

private:
	NumericAttribute *_time;
	NumericAttribute *_outPoint;
};

BOOST_PYTHON_MODULE(sineNodePolymorphic){
	pythonWrapperUtils::pythonWrapper<SineNodePolymorphic, Node>("SineNodePolymorphic");
}

#endif
