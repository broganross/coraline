#include "EnumAttribute.h"

using namespace coral;

Enum::Enum(): 
Value(),
_currentIndex(0),
_currentIndexChangedCallback(0),
_parentNode(0){
}

void Enum::addEntry(int id, const std::string &value){
	_enum[id] = value;
}

std::vector<int> Enum::indices(){
	std::vector<int> ids;
	for(std::map<int, std::string>::iterator i = _enum.begin(); i != _enum.end(); ++i){
		ids.push_back(i->first);
	}
	
	return ids;
}

std::vector<std::string> Enum::entries(){
	std::vector<std::string> vals;
	for(std::map<int, std::string>::iterator i = _enum.begin(); i != _enum.end(); ++i){
		vals.push_back(i->second);
	}
	
	return vals;
}

std::string Enum::asString(){
	std::string value = "[{";
	for(std::map<int, std::string>::iterator i = _enum.begin(); i != _enum.end(); ++i){
		value += stringUtils::intToString(i->first) + ":\"" + i->second + "\",";
	}
	
	value += "}," + stringUtils::intToString(_currentIndex) + "]";
	
	return value;
}

void Enum::setFromString(const std::string &value){
	std::string fieldsStr = stringUtils::strip(value, "[]{");
	std::vector<std::string> fields;
	stringUtils::split(fieldsStr, fields, "},");

	if(fields.size() == 2){
		std::vector<std::string> entries;
		stringUtils::split(fields[0], entries, ",");
		
		for(int i = 0; i < entries.size(); ++i){
			std::string &entry = entries[i];
			if(entry != ""){
				std::vector<std::string> keyVal;
				stringUtils::split(entry, keyVal, ":");
				
				if(keyVal.size() == 2){
					int key = stringUtils::parseInt(keyVal[0]);
					std::string val = stringUtils::strip(keyVal[1], "\"");
					_enum[key] = val;
				}
			}
		}

		setCurrentIndex(stringUtils::parseInt(fields[1]));
	}
}

void Enum::setCurrentIndex(int index){
	if(_enum.find(index) != _enum.end()){
		_currentIndex = index;
	
		if(_currentIndexChangedCallback && _parentNode && !isDeleted()){
			_currentIndexChangedCallback(_parentNode, this);
		}
	}
}

int Enum::currentIndex(){
	return _currentIndex;
}

void Enum::setCurrentIndexChangedCallback(Node * parentNode, void(*callback)(Node *, Enum *)){
	_currentIndexChangedCallback = callback;
	_parentNode = parentNode;
}


