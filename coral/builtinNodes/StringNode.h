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

#ifndef STRINGNODE_H
#define STRINGNODE_H

#include "../src/Node.h"
#include "../src/StringAttribute.h"
#include "../src/containerUtils.h"

namespace coral{

class StringNode : public Node{
public:
	StringNode(const std::string &name, Node *parent) : Node(name, parent){
		_string = new StringAttribute("string", this);
		setAttributeAllowedSpecialization(_string, "String");
		addOutputAttribute(_string);
	}

private:
	StringAttribute *_string;
};

class FilePathNode:public Node{
public:
	FilePathNode(const std::string &name, Node *parent) : Node(name, parent){
		_string = new StringAttribute("filepath", this);
		setAttributeAllowedSpecialization(_string, "Path");
		addOutputAttribute(_string);
	}
private:
	StringAttribute *_string;
};

class AddStringNode : public Node {
public:
	AddStringNode(const std::string &name, Node *parent);
	void updateSpecializationLink(Attribute *attrA, Attribute *attrB, std::vector<std::string> &specA, std::vector<std::string> &specB);
	void attributeSpecializationChanged(Attribute *attr);
	void updateSlice(Attribute *attr, unsigned int slice);
private:
	StringAttribute *_in0;
	StringAttribute *_in1;
	StringAttribute *_out;
	void(AddStringNode::*_selectedOperation)(String*, String*, String*, unsigned int);

	void updatePath(String *in0, String* in1, String *out, unsigned int slice);
	void updateString(String *in0, String *in1, String *out, unsigned int slice);
};

}

#endif
