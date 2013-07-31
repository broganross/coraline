
#include <map> 
#include <vector>
#include <ImathVec.h>
#include <ImathMatrix.h>

#include "DeformerNodes.h"
#include "../src/Numeric.h"

using namespace coral;

namespace {

	int findMinorNumericSize(NumericAttribute *attrs[], int numAttrs){
		int minorSize = -1;
		for(int i = 0; i < numAttrs; ++i){
			Numeric *num = attrs[i]->value();
			int size = num->size();
			if(size < minorSize || minorSize == -1){
				minorSize = size;
			}
		}
		if(minorSize == -1){
			minorSize = 0;
		}
		
		return minorSize;
	}

}

SkinWeightDeformer::SkinWeightDeformer(const std::string &name, Node *parent): Node(name, parent){
	_skinWeightVertices = new NumericAttribute("skinWeightVertices", this);
	_skinWeightDeformers = new NumericAttribute("skinWeightDeformers", this);
	_skinWeightValues = new NumericAttribute("skinWeightValues", this);
	_points = new NumericAttribute("points", this);
	_deformers = new NumericAttribute("deformers", this);
	_bindPoseDeformers = new NumericAttribute("bindPoseDeformers", this);
	_outPoints = new NumericAttribute("outPoints", this);

	addInputAttribute(_skinWeightVertices);
	addInputAttribute(_skinWeightDeformers);
	addInputAttribute(_skinWeightValues);
	addInputAttribute(_points);
	addInputAttribute(_deformers);
	addInputAttribute(_bindPoseDeformers);
	addOutputAttribute(_outPoints);

	setAttributeAffect(_skinWeightVertices, _outPoints);
	setAttributeAffect(_skinWeightDeformers, _outPoints);
	setAttributeAffect(_skinWeightValues, _outPoints);
	setAttributeAffect(_points, _outPoints);
	setAttributeAffect(_deformers, _outPoints);
	setAttributeAffect(_bindPoseDeformers, _outPoints);

	setAttributeAllowedSpecialization(_skinWeightVertices, "IntArray");
	setAttributeAllowedSpecialization(_skinWeightDeformers, "IntArray");
	setAttributeAllowedSpecialization(_skinWeightValues, "FloatArray");
	setAttributeAllowedSpecialization(_points, "Vec3Array");
	setAttributeAllowedSpecialization(_deformers, "Matrix44Array");
	setAttributeAllowedSpecialization(_bindPoseDeformers, "Matrix44Array");
	setAttributeAllowedSpecialization(_outPoints, "Vec3Array");


}

void SkinWeightDeformer::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<int> &skinWeightVertices = _skinWeightVertices->value()->intValuesSlice(slice);
	const std::vector<int> &skinWeightDeformers = _skinWeightDeformers->value()->intValuesSlice(slice);
	const std::vector<float> &skinWeightValues = _skinWeightValues->value()->floatValuesSlice(slice);
	const std::vector<Imath::V3f> &points = _points->value()->vec3ValuesSlice(slice);
	const std::vector<Imath::M44f> &deformers = _deformers->value()->matrix44ValuesSlice(slice);
	const std::vector<Imath::M44f> &bindPoseDeformers = _bindPoseDeformers->value()->matrix44ValuesSlice(slice);

	NumericAttribute *attrs[] = {_skinWeightVertices, _skinWeightDeformers, _skinWeightValues};
	int minorSize = findMinorNumericSize(attrs, 3);
	
	std::map<int, std::map<int, Imath::V3f> > displaceMap;

	int pointsSize = points.size();

	NumericAttribute *deformerAttrs[] = {_deformers, _skinWeightValues};
	int deformersSize = findMinorNumericSize(deformerAttrs, 2);
	for(int i = 0; i < minorSize; ++i){
		int vertexId = skinWeightVertices[i];
		int deformerId = skinWeightDeformers[i];

		if(vertexId > -1 && vertexId < pointsSize && deformerId > -1 && deformerId < deformersSize){
			const Imath::V3f &point = points[vertexId];
			const Imath::M44f &deformer = deformers[deformerId];
			const Imath::M44f &bindPoseDeformer = bindPoseDeformers[deformerId];
			float weight = skinWeightValues[i];

			Imath::V3f bindedPoint = point * bindPoseDeformer.inverse();
			bindedPoint = bindedPoint * deformer;
			bindedPoint *= weight;

			displaceMap[vertexId][deformerId] = bindedPoint;
		}
	}

	std::vector<Imath::V3f> outPoints = points;
	for(std::map<int, std::map<int, Imath::V3f> >::iterator it = displaceMap.begin(); it != displaceMap.end(); ++it){
		Imath::V3f &outPoint = outPoints[it->first];
		outPoint = Imath::V3f(0.0, 0.0, 0.0);
		for(std::map<int, Imath::V3f>::iterator subit = it->second.begin(); subit != it->second.end(); ++subit){
			Imath::V3f &displacedPoint = subit->second;
			outPoint += displacedPoint;
		}
	}

	_outPoints->outValue()->setVec3ValuesSlice(slice, outPoints);
}
