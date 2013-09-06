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

#ifndef CORAL_NUMERICOPERATION_H
#define CORAL_NUMERICOPERATION_H

#include <vector>
#include "../src/Numeric.h"

#define DECLARE_NUMERIC_OPERATION(operation, typeA, typeB) \
	void operation_##operation##_##typeA##_##typeB##_array_to_array(Numeric *operandA, Numeric *operandB, Numeric *result, unsigned int slice); \
	void operation_##operation##_##typeA##_##typeB##_single_to_array(Numeric *operandA, Numeric *operandB, Numeric *result, unsigned int slice); \
	void operation_##operation##_##typeA##_##typeB##_array_to_single(Numeric *operandA, Numeric *operandB, Numeric *result, unsigned int slice) \

#define DECLARE_PASSTRHOUGH_OPERATION(type) \
	void operation_##type##_passThrough(Numeric *operandA, Numeric *operandB, Numeric *result, unsigned int slice); \

namespace coral{

class NumericOperation{
public:
	enum Operation{
		numericOperationAdd = 0,
		numericOperationSub,
		numericOperationMul,
		numericOperationDiv,
	};
	
	NumericOperation();
	void selectOperands(Numeric::Type typeA, Numeric::Type typeB);
	void executeSelectedOperation(Numeric *operandA, Numeric *operandB, Numeric *out, unsigned int slice);
	
	void clearSelectedOperation(){
		_selectedOperation = 0;
	}
	
	bool operationSelected(){
		bool selected = false;
		if(_selectedOperation){
			selected = true;
		}
		
		return selected;
	}
	
	Operation operation(){
		return _operation;
	}
	
	void setOperation(Operation operation){
		_operation = operation;
	}
	
	static bool allowOperation(Operation operation, Numeric::Type typeA, Numeric::Type typeB);
	
private:
	void(NumericOperation::*_selectedOperation)(Numeric*, Numeric*, Numeric*, unsigned int);
	Operation _operation;
	
	DECLARE_NUMERIC_OPERATION(add, int, int);
	DECLARE_NUMERIC_OPERATION(add, int, float);
	DECLARE_NUMERIC_OPERATION(add, float, float);
	DECLARE_NUMERIC_OPERATION(add, float, int);
	DECLARE_NUMERIC_OPERATION(add, vec3, vec3);
	DECLARE_NUMERIC_OPERATION(add, col4, col4);
	
	DECLARE_NUMERIC_OPERATION(sub, int, int);
	DECLARE_NUMERIC_OPERATION(sub, int, float);
	DECLARE_NUMERIC_OPERATION(sub, float, float);
	DECLARE_NUMERIC_OPERATION(sub, float, int);
	DECLARE_NUMERIC_OPERATION(sub, vec3, vec3);
	DECLARE_NUMERIC_OPERATION(sub, col4, col4);
	
	DECLARE_NUMERIC_OPERATION(mul, int, int);
	DECLARE_NUMERIC_OPERATION(mul, int, float);
	DECLARE_NUMERIC_OPERATION(mul, float, float);
	DECLARE_NUMERIC_OPERATION(mul, float, int);
	DECLARE_NUMERIC_OPERATION(mul, vec3, vec3);
	DECLARE_NUMERIC_OPERATION(mul, vec3, float);
	DECLARE_NUMERIC_OPERATION(mul, vec3, int);
	DECLARE_NUMERIC_OPERATION(mul, vec3, matrix44);
	DECLARE_NUMERIC_OPERATION(mul, col4, col4);
	DECLARE_NUMERIC_OPERATION(mul, col4, float);
	DECLARE_NUMERIC_OPERATION(mul, col4, int);
	DECLARE_NUMERIC_OPERATION(mul, matrix44, matrix44);
	
	DECLARE_NUMERIC_OPERATION(div, int, int);
	DECLARE_NUMERIC_OPERATION(div, int, float);
	DECLARE_NUMERIC_OPERATION(div, float, float);
	DECLARE_NUMERIC_OPERATION(div, float, int);
	DECLARE_NUMERIC_OPERATION(div, vec3, vec3);
	DECLARE_NUMERIC_OPERATION(div, vec3, float);
	DECLARE_NUMERIC_OPERATION(div, vec3, int);
	DECLARE_NUMERIC_OPERATION(div, col4, col4);
	DECLARE_NUMERIC_OPERATION(div, col4, float);
	DECLARE_NUMERIC_OPERATION(div, col4, int);
	
	DECLARE_PASSTRHOUGH_OPERATION(int);
	DECLARE_PASSTRHOUGH_OPERATION(float);
	DECLARE_PASSTRHOUGH_OPERATION(vec3);
	DECLARE_PASSTRHOUGH_OPERATION(col4);
	DECLARE_PASSTRHOUGH_OPERATION(matrix44);
};

}

#endif
