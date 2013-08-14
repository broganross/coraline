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
		enum Type{
				stringTypeAny = 0,
				stringType,
				stringTypeArray,
				pathType,
				pathTypeArray
			};

		String();
		std::string asString();
		std::string asScript();
		std::string sliceAsString(unsigned int slice);
		void setFromString(const std::string &value);
		void setType(String::Type type);
		String::Type type();
		bool isArray();
		void setIsArray(bool array);
		unsigned int size();
		unsigned int sizeSlice(unsigned int slice);
		unsigned int slices(){return _slices;}
		void setSlices(unsigned int slices);
		void resize(unsigned int newSize);
		void resizeSlice(unsigned int slice, unsigned int newSize);
		void resizeSlices(unsigned int slices);
		bool isArrayType(String::Type type);
		void setStringValueAt(unsigned int id, std::string& value);
		void setPathValueAt(unsigned int id, std::string& value);
		void setStringValues(const std::vector<std::string> &values);
		void setPathValues(const std::vector<std::string> &values);
		void setStringValueAtSlice(unsigned int slice, unsigned int id, std::string& value);
		void setPathValueAtSlice(unsigned int slice, unsigned int id, std::string& value);
		void setStringValuesSlice(unsigned int slice, const std::vector<std::string> &values);
		void setPathValuesSlice(unsigned int slice, const std::vector<std::string> &values);

		const std::string stringValueAt(unsigned int id);
		const std::string pathValueAt(unsigned int id);
		const std::vector<std::string> &stringValues();
		const std::vector<std::string> &pathValues();
		std::string stringValueAtSlice(unsigned int slice, unsigned int id);
		std::string pathValueAtSlice(unsigned int slice, unsigned int id);
		const std::vector<std::string> &valuesSlice(unsigned int slice);

	private:
		std::string _value;
		std::vector<std::vector<std::string> > _stringValuesSliced;
		Type _type;
		bool _isArray;
		unsigned int _slices;
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
		void onSettingSpecialization(const std::vector<std::string> &specialization);
		String::Type stringTypeFromString(const std::string &typeStr);

	private:
		bool _longString;
	};

}

#endif
