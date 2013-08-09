#ifndef CORAL_LOOPNODES_H
#define CORAL_LOOPNODES_H

#include "../src/Node.h"
#include "../src/PolyAttribute.h"
#include "../src/Value.h"
#include "../src/PolyValue.h"
#include "../src/NumericAttribute.h"
#include "../src/PassThroughAttribute.h"

namespace coral{

class LoopInputNode: public Node{
public:
	LoopInputNode(const std::string &name, Node *parent);
	void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB);
	void attributeSpecializationChanged(Attribute *attribute);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	PolyAttribute *_globalArray;
	NumericAttribute *_localIndex;
	PolyAttribute *_localElement;
	void(LoopInputNode::*_selectedOperation)(unsigned int, PolyValue *, PolyValue *);

	void updateInt(unsigned int slice, PolyValue *globalArray, PolyValue *localElement);
	void updateFloat(unsigned int slice, PolyValue *globalArray, PolyValue *localElement);
	void updateVec3(unsigned int slice, PolyValue *globalArray, PolyValue *localElement);
	void updateCol4(unsigned int slice, PolyValue *globalArray, PolyValue *localElement);
	void updateMatrix44(unsigned int slice, PolyValue *globalArray, PolyValue *localElement);
	void updateString(unsigned int slice, PolyValue *globaArray, PolyValue *localElement);
	void updatePath(unsigned int slice, PolyValue *globalArray, PolyValue *localElement);
	void updateBool(unsigned int slice, PolyValue *globalArray, PolyValue *localElement);
};

class LoopOutputNode: public Node{
public:
	LoopOutputNode(const std::string &name, Node *parent);
	void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB);
	void attributeSpecializationChanged(Attribute *attribute);
	void update(Attribute *attribute);

private:
	PolyAttribute *_localElement;
	PolyAttribute *_globalArray;
	void(LoopOutputNode::*_selectedOperation)(unsigned int, PolyValue *, PolyValue *);

	void updateInt(unsigned int slices, PolyValue *element, PolyValue *array);
	void updateFloat(unsigned int slices, PolyValue *element, PolyValue *array);
	void updateVec3(unsigned int slices, PolyValue *element, PolyValue *array);
	void updateCol4(unsigned int slices, PolyValue *element, PolyValue *array);
	void updateMatrix44(unsigned int slices, PolyValue *element, PolyValue *array);
	void updateString(unsigned int slice, PolyValue *globaArray, PolyValue *localElement);
	void updatePath(unsigned int slice, PolyValue *globalArray, PolyValue *localElement);
	void updateBool(unsigned int slice, PolyValue *globalArray, PolyValue *localElement);
};

class ForLoopNode: public Node{
public:
	ForLoopNode(const std::string &name, Node *parent);

protected:
	unsigned int computeSlices();

private:
	PolyAttribute *_globalArray;
};
}

#endif
