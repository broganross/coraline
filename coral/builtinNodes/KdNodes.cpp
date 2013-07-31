
#include "KdNodes.h"

using namespace coral;

FindPointsInRange::FindPointsInRange(const std::string &name, Node *parent): Node(name, parent){
	setSliceable(true);
	
	_point = new NumericAttribute("point", this);
	_range = new NumericAttribute("range", this);
	_points = new NumericAttribute("points", this);
	_pointsInRange = new NumericAttribute("pointsInRange", this);
	_pointsInRangeId = new NumericAttribute("pointsInRangeId", this);
	_pointsInRangeSize = new NumericAttribute("pointsInRangeSize", this);

	addInputAttribute(_point);
	addInputAttribute(_range);
	addInputAttribute(_points);
	addOutputAttribute(_pointsInRange);
	addOutputAttribute(_pointsInRangeId);
	addOutputAttribute(_pointsInRangeSize);

	setAttributeAffect(_point, _pointsInRange);
	setAttributeAffect(_point, _pointsInRangeId);
	setAttributeAffect(_point, _pointsInRangeSize);

	setAttributeAffect(_range, _pointsInRange);
	setAttributeAffect(_range, _pointsInRangeId);
	setAttributeAffect(_range, _pointsInRangeSize);

	setAttributeAffect(_points, _pointsInRange);
	setAttributeAffect(_points, _pointsInRangeId);
	setAttributeAffect(_points, _pointsInRangeSize);

	setAttributeAllowedSpecialization(_point, "Vec3");
	setAttributeAllowedSpecialization(_range, "Float");
	setAttributeAllowedSpecialization(_points, "Vec3Array");
	setAttributeAllowedSpecialization(_pointsInRange, "Vec3Array");
	setAttributeAllowedSpecialization(_pointsInRangeId, "IntArray");
	setAttributeAllowedSpecialization(_pointsInRangeSize, "Int");
}

void FindPointsInRange::updateSlice(Attribute *attribute, unsigned int slice){
	const Imath::V3f &point = _point->value()->vec3ValuesSlice(slice)[0];
	float range = _range->value()->floatValuesSlice(slice)[0];
	if(range < 0.0){
		range = 0.0;
	}

	const std::vector<Imath::V3f> &points = _points->value()->vec3ValuesSlice(slice);

	int dimentions = 3;
	kdtree *tree = kd_create(dimentions);

	int pointsSize = points.size();
	std::vector<int> indices(pointsSize);

	for(int i = 0; i < pointsSize; ++i){
		indices[i] = i;
		const Imath::V3f &currentPoint = points[i];
		kd_insert3f(tree, currentPoint.x, currentPoint.y, currentPoint.z, (void*)&indices[i]);
	}
	
	kdres *res = kd_nearest_range3f(tree, point.x, point.y, point.z, range);

	int resultSize = kd_res_size(res);
	std::vector<Imath::V3f> pointsInRange(resultSize);
	std::vector<int> pointsInRangeId(resultSize);

	int i = 0;
	while(!kd_res_end(res)){
		int pointIndex = *(int*)kd_res_item_data(res);
		pointsInRange[i] = points[pointIndex];
		pointsInRangeId[i] = pointIndex;

		kd_res_next(res);
		i += 1;
	}

	kd_res_free(res);
	kd_free(tree);

	_pointsInRange->outValue()->setVec3ValuesSlice(slice, pointsInRange);
	_pointsInRangeId->outValue()->setIntValuesSlice(slice, pointsInRangeId);
	_pointsInRangeSize->outValue()->setIntValueAtSlice(slice, 0, resultSize);

	setAttributeIsClean(_pointsInRange, true);
	setAttributeIsClean(_pointsInRangeId, true);
	setAttributeIsClean(_pointsInRangeSize, true);
}


