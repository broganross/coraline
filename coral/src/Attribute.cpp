// <license>
// Copyright (C) 2011 Andrea Interguglielmi, All rights reserved.
// This file is part of the coral repository downloaded from http://code.google.com/p/coral-repo.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
// 
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
// 
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// </license>

#ifdef CORAL_PARALLEL_TBB
	#include <tbb/parallel_for.h>
	#include <tbb/mutex.h>
	#include "coreParallelAlgos.h"
#endif

#include <boost/date_time/posix_time/posix_time.hpp>
#include <string>
#include <map>
#include <vector>
#include <assert.h>

#include "Attribute.h"
#include "Node.h"
#include "NetworkManager.h"
#include "Value.h"
#include "containerUtils.h"
#include "Command.h"
#include "ErrorObject.h"
#include "stringUtils.h"

using namespace coral;

void(*Attribute::_connectToCallback)(Attribute *self, Attribute *other) = 0;
void(*Attribute::_disconnectInputCallback)(Attribute *self) = 0;
void(*Attribute::_disconnectOutputCallback)(Attribute *self, Attribute *other) = 0;
void(*Attribute::_deleteItCallback)(Attribute *self) = 0;
void(*Attribute::_specializationCallBack)(Attribute *self) = 0;

std::vector<void(*)(Attribute *)> _dirtyingDoneCallbackQueue;
bool _cleaningLocked = false;

namespace {
	std::vector<std::string> intersectedSpecialization(const std::vector<std::string> &specialization1, const std::vector<std::string> &specialization2){
		std::vector<std::string> newSpecialization;
	
		for(int i = 0; i < specialization1.size(); ++i){
			if(containerUtils::elementInContainer(specialization1[i], specialization2)){
				newSpecialization.push_back(specialization1[i]);
			}
		}
	
		return newSpecialization;
	}

	#ifdef CORAL_PARALLEL_TBB
		tbb::mutex _globalMutex;
	#endif
}

Attribute::Attribute(const std::string &name, Node *parent):
	NestedObject(name, parent),
	_value(0),
	_inputValue(0),
	_input(0),
	_isClean(false),
	_isOutput(false),
	_isInput(false),
	_passThrough(false),
	// _valueObserved(0),
	_computeTimeSeconds(0),
	_computeTimeMilliseconds(0),
	_notifyParentNodeOnDirty(false){
		
	_dirtyChain.push_back(this);
}

Attribute::~Attribute(){
	if(!isDeleted()){
		setIsDeleted(true);
		deleteIt();
	}
}

void Attribute::setPassThrough(bool value){
	_passThrough = value;
}

bool Attribute::isPassThrough(){
	return _passThrough;
}

Node* Attribute::parent(){
	return (Node*)parentObject();
}

void Attribute::setParent(Node *parent){
	setParentObject(parent);
}

void Attribute::removeAffectFrom(Attribute *attribute){
	if(containerUtils::elementInContainer(attribute, _affectedBy)){
		containerUtils::eraseElementInContainer(attribute, _affectedBy);
	}
}

void Attribute::removeAffect(Attribute *attribute){
	if(containerUtils::elementInContainer(attribute, _affect)){
		containerUtils::eraseElementInContainer(attribute, _affect);
		
		if(attribute->isAffectedBy(this)){
			attribute->removeAffectFrom(this);
		}
		
		NetworkManager::removeEdge(this, attribute);
	}
}

bool Attribute::isConnectedTo(Attribute *attribute){
	return attribute && containerUtils::elementInContainer(attribute, _outputs);
}

void Attribute::resetInputValuesInChain(){
	_inputValue = _value;
	
	if(_input){
		if(_input->_inputValue){
			_inputValue = _input->_inputValue;
		}
	}
	
	for(int i = 0; i < _outputs.size(); ++i){
		_outputs[i]->resetInputValuesInChain();
	}
	
}

