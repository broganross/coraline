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

#ifndef CORAL_BOOLATTRIBUTE_H
#define CORAL_BOOLATTRIBUTE_H

#include <vector>
#include "Value.h"
#include "Attribute.h"
#include "Node.h"

namespace coral{

//! Wrapper to bool and array of bool, used by BoolAttribute.
class CORAL_EXPORT Bool : public Value{
public:
	Bool();
	unsigned int size();
	void setBoolValueAt(unsigned int id, bool value);
	bool boolValueAt(unsigned int id);
	const std::vector<bool> &boolValues();
	void setBoolValues(const std::vector<bool> &values);
	void setIsArray(bool value);
	bool isArray();
	void resize(unsigned int size);
	std::string asString();
	void setFromString(const std::string &value);
	unsigned int slices();

	unsigned int sizeSlice(unsigned int slice);
	void resizeSlices(unsigned int slices);
	void setBoolValueAtSlice(unsigned int slice, unsigned int id, bool value);
	bool boolValueAtSlice(unsigned int slice, unsigned int id);
	void setBoolValuesSlice(unsigned int slice, const std::vector<bool> &values);
	const std::vector<bool> &boolValuesSlice(unsigned int slice);
	std::string sliceAsString(unsigned int slice);

private:
	std::vector<std::vector<bool> > _boolValuesSliced;
	bool _isArray;
	unsigned int _slices;
};

//! Wraps the Bool value in an attribute.
class CORAL_EXPORT BoolAttribute: public Attribute{
public:
	BoolAttribute(const std::string &name, Node *parent);
	Bool *value();
	Bool *outValue();
	void onSettingSpecialization(const std::vector<std::string> &specialization);
	std::string shortDebugInfo();
};

}

#endif
