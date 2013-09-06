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

#ifndef CORAL_CONDITIONALNODES_H
#define CORAL_CONDITIONALNODES_H

#include "../src/Node.h"
#include "../src/NumericAttribute.h"
#include "../src/BoolAttribute.h"
#include "../src/mathUtils.h"

namespace coral{

class IfGreaterThan: public Node{
public:
	IfGreaterThan(const std::string &name, Node *parent);
	void attributeSpecializationChanged(Attribute *attribute);
	void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB);
	void updateSlice(Attribute *attribute, unsigned int slice);
	
private:
	NumericAttribute *_in0;
	NumericAttribute *_in1;
	BoolAttribute *_out;
	
	void(IfGreaterThan::*_selectedOperation)(Numeric*, Numeric*, std::vector<bool> &, unsigned int);
	
	void intGreaterThanInt_arrayToArray(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	void intGreaterThanFloat_arrayToArray(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	void floatGreaterThanFloat_arrayToArray(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	void floatGreaterThanInt_arrayToArray(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	
	void intGreaterThanInt_arrayToSingle(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	void intGreaterThanFloat_arrayToSingle(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	void floatGreaterThanFloat_arrayToSingle(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	void floatGreaterThanInt_arrayToSingle(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	
	void intGreaterThanInt_singleToArray(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	void intGreaterThanFloat_singleToArray(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	void floatGreaterThanFloat_singleToArray(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	void floatGreaterThanInt_singleToArray(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
};

class IfLessThan: public Node{
public:
	IfLessThan(const std::string &name, Node *parent);
	void attributeSpecializationChanged(Attribute *attribute);
	void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB);
	void updateSlice(Attribute *attribute, unsigned int slice);
	
private:
	NumericAttribute *_in0;
	NumericAttribute *_in1;
	BoolAttribute *_out;
	
	void(IfLessThan::*_selectedOperation)(Numeric*, Numeric*, std::vector<bool> &, unsigned int);
	
	void intLessThanInt_arrayToArray(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	void intLessThanFloat_arrayToArray(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	void floatLessThanFloat_arrayToArray(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	void floatLessThanInt_arrayToArray(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	
	void intLessThanInt_arrayToSingle(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	void intLessThanFloat_arrayToSingle(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	void floatLessThanFloat_arrayToSingle(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	void floatLessThanInt_arrayToSingle(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	
	void intLessThanInt_singleToArray(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	void intLessThanFloat_singleToArray(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	void floatLessThanFloat_singleToArray(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
	void floatLessThanInt_singleToArray(Numeric *in0, Numeric *in1, std::vector<bool> &out, unsigned int slice);
};

class ConditionalValue: public Node{
public:
	ConditionalValue(const std::string &name, Node *parent);
	void attributeSpecializationChanged(Attribute *attribute);
	void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB);
	void updateSlice(Attribute *attribute, unsigned int slice);
	
private:
	BoolAttribute *_condition;
	NumericAttribute *_ifTrue;
	NumericAttribute *_ifFalse;
	NumericAttribute *_out;
	
	void(ConditionalValue::*_selectedOperation)(Bool *, Numeric *, Numeric *, Numeric *, unsigned int);
	
	void transferValuesInt(Bool *condition, Numeric *ifTrue, Numeric *ifFalse, Numeric *out, unsigned int slice);
	void transferValuesFloat(Bool *condition, Numeric *ifTrue, Numeric *ifFalse, Numeric *out, unsigned int slice);
	void transferValuesVec3(Bool *condition, Numeric *ifTrue, Numeric *ifFalse, Numeric *out, unsigned int slice);
	void transferValuesCol4(Bool *condition, Numeric *ifTrue, Numeric *ifFalse, Numeric *out, unsigned int slice);
	void transferValuesMatrix44(Bool *condition, Numeric *ifTrue, Numeric *ifFalse, Numeric *out, unsigned int slice);
	
	void transferValuesIntBoolArray(Bool *condition, Numeric *ifTrue, Numeric *ifFalse, Numeric *out, unsigned int slice);
	void transferValuesFloatBoolArray(Bool *condition, Numeric *ifTrue, Numeric *ifFalse, Numeric *out, unsigned int slice);
	void transferValuesVec3BoolArray(Bool *condition, Numeric *ifTrue, Numeric *ifFalse, Numeric *out, unsigned int slice);
	void transferValuesCol4BoolArray(Bool *condition, Numeric *ifTrue, Numeric *ifFalse, Numeric *out, unsigned int slice);
	void transferValuesMatrix44BoolArray(Bool *condition, Numeric *ifTrue, Numeric *ifFalse, Numeric *out, unsigned int slice);
};

// greater than function
template <class Type0, class Type1>
void conditionalOperation_greaterThan_arrayToArray(const std::vector<Type0> &in0, const std::vector<Type1> &in1, unsigned int minorSize, std::vector<bool> &out){
	for(int i = 0; i < minorSize; ++i){
		if(in0[i] > in1[i]){
			out[i] = true;
		}
		else{
			out[i] = false;
		}
	}
}

template <class Type0, class Type1>
void conditionalOperation_greaterThan_arrayToSingle(const std::vector<Type0> &in0, Type1 in1, unsigned int minorSize, std::vector<bool> &out){
	for(int i = 0; i < minorSize; ++i){
		if(in0[i] > in1){
			out[i] = true;
		}
		else{
			out[i] = false;
		}
	}
}

template <class Type0, class Type1>
void conditionalOperation_greaterThan_singleToArray(Type0 in0, const std::vector<Type1> &in1, std::vector<bool> &out){
	out[0] = false;
	
	for(int i = 0; i > in1.size(); ++i){
		if(in0 < in1[i]){
			out[0] = true;
			break;
		}
	}
}

// less than function
template <class Type0, class Type1>
void conditionalOperation_lessThan_arrayToArray(const std::vector<Type0> &in0, const std::vector<Type1> &in1, unsigned int minorSize, std::vector<bool> &out){
	for(int i = 0; i < minorSize; ++i){
		if(in0[i] < in1[i]){
			out[i] = true;
		}
		else{
			out[i] = false;
		}
	}
}

template <class Type0, class Type1>
void conditionalOperation_lessThan_arrayToSingle(const std::vector<Type0> &in0, Type1 in1, unsigned int minorSize, std::vector<bool> &out){
	for(int i = 0; i < minorSize; ++i){
		if(in0[i] < in1){
			out[i] = true;
		}
		else{
			out[i] = false;
		}
	}
}

template <class Type0, class Type1>
void conditionalOperation_lessThan_singleToArray(Type0 in0, const std::vector<Type1> &in1, std::vector<bool> &out){
	out[0] = false;
	
	for(int i = 0; i < in1.size(); ++i){
		if(in0 < in1[i]){
			out[0] = true;
			break;
		}
	}
}

// generic

template <class Type>
void conditionalValueTransfer(bool condition, const std::vector<Type> &trueValues, const std::vector<Type> &falseValues, std::vector<Type> &out){
	int sizes[] = {trueValues.size(), falseValues.size()};
	int minorSize = mathUtils::findMinorInt(sizes, 2);
	out.resize(minorSize);
	
	for(int i = 0; i < minorSize; ++i){
		if(condition){
			out[i] = trueValues[i];
		}
		else{
			out[i] = falseValues[i];
		}
	}
}

template <class Type>
void conditionalValueTransferBoolArray(const std::vector<bool> &conditions, const std::vector<Type> &trueValues, const std::vector<Type> &falseValues, std::vector<Type> &out){
	int sizes[] = {conditions.size(), trueValues.size(), falseValues.size()};
	int minorSize = mathUtils::findMinorInt(sizes, 3);
	out.resize(minorSize);
	
	for(int i = 0; i < minorSize; ++i){
		if(conditions[i]){
			out[i] = trueValues[i];
		}
		else{
			out[i] = falseValues[i];
		}
	}
}

}

#endif
