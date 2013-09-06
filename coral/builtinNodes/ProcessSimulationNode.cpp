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

#include "ProcessSimulationNode.h"
#include "../src/Numeric.h"
#include "../src/stringUtils.h"

using namespace coral;

ProcessSimulationNode::ProcessSimulationNode(const std::string &name, Node *parent) : Node(name, parent){
	setClassName("ProcessSimulation");
	setAllowDynamicAttributes(true);

	_getDataFrom = new EnumAttribute("getDataFrom", this);
	_data0 = new NumericAttribute("data0", this);
	_out = new NumericAttribute("out", this);

	addInputAttribute(_getDataFrom);
	addInputAttribute(_data0);
	addOutputAttribute(_out);

	setAttributeAffect(_getDataFrom, _out);
	setAttributeAffect(_data0, _out);

	addAttributeSpecializationLink(_data0, _out);

	_getDataFrom->outValue()->addEntry(0, "data0");
}

void ProcessSimulationNode::addInputData(){
	int newId = inputAttributes().size() - 1;
	std::string numStr = stringUtils::intToString(newId);
	NumericAttribute *attr = new NumericAttribute("data" + numStr, this);
	addInputAttribute(attr);
	setAttributeAffect(attr, _out);
	
	addAttributeSpecializationLink(attr, _out);

	addDynamicAttribute(attr);

	updateAttributeSpecialization(attr);

	_getDataFrom->outValue()->addEntry(newId, attr->name());
}

void ProcessSimulationNode::updateSlice(Attribute *attribute, unsigned int slice){
	int attrToTransfer = _getDataFrom->value()->currentIndex() + 1;

	std::vector<Attribute*> attrs = inputAttributes();
	if(attrToTransfer < attrs.size()){
		_out->outValue()->copy(attrs[attrToTransfer]->value());
	}
}
