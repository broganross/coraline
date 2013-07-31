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

#include "Command.h"

using namespace coral;

Command::Command():
	_name(""){
}

void Command::setName(const std::string &name){
	_name = name;
}

std::string Command::name(){
	return _name;
}

void Command::setArgUndefined(const std::string &name, const std::string &value){
	CommandValue cmdValue;
	cmdValue.setUndefined(value);
	setCommandValueArg(name, cmdValue);
}

void Command::setArgBool(const std::string &name, bool value){
	CommandValue cmdValue;
	cmdValue.setBool(value);
	setCommandValueArg(name, cmdValue);
}

void Command::setArgInt(const std::string &name, int value){
	CommandValue cmdValue;
	cmdValue.setInt(value);
	setCommandValueArg(name, cmdValue);
}

void Command::setArgFloat(const std::string &name, float value){
	CommandValue cmdValue;
	cmdValue.setFloat(value);
	setCommandValueArg(name, cmdValue);
}

void Command::setArgString(const std::string &name, const std::string &value){
	CommandValue cmdValue;
	cmdValue.setString(value);
	setCommandValueArg(name, cmdValue);
}

void Command::setCommandValueArg(const std::string &name, const CommandValue &value){
	_args[name] = value;
	
	if(containerUtils::elementInContainer(name, _argNames) == false){
		_argNames.push_back(name);
	}
}

bool Command::argAsBool(const std::string &name){
	return _args[name].asBool();
}

int Command::argAsInt(const std::string &name){
	return _args[name].asInt();
}

float Command::argAsFloat(const std::string &name){
	return _args[name].asFloat();
}

std::string Command::argAsString(const std::string &name){
	return _args[name].asString();
}

void Command::setResultBool(bool value){
	CommandValue cmdValue;
	cmdValue.setBool(value);
	setCommandValueResult(cmdValue);
}

void Command::setResultInt(int value){
	CommandValue cmdValue;
	cmdValue.setInt(value);
	setCommandValueResult(cmdValue);
}

void Command::setResultFloat(float value){
	CommandValue cmdValue;
	cmdValue.setFloat(value);
	setCommandValueResult(cmdValue);
}

void Command::setResultString(const std::string &value){
	CommandValue cmdValue;
	cmdValue.setString(value);
	setCommandValueResult(cmdValue);
}

bool Command::resultAsBool(){
	return _result.asBool();
}

int Command::resultAsInt(){
	return _result.asInt();
}

float Command::resultAsFloat(){
	return _result.asFloat();
}

std::string Command::resultAsString(){
	return _result.asString();
}

Command::CommandValueType Command::argType(const std::string &argName){
	return (Command::CommandValueType)_args[argName].type();
}

Command::CommandValueType Command::resultType(){
	return (Command::CommandValueType)_result.type();
}

void Command::setCommandValueResult(const CommandValue &value){
	_result = value;
}

std::string Command::argToString(CommandValue &arg){
	std::string argString = "undefinedValue";
	
	CommandValue::CommandValueType argType = arg.type();
	if(argType == CommandValue::boolType){
		if(arg.asBool()){
			argString = "True";
		}
		else{
			argString = "False";
		}
	}
	else if(argType == CommandValue::intType){
		char buffer[sizeof(int)];
		sprintf(buffer, "%i", arg.asInt());
		
		argString = buffer;
	}
	else if(argType == CommandValue::floatType){
		char buffer[sizeof(float)];
		sprintf(buffer, "%f", arg.asFloat());
		
		argString = buffer;
	}
	else if(argType == CommandValue::stringType){
		argString = "'" + arg.asString() + "'";
	}
	else if(argType == CommandValue::undefinedType){
		argString = arg.asString();
	}
	
	return argString;
}

std::string Command::asScript(){
	std::string cmdString = "executeCommand('" + _name + "', ";
	int numArgs = _argNames.size();
	for(int i = 0; i < numArgs; ++i){
		std::string argName = _argNames[i];
		cmdString += argName + " = " + argToString(_args[argName]);
		
		if(i < numArgs - 1){
			cmdString += ", ";
		}
	}
	
	cmdString += ")";
	
	return cmdString;
}

void Command::doIt(){
}

void Command::undoIt(){
}

std::vector<std::string> Command::argNames(){
	return _argNames;
}
