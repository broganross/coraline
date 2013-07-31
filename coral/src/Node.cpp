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
	#include "coreParallelAlgos.h"
#endif

#include <boost/date_time/posix_time/posix_time.hpp>

#include "Node.h"
#include "Attribute.h"
#include "NetworkManager.h"
#include "containerUtils.h"
#include "Command.h"
#include "stringUtils.h"

using namespace coral;

void(*Node::_addNodeCallback)(Node *self, Node *node) = 0;
void(*Node::_removeNodeCallback)(Node *self, Node *node) = 0;
void(*Node::_addInputAttributeCallback)(Node *self, Attribute *attribute) = 0;
void(*Node::_addOutputAttributeCallback)(Node *self, Attribute *attribute) = 0;
void(*Node::_removeAttributeCallback)(Node *self, Attribute *attribute) = 0;
void(*Node::_deleteItCallback)(Node *self) = 0;
void(*Node::_connectionChangedCallback)(Node *self, Attribute *attribute) = 0;

namespace {

bool specializationContained(const std::vector<std::string> &specialization1, const std::vector<std::string> &specialization2){
	bool contained = false;
	for(int i = 0; i < specialization1.size(); ++i){
		contained = true;
		if(!containerUtils::elementInContainer(specialization1[i], specialization2)){
			contained = false;
			break;
		}
	}
	
	return contained;
}

}

Node::Node(const std::string &name, Node *parent): 
	NestedObject(name, parent),
	_isInvalid(false),
	_updateEnabled(true),
	_computeTimeSeconds(0),
	_computeTimeMilliseconds(0),
	_computeTimeTicks(0),
	_allowDynamicAttributes(false),
	_constructorDone(false),
	_specializationPreset("none"),
	_slices(1),
	_isSlicer(false),
	_sliceable(false){
	
	_slicer = findParentSlicer();
}

Node::~Node(){
	if(!isDeleted()){
		setIsDeleted(true);
		deleteIt();
	}
}

Node *Node::findParentSlicer(){
	Node *parentSlicer = 0;

	Node *parentNode = parent();
	while(parentNode){
		if(parentNode->_isSlicer){
			parentSlicer = parentNode;
			break;
		}

		parentNode = parentNode->parent();
	}

	return parentSlicer;
}

void Node::setAllowDynamicAttributes(bool value){
	_allowDynamicAttributes = value;
}

bool Node::allowDynamicAttributes(){
	return _allowDynamicAttributes;
}

void Node::setUpdateEnabled(bool value){
	_updateEnabled = value;
}

bool Node::updateEnabled(){
	return _updateEnabled;
}

bool Node::containsNode(Node *node){
	return containerUtils::elementInContainer(node, _nodes);
}

void Node::removeNode(Node *node){
	if(containerUtils::elementInContainer(node, _nodes)){
		containerUtils::eraseElementInContainer(node, _nodes);
		
		if(node->parent() != 0){
			node->setParent(0);
		}
		
		if(_removeNodeCallback && !isDeleted()){
			_removeNodeCallback(this, node);
		}
		
		removeObject(node);
	}
}

void Node::removeAttribute(Attribute *attribute){
	bool removed = false;
	if(containerUtils::elementInContainer(attribute, _inputAttributes)){
		containerUtils::eraseElementInContainer(attribute, _inputAttributes);
		removed = true;
	}
	else if(containerUtils::elementInContainer(attribute, _outputAttributes)){
		containerUtils::eraseElementInContainer(attribute, _outputAttributes);
		removed = true;
	}
	
	if(containerUtils::elementInContainer(attribute, _dynamicAttributes)){
		removeDynamicAttribute(attribute);
	}
	
	if(attribute->parent() == this){
		attribute->setParent(0);
	}
	
	if(removed){
		removeObject(attribute);
	}
}

void Node::addInputAttribute(Attribute *attribute){
	if(containerUtils::elementInContainer(attribute, _inputAttributes) == false){
		attribute->setIsInput(true);
		_inputAttributes.push_back(attribute);
		
		if(attribute->parent() != this){
			attribute->parent()->removeAttribute(attribute);
			attribute->setParent(this);
		}
		
		addObject(attribute);
		
		if(_addInputAttributeCallback && !isDeleted()){
			_addInputAttributeCallback(this, attribute);
		}
	}
}

