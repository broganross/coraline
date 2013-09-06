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

#ifndef NODEACCESSOR_H
#define NODEACCESSOR_H

#include "Node.h"

namespace coral{

// The NodeAccessor is used to give core classes access to Node's private and protected members,
// this class should never go in the public SDK as it's for core use only.
class NodeAccessor{
public:
	static void _setAttributeAffect(Node &self, Attribute *source, Attribute *destination){
 		self.setAttributeAffect(source, destination);
	}
	
	static void _setAttributeAllowedSpecializations(Node &self, Attribute *attribute, const std::vector<std::string> &specialization){
		self.setAttributeAllowedSpecializations(attribute, specialization);
	}
	
	static void _setAllowDynamicAttributes(Node &self, bool value){
		self.setAllowDynamicAttributes(value);
	}
	
	static void _addAttributeSpecializationLink(Node &self, Attribute *attributeA, Attribute *attributeB){
		self.addAttributeSpecializationLink(attributeA, attributeB);
	}
	
	static void _updateAttributeSpecialization(Node &self, Attribute *attribute){
		self.updateAttributeSpecialization(attribute);
	}
	
	static void _setAttributeIsClean(Node &self, Attribute *attribute, bool value){
		self.setAttributeIsClean(attribute, value);
	}
	
	static void _setIsInvalid(Node &self, bool value, const std::string &message){
		self.setIsInvalid(value, message);
	}
	
	static void _setUpdateEnabled(Node &self, bool value){
		self.setUpdateEnabled(value);
	}
	
	static void _setConstructorDone(Node &self, bool value){
		self._constructorDone = value;
	}

	static void _catchAttributeDirtied(Node &self, Attribute *attribute, bool value){
		self.catchAttributeDirtied(attribute, value);
	}

	static void _setSliceable(Node &self, bool value){
		self.setSliceable(value);
	}
};

}

#endif
