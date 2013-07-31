#ifndef CORAL_LOOPNODES_H
#define CORAL_LOOPNODES_H

#include "../src/Node.h"
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
	NumericAttribute *_globalArray;
	NumericAttribute *_localIndex;
	NumericAttribute *_localElement;
	void(LoopInputNode::*_selectedOperation)(unsigned int, Numeric *, Numeric *);

	void updateInt(unsigned int slice, Numeric *globalArray, Numeric *localElement);
	void updateFloat(unsigned int slice, Numeric *globalArray, Numeric *localElement);
	void updateVec3(unsigned int slice, Numeric *globalArray, Numeric *localElement);
	void updateCol4(unsigned int slice, Numeric *globalArray, Numeric *localElement);
	void updateMatrix44(unsigned int slice, Numeric *globalArray, Numeric *localElement);
};

class LoopOutputNode: public Node{
public:
	LoopOutputNode(const std::string &name, Node *parent);
	void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB);
	void attributeSpecializationChanged(Attribute *attribute);
	void update(Attribute *attribute);

private:
	NumericAttribute *_localElement;
	NumericAttribute *_globalArray;
	void(LoopOutputNode::*_selectedOperation)(unsigned int, Numeric *, Numeric *);
	
	void updateInt(unsigned int slices, Numeric *element, Numeric *array);
	void updateFloat(unsigned int slices, Numeric *element, Numeric *array);
	void updateVec3(unsigned int slices, Numeric *element, Numeric *array);
	void updateCol4(unsigned int slices, Numeric *element, Numeric *array);
	void updateMatrix44(unsigned int slices, Numeric *element, Numeric *array);
};

class ForLoopNode: public Node{
public:
	ForLoopNode(const std::string &name, Node *parent);

protected:
	unsigned int computeSlices();

private:
	NumericAttribute *_globalArray;
};

}

#endif
