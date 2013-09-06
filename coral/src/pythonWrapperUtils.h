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

#ifndef CORALPYTHONWRAPPERUTILS_H
#define CORALPYTHONWRAPPERUTILS_H

#include <boost/python.hpp>
#include <vector>
#include "PythonDataCollector.h"
#include "Node.h"

//CORAL_EXPORT
namespace coral{

namespace pythonWrapperUtils{
	extern bool pyGILEnsured;

	template <class T>
	struct stdVectorToPythonList
	{
		static PyObject* convert(std::vector<T> const& vectorToConvert)
		{
			boost::python::list convertedList;
			for(unsigned int i = 0; i < vectorToConvert.size(); ++i){
				convertedList.append(vectorToConvert.at(i));
			}

			return boost::python::incref(convertedList.ptr());
		}
	};
	
	template<class WrappedType>
	boost::python::object _common__init__impl(boost::python::object &self){
		WrappedType *object = boost::python::extract<WrappedType*>(self);
		
		PythonDataCollector::storeBorrowedPyObject(object->id(), self.ptr());
		
		return boost::python::object();
	}
	
	// createUnwrapped functions - for some reason I needed to number them to avoid shadowing
	template<class WrappedType>
	boost::python::object createUnwrapped(){
		boost::shared_ptr<WrappedType> object(new WrappedType());

		boost::python::object pyObject(object);
		PythonDataCollector::storePyObject(object->id(), boost::python::object(boost::python::handle<>(pyObject.ptr())));
		
		return pyObject;
	}
	
	template<class WrappedType, class ArgType1>
	boost::python::object createUnwrapped1(ArgType1 arg1){
		boost::shared_ptr<WrappedType> object(new WrappedType(arg1));
		
		boost::python::object pyObject(object);
		PythonDataCollector::storePyObject(object->id(), boost::python::object(boost::python::handle<>(pyObject.ptr())));
		
		return pyObject;
	}
	
	template<class WrappedType, class ArgType1, class ArgType2>
	boost::python::object createUnwrapped2(ArgType1 arg1, ArgType2 arg2){
		boost::shared_ptr<WrappedType> object(new WrappedType(arg1, arg2));
		
		boost::python::object self(object);
		
		_common__init__impl<WrappedType>(self);
		
		return self;
	}
	
	// __init__ functions
	
	
	// 0 args overload
	template<class WrappedType>
	boost::shared_ptr<WrappedType> __cons__(){
		boost::shared_ptr<WrappedType> object(new WrappedType());
		
        return object;
	}
	
	template<class WrappedType>
	boost::python::object __init__(boost::python::object self){
		self.attr("__cons__") = boost::python::make_constructor(__cons__<WrappedType>);
		boost::python::call_method<boost::python::object>(self.ptr(), "__cons__", self);
		
		return _common__init__impl<WrappedType>(self);
	}
	
	// 1 args overload
	template<class WrappedType, class ArgType1>
	boost::shared_ptr<WrappedType> __cons__(boost::python::object arg1Obj){
		boost::shared_ptr<WrappedType> object(new WrappedType(
			boost::python::extract<ArgType1>(arg1Obj)
		));
		
        return object;
	}
	
	template<class WrappedType, class ArgType1>
	boost::python::object __init__(boost::python::object self, boost::python::object arg1Obj){
		self.attr("__cons__") = boost::python::make_constructor(__cons__<WrappedType, ArgType1>);
		boost::python::call_method<boost::python::object>(self.ptr(), "__cons__", self, arg1Obj);
		
		return _common__init__impl<WrappedType>(self);
	}
	
	// 2 args overload
	template<class WrappedType, class ArgType1, class ArgType2>
	boost::shared_ptr<WrappedType> __cons__(boost::python::object arg1Obj, boost::python::object arg2Obj){
		boost::shared_ptr<WrappedType> object(new WrappedType(
			boost::python::extract<ArgType1>(arg1Obj), 
			boost::python::extract<ArgType2>(arg2Obj)
		));
		
        return object;
	}
	
	template<class WrappedType, class ArgType1, class ArgType2>
	boost::python::object __init__(boost::python::object self, boost::python::object arg1Obj, boost::python::object arg2Obj){
		self.attr("__cons__") = boost::python::make_constructor(__cons__<WrappedType, ArgType1, ArgType2>);
		boost::python::call_method<boost::python::object>(self.ptr(), "__cons__", self, arg1Obj, arg2Obj);
		
		return _common__init__impl<WrappedType>(self);
	}
	