void Node::addOutputAttribute(Attribute *attribute){
	if(containerUtils::elementInContainer(attribute, _outputAttributes) == false){
		attribute->setIsOutput(true);
		_outputAttributes.push_back(attribute);
		
		if(attribute->parent() != this){
			attribute->parent()->removeAttribute(attribute);
			attribute->setParent(this);
		}
		
		addObject(attribute);
		
		if(_addOutputAttributeCallback && !isDeleted()){
			_addOutputAttributeCallback(this, attribute);
		}
	}
}

unsigned int Node::slices(){
	return _slices;
}

void Node::resizedSlices(unsigned int slices){
}

void Node::doUpdate(Attribute *attribute){
	boost::posix_time::ptime startTime = boost::posix_time::microsec_clock::universal_time();
	
	update(attribute);
	
	boost::posix_time::ptime endTime = boost::posix_time::microsec_clock::universal_time();
	boost::posix_time::time_period enlapsed(startTime, endTime);
	
	_computeTimeSeconds = enlapsed.length().total_seconds();
	_computeTimeMilliseconds = enlapsed.length().total_milliseconds() % 1000;
	_computeTimeTicks = enlapsed.length().ticks();
}

void Node::updateSlice(Attribute *attribute, unsigned int slice){
}

void Node::update(Attribute *attribute){
	if(_slicer){ // this node is nested in a slicer node such as the ForLoop node and this node is supposed to be sliced
		// here we resize the slices for the output attributes so that the node can put values in each slice.

		unsigned int slices = _slicer->computeSlices();
		if(_inputAttributes.size()){
			for(int i = 0; i < _outputAttributes.size(); ++i){
				_outputAttributes[i]->outValue()->resizeSlices(slices);
			}
			
			_slices = slices;
			resizedSlices(slices);
		}

		#ifdef CORAL_PARALLEL_TBB
			tbb::parallel_for(tbb::blocked_range<size_t>(0, _slices), node_parallelUpdate(this, attribute));
		#else
			for(int i = 0; i < _slices; ++i){
				updateSlice(attribute, i);
			}
		#endif
	}
	else{
		updateSlice(attribute, 0);
	}
}

Node *Node::parent(){
	return (Node*)parentObject();
}

void Node::setParent(Node *parent){
	// TODO : remove from old parent (if any), or make it private
	setParentObject(parent);

	_slicer = findParentSlicer();
	_slices = 1;
}

void Node::addNode(Node *node){
	if(containerUtils::elementInContainer(node, _nodes) == false){
		_nodes.push_back(node);
		
		addObject(node);
		
		if(node->parent() != this){
			node->setParent(this);
		}
		
		if(_addNodeCallback && !isDeleted()){
			_addNodeCallback(this, node);
		}
	}
}

Node *Node::nodeAt(int position){
	Node *node = 0;
	if(position >= 0 && position < (int)_nodes.size()){
		node = _nodes[position];
	}
	
	return node;
}

Attribute *Node::inputAttributeAt(int position){
	Attribute *attribute = 0;
	if(position >= 0 && position < (int)_inputAttributes.size()){
		attribute = _inputAttributes[position];
	}
	
	return attribute;
}

Attribute *Node::outputAttributeAt(int position){
	Attribute *attribute = 0;
	if(position >= 0 && position < (int)_outputAttributes.size()){
		attribute = _outputAttributes[position];
	}
	
	return attribute;
}

std::vector <Node*> Node::nodes(){
	return _nodes;
}

const std::vector<Attribute*> &Node::inputAttributes(){
	return _inputAttributes;
}

const std::vector<Attribute*> &Node::outputAttributes(){
	return _outputAttributes;
}

Node *Node::findNode(const std::string &name){
	Node *foundNode = 0;
	for(std::vector<Node*>::iterator it = _nodes.begin(); it != _nodes.end(); ++it){
		Node *node = *it;
		if(node->name() == name){
			foundNode = node;
			break;
		}
	}
	
	return foundNode;
}

Attribute *Node::findAttribute(const std::string &name){
	Attribute *foundAttribute = 0;
	
	for(std::vector<Attribute*>::iterator it = _inputAttributes.begin(); it != _inputAttributes.end(); ++it){
		Attribute *attribute = *it;
		if(attribute->name() == name){
			foundAttribute = attribute;
			break;
		}
	}
	
	if(foundAttribute == 0){
		for(std::vector<Attribute*>::iterator it = _outputAttributes.begin(); it != _outputAttributes.end(); ++it){
			Attribute *attribute = *it;
			if(attribute->name() == name){
				foundAttribute = attribute;
				break;
			}
		}
	}
	
	return foundAttribute;
}

