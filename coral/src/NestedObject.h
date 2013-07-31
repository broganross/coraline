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


#ifndef CORAL_NESTEDOBJECT_H
#define CORAL_NESTEDOBJECT_H

#include <string>
#include <vector>
#include <iostream>

#include "Object.h"

namespace coral{

//!A specialized Object that can be composed in a tree structure, it's used as base for Node and Attribute.
class CORAL_EXPORT NestedObject: public Object{
	friend class Node;
	friend class Attribute;

public:
	NestedObject(const std::string &name, NestedObject *parent);
	virtual ~NestedObject();	// pure virtual dtor, nobody can create a NestedObject, one can only derive from
	
	void setClassName(const std::string &className);
	std::string className() const;
	bool hasClassName(const std::string &className) const;
	void setName(const std::string &name);
	std::string name();
	std::string fullName();
	std::vector<std::string> classNames();
	NestedObject *findObject(const std::string &name);
	std::vector<NestedObject*> objects();
	Object *parentObject();
	std::vector<NestedObject*> allParentObjects();
	bool isChildOf(NestedObject *parent);
	
	static void(*_setNameCallback)(NestedObject *self, const std::string &name);
	
protected:
	void setParentObject(NestedObject *object);
	void addObject(NestedObject *object);
	void removeObject(NestedObject *object);
	
private:
	std::string _name;
	std::vector<std::string> _classNames;
	NestedObject *_parentObject;
	std::vector<NestedObject*> _objects;
	bool _isConstructing;

	NestedObject();
	NestedObject(const NestedObject &other);
	NestedObject &operator =(const NestedObject &other);
	std::string generateUniqueName(const std::string &name);
};
}
#endif
