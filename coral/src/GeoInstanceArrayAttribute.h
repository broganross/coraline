#ifndef CORAL_GEOINSTANCEARRAYATTRIBUTE_H
#define CORAL_GEOINSTANCEARRAYATTRIBUTE_H

#include "GeoInstanceArray.h"
#include "Node.h"
#include "Attribute.h"

namespace coral{

class CORAL_EXPORT GeoInstanceArrayAttribute: public Attribute{
public:
	GeoInstanceArrayAttribute(const std::string &name, Node* parent);
	
	GeoInstanceArray *value();
	GeoInstanceArray *outValue();
};

}

#endif
