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


#ifndef CORAL_COMMAND_H
#define CORAL_COMMAND_H

#include <cstdio> 
#include <map>
#include <vector>
#include <string>

#include "containerUtils.h"
#include "coralDefinitions.h"

namespace coral{

class CommandValue{
// CommandValue is used internally by Command but it's not meant for public use
public:
	enum CommandValueType{
		undefinedType,
		boolType,
		intType,
		floatType,
		stringType
	};
	
	CommandValue():
		_boolValue(true),
		_intValue(0),
		_floatValue(0.0),
		_stringValue(""),
		_type(CommandValue::undefinedType){
	}
	
	void setUndefined(const std::string &value){
		_stringValue = value;
		_type = CommandValue::undefinedType;
	}
	
	void setBool(bool value){
		_boolValue = value;
		_type = CommandValue::boolType;
	}
	
	void setInt(int value){
		_intValue = value;
		_type = CommandValue::intType;
	}
	
	void setFloat(float value){
		_floatValue = value;
		_type = CommandValue::floatType;
	}
	
	void setString(const std::string &value){
		_stringValue = value;
		_type = CommandValue::stringType;
	}
	
	bool asBool(){
		return _boolValue;
	}
	
	int asInt(){
		return _intValue;
	}
	
	float asFloat(){
		return _floatValue;
	}
	
	std::string asString(){
		return _stringValue;
	}
	
	CommandValueType type(){
		return _type;
	}
	
private:
	bool _boolValue;
	int _intValue;
	float _floatValue;
	std::string _stringValue;
	CommandValueType _type;
};

//! Base class to all commands willing to gain undo functionality.
//
//! Use coralApp.registerCommandClass() in order to register your custom command class.
class CORAL_EXPORT Command{
// system commands are all grouped under coral/py/coral/commands.py and loaded by coralApp._registerCommands()
public:
	enum CommandValueType{
		undefinedType,
		boolType,
		intType,
		floatType,
		stringType
	};
	
	Command();
	Command(const std::string &name);
	
	void setName(const std::string &name);
	std::string name();
	virtual void doIt();
	virtual void undoIt();
	void setArgBool(const std::string &name, bool value);
	void setArgInt(const std::string &name, int value);
	void setArgFloat(const std::string &name, float value);
	void setArgString(const std::string &name, const std::string &value);
	bool argAsBool(const std::string &name);
	int argAsInt(const std::string &name);
	float argAsFloat(const std::string &name);
	std::string argAsString(const std::string &name);
	void setResultBool(bool value);
	void setResultInt(int value);
	void setResultFloat(float value);
	void setResultString(const std::string &value);
	bool resultAsBool();
	int resultAsInt();
	float resultAsFloat();
	std::string resultAsString();
	CommandValueType argType(const std::string &argName);
	CommandValueType resultType();
	std::vector<std::string> argNames();
	
	//! Sets a value that is not of any known type (bool, int, float or string),
	//! the value is provided in the form of a string but won't be represented as a string when the command is saved to disk.
	//! example: 
	//! Command myCommand("MyCommand")
	//! myCommand.setArgUndefined("myUndefinedValue", "SomeClass()")
	//! print myCommand.asScript()
	//! # executeCommand('MyCommand', myUndefinedValue = SomeClass())
	void setArgUndefined(const std::string &name, const std::string &value);
	
	//! Returns a string of python code in the form of:
	//! executeCommand('MyCommand', myBoolArg = True, myIntArg = 1, myFloatArg = 0.0, myStringArg = 'something', myUndefinedArg = SomeClass())
	std::string asScript();
	
private:
	void setCommandValueArg(const std::string &name, const CommandValue &value);
	void setCommandValueResult(const CommandValue &value);
	std::string argToString(CommandValue &arg);
	
	std::string _name;
	std::map<std::string, CommandValue> _args;
	std::vector<std::string> _argNames;
	CommandValue _result;
};

}

#endif
