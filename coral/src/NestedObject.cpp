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

#include "NestedObject.h"
#include "NetworkManager.h"
#include "containerUtils.h"
#include "stringUtils.h"

using namespace coral;

namespace{
	std::string increaseNameNumber(const std::string &name){
		std::string newName = name;
		
		std::string numberStr;
		for(int i = 0; i < name.size(); ++i){
			if(isdigit(name[i])){
				numberStr += name[i];
			}
		}
		
		int number = 1;
		if(!numberStr.empty()){
			number = stringUtils::parseInt(numberStr) + 1;
			newName = stringUtils::replace(name, numberStr, "");
		}
		
		newName += stringUtils::intToString(number);
		
		return newName;
	}
}

void(*NestedObject::_setNameCallback)(NestedObject *self, const std::string &name) = 0;

NestedObject::NestedObject(const std::string &name, NestedObject *parent): 
	Object(),
	_parentObject(parent){
	
	_name = generateUniqueName(name);
}

NestedObject::~NestedObject(){
}

void NestedObject::setClassName(const std::string &className){
	_classNames.push_back(className);
}

std::string NestedObject::generateUniqueName(const std::string &name){
	std::string uniqueName = name;
	if(_parentObject){
		while(_parentObject->findObject(uniqueName)){
			uniqueName = increaseNameNumber(uniqueName);
		}
	}
	
	return uniqueName;
}

void NestedObject::setName(const std::string &name){
	std::string uniqueName = generateUniqueName(name);
	_name = uniqueName;
	
	if(_setNameCallback && !isDeleted()){
		_setNameCallback(this, _name);
	}
}

std::string NestedObject::name(){
	return _name;
}

std::string NestedObject::className() const{
	std::string className = "";
	
	if(_classNames.size()){
		className = _classNames[_classNames.size() - 1];
	}
	
	return className;
}

bool NestedObject::hasClassName(const std::string &className) const{
	return containerUtils::elementInContainer(className, _classNames);
}

std::vector<std::string> NestedObject::classNames(){
	return _classNames;
}

std::string NestedObject::fullName(){
	NestedObject *parent = _parentObject;
	std::string fullName = name();
	
	while(parent){
		fullName = parent->name() + std::string(".") + fullName;
		
		parent = parent->_parentObject;
	}
	
	return fullName;
}

void NestedObject::setParentObject(NestedObject *object){
	_parentObject = object;
}

Object *NestedObject::parentObject(){
	return _parentObject;
}

void NestedObject::addObject(NestedObject *object){
	if(containerUtils::elementInContainer(object, _objects) == false){
		_objects.push_back(object);
		
		object->addReference();
	}
}

void NestedObject::removeObject(NestedObject *object){
	if(containerUtils::elementInContainer(object, _objects)){
		containerUtils::eraseElementInContainer(object, _objects);

		object->removeReference();
	}
}

NestedObject *NestedObject::findObject(const std::string &name){
	NestedObject *foundObject = 0;
	
	for(std::vector<NestedObject*>::iterator it = _objects.begin(); it != _objects.end(); ++it){
		NestedObject *object = *it;
		if(object->_name == name){
			foundObject = object;
			break;
		}
	}
	
	return foundObject;
}

std::vector<NestedObject*> NestedObject::objects(){
	return _objects;
}

std::vector<NestedObject*> NestedObject::allParentObjects(){
	std::vector<NestedObject*> parents;
	
	NestedObject *parent = _parentObject;
	while(parent){
		parents.push_back(parent);
		parent = parent->_parentObject;
	}
	
	return parents;
}

bool NestedObject::isChildOf(NestedObject *parent){	
	NestedObject *currentParent = _parentObject;
	while(currentParent){
		if(parent == currentParent){
			return true;
		}
		
		currentParent = currentParent->_parentObject;
	}
	
	return false;
}

