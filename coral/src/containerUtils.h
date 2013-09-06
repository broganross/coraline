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

#ifndef CORALUTILS_H
#define CORALUTILS_H

#include <vector>
#include <algorithm>
#include <iostream>

//CORAL_EXPORT

namespace containerUtils{
	template<class T> 
	void eraseElementInContainer(const T &element, std::vector<T> &container){
		typename std::vector<T>::iterator iter = std::find(container.begin(), container.end(), element);
	    
		if(iter != container.end())
			container.erase(iter);
	}

	template<class T>
	bool elementInContainer(const T &element, const std::vector<T> &container){
		return std::find(container.begin(), container.end(), element) != container.end();
	}

	template<class T> 
	void addUniqueElementInContainer(const T &element, std::vector<T> &container){
		if(elementInContainer(element, container) == false)
			container.push_back(element);
	}

	template<class T> 
	void moveUniqueElement(const T &element, std::vector<T> &removeFrom, std::vector<T> &addTo){
		eraseElementInContainer(element, removeFrom);
		addUniqueElementInContainer(element, addTo);
	}
	
	template<class T>
	void printContainer(const std::vector<T> &container){
		std::cout << "[";
		for(int i = 0; i < container.size(); ++i){
			std::cout << container[i] << ",";
		}
		
		std::cout << "]" << std::endl;
	}
	
}

#endif
