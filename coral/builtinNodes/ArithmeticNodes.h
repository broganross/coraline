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

#ifndef CORAL_ARITHMETICNODES_H
#define CORAL_ARITHMETICNODES_H

#include "../src/Node.h"
#include "../src/Attribute.h"
#include "../src/NumericAttribute.h"
#include "NumericOperation.h"

namespace coral{
	
class ArithmeticNode: public Node{
public:
	ArithmeticNode(const std::string &name, Node *parent);

	void updateSlice(Attribute *attribute, unsigned int slice);
	void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB);
	void attributeSpecializationChanged(Attribute *attribute);

protected:
	NumericAttribute *_in0;
	NumericAttribute *_in1;
	NumericAttribute *_out;
	
	NumericOperation &numericOperation(){
		return _numericOperation;
	}
	
private:
	void updateNumericOperationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB);
	
	
	NumericOperation _numericOperation;
};

class CORAL_EXPORT AddNode : public ArithmeticNode{
public:
	AddNode(const std::string &name, Node *parent);
};

class CORAL_EXPORT SubNode : public ArithmeticNode{
public:
	SubNode(const std::string &name, Node *parent);
};

class CORAL_EXPORT MulNode : public ArithmeticNode{
public:
	MulNode(const std::string &name, Node *parent);
};

class CORAL_EXPORT DivNode : public ArithmeticNode{
public:
	DivNode(const std::string &name, Node *parent);
};


}

#endif
