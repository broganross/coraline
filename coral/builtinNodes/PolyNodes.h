

#ifndef CORAL_POLYNODES_H
#define CORAL_POLYNODES_H

#include "../src/Node.h"
#include "../src/Attribute.h"
#include "../src/NumericAttribute.h"
#include "../src/PolyAttribute.h"
#include "../src/PassThroughAttribute.h"

namespace coral{

class ConstantArray: public Node{
public:
	ConstantArray(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);
	void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB);

private:
	NumericAttribute *_size;
	PolyAttribute *_constant;
	PolyAttribute *_array;
};

class BuildArray: public Node{
public:
	BuildArray(const std::string &name, Node *parent);
	void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB);
	void attributeSpecializationChanged(Attribute *attribute);
	void updateSlice(Attribute *attribute, unsigned int slice);
	void addAttribute();

private:
	PolyAttribute *_array;
	void(BuildArray::*_selectedOperation)(const std::vector<Attribute*>&, int, PolyValue*, unsigned int);
	void updateInt(const std::vector<Attribute*> &inAttrs, int arraySize, PolyValue *array, unsigned int slice);
	void updateFloat(const std::vector<Attribute*> &inAttrs, int arraySize, PolyValue *array, unsigned int slice);
	void updateVec3(const std::vector<Attribute*> &inAttrs, int arraySize, PolyValue *array, unsigned int slice);
	void updateCol4(const std::vector<Attribute*> &inAttrs, int arraySize, PolyValue *array, unsigned int slice);
	void updateMatrix44(const std::vector<Attribute*> &inAttrs, int arraySize, PolyValue *array, unsigned int slice);
	void updateString(const std::vector<Attribute*> &inAttrs, int arraySize, PolyValue *array, unsigned int slice);
	void updatePath(const std::vector<Attribute*> &inAttrs, int arraySize, PolyValue *array, unsigned int slice);
	void updateBool(const std::vector<Attribute*> &inAttrs, int arraySize, PolyValue *array, unsigned int slice);

};

class GetArrayElement: public Node{
public:
	GetArrayElement(const std::string &name, Node *parent);
	void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB);
	void attributeSpecializationChanged(Attribute *attribute);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	PolyAttribute *_array;
	NumericAttribute *_index;
	PolyAttribute *_element;
	void(GetArrayElement::*_selectedOperation)(PolyValue *, const std::vector<int> &, PolyValue *, unsigned int);

	void updateInt(PolyValue *array, const std::vector<int> &index, PolyValue *element, unsigned int slice);
	void updateFloat(PolyValue *array, const std::vector<int> &index, PolyValue *element, unsigned int slice);
	void updateVec3(PolyValue *array, const std::vector<int> &index, PolyValue *element, unsigned int slice);
	void updateCol4(PolyValue *array, const std::vector<int> &index, PolyValue *element, unsigned int slice);
	void updateMatrix44(PolyValue *array, const std::vector<int> &index, PolyValue *element, unsigned int slice);
	void updateString(PolyValue *array, const std::vector<int> &index, PolyValue *element, unsigned int slice);
	void updatePath(PolyValue *array, const std::vector<int> &index, PolyValue *element, unsigned int slice);
	void updateBool(PolyValue *array, const std::vector<int> &index, PolyValue *element, unsigned int slice);
};

class SetArrayElement: public Node{
public:
	SetArrayElement(const std::string &name, Node *parent);
	void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB);
	void attributeSpecializationChanged(Attribute *attribute);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	PolyAttribute *_array;
	NumericAttribute *_index;
	PolyAttribute *_element;
	PolyAttribute *_outArray;
	void(SetArrayElement::*_selectedOperation)(PolyValue *, const std::vector<int> &, PolyValue *, PolyValue *, unsigned int);

	void updateInt(PolyValue *array, const std::vector<int> &index, PolyValue *element, PolyValue *outArray, unsigned int slice);
	void updateFloat(PolyValue *array, const std::vector<int> &index, PolyValue *element, PolyValue *outArray, unsigned int slice);
	void updateVec3(PolyValue *array, const std::vector<int> &index, PolyValue *element, PolyValue *outArray, unsigned int slice);
	void updateCol4(PolyValue *array, const std::vector<int> &index, PolyValue *element, PolyValue *outArray, unsigned int slice);
	void updateMatrix44(PolyValue *array, const std::vector<int> &index, PolyValue *element, PolyValue *outArray, unsigned int slice);
	void updateString(PolyValue *array, const std::vector<int> &index, PolyValue *element, PolyValue *outArray, unsigned int slice);
	void updatePath(PolyValue *array, const std::vector<int> &index, PolyValue *element, PolyValue *outArray, unsigned int slice);
	void updateBool(PolyValue *array, const std::vector<int> &index, PolyValue *element, PolyValue *outArray, unsigned int slice);

};

}


#endif