void Attribute::disconnectInput(){
	if(_input){
		NetworkManager::removeEdge(_input, this);
		
		Attribute *oldInput = _input;
		_input = 0;
		_inputValue = _value;
		
		if(oldInput->isConnectedTo(this)){
			oldInput->disconnectOutput(this);
		}
		
		if(!isDeleted()){
			ErrorObject *error = new ErrorObject();
			
			resetInputValuesInChain();
			cacheEvaluationChain();
			
			bool resetBranchSpecialization = true;
			updateBranchSpecializations(resetBranchSpecialization);
			
			dirty();
			
			if(parent()){
				parent()->_attributeConnectionChanged(this);
			}
			
			if(_disconnectInputCallback){
				_disconnectInputCallback(this);
			}
			
			delete error;
		}
	}
}

void Attribute::disconnectOutput(Attribute *attribute){
	if(this->isConnectedTo(attribute)){		
		containerUtils::eraseElementInContainer(attribute, _outputs);
		
		if(attribute->input() == this){
			attribute->disconnectInput();
		}
		
		if(!isDeleted()){
			ErrorObject *error = new ErrorObject();
			
			cacheEvaluationChain();
			
			bool resetBranchSpecialization = true;
			updateBranchSpecializations(resetBranchSpecialization);
			
			if(parent()){
				parent()->_attributeConnectionChanged(this);
			}
			
			delete error;
		}
	}
}

bool Attribute::isClean(){
	return _isClean;
}

void Attribute::setIsClean(bool value){
	_isClean = value;
}

void Attribute::setIsOutput(bool value){
	_isOutput = value;

	_isInput = !_isOutput;
}

void Attribute::setIsInput(bool value){
	_isInput = value;

	_isOutput = !_isInput;
}

Attribute *Attribute::input(){
	return _input;
}

bool Attribute::isAffectedBy(Attribute *attribute){
	return containerUtils::elementInContainer(attribute, _affectedBy);
}

const std::vector<Attribute*> &Attribute::affectedBy(){
	return _affectedBy;
}

const std::vector<Attribute*> &Attribute::affecting(){
	return _affect;
}

void Attribute::addAffectedFrom(Attribute *attribute){
	if(attribute){
		if(containerUtils::elementInContainer(attribute, _affectedBy) == false){
			_affectedBy.push_back(attribute);
		}
	}
}

void Attribute::addAffect(Attribute *attribute){
	if(attribute){
		if(containerUtils::elementInContainer(attribute, _affect) == false){
			_affect.push_back(attribute);
			
			if(attribute->isAffectedBy(this) == false){
				attribute->addAffectedFrom(this);
			}
			
			NetworkManager::addEdge(this, attribute);
			
			cacheEvaluationChain();
		}
	}
}

void Attribute::setValuePtr(Value *value){
	if(_value){
		_value->removeReference();
	}
	
	_value = value;
	_value->addReference();
	_inputValue = _value;
}

void Attribute::valueChanged(){
	dirty();
}

Value *Attribute::value(){
	clean();
	
	return _inputValue;
}

Value *Attribute::outValue(){
	return _value;
}

void Attribute::clean(){
	if(!_cleaningLocked){
		if(_isClean == false){
			if(_isInput && _input == 0){
				_isClean = true;
			}

			_cleaningLocked = true;
			
			boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::universal_time();
			
			for(std::map<int, std::vector<Attribute*> >::iterator i = _cleanChain.begin(); i != _cleanChain.end(); ++i){
				std::vector<Attribute*> &outputAttributes = i->second;

				#ifdef CORAL_PARALLEL_TBB
					tbb::parallel_for(tbb::blocked_range<size_t>(0, outputAttributes.size()), attribute_parallelClean(&outputAttributes));
				#else
					for(int j = 0; j < outputAttributes.size(); ++j){
						outputAttributes[j]->cleanSelf();
					}
				#endif
			}
			
			boost::posix_time::ptime endTime = boost::posix_time::microsec_clock::universal_time();
			_computeTimeSeconds = boost::posix_time::time_period(startTime, endTime).length().total_seconds();
			_computeTimeMilliseconds = boost::posix_time::time_period(startTime, endTime).length().total_milliseconds() % 1000;
			
			_cleaningLocked = false;
		}
	}
}

