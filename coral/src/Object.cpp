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

#include "Object.h"
#include "NetworkManager.h"
using namespace coral;

void(*Object::_addReferenceCallback)(Object*) = 0;
void(*Object::_removeReferenceCallback)(Object*) = 0;

Object::Object():
	_refCount(0),
	_isDeleted(false){
	_id = NetworkManager::useNextAvailableId();
	NetworkManager::storeObject(_id, this);
}

Object::~Object(){
	NetworkManager::removeObject(_id);
}

int Object::id(){
	return _id;
}

void Object::setIsDeleted(bool value){
	_isDeleted = value;
}

bool Object::isDeleted(){
	return _isDeleted;
}

void Object::addReference(){
	// TODO: if _addReferenceCallback is 0 then it means python is not used to hold our instances, in that case we should use our own ref counting.
	if(_addReferenceCallback){
		_addReferenceCallback(this);
	}
	else{
		_refCount++;
	}
}

void Object::removeReference(){
	if(_removeReferenceCallback){
		_removeReferenceCallback(this);
	}
	else{
		_refCount--;
		if(_refCount == 0){
			delete this;
		}
	}
}
