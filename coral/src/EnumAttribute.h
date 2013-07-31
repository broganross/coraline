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

#ifndef CORAL_ENUMATTRIBUTE_H
#define CORAL_ENUMATTRIBUTE_H

#include <string>
#include <map>
#include <vector>

#include "Value.h"
#include "Attribute.h"
#include "stringUtils.h"

namespace coral{

//! An Enum to store int:string pairs inside an EnumAttribute.
class CORAL_EXPORT Enum: public Value{
public:
	Enum();
	void addEntry(int id, const std::string &value);
	std::vector<int> indices();
	std::vector<std::string> entries();
	void setCurrentIndex(int index);
	int currentIndex();
	std::string asString();
	void setFromString(const std::string &value);
	void setCurrentIndexChangedCallback(Node * parentNode, void(*callback)(Node *, Enum *));
	
private:
	int _currentIndex;
	std::map<int, std::string> _enum;
	void(*_currentIndexChangedCallback)(Node *, Enum *);
	Node *_parentNode;
};

//! Stores an Enum value.
class CORAL_EXPORT EnumAttribute: public Attribute{
public:
	EnumAttribute(const std::string &name, Node *parent) : Attribute(name, parent){
		setClassName("EnumAttribute");
		setValuePtr(new Enum());
		
		std::vector<std::string> allowedSpecialization;
		allowedSpecialization.push_back("Enum");
		setAllowedSpecialization(allowedSpecialization);
	}

	Enum *value(){
		return (Enum*)Attribute::value();
	}
	Enum *outValue(){
		return (Enum*)Attribute::outValue();
	}
};

}

#endif