void Node::deleteIt(){
	if(_deleteItCallback && !isDeleted()){
		_deleteItCallback(this);
	}

	if(parent()){
		if(parent()->containsNode(this)){
			parent()->removeNode(this);
		}
	}
	
	std::vector<Node*> nodesCopy = _nodes;
	for(std::vector<Node*>::iterator itNode = nodesCopy.begin(); itNode != nodesCopy.end(); ++itNode){
		removeNode(*itNode);
	}
	
	std::vector<Attribute*> childrenAttributes = attributes();
	for(std::vector<Attribute*>::iterator itAttribute = childrenAttributes.begin(); itAttribute != childrenAttributes.end(); ++itAttribute){
		(*itAttribute)->setParent(0); // stop attributes from calling the parent node's update methods
	}
	
	for(std::vector<Attribute*>::iterator itAttribute = childrenAttributes.begin(); itAttribute != childrenAttributes.end(); ++itAttribute){
		removeAttribute(*itAttribute);
	}
	
	setIsDeleted(true);
}

void Node::setIsInvalid(bool value, const std::string &message){
	_isInvalid = value;
	_invalidityMessage = message;
}

bool Node::isInvalid(){
	return _isInvalid;
}

bool Node::isValid(){
	return !_isInvalid;
}

std::string Node::invalidityMessage(){
	return _invalidityMessage;
}

std::vector<Attribute*> Node::attributes(){
	std::vector<Attribute*> collectedAttributes;
	
	collectedAttributes.insert(collectedAttributes.end(), _inputAttributes.begin(), _inputAttributes.end());
	collectedAttributes.insert(collectedAttributes.end(), _outputAttributes.begin(), _outputAttributes.end());
	
	return collectedAttributes;
}

void Node::setAttributeAffect(Attribute *source, Attribute *destination){
	if(containerUtils::elementInContainer((NestedObject*)source, _objects) && containerUtils::elementInContainer((NestedObject*)destination, _objects)){
		if(destination->isOutput() && destination->isAffectedBy(source) == false)
			if(NetworkManager::isCycle(source, destination) == false)
				source->addAffect(destination);
	}
}


void Node::setAttributeIsClean(Attribute *attribute, bool value){
	if(containerUtils::elementInContainer((NestedObject*)attribute, _objects)){
		attribute->setIsClean(value);
	}
}

void Node::addDynamicAttribute(Attribute *attribute){
	if(_allowDynamicAttributes){
		if(containerUtils::elementInContainer(attribute, attributes()) && containerUtils::elementInContainer(attribute, _dynamicAttributes) == false){
	        _dynamicAttributes.push_back(attribute);
		}
	}
}

void Node::removeDynamicAttribute(Attribute *attribute){
	if(containerUtils::elementInContainer(attribute, _dynamicAttributes)){
		if(_removeAttributeCallback && !isDeleted()){
			_removeAttributeCallback(this, attribute);
		}
		
		containerUtils::eraseElementInContainer(attribute, _dynamicAttributes);
	}
}

std::string Node::attrsVectorToStr(const std::vector<Attribute*> &vec){
	std::string str = "[";
	
	int vecSize = vec.size();
	for(int i = 0; i < vecSize; ++i){
		str += "'" + vec[i]->fullName() + "'";
		
		if(i < vecSize - 1){
			str += ",";
		}
	}
	str += "]";
	
	return str;
}