void Attribute::cleanSelf(){
	if(_isClean == false){
		_isClean = true;

		Node *parentNode = parent();
		if(parentNode){
			std::vector<Attribute*> inputsToClean = _inputsCleanChain[id()];
			for(int i = 0; i < inputsToClean.size(); ++i){
				inputsToClean[i]->_isClean = true;
			}	
			
			if(parentNode->updateEnabled()){
				parentNode->doUpdate(this);
			}
		}
	}
}

void Attribute::setNotifyParentNodeOnDirty(bool value){
	_notifyParentNodeOnDirty = value;
}

void Attribute::dirty(bool force){
	if(!_cleaningLocked){
		if(_isClean || force){
			for(int i = 0; i < _dirtyChain.size(); ++i){
				Attribute* attr = _dirtyChain[i];
				attr->_isClean = false;
				attr->onDirtied();
				
				if(attr->_notifyParentNodeOnDirty){
					Node *parentNode = attr->parent();
					if(parentNode){
						parentNode->attributeDirtied(attr);
					}
				}
			}

			processDirtyingDoneCallbackQueue();
		}
	}
	
	for(int i = 0; i < _dirtyChain.size(); ++i){
		Attribute* attr = _dirtyChain[i];
	}
}

void Attribute::forceDirty(){
	dirty(true);
}

void Attribute::processDirtyingDoneCallbackQueue(){
	for(int i = 0; i < _dirtyingDoneCallbackQueue.size(); ++i){
		_dirtyingDoneCallbackQueue[i](this);
	}
	
	_dirtyingDoneCallbackQueue.clear();
}

void Attribute::queueDirtyingDoneCallback(void(*callback)(Attribute *)){
	#ifdef CORAL_PARALLEL_TBB
		tbb::mutex::scoped_lock lock(_globalMutex);
	#endif
	
	_dirtyingDoneCallbackQueue.push_back(callback);
}

Attribute *Attribute::findFirstOutputNotPassThrough(){
	Attribute *attr = 0;
	for(int i = 0; i < _outputs.size(); ++i){
		Attribute *outAttr = _outputs[i];
		if(outAttr->_passThrough){
			attr = outAttr->findFirstOutputNotPassThrough();
			break;
		}
		else{
			attr = outAttr;
			break;
		}
	}
	
	return attr;
}

void Attribute::initValueFromPassThroughFirstOutput(Attribute *attribute){
	// if this attribute got connected to a passThrough, 
	// then we check if we need to initialize this value from the first connected output of the passThrough,
	// we do this to uniform the outputs of a passThrough even before the passThrough gets an input...it's not foundamental, but it's nice to do so.
	
	if(attribute->_passThrough && _value){
		if(!attribute->_input && attribute->_outputs.size()){
			Attribute *source = attribute->findFirstOutputNotPassThrough();
			if(source && source != this){
				_value->copy(source->_value);
			}
		}
	}
}

void Attribute::setInput(Attribute *attribute){
	if(attribute){
		if(_input != attribute){
			if(_input){
				disconnectInput();
			}
			
			_input = attribute;
			_inputValue = _value;

			NetworkManager::addEdge(_input, this);

			resetInputValuesInChain();
			
			if(_input->_passThrough){
				initValueFromPassThroughFirstOutput(_input);
			}
		}
	}
}

