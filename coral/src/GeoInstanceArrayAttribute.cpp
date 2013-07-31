#include "GeoInstanceArrayAttribute.h"

using namespace coral;

GeoInstanceArrayAttribute::GeoInstanceArrayAttribute(const std::string &name, Node* parent): 
Attribute(name, parent){
	setClassName("GeoInstanceArrayAttribute");	
	setValuePtr(new GeoInstanceArray());
	
	std::vector<std::string> allowedSpecialization;
	allowedSpecialization.push_back("GeoInstanceArray");
	setAllowedSpecialization(allowedSpecialization);
}

GeoInstanceArray *GeoInstanceArrayAttribute::value(){
	return (GeoInstanceArray*)Attribute::value();
}

GeoInstanceArray *GeoInstanceArrayAttribute::outValue(){
	return (GeoInstanceArray*)Attribute::outValue();
}


