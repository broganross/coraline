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

#ifndef CORAL_COMMANDWRAPPER_H
#define CORAL_COMMANDWRAPPER_H

#include <boost/python.hpp>

#include "../src/Command.h"
#include "../src/pythonWrapperUtils.h"

using namespace coral;

class CommandWrapper: public Command, public boost::python::wrapper<Command>{
public:
	CommandWrapper(): Command(){
	}
	
	void doIt(){
		return boost::python::call<void>(get_override("doIt").ptr());
	}
	
	void doIt_default(){
		Command::doIt();
	}
	
	void undoIt(){
		return boost::python::call<void>(get_override("undoIt").ptr());
	}
	
	void undoIt_default(){
		Command::undoIt();
	}
};

int command_undefinedType(){
	return int(Command::undefinedType);
}

int command_boolType(){
	return int(Command::boolType);
}

int command_intType(){
	return int(Command::intType);
}

int command_floatType(){
	return int(Command::floatType);
}

int command_stringType(){
	return int(Command::stringType);
}

int command_argType(Command &self, const std::string &argName){
	return int(self.argType(argName));
}

int command_resultType(Command &self){
	return int(self.resultType());
}

void commandWrapper(){
	boost::python::class_<CommandWrapper, boost::shared_ptr<CommandWrapper>, boost::noncopyable>("Command")
		.def("doIt", &Command::doIt, &CommandWrapper::doIt_default)
		.def("undoIt", &Command::undoIt, &CommandWrapper::undoIt_default)
		.def("setName", &Command::setName)
		.def("name", &Command::name)
		.def("setArgBool", &Command::setArgBool)
		.def("setArgFloat", &Command::setArgFloat)
		.def("setArgInt", &Command::setArgInt)
		.def("setArgString", &Command::setArgString)
		.def("setArgUndefined", &Command::setArgUndefined)
		.def("argAsFloat", &Command::argAsFloat)
		.def("argAsInt", &Command::argAsInt)
		.def("argAsBool", &Command::argAsBool)
		.def("argAsString", &Command::argAsString)
		.def("setResultBool", &Command::setResultBool)
		.def("setResultInt", &Command::setResultInt)
		.def("setResultFloat", &Command::setResultFloat)
		.def("setResultString", &Command::setResultString)
		.def("resultAsBool", &Command::resultAsBool)
		.def("resultAsInt", &Command::resultAsInt)
		.def("resultAsFloat", &Command::resultAsFloat)
		.def("resultAsString", &Command::resultAsString)
		.def("argType", command_argType)
		.def("resultType", command_resultType)
		.def("asScript", &Command::asScript)
		.def("argNames", &Command::argNames)
		.add_static_property("undefinedType", command_undefinedType)
		.add_static_property("boolType", command_boolType)
		.add_static_property("intType", command_intType)
		.add_static_property("floatType", command_floatType)
		.add_static_property("stringType", command_stringType)
	;
}

#endif
