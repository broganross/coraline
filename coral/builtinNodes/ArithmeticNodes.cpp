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

#include "ArithmeticNodes.h"
#include "../src/Numeric.h"
#include "../src/containerUtils.h"

using namespace coral;

ArithmeticNode::ArithmeticNode(const std::string &name, Node *parent) : Node(name, parent){
	setSliceable(true);

	_in0 = new NumericAttribute("in0", this);
	_in1 = new NumericAttribute("in1", this);
	_out = new NumericAttribute("out", this);
	
	addInputAttribute(_in0);
	addInputAttribute(_in1);
	addOutputAttribute(_out);
	
	setAttributeAffect(_in0, _out);
	setAttributeAffect(_in1, _out);
	
	addAttributeSpecializationLink(_in0, _out);
	addAttributeSpecializationLink(_in1, _out);
}

void ArithmeticNode::updateNumericOperationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	std::vector<std::string> newSpecializationA;
	std::vector<std::string> newSpecializationB;

	for(int i = 0; i < specializationB.size(); ++i){
		for(int j = 0; j < specializationA.size(); ++j){
			std::string typeAStr = specializationA[j];
			std::string typeBStr = specializationB[i];
			
			Numeric::Type typeA = NumericAttribute::numericTypeFromString(typeAStr);
			Numeric::Type typeB = NumericAttribute::numericTypeFromString(typeBStr);
			
			if(NumericOperation::allowOperation(_numericOperation.operation(), typeB, typeA)){
				if(!containerUtils::elementInContainer(typeAStr, newSpecializationA)){
					newSpecializationA.push_back(typeAStr);
				}
		
				if(!containerUtils::elementInContainer(typeBStr, newSpecializationB)){
					newSpecializationB.push_back(typeBStr);
				}
			}
		}
	}
	
	specializationA = newSpecializationA;
	specializationB = newSpecializationB;
}

void ArithmeticNode::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(attributeA == _in0 && attributeB == _out){
		Node::updateSpecializationLink(attributeA, attributeB, specializationA, specializationB);
	}
	else if(attributeA == _in1 && attributeB == _out){
		updateNumericOperationLink(attributeA, attributeB, specializationA, specializationB);
	}
}

void ArithmeticNode::attributeSpecializationChanged(Attribute *attribute){
	_numericOperation.clearSelectedOperation();
	
	Numeric::Type in0Type = _in0->outValue()->type();
	Numeric::Type in1Type = _in1->outValue()->type();
	
	if(in0Type != Numeric::numericTypeAny){
		_numericOperation.selectOperands(in0Type, in1Type);		
	}
}

void ArithmeticNode::updateSlice(Attribute *attribute, unsigned int slice){
	if(_numericOperation.operationSelected()){
		Numeric *in0 = _in0->value();
		Numeric *in1 = _in1->value();
		Numeric *out = _out->outValue();
		
		_numericOperation.executeSelectedOperation(in0, in1, out, slice);
	}
}

AddNode::AddNode(const std::string &name, Node *parent) : ArithmeticNode(name, parent){
	std::vector<std::string> specs;
	specs.push_back("Int");
	specs.push_back("IntArray");
	specs.push_back("Float");
	specs.push_back("FloatArray");
	specs.push_back("Vec3");
	specs.push_back("Vec3Array");
	specs.push_back("Col4");
	specs.push_back("Col4Array");
	
	setAttributeAllowedSpecializations(_in0, specs);
	setAttributeAllowedSpecializations(_in1, specs);
	setAttributeAllowedSpecializations(_out, specs);
	
	numericOperation().setOperation(NumericOperation::numericOperationAdd);
}

SubNode::SubNode(const std::string &name, Node *parent) : ArithmeticNode(name, parent){
	std::vector<std::string> specs;
	specs.push_back("Int");
	specs.push_back("IntArray");
	specs.push_back("Float");
	specs.push_back("FloatArray");
	specs.push_back("Vec3");
	specs.push_back("Vec3Array");
	specs.push_back("Col4");
	specs.push_back("Col4Array");
	
	setAttributeAllowedSpecializations(_in0, specs);
	setAttributeAllowedSpecializations(_in1, specs);
	setAttributeAllowedSpecializations(_out, specs);
	
	numericOperation().setOperation(NumericOperation::numericOperationSub);
}

MulNode::MulNode(const std::string &name, Node *parent) : ArithmeticNode(name, parent){
	numericOperation().setOperation(NumericOperation::numericOperationMul);
}

DivNode::DivNode(const std::string &name, Node *parent) : ArithmeticNode(name, parent){
	std::vector<std::string> specs;
	specs.push_back("Int");
	specs.push_back("IntArray");
	specs.push_back("Float");
	specs.push_back("FloatArray");
	specs.push_back("Vec3");
	specs.push_back("Vec3Array");
	specs.push_back("Col4");
	specs.push_back("Col4Array");
	
	setAttributeAllowedSpecializations(_in0, specs);
	setAttributeAllowedSpecializations(_in1, specs);
	setAttributeAllowedSpecializations(_out, specs);
	
	numericOperation().setOperation(NumericOperation::numericOperationDiv);
}


