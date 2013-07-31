
#include "GeoArrayInstanceNodes.h"
#include "../src/Numeric.h"
#include "../src/stringUtils.h"
#include <ImathMatrix.h>

using namespace coral;

GeoInstanceGenerator::GeoInstanceGenerator(const std::string &name, Node *parent):
Node(name, parent){
	setAllowDynamicAttributes(true);

	_geo = new GeoAttribute("geo", this);
	_locations = new NumericAttribute("locations", this);
	_selector = new NumericAttribute("selector", this);
	_geoInstance = new GeoInstanceArrayAttribute("geoInstance", this);

	addInputAttribute(_geo);
	addInputAttribute(_locations);
	addInputAttribute(_selector);
	addOutputAttribute(_geoInstance);

	setAttributeAffect(_geo, _geoInstance);
	setAttributeAffect(_locations, _geoInstance);
	setAttributeAffect(_selector, _geoInstance);

	setAttributeAllowedSpecialization(_locations, "Matrix44Array");
	setAttributeAllowedSpecialization(_selector, "IntArray");
}

void GeoInstanceGenerator::addInputGeo(){
	std::string numStr = stringUtils::intToString(dynamicAttributes().size());
	GeoAttribute *attr = new GeoAttribute("geo" + numStr, this);
	addInputAttribute(attr);
	setAttributeAffect(attr, _geoInstance);
	addDynamicAttribute(attr);
}

void GeoInstanceGenerator::updateSlice(Attribute *attribute, unsigned int slice){
	const std::vector<Imath::M44f> &locations = _locations->value()->matrix44ValuesSlice(slice);
	const std::vector<int> &selector = _selector->value()->intValuesSlice(slice);

	std::vector<Geo*> sourceGeos;
	sourceGeos.push_back(_geo->value());

	std::vector<Attribute*> attrs = dynamicAttributes();
	for(int i = 0; i < attrs.size(); ++i){
		Geo *geo = (Geo*)attrs[i]->value();
		sourceGeos.push_back(geo);
	}
	
	GeoInstanceArray *geoInstance = _geoInstance->outValue();
	geoInstance->setData(sourceGeos, locations, selector);
}

