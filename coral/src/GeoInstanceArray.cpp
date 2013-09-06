#include "GeoInstanceArray.h"

using namespace coral;

GeoInstanceArray::GeoInstanceArray(): Value(){

}

void GeoInstanceArray::setData(const std::vector<Geo*> &sourceGeos, const std::vector<Imath::M44f> &locations, const std::vector<int> &selector){
	_locations = locations;
	_sourceGeos = sourceGeos;

	// resize and validate selector according to sourceGeos and locations
	int locationsSize = locations.size();
	int selectorSize = selector.size();
	int sourceGeosSize = sourceGeos.size();

	_selector.clear();
	_selector.resize(locationsSize);

	if(selectorSize > locationsSize){
		selectorSize = locationsSize;
	}
	
	int lastSelector = 0;
	for(int i = 0; i < selectorSize; ++i){
		lastSelector = selector[i];
		if(lastSelector >= sourceGeosSize){
			lastSelector = sourceGeosSize - 1;
		}
		else if(lastSelector < 0){
			lastSelector = 0;
		}

		_selector[i] = lastSelector;
	}

	for(int i = selectorSize; i < locationsSize; ++i){
		_selector[i] = lastSelector;
	}

	// generate locations per geo using selector
	_selectedLocations.clear();
	_selectedLocations.resize(sourceGeosSize);

	for(int i = 0; i < _selector.size(); ++i){
		int geoPos = _selector[i];
		_selectedLocations[geoPos].push_back(_locations[i]);
	}
}

const std::vector<Geo*> &GeoInstanceArray::sourceGeos(){
	return _sourceGeos;
}

const std::vector<Imath::M44f> &GeoInstanceArray::locations(){
	return _locations;
}

const std::vector<int> &GeoInstanceArray::selector(){
	return _selector;
}

const std::vector<std::vector<Imath::M44f> > &GeoInstanceArray::selectedLocations(){
	return _selectedLocations;
}
