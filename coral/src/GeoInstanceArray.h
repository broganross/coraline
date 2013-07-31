#ifndef CORAL_GEOINSTANCEARRAY_H
#define CORAL_GEOINSTANCEARRAY_H

#include <vector>
#include <ImathMatrix.h>
#include "Value.h"
#include "Geo.h"

namespace coral{

class CORAL_EXPORT GeoInstanceArray: public Value{
public:
	GeoInstanceArray();

	void setData(const std::vector<Geo*> &sourceGeos, const std::vector<Imath::M44f> &locations, const std::vector<int> &selector);
	const std::vector<Geo*> &sourceGeos();
	const std::vector<Imath::M44f> &locations();
	const std::vector<int> &selector();
	const std::vector<std::vector<Imath::M44f> > &selectedLocations();

private:
	std::vector<Geo*> _sourceGeos;
	std::vector<Imath::M44f> _locations;
	std::vector<int> _selector;
	std::vector<std::vector<Imath::M44f> > _selectedLocations;
};

}

#endif