bool Attribute::connectTo(Attribute *attribute, ErrorObject *errorObject){
	_outputs.push_back(attribute);
	attribute->setInput(this);
	
	cacheEvaluationChain();
	
	bool resetBranchSpecialization = false;
	bool success = updateBranchSpecializations(resetBranchSpecialization);
	
	if(parent()){
		parent()->_attributeConnectionChanged(this);
	}
	
	if(attribute->parent()){
		attribute->parent()->_attributeConnectionChanged(attribute);
	}
	
	if(_connectToCallback && !isDeleted())
		_connectToCallback(this, attribute);
	
	bool forceDirty = true;
	attribute->dirty(forceDirty);
	
	return success;
}

void Attribute::forceSpecializationUpdate(){
	ErrorObject *error = new ErrorObject();

	bool resetBranchSpecialization = true;
	updateBranchSpecializations(resetBranchSpecialization);
	delete error;
}

const std::vector<Attribute*> &Attribute::outputs(){
	return _outputs;
}

void Attribute::deleteIt(){
	if(_deleteItCallback && !isDeleted()){
		_deleteItCallback(this);
	}
	
	if(parent()){
		parent()->removeAttribute(this);
	}
	
	if(_input){
		disconnectInput();
	}
	
	std::vector<Attribute*> oldAffect = _affect;
	for(std::vector<Attribute*>::iterator itAffect = oldAffect.begin(); itAffect != oldAffect.end(); ++itAffect){
		removeAffect(*itAffect);
	}
	
	std::vector<Attribute*> oldAffectFrom = _affectedBy;
	for(std::vector<Attribute*>::iterator itAffect = oldAffectFrom.begin(); itAffect != oldAffectFrom.end(); ++itAffect){
		(*itAffect)->removeAffect(this);
	}
	
	std::vector<Attribute*> oldOutputs = _outputs;
	for(std::vector<Attribute*>::iterator itOutput = oldOutputs.begin(); itOutput != oldOutputs.end(); ++itOutput){
		disconnectOutput(*itOutput);
	}
	
	std::vector<SpecializationLink*> oldSpecializationLinks = _specializationLinks;
	for(std::vector<SpecializationLink*>::iterator itSpecializationLink = oldSpecializationLinks.begin(); itSpecializationLink != oldSpecializationLinks.end(); ++itSpecializationLink){
		removeSpecializationLink(*itSpecializationLink);
	}
	
	if(_value){
		_value->removeReference();
		_value = 0;
	}
	
	setIsDeleted(true);
}

void Attribute::onDirtied(){
}

bool Attribute::isInput(){
	return _isInput;
}

bool Attribute::isOutput(){
	return _isOutput;
}

std::string Attribute::asScript(){
	std::string script;
	
	if(_value && connectedInputNonPassThrough(this) == 0 && _affectedBy.size() == 0){
		std::string valueSaveScript = _value->asString();
		if(valueSaveScript.empty() == false){
			Command setAttributeValueCmd;
			setAttributeValueCmd.setName("SetAttributeValue");
			setAttributeValueCmd.setArgString("attribute", fullName());
			setAttributeValueCmd.setArgString("value", valueSaveScript);
			script += setAttributeValueCmd.asScript() += "\n";
		}
	}
	
	return script;
}

bool Attribute::specializationContainedOne(const std::vector<std::string> &specialization1, const std::vector<std::string> &specialization2){
	for(int i = 0; i < specialization1.size(); ++i){
		if(containerUtils::elementInContainer(specialization1[i], specialization2)){
			return true;
		}
	}
	
	return false;
}

std::vector<std::string> Attribute::allowedSpecialization(){
	return _allowedSpecialization;
}

