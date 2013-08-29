
#include "StringNode.h"

using namespace coral;

AddStringNode::AddStringNode(const std::string &name, Node *parent):
Node(name, parent),
_selectedOperation(0){
	setSliceable(true);
	_in0 = new StringAttribute("in0", this);
	_in1 = new StringAttribute("in1", this);
	_out = new StringAttribute("out", this);

	addInputAttribute(_in0);
	addInputAttribute(_in1);
	addOutputAttribute(_out);

	setAttributeAffect(_in0, _out);
	setAttributeAffect(_in1, _out);

	std::vector<std::string> specs;
	specs.push_back("String");
	specs.push_back("StringArray");
	specs.push_back("Path");
	specs.push_back("PathArray");

	setAttributeAllowedSpecializations(_in0, specs);
	setAttributeAllowedSpecializations(_in1, specs);
	setAttributeAllowedSpecializations(_out, specs);

	addAttributeSpecializationLink(_in0, _out);
}

void AddStringNode::updateSpecializationLink(Attribute *attrA, Attribute *attrB, std::vector<std::string> &specA, std::vector<std::string> &specB){
	specB.resize(specA.size());
	for (int i=0; i < specA.size(); ++i){
		specB[i] = specA[i];
	}

}

void AddStringNode::attributeSpecializationChanged(Attribute *attr){
	_selectedOperation = 0;
	String::Type in0type = _in0->outValue()->type();
	if(in0type != String::stringTypeAny){
		if (in0type == String::stringType){
			_selectedOperation = &AddStringNode::updateString;
		} else if (in0type == String::pathType){
			_selectedOperation = &AddStringNode::updatePath;
		}
	}

}

void AddStringNode::updateSlice(Attribute *attr, unsigned int slice){
	if (_selectedOperation){
		(this->*_selectedOperation)(_in0->value(), _in1->value(), _out->outValue(), slice);
	}
}

void AddStringNode::updateString(String *in0, String *in1, String *out, unsigned int slice){
	const std::vector<std::string> in0vals = in0->stringValuesSlice(slice);
	const std::vector<std::string> in1vals = in1->stringValuesSlice(slice);
	unsigned int size = in0vals.size();
	std::vector<std::string> outVals(size);
	for (int i=0; i < size; ++i){
		std::string strValue = in0vals[i];
		strValue += in1vals[i];
		outVals[i] = strValue;
	}
	out->setStringValuesSlice(slice, outVals);
}

void AddStringNode::updatePath(String *in0, String *in1, String *out, unsigned int slice){
	const std::vector<std::string> in0vals = in0->pathValuesSlice(slice);
	const std::vector<std::string> in1vals = in1->pathValuesSlice(slice);
	unsigned int size = in0vals.size();
	std::vector<std::string> outVals(size);
#ifdef _WIN32
	std::string osSep = "\\";
#else
	std::string osSep = "/";
#endif
	for (int i=0; i<size; ++i){
		std::string strValue = in0vals[i];
		strValue += osSep + in1vals[i];
		outVals[i] = strValue;
	}
	out->setPathValuesSlice(slice, outVals);
}
