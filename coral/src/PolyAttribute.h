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


#ifndef CORAL_POLYATTRIBUTE_H
#define CORAL_POLYATTRIBUTE_H

#include <vector>
#include <map>
#include <time.h>
#include <iostream>

#include "Attribute.h"
#include "Node.h"
#include "PolyValue.h"

namespace coral{

//! A PolyMorphic attribute capable of holding and handling any values.
class CORAL_EXPORT PolyAttribute : public Attribute{
	public:
		PolyAttribute(const std::string &name, Node *parent);

		/*! Convert the clean input attribute into a PolyValue with the same
		 * values.  Be sure to cast any Attribute pointers to be a PolyAttribute
		 * otherwise you might get crashes*/
		PolyValue *value();
		PolyValue *outValue();
		void onSettingSpecialization(const std::vector<std::string> &specialization);
		PolyValue::ValueType typeFromString(const std::string &typeStr);
	};
}
#endif