void Attribute::collectSpecializationBranch(std::vector<std::pair<Attribute*, Attribute*> > &specializationPairs, std::vector<std::pair<Attribute*, Attribute*> > &specializationLinks, std::map<int, std::vector<std::string> > &specializationMap, bool reset){
	int thisId = id();
	if(specializationMap.find(thisId) == specializationMap.end()){
		if(reset){
			if(_specializationOverride.size() == 1){
				specializationMap[thisId] = _specializationOverride;
			}
			else{
				specializationMap[thisId] = _allowedSpecialization;
			}
		}
		else{
			specializationMap[thisId] = _specialization;
		}
		
		for(int i = 0; i < _specializationLinks.size(); ++i){
			Attribute *attributeA = _specializationLinks[i]->attributeA;
			Attribute *attributeB = _specializationLinks[i]->attributeB;

			std::pair<Attribute*, Attribute*> specLink(attributeA, attributeB);
			if(containerUtils::elementInContainer(specLink, specializationLinks) == false){
				specializationLinks.push_back(specLink);
			}

			if(this != attributeA){
				attributeA->collectSpecializationBranch(specializationPairs, specializationLinks, specializationMap, reset);
			}
			else{
				attributeB->collectSpecializationBranch(specializationPairs, specializationLinks, specializationMap, reset);
			}
		}
		
		if(_input){
			std::pair<Attribute*, Attribute*> specPair(_input, this);
			if(containerUtils::elementInContainer(specPair, specializationPairs) == false){
				specializationPairs.push_back(specPair);
			}

			_input->collectSpecializationBranch(specializationPairs, specializationLinks, specializationMap, reset);
		}

		for(int i = 0; i < _outputs.size(); ++i){
			std::pair<Attribute*, Attribute*> specPair(this, _outputs[i]);
			if(containerUtils::elementInContainer(specPair, specializationPairs) == false){
				specializationPairs.push_back(specPair);
			}

			_outputs[i]->collectSpecializationBranch(specializationPairs, specializationLinks, specializationMap, reset);
		}
	}
}

bool Attribute::updateSpecialization(std::vector<std::pair<Attribute*, Attribute*> > &specializationPairs, std::vector<std::pair<Attribute*, Attribute*> > &specializationLinks, std::map<int, std::vector<std::string> > &specializationMap){
	int oldUnresolved = specializationLinks.size() + specializationPairs.size();

	// solve links
	std::vector<std::pair<Attribute*, Attribute*> >::iterator specLinksIt = specializationLinks.begin();
	while(specLinksIt != specializationLinks.end()){
		std::pair<Attribute*, Attribute*> &specLink = *specLinksIt;
		Attribute *attrA = specLink.first;
		Attribute *attrB = specLink.second;

		std::vector<std::string> &specA = specializationMap[attrA->id()];
		std::vector<std::string> &specB = specializationMap[attrB->id()];

		if(specA.size() > 1 || specB.size() > 1){
			Node *parentNode = attrA->parent();
			if(parentNode){
				parentNode->updateSpecializationLink(attrA, attrB, specA, specB);
			}
		}

		if(specA.size() == 1 && specB.size() == 1){
			specLinksIt = specializationLinks.erase(specLinksIt);
		}
		else{
			++specLinksIt;
		}
	}

	// solve pairs
	std::vector<std::pair<Attribute*, Attribute*> >::iterator specPairsIt = specializationPairs.begin();
	while(specPairsIt != specializationPairs.end()){
		std::pair<Attribute*, Attribute*> &specPair = *specPairsIt;

		Attribute *attrA = specPair.first;
		Attribute *attrB = specPair.second;

		std::vector<std::string> &specA = specializationMap[attrA->id()];
		std::vector<std::string> &specB = specializationMap[attrB->id()];

		if(attrA->_passThrough && specA.size() == 0){
			specA = specB;
		}
		else if(attrB->_passThrough && specB.size() == 0){
			specB = specA;
		}
		else if(specA != specB){
			std::vector<std::string> newSpec = intersectedSpecialization(specA, specB);
			
			specA = newSpec;
			specB = newSpec;
		}

		if(specA.size() == 1 && specB.size() == 1){
			specPairsIt = specializationPairs.erase(specPairsIt);
		}
		else{
			++specPairsIt;
		}
	}

	int currentUnresolved = specializationLinks.size() + specializationPairs.size();

	bool complete = false;
	if(oldUnresolved == currentUnresolved){
		complete = true;
	}

	return complete;
}

