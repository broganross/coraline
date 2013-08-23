#ifndef CORAL_LOOPNODES_H
#define CORAL_LOOPNODES_H

#include "../src/Node.h"
#include "../src/NumericAttribute.h"
#include "../src/PassThroughAttribute.h"
#include "../src/StringAttribute.h"


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


class StringLoopInputNode: public Node{
public:
	StringLoopInputNode(const std::string &name, Node *parent);
	void updateSpecializationLink(Attribute *attrA, Attribute *attrB, std::vector<std::string> &specA, std::vector<std::string> &specB);
	void attributeSpecializationChanged(Attribute *attribute);
	void updateSlice(Attribute *attribute, unsigned int slice);


private:
	StringAttribute *_globalArray;
	NumericAttribute *_localIndex;
	StringAttribute *_localElement;
	void(StringLoopInputNode::*_selectedOperation)(unsigned int, String *, String *);

	void updateString(unsigned int slice, String *globalArray, String *localElement);
};


class StringLoopOutputNode: public Node{
public:
	StringLoopOutputNode(const std::string &name, Node *parent);
	void updateSpecializationLink(Attribute *attrA, Attribute *attrB, std::vector<std::string> &specA, std::vector<std::string> &specB);
	void attributeSpecializationChanged(Attribute *attr);
	void update(Attribute *attribute);

private:
	StringAttribute *_localElement;
	StringAttribute *_globalArray;
	void(StringLoopOutputNode::*_selectedOperation)(unsigned int, String *, String *);
	void updateString(unsigned int slices, String *element, String *array);
};


class StringForLoopNode: public Node{
public:
	StringForLoopNode(const std::string &name, Node *parent);

protected:
	unsigned int computeSlices();

private:
	StringAttribute *_globalArray;

};
}

#endif