std::string Node::asScript(){
	std::string script;
	 
	Command createNodeCmd;
	createNodeCmd.setName("CreateNode");
	createNodeCmd.setArgString("className", className());
	createNodeCmd.setArgString("name", name());
	createNodeCmd.setArgString("specializationPreset", _specializationPreset);
	
	std::string parentNode;
	if(parent()){
		parentNode = parent()->fullName();
	}
	
	createNodeCmd.setArgString("parentNode", parentNode);
	
	script += createNodeCmd.asScript() + "\n";
	
	if(_allowDynamicAttributes){
		for(int i = 0; i < _dynamicAttributes.size(); ++i){
			Attribute *attr = _dynamicAttributes[i];
		
			Command createAttributeCmd;
			createAttributeCmd.setName("CreateAttribute");
			createAttributeCmd.setArgString("name", attr->name());
			createAttributeCmd.setArgString("className", attr->className());
			createAttributeCmd.setArgString("parentNode", fullName());
		
			if(attr->isInput()){
				createAttributeCmd.setArgBool("input", true);
			}
			else if(attr->isOutput()){
				createAttributeCmd.setArgBool("output", true);
			}
			
			std::string spec = attr->specializationOverride();
			if(!spec.empty()){
				createAttributeCmd.setArgString("specializationOverride", spec);
			}
			
			script += createAttributeCmd.asScript() + "\n";
		}
		
		for(int i = 0; i < _dynamicAttributes.size(); ++i){
			Attribute* attr = _dynamicAttributes[i];
			std::vector<Attribute*> affect = attr->affecting();
			std::vector<Attribute*> affectedBy = attr->affectedBy();
			std::vector<std::string> allowedSpecialization = attr->allowedSpecialization();
			std::vector<Attribute*> specLinkedTo = attr->specializationLinkedTo();
			std::vector<Attribute*> specLinkedBy = attr->specializationLinkedBy();
			
			if(affect.size() || affectedBy.size() || allowedSpecialization.size() || specLinkedTo.size() || specLinkedBy.size()){
				Command setupDynamicAttributeCmd;
				setupDynamicAttributeCmd.setName("SetupDynamicAttribute");
				
				setupDynamicAttributeCmd.setArgString("attribute", attr->fullName());
				setupDynamicAttributeCmd.setArgUndefined("affect", attrsVectorToStr(affect));
				setupDynamicAttributeCmd.setArgUndefined("affectedBy", attrsVectorToStr(affectedBy));
				setupDynamicAttributeCmd.setArgUndefined("allowedSpecialization", stringUtils::vectorToString<std::string>(allowedSpecialization));
				setupDynamicAttributeCmd.setArgUndefined("specializationLinkedTo", attrsVectorToStr(specLinkedTo));
				setupDynamicAttributeCmd.setArgUndefined("specializationLinkedBy", attrsVectorToStr(specLinkedBy));
				
				script += setupDynamicAttributeCmd.asScript() + "\n";
			}
		}
	}
	
	std::vector<Attribute*> attrs = attributes();
	for(int i = 0; i < attrs.size(); ++i){
		script += attrs[i]->asScript();
	}
	
	return script;
}

std::string Node::saveNodeConnectionsScript(Node *node){
	std::string script;
	
	Command connectCmd;
	connectCmd.setName("ConnectAttributes");
	
	std::vector<Attribute*> attributes = node->attributes();
	for(int i = 0; i < attributes.size(); ++i){
		Attribute *attribute = attributes[i];
		
		Attribute *input = attribute->input();
		if(input){
			connectCmd.setArgString("sourceAttribute", input->fullName());
			connectCmd.setArgString("destinationAttribute", attribute->fullName());
			script += connectCmd.asScript() += "\n";
		}
		
		std::vector<Attribute*> outputs = attribute->outputs();
		for(int i = 0; i < outputs.size(); ++i){
			if(outputs[i]->isPassThrough()){
				connectCmd.setArgString("sourceAttribute", attribute->fullName());
				connectCmd.setArgString("destinationAttribute", outputs[i]->fullName());
				script += connectCmd.asScript() += "\n";
			}
		}
	}
	
	return script;
}

std::string Node::saveContentRecursive(bool thisIsRoot){
	std::string script;
	
	if(!thisIsRoot){
		script += asScript() + "\n";
	}
	
	for(int i = 0; i < _nodes.size(); ++i){
		script += _nodes[i]->saveContentRecursive(false);
	}
	
	for(int i = 0; i < _nodes.size(); ++i){
		script += saveNodeConnectionsScript(_nodes[i]);
	}
	
	return script;
}

std::string Node::contentAsScript(){
	bool thisIsRoot = true;
	return saveContentRecursive(thisIsRoot);
}

void Node::setName(const std::string &name){
	NestedObject::setName(name);
}

void Node::_attributeConnectionChanged(Attribute *attribute){
	attributeConnectionChanged(attribute);
	
	if(_connectionChangedCallback && !isDeleted()){
		_connectionChangedCallback(this, attribute);
	}
}

void Node::attributeConnectionChanged(Attribute *attribute){	
}

void Node::attributeSpecializationChanged(Attribute *attribute){
}

void Node::addAttributeSpecializationLink(Attribute *attributeA, Attribute *attributeB){
	attributeA->linkSpecializationTo(attributeB);
}

void Node::updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
	if(specializationContained(specializationA, specializationB)){
		specializationB = specializationA;
	}
	else if(specializationContained(specializationB, specializationA)){
		specializationA = specializationB;
	}
}