bool Attribute::updateBranchSpecializations(bool reset){
	bool success = true;

	// collect
	std::map<int, std::vector<std::string> > specializationMap;
	std::vector<std::pair<Attribute*, Attribute*> > specializationPairs;
	std::vector<std::pair<Attribute*, Attribute*> > specializationLinks;
	collectSpecializationBranch(specializationPairs, specializationLinks, specializationMap, reset);

	// specialize
	int maxIter = 100;
	int i = 0;
	for(; i < maxIter; ++i){
		bool complete = updateSpecialization(specializationPairs, specializationLinks, specializationMap);
		if(complete){
			break;
		}
	}

	if(i == maxIter){
		std::cout << "Maximum recursion reached during specialization." << std::endl;
		success = false;
	}

	// set resulting specialization
	std::map<int, std::vector<std::string> >::iterator specializationMapIt = specializationMap.begin();
	for(; specializationMapIt != specializationMap.end(); ++specializationMapIt){
		int id = specializationMapIt->first;
		std::vector<std::string> newSpec = specializationMapIt->second;
	
		Object *object = NetworkManager::findObjectById(id);
		if(object){
			Attribute *attribute = (Attribute*)object;
			attribute->setSpecialization(newSpec);
		}
	}

	return success;
}

void Attribute::onSettingSpecialization(const std::vector<std::string> &specialization){
}

void Attribute::setSpecialization(const std::vector<std::string> &specialization){
	if(specialization != _specialization && !isDeleted()){
		_specialization = specialization;
		
		onSettingSpecialization(specialization);
		
		Node *parentNode = parent();
		if(parentNode){
			parentNode->attributeSpecializationChanged(this);
		}
		
		if(_specializationCallBack){
			_specializationCallBack(this);
		}
		
		dirty();
	}
	else if(_passThrough && !isDeleted()){
		// if this is a passThrough we need to update the ui so it can pickup the right colors from the connected attributes
		if(_specializationCallBack){
			_specializationCallBack(this);
		}
	}
}

std::vector<std::string> Attribute::specialization(){
	return _specialization;
}

bool Attribute::allowConnectionTo(Attribute *attribute){
	bool allow = false;
	
	if((isPassThrough() || attribute->isPassThrough()) && (specialization().size() == 0 || attribute->specialization().size() == 0)){
		allow = true;
	}
	else if(specialization().size() > 0 && attribute->specialization().size() > 0){
		allow = specializationContainedOne(_specialization, attribute->_specialization);
	}
	else{
		allow = classNames() == attribute->classNames();
	}
	
	return allow;
}

void Attribute::setAllowedSpecialization(const std::vector<std::string> &specialization){
	_allowedSpecialization = specialization;
	_specialization = specialization;

	onSettingSpecialization(specialization);
}

void Attribute::linkSpecializationTo(Attribute *attribute){
	SpecializationLink *specializationLink = new SpecializationLink();
	specializationLink->attributeA = this;
	specializationLink->attributeB = attribute;
	
	_specializationLinks.push_back(specializationLink);
	attribute->_specializationLinks.push_back(specializationLink);
}

void Attribute::removeSpecializationLink(SpecializationLink *specializationLink){
	containerUtils::eraseElementInContainer(specializationLink, specializationLink->attributeA->_specializationLinks);
	containerUtils::eraseElementInContainer(specializationLink, specializationLink->attributeB->_specializationLinks);
	delete specializationLink;
}

std::string Attribute::shortDebugInfo(){
	std::string info;

	info += "attribute: " + fullName() + "\n";
	info += "className: " + className() + "\n";
	info += "isClean: " + stringUtils::boolToString(_isClean) + "\n";
	info += "allowedSpecialization: " + stringUtils::vectorToString<std::string>(_allowedSpecialization) + "\n";
	info += "specialization: " + stringUtils::vectorToString<std::string>(_specialization);

	return info;
}

