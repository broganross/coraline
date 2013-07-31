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

#ifndef STRINGATTRIBUTE_H
#define STRINGATTRIBUTE_H

#include <string>
#include "Value.h"
#include "Attribute.h"
#include "stringUtils.h"

namespace coral{

	//! Wraps an std::string, used by StringAttribute.
	class CORAL_EXPORT String : public Value{
	public:
		void setStringValue(std::string value);

		const std::string &stringValue();
	
		std::string asString();
	
		void setFromString(const std::string &value);

	private:
		std::string _value;
	};

	//! Stores a String value and allows for strings to be manipulated by a Node.	
	class CORAL_EXPORT StringAttribute: public Attribute{
	public:
		StringAttribute(const std::string &name, Node *parent);

		String *value();

		String *outValue();

		/*! Will display this attribute in the NodeInspector as an aditable text box, rather then the usual one-line field.*/
		void setLongString(bool value);

		bool longString();

	private:
		bool _longString;
	};

}

#endif
