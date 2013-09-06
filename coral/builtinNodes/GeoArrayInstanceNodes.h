#ifndef CORAL_GEOARRAYINSTANCENODES_H
#define CORAL_GEOARRAYINSTANCENODES_H

#include "../src/GeoAttribute.h"
#include "../src/NumericAttribute.h"
#include "../src/GeoInstanceArrayAttribute.h"

namespace coral{

class GeoInstanceGenerator: public Node{
public:
	GeoInstanceGenerator(const std::string &name, Node *parent);
	void updateSlice(Attribute *attribute, unsigned int slice);
	void addInputGeo();

private:
	GeoAttribute *_geo;
	NumericAttribute *_locations;
	NumericAttribute *_selector;
	GeoInstanceArrayAttribute *_geoInstance;
};

}

#endif