void Node::setAttributeAllowedSpecializations(Attribute *attribute, const std::vector<std::string> &specializations){
	attribute->setAllowedSpecialization(specializations);
}

void Node::setAttributeAllowedSpecialization(Attribute *attribute, const std::string &specialization){
	std::vector<std::string> specializations;
	specializations.push_back(specialization);
	attribute->setAllowedSpecialization(specializations);
}

std::string Node::shortDebugInfo(){
	std::string info;

	info += "node: " + fullName() + "\n";
	info += "className: " + className() + "\n";
	
	if(_sliceable){
		info += "slices: " + stringUtils::intToString(_slices);
	}
	else{
		info += "sliceable: False (this node can't be nested in a loop node)";
	}

	return info;
}

std::string Node::debugInfo(){
	std::string info = shortDebugInfo() + "\n";
	
	info += "last update took: secs:" + stringUtils::intToString(_computeTimeSeconds) + ", millisecs: " + stringUtils::intToString(_computeTimeMilliseconds) + "\n";
	info += "attributes:\n";
	
	for(int i = 0; i < _inputAttributes.size(); ++i){
		info += _inputAttributes[i]->debugInfo() + "\n";
	}
	
	for(int i = 0; i < _outputAttributes.size(); ++i){
		info += _outputAttributes[i]->debugInfo() + "\n";
	}
	
	return info;
}

int Node::computeTimeTicks(){
	return _computeTimeTicks;
}

int Node::computeTimeMilliseconds(){
	return _computeTimeMilliseconds;
}

int Node::computeTimeSeconds(){
	return _computeTimeSeconds;
}

const std::vector<Attribute*> &Node::dynamicAttributes(){
	return _dynamicAttributes;
}

void Node::updateAttributeSpecialization(Attribute *attribute){
	attribute->forceSpecializationUpdate();
}

void Node::setSpecializationPreset(const std::string &presetName, Attribute *attribute, const std::string &specialization){
	_specializationPresets[presetName][attribute->id()] = specialization;
}

std::string Node::enabledSpecializationPreset(){
	return _specializationPreset;
}

void Node::enableSpecializationPreset(const std::string &preset){
	if(preset != _specializationPreset && _specializationPresets.find(preset) != _specializationPresets.end() || preset == "none"){
		_specializationPreset = preset;
		std::vector<Attribute*> attrs = attributes();
		
		if(preset == "none"){
			for(int i = 0; i < attrs.size(); ++i){
				attrs[i]->removeSpecializationOverride();
			}
		}
		else{
			std::map<int, std::string> &attrsPreset = _specializationPresets[preset];
			for(int i = 0; i < attrs.size(); ++i){
				Attribute *attr = attrs[i];
				int attrId = attr->id();
				if(attrsPreset.find(attrId) != attrsPreset.end()){
					std::string specialization = attrsPreset[attrId];
					attr->setSpecializationOverride(specialization);
				}
			}
		}
		
		if(_constructorDone){
			for(int i = 0; i < attrs.size(); ++i){
				attrs[i]->forceSpecializationUpdate();
			}
		}
	}
}

std::vector<std::string> Node::specializationPresets(){
	std::vector<std::string> presets;
	
	std::map<std::string, std::map<int, std::string> >::iterator it = _specializationPresets.begin();
	for(; it != _specializationPresets.end(); ++it){
		std::string preset = it->first;
		
		presets.push_back(preset);
	}
	
	presets.push_back("none");
	
	return presets;
}

std::string Node::attributeSpecializationPreset(const std::string &preset, Attribute *attribute){
	std::string specializationPreset = "";

	if(_specializationPresets.find(preset) != _specializationPresets.end()){
		std::map<int, std::string> &attrsPreset = _specializationPresets[preset];
		int attrId = attribute->id();
		if(attrsPreset.find(attrId) != attrsPreset.end()){
			specializationPreset = attrsPreset[attrId];
		}
	}

	return specializationPreset;
}

void Node::catchAttributeDirtied(Attribute *attribute, bool value){
	if(containerUtils::elementInContainer(attribute, attributes())){
		attribute->setNotifyParentNodeOnDirty(value);
	}
}

void Node::attributeDirtied(Attribute *attribute){
}

void Node::setIsSlicer(bool value){
	_isSlicer = value;
}

unsigned int Node::computeSlices(){
	return 1;
}

Node *Node::slicer(){
	return _slicer;
}

void Node::setSliceable(bool value){
	_sliceable = value;
}

bool Node::sliceable(){
	return _sliceable;
}
