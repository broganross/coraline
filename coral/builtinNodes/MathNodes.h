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

#ifndef CORAL_MATHNODES_H
#define CORAL_MATHNODES_H

#include <vector>
#include <cmath>
#include "../src/Node.h"
#include "../src/Attribute.h"
#include "../src/NumericAttribute.h"
#include "../src/EnumAttribute.h"

namespace coral{

class Length: public Node{
public:
	Length(const std::string &name, Node *parent);
	void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB);
	void attributeSpecializationChanged(Attribute *attribute);
	void updateSlice(Attribute *attribute, unsigned int slice);
	
private:
	NumericAttribute *_element;
	NumericAttribute *_length;
	void(Length::*_selectedOperation)(Numeric*, Numeric*, unsigned int);

	void updateVec3(Numeric *element, Numeric *length, unsigned int slice);
	void updateQuat(Numeric *element, Numeric *length, unsigned int slice);

};

class Inverse: public Node{
public:
	Inverse(const std::string &name, Node *parent);
	void attributeSpecializationChanged(Attribute *attribute);
	void updateSlice(Attribute *attribute, unsigned int slice);
	
private:
	NumericAttribute *_element;
	NumericAttribute *_inverse;
	void(Inverse::*_selectedOperation)(Numeric*, Numeric*, unsigned int);

	void updateMatrix44(Numeric *element, Numeric *inverse, unsigned int slice);
	void updateQuat(Numeric *element, Numeric *inverse, unsigned int slice);
};

class Abs: public Node{
public:
	Abs(const std::string &name, Node *parent);
	void attributeSpecializationChanged(Attribute *attribute);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	NumericAttribute *_inNumber;
	NumericAttribute *_outNumber;
	void(Abs::*_selectedOperation)(Numeric*, Numeric*, unsigned int);
	
	void abs_int(Numeric *inNumber, Numeric *outNumber, unsigned int slice);
	void abs_float(Numeric *inNumber, Numeric *outNumber, unsigned int slice);
};

class CrossProduct: public Node{
public:
	CrossProduct(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);
	
private:
	NumericAttribute *_vector0;
	NumericAttribute *_vector1;
	NumericAttribute *_crossProduct;
};

class DotProduct: public Node{
public:
	DotProduct(const std::string &name, Node *parent);
	void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB);
	void attributeSpecializationChanged(Attribute *attribute);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	NumericAttribute *_element0;
	NumericAttribute *_element1;
	NumericAttribute *_dotProduct;
	void(DotProduct::*_selectedOperation)(Numeric*, Numeric*,  Numeric*, unsigned int);

	void updateVec3(Numeric *element0, Numeric *element1, Numeric *dotProduct, unsigned int slice);
	void updateQuat(Numeric *element0, Numeric *element1, Numeric *dotProduct, unsigned int slice);
};

class Normalize: public Node{
public:
	Normalize(const std::string &name, Node *parent);
	void attributeSpecializationChanged(Attribute *attribute);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	NumericAttribute *_element;
	NumericAttribute *_normalized;
	void(Normalize::*_selectedOperation)(Numeric*, Numeric*, unsigned int);

	void updateVec3(Numeric *element, Numeric *normalized, unsigned int slice);
	void updateQuat(Numeric *element, Numeric *normalized, unsigned int slice);
};

class TrigonometricFunctions: public Node{
public:
	TrigonometricFunctions(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	NumericAttribute *_inNumber;
	NumericAttribute *_outNumber;
	EnumAttribute *_function;
};

class Radians: public Node{
public:
	Radians(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	NumericAttribute *_inNumber;
	NumericAttribute *_outNumber;
};

class Degrees: public Node{
public:
	Degrees(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	NumericAttribute *_inNumber;
	NumericAttribute *_outNumber;
};

class Ceil: public Node{
public:
	Ceil(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	NumericAttribute *_inNumber;
	NumericAttribute *_outNumber;
};

class Floor: public Node{
public:
	Floor(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	NumericAttribute *_inNumber;
	NumericAttribute *_outNumber;
};

class Round: public Node{
public:
	Round(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	NumericAttribute *_inNumber;
	NumericAttribute *_outNumber;
};

class Exp: public Node{
public:
	Exp(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	NumericAttribute *_inNumber;
	NumericAttribute *_outNumber;
};

class Log: public Node{
public:
	Log(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	NumericAttribute *_inNumber;
	NumericAttribute *_outNumber;
};

class Pow: public Node{
public:
	Pow(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	NumericAttribute *_base;
	NumericAttribute *_exponent;
	NumericAttribute *_outNumber;
};

class Sqrt: public Node{
public:
	Sqrt(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	NumericAttribute *_inNumber;
	NumericAttribute *_outNumber;
};

class Atan2: public Node{
public:
	Atan2(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	NumericAttribute *_inNumberX;
	NumericAttribute *_inNumberY;
	NumericAttribute *_outNumber;
};

class Min: public Node{
public:
	Min(const std::string &name, Node *parent);
	void attributeSpecializationChanged(Attribute *attribute);
	void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	NumericAttribute *_inNumber;
	NumericAttribute *_outNumber;
	void(Min::*_selectedOperation)(Numeric*, Numeric*, unsigned int);

	void min_int(Numeric *inNumber, Numeric *outNumber, unsigned int slice);
	void min_float(Numeric *inNumber, Numeric *outNumber, unsigned int slice);
};
class Max: public Node{
public:
	Max(const std::string &name, Node *parent);
	void attributeSpecializationChanged(Attribute *attribute);
	void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	NumericAttribute *_inNumber;
	NumericAttribute *_outNumber;
	void(Max::*_selectedOperation)(Numeric*, Numeric*, unsigned int);

	void max_int(Numeric *inNumber, Numeric *outNumber, unsigned int slice);
	void max_float(Numeric *inNumber, Numeric *outNumber, unsigned int slice);
};

class Average: public Node{
public:
	Average(const std::string &name, Node *parent);
	void attributeSpecializationChanged(Attribute *attribute);
	void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	NumericAttribute *_inNumber;
	NumericAttribute *_outNumber;
	void(Average::*_selectedOperation)(Numeric*, Numeric*, unsigned int);

	void average_int(Numeric *inNumber, Numeric *outNumber, unsigned int slice);
	void average_float(Numeric *inNumber, Numeric *outNumber, unsigned int slice);
	void average_vec3(Numeric *inNumber, Numeric *outNumber, unsigned int slice);
};

class Slerp: public Node{
public:
	Slerp(const std::string &name, Node *parent);
	void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	NumericAttribute *_inQuat1;
	NumericAttribute *_inQuat2;
	NumericAttribute *_param;
	NumericAttribute *_outNumber;
};

class QuatMultiply: public Node{
public:
	QuatMultiply(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	NumericAttribute *_quat0;
	NumericAttribute *_quat1;
	NumericAttribute *_outQuat;
};

class Negate: public Node{
public:
	Negate(const std::string &name, Node *parent);
	void attributeSpecializationChanged(Attribute *attribute);
	void updateSlice(Attribute *attribute, unsigned int slice);
	
private:
	NumericAttribute *_element;
	NumericAttribute *_negated;
	void(Negate::*_selectedOperation)(Numeric*, Numeric*, unsigned int);

	void updateVec3(Numeric *element, Numeric *negated, unsigned int slice);
	void updateMatrix44(Numeric *element, Numeric *negated, unsigned int slice);
};

}

#endif
