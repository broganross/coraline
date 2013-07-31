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

#ifndef CORAL_NUMERICOPERATIONS_H
#define CORAL_NUMERICOPERATIONS_H

#include "../src/Numeric.h"

typedef Imath::V3f vec3;
typedef Imath::Color4f col4;
typedef Imath::M44f matrix44;

using namespace coral;

template <class type>
void numericOperation_passThrough(const std::vector<type> &containerA, std::vector<type> &resultContainer){
	resultContainer = containerA;
}

template <class TypeA, class TypeB>
void numericOperation_addArrayToSingle(const std::vector<TypeA> &containerA, const std::vector<TypeB> &containerB, std::vector<TypeA> &resultContainer){
	if(containerB.size()){
		resultContainer.resize(containerA.size());

		const TypeB &valueB = containerB[0];
		for(int i = 0; i < containerA.size(); ++i){
			resultContainer[i] = containerA[i] + valueB;
		}
	}
}

template <class TypeA, class TypeB>
void numericOperation_addArrayToArray(const std::vector<TypeA> &containerA, const std::vector<TypeB> &containerB, std::vector<TypeA> &resultContainer){
	int sizeA = containerA.size();
	int sizeB = containerB.size();
	
	int minorSize = sizeA;
	if(sizeB < sizeA){
		minorSize = sizeB;
	}
	
	resultContainer.resize(minorSize);

	for(int i = 0; i < minorSize; ++i){
		resultContainer[i] = containerA[i] + containerB[i];
	}
}

template <class TypeA, class TypeB>
void numericOperation_addSingleToArray(const std::vector<TypeA> &containerA, const std::vector<TypeB> &containerB, std::vector<TypeA> &resultContainer){
	if(containerA.size()){
		TypeA valueA = containerA[0];
		resultContainer.resize(1);
		
		for(int i = 0; i < containerB.size(); ++i){
			valueA += containerB[i];
		}
		
		resultContainer[0] = valueA;
	}
}

template <class TypeA, class TypeB>
void numericOperation_subArrayToSingle(const std::vector<TypeA> &containerA, const std::vector<TypeB> &containerB, std::vector<TypeA> &resultContainer){
	if(containerB.size()){
		resultContainer.resize(containerA.size());

		const TypeB &valueB = containerB[0];
		for(int i = 0; i < containerA.size(); ++i){
			resultContainer[i] = containerA[i] - valueB;
		}
	}
}

template <class TypeA, class TypeB>
void numericOperation_subSingleToArray(const std::vector<TypeA> &containerA, const std::vector<TypeB> &containerB, std::vector<TypeA> &resultContainer){
	if(containerA.size()){
		TypeA valueA = containerA[0];
		resultContainer.resize(1);
		
		for(int i = 0; i < containerB.size(); ++i){
			valueA -= containerB[i];
		}
		
		resultContainer[0] = valueA;
	}
}

template <class TypeA, class TypeB>
void numericOperation_subArrayToArray(const std::vector<TypeA> &containerA, const std::vector<TypeB> &containerB, std::vector<TypeA> &resultContainer){
	int sizeA = containerA.size();
	int sizeB = containerB.size();
	
	int minorSize = sizeA;
	if(sizeB < sizeA){
		minorSize = sizeB;
	}
	
	resultContainer.resize(minorSize);
	
	for(int i = 0; i < minorSize; ++i){
		resultContainer[i] = containerA[i] - containerB[i];
	}
}

template <class TypeA, class TypeB>
void numericOperation_mulArrayToSingle(const std::vector<TypeA> &containerA, const std::vector<TypeB> &containerB, std::vector<TypeA> &resultContainer){
	if(containerB.size()){
		resultContainer.resize(containerA.size());
		
		const TypeB &valueB = containerB[0];
		for(int i = 0; i < containerA.size(); ++i){
			resultContainer[i] = containerA[i] * valueB;
		}
	}
}

template <class TypeA, class TypeB>
void numericOperation_mulSingleToArray(const std::vector<TypeA> &containerA, const std::vector<TypeB> &containerB, std::vector<TypeA> &resultContainer){
	if(containerA.size()){
		TypeA valueA = containerA[0];
		resultContainer.resize(1);
		
		for(int i = 0; i < containerB.size(); ++i){
			valueA *= containerB[i];
		}
		
		resultContainer[0] = valueA;
	}
}

template <class TypeA, class TypeB>
void numericOperation_mulArrayToArray(const std::vector<TypeA> &containerA, const std::vector<TypeB> &containerB, std::vector<TypeA> &resultContainer){
	int sizeA = containerA.size();
	int sizeB = containerB.size();
	
	int minorSize = sizeA;
	if(sizeB < sizeA){
		minorSize = sizeB;
	}
	
	resultContainer.resize(minorSize);
	
	for(int i = 0; i < minorSize; ++i){
		resultContainer[i] = containerA[i] * containerB[i];
	}
}

template <class TypeA, class TypeB>
void numericOperation_divArrayToSingle(const std::vector<TypeA> &containerA, const std::vector<TypeB> &containerB, std::vector<TypeA> &resultContainer){
	if(containerB.size()){
		resultContainer.resize(containerA.size());
		
		const TypeB &valueB = containerB[0];
		for(int i = 0; i < containerA.size(); ++i){
			resultContainer[i] = containerA[i] / valueB;
		}
	}
}

template <class TypeA, class TypeB>
void numericOperation_divSingleToArray(const std::vector<TypeA> &containerA, const std::vector<TypeB> &containerB, std::vector<TypeA> &resultContainer){
	if(containerA.size()){
		TypeA valueA = containerA[0];
		resultContainer.resize(1);
		
		for(int i = 0; i < containerB.size(); ++i){
			valueA /= containerB[i];
		}
		
		resultContainer[0] = valueA;
	}
}

template <class TypeA, class TypeB>
void numericOperation_divArrayToArray(const std::vector<TypeA> &containerA, const std::vector<TypeB> &containerB, std::vector<TypeA> &resultContainer){
	int sizeA = containerA.size();
	int sizeB = containerB.size();
	
	int minorSize = sizeA;
	if(sizeB < sizeA){
		minorSize = sizeB;
	}
	
	resultContainer.resize(minorSize);
	
	for(int i = 0; i < minorSize; ++i){
		resultContainer[i] = containerA[i] / containerB[i];
	}
}

#endif