std::string Attribute::debugInfo(){
	std::string info = shortDebugInfo() + "\n";
	
	info += "last cleaning took: secs:" + stringUtils::intToString(_computeTimeSeconds) + ", millisecs: " + stringUtils::intToString(_computeTimeMilliseconds) + "\n";
	
	info += "dirty chain: [";
	for(int i = 0; i < _dirtyChain.size(); ++i){
		info += _dirtyChain[i]->fullName() + ", ";
	}
	info += "]\n";
	
	info += "clean chain: [";
	for(std::map<int, std::vector<Attribute*> >::iterator i = _cleanChain.begin(); i != _cleanChain.end(); ++i){
		std::vector<Attribute*> &outputAttributes = i->second;
		info += "[";
		for(int j = 0; j < outputAttributes.size(); ++j){
			Node *parentNode = outputAttributes[j]->parent();
			info += parentNode->fullName() + ", ";
		}
		info += "] -> ";
	}
	info += "]\n";
	
	return info;
}

void Attribute::cacheDirtyChainUpstream(){
	std::vector<Attribute*> attributes;
	NetworkManager::getUpstreamChain(this, attributes);
	for(int i = 0; i < attributes.size(); ++i){
		NetworkManager::getDownstreamChain(attributes[i], attributes[i]->_dirtyChain);
	}
}

void Attribute::cacheCleanChainDownstream(){
	std::vector<Attribute*> attributes;
	NetworkManager::getDownstreamChain(this, attributes);
	for(int i = 0; i < attributes.size(); ++i){
		NetworkManager::getCleanChain(attributes[i], attributes[i]->_cleanChain, attributes[i]->_inputsCleanChain);
	}
}

void Attribute::cacheEvaluationChain(){
	cacheDirtyChainUpstream();
	cacheCleanChainDownstream();
}

std::vector<Attribute*> Attribute::specializationLinkedTo(){
	std::vector<Attribute*> attrs;
	for(int i = 0; i < _specializationLinks.size(); ++i){
		if(_specializationLinks[i]->attributeB != this){
			attrs.push_back(_specializationLinks[i]->attributeB);
		}
	}
	
	return attrs;
}

std::vector<Attribute*> Attribute::specializationLinkedBy(){
	std::vector<Attribute*> attrs;
	for(int i = 0; i < _specializationLinks.size(); ++i){
		if(_specializationLinks[i]->attributeB == this){
			attrs.push_back(_specializationLinks[i]->attributeB);
		}
	}
	
	return attrs;
}

Attribute *Attribute::connectedNonPassThrough(){
    Attribute *input = _input;
    while(input){
    	if(!input->_passThrough){
    		return input;
    	}
    	else{
    		input = input->_input;
    	}
    }

	for(int i = 0; i < _dirtyChain.size(); ++i){
		Attribute *outAttr = _dirtyChain[i];
		if(!outAttr->_passThrough){
			return  outAttr;
		}
	} 

    return 0;
}

Attribute *Attribute::connectedInputNonPassThrough(Attribute *attribute){
	Attribute *input = 0;
	if(attribute->_input){
		input = connectedInputNonPassThrough(attribute->_input);
	}

	if(input == 0 && attribute != this && !attribute->_passThrough){
		input = attribute;
	}
	
	return input;
}

void Attribute::setSpecializationOverride(const std::string &specialization){
	std::vector<std::string> spec(1);
	spec[0] = specialization;

	_specializationOverride = spec;
	setSpecialization(_specializationOverride);
}

void Attribute::removeSpecializationOverride(){
	_specializationOverride.clear();
	setSpecialization(_allowedSpecialization);
}

std::string Attribute::specializationOverride(){
	std::string override = "";
	if(_specializationOverride.size()){
		override = _specializationOverride[0];
	}
	
	return override;
}