	// 3 args overload
	template<class WrappedType, class ArgType1, class ArgType2, class ArgType3>
	boost::shared_ptr<WrappedType> __cons__(boost::python::object arg1Obj, boost::python::object arg2Obj, boost::python::object arg3Obj){
		boost::shared_ptr<WrappedType> object(new WrappedType(
			boost::python::extract<ArgType1>(arg1Obj), 
			boost::python::extract<ArgType2>(arg2Obj), 
			boost::python::extract<ArgType3>(arg3Obj)
		));
		
        return object;
	}
	
	template<class WrappedType, class ArgType1, class ArgType2, class ArgType3>
	boost::python::object __init__(boost::python::object self, boost::python::object arg1Obj, boost::python::object arg2Obj, boost::python::object arg3Obj){
		self.attr("__cons__") = boost::python::make_constructor(__cons__<WrappedType, ArgType1, ArgType2, ArgType3>);
		boost::python::call_method<boost::python::object>(self.ptr(), "__cons__", self, arg1Obj, arg2Obj, arg3Obj);
		
		return _common__init__impl<WrappedType>(self);
	}
	
	// 4 args overload
	template<class WrappedType, class ArgType1, class ArgType2, class ArgType3, class ArgType4>
	boost::shared_ptr<WrappedType> __cons__(boost::python::object arg1Obj, boost::python::object arg2Obj, boost::python::object arg3Obj, boost::python::object arg4Obj){
		boost::shared_ptr<WrappedType> object(new WrappedType(
			boost::python::extract<ArgType1>(arg1Obj), 
			boost::python::extract<ArgType2>(arg2Obj), 
			boost::python::extract<ArgType3>(arg3Obj), 
			boost::python::extract<ArgType4>(arg4Obj)
		));
		
        return object;
	}
	
	template<class WrappedType, class ArgType1, class ArgType2, class ArgType3, class ArgType4>
	boost::python::object __init__(boost::python::object self, boost::python::object arg1Obj, boost::python::object arg2Obj, boost::python::object arg3Obj, boost::python::object arg4Obj){
		self.attr("__cons__") = boost::python::make_constructor(__cons__<WrappedType, ArgType1, ArgType2, ArgType3, ArgType4>);
		boost::python::call_method<boost::python::object>(self.ptr(), "__cons__", self, arg1Obj, arg2Obj, arg3Obj, arg4Obj);
		
		return _common__init__impl<WrappedType>(self);
	}

	// 5 args overload
	template<class WrappedType, class ArgType1, class ArgType2, class ArgType3, class ArgType4, class ArgType5>
	boost::shared_ptr<WrappedType> __cons__(boost::python::object arg1Obj, boost::python::object arg2Obj, boost::python::object arg3Obj, boost::python::object arg4Obj, boost::python::object arg5Obj){
		boost::shared_ptr<WrappedType> object(new WrappedType(
			boost::python::extract<ArgType1>(arg1Obj), 
			boost::python::extract<ArgType2>(arg2Obj), 
			boost::python::extract<ArgType3>(arg3Obj), 
			boost::python::extract<ArgType4>(arg4Obj),
			boost::python::extract<ArgType4>(arg5Obj)
		));
		
        return object;
	}
	
	template<class WrappedType, class ArgType1, class ArgType2, class ArgType3, class ArgType4, class ArgType5>
	boost::python::object __init__(boost::python::object self, boost::python::object arg1Obj, boost::python::object arg2Obj, boost::python::object arg3Obj, boost::python::object arg4Obj, boost::python::object arg5Obj){
		self.attr("__cons__") = boost::python::make_constructor(__cons__<WrappedType, ArgType1, ArgType2, ArgType3, ArgType4, ArgType5>);
		boost::python::call_method<boost::python::object>(self.ptr(), "__cons__", self, arg1Obj, arg2Obj, arg3Obj, arg4Obj, arg5Obj);
		
		return _common__init__impl<WrappedType>(self);
	}
	
	// simplified wrappers
	template<class WrappedType, class BaseClass>
	boost::python::class_<WrappedType, boost::shared_ptr<WrappedType>, boost::python::bases<BaseClass>, boost::noncopyable>
	pythonWrapper(char const* className){
		boost::python::class_<WrappedType, boost::shared_ptr<WrappedType>, boost::python::bases<BaseClass>, boost::noncopyable> wrapper(className, boost::python::no_init);
		wrapper.def("__init__", pythonWrapperUtils::__init__<WrappedType, const std::string, coral::Node*>);
		wrapper.def("createUnwrapped", pythonWrapperUtils::createUnwrapped2<WrappedType, const std::string, Node*>);
		wrapper.staticmethod("createUnwrapped");
		
		return wrapper;
	}
}

}

#endif
