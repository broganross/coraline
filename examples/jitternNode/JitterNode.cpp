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
#include <coral/pythonWrapperUtils.h>
#include <ImathVec.h>
#include <ImathRandom.h>
#include <coral/StringAttribute.h>

using namespace coral;

class JitterNode: public Node{
public:
	JitterNode(const std::string &name, Node *parent): Node(name, parent){		
		_points = new NumericAttribute("points", this);
		_time = new NumericAttribute("time", this);
		_scale = new NumericAttribute("scale", this);
		_str = new NumericAttribute("str", this);
		_outPoints = new NumericAttribute("outPoints", this);
		
		addInputAttribute(_points);
		addInputAttribute(_time);
		addInputAttribute(_scale);
		addInputAttribute(_str);
		addOutputAttribute(_outPoints);
		
		setAttributeAffect(_points, _outPoints);
		setAttributeAffect(_time, _outPoints);
		setAttributeAffect(_scale, _outPoints);
		
		setAttributeAllowedSpecialization(_points, "Vec3Array");
		setAttributeAllowedSpecialization(_time, "Float");
		setAttributeAllowedSpecialization(_scale, "Float");
		setAttributeAllowedSpecialization(_outPoints, "Vec3Array");
		
		_scale->outValue()->setFloatValueAt(0, 1.0);
	}
	
	void update(Attribute *attribute){
		Numeric *points = _points->value();
		float time = _time->value()->floatValueAt(0);
		float scale = _scale->value()->floatValueAt(0);
		Numeric *outPoints = _outPoints->outValue();
		
		int pointsSize = points->size();
		std::vector<Imath::V3f> newPoints(pointsSize);
		Imath::Rand32 rand;
		for(int i = 0; i < pointsSize; ++i){
			Imath::V3f point = points->vec3ValueAt(i);
			
			point.y += sin(time + rand.nextf(-1.0, 1.0)) * scale;
			
			newPoints[i] = point;
		}
		
		outPoints->setVec3Values(newPoints);
	}
	
	// Because nodes are wrapped in python we can expose custom methods that were not part of the original interface.
	std::string sayHi(){
		return "Hi : )";
	}

private:
	NumericAttribute *_points;
	NumericAttribute *_time;
	NumericAttribute *_scale;
	StringAttribute *_str;
	NumericAttribute *_outPoints;
};

BOOST_PYTHON_MODULE(jitterNode){
	// Here we wrap the JitterNode as a python module, we specify the node to wrap and its parent class.
	// Exposing nodes to python is the only way to register a new node in coral, 
	// this is especially good if we want our node to expose new methods.
	pythonWrapperUtils::pythonWrapper<JitterNode, Node>("JitterNode")
		.def("sayHi", &JitterNode::sayHi);
}

#endif
