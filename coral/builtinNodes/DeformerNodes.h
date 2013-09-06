#ifndef CORAL_DEFORMERNODES_H
#define CORAL_DEFORMERNODES_H

#include "../src/Node.h"
#include "../src/NumericAttribute.h"

namespace coral{

class SkinWeightDeformer: public Node{
public:
	SkinWeightDeformer(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);

private:
	NumericAttribute *_skinWeightVertices;
	NumericAttribute *_skinWeightDeformers;
	NumericAttribute *_skinWeightValues;
	NumericAttribute *_points;
	NumericAttribute *_deformers;
	NumericAttribute *_bindPoseDeformers;
	NumericAttribute *_outPoints;
};

}

#endif
