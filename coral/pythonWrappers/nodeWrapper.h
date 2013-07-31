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


// TODO: split implementation code into proper .cpp or we'll suffer from the sources not recompiling correctly

#ifndef NODEWRAPPER_H
#define NODEWRAPPER_H

#include <boost/python.hpp>
#include <boost/weak_ptr.hpp>

#include "../src/Node.h"
#include "../src/NodeAccessor.h"
#include "../src/ObjectAccessor.h"
#include "../src/Attribute.h"
#include "../src/NetworkManager.h"
#include "../src/ErrorObject.h"
#include "../src/PythonDataCollector.h"
#include "../src/pythonWrapperUtils.h"

using namespace coral;

class NodeWrapper: public Node, public boost::python::wrapper<Node>{
public:
	NodeWrapper(const std::string &name, Node *parent): Node(name, parent){
	}
	
	void setName(const std::string &name){
		// TODO: findPyObject shouldn't be needed to get a pointer to self, investigate...
		boost::python::object self = PythonDataCollector::findPyObject(id());
		
		boost::python::call_method<void>(self.ptr(), "setName", name);
	}
	
	void setName_default(const std::string &name){
		Node::setName(name);
	}
	
	void update(Attribute *attribute){
		bool releaseGIL = false;
		PyGILState_STATE state;
		if(!pythonWrapperUtils::pyGILEnsured){
			releaseGIL = true;
			state = PyGILState_Ensure();
		}

		boost::python::object attr = PythonDataCollector::findPyObject(attribute->id());
		boost::python::object self = PythonDataCollector::findPyObject(id());
		
		try{
			boost::python::call_method<void>(self.ptr(), "update", attr);
		}
		catch(...){
			PyErr_Print();
		}

		if(releaseGIL){
			PyGILState_Release(state);
			pythonWrapperUtils::pyGILEnsured = false;
		}
	}
	
	void update_default(Attribute *attribute){
		Node::update(attribute);
	}
	
	void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
		boost::python::object attrA = PythonDataCollector::findPyObject(attributeA->id());
		boost::python::object attrB = PythonDataCollector::findPyObject(attributeB->id());
		boost::python::object self = PythonDataCollector::findPyObject(id());
		
		boost::python::list specA;
		for(int i = 0; i < specializationA.size(); ++i){
			specA.append(specializationA[i]);
		}
		boost::python::incref(specA.ptr());
		
		boost::python::list specB;
		for(int i = 0; i < specializationB.size(); ++i){
			specB.append(specializationB[i]);
		}
		boost::python::incref(specB.ptr());
		
		try{
			boost::python::call_method<void>(self.ptr(), "updateSpecializationLink", attrA, attrB, specA, specB);
		}
		catch(...){
			PyErr_Print();
		}
		
		specializationA.clear();
		for(int i = 0; i < boost::python::len(specA); ++i){
			specializationA.push_back(boost::python::extract<std::string>(specA[i]));
		}
		
		specializationB.clear();
		for(int i = 0; i < boost::python::len(specB); ++i){
			specializationB.push_back(boost::python::extract<std::string>(specB[i]));
		}
	}
	
	void updateSpecializationLink_default(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB){
		Node::updateSpecializationLink(attributeA, attributeB, specializationA, specializationB);
	}
	
	std::string asScript(){
		boost::python::object self = PythonDataCollector::findPyObject(id());
		
		return boost::python::call_method<std::string>(self.ptr(), "asScript");
	}
	
	std::string asScript_default(){
		return Node::asScript();
	}
	
	void deleteIt(){
		boost::python::object self = PythonDataCollector::findPyObject(id());

		try{
			boost::python::call_method<void>(self.ptr(), "deleteIt");
		}
		catch(...){
			PyErr_Print();
		}
	}
	
	void deleteIt_default(){
		Node::deleteIt();
	}

	void attributeDirtied(Attribute *attribute){
		boost::python::object self = PythonDataCollector::findPyObject(id());
		boost::python::object attr = PythonDataCollector::findPyObject(attribute->id());

		try{
			boost::python::call_method<void>(self.ptr(), "attributeDirtied", attr);
		}
		catch(...){
			PyErr_Print();
		}

	}

	void attributeDirtied_default(Attribute *attribute){
		Node::attributeDirtied(attribute);
	}

	void addDynamicAttribute(Attribute *attribute){
		boost::python::object self = PythonDataCollector::findPyObject(id());
		boost::python::object attr = PythonDataCollector::findPyObject(attribute->id());

		try{
			boost::python::call_method<void>(self.ptr(), "addDynamicAttribute", attr);
		}
		catch(...){
			PyErr_Print();
		}
	}

	void addDynamicAttribute_default(Attribute *attribute){
		Node::addDynamicAttribute(attribute);
	}

	std::string shortDebugInfo(){
		boost::python::object self = PythonDataCollector::findPyObject(id());
		
		return boost::python::call_method<std::string>(self.ptr(), "shortDebugInfo");
	}
	
	std::string shortDebugInfo_default(){
		return Node::shortDebugInfo();
	}
};

boost::python::object node_parent(Node &self){
	boost::python::object node;
	
	Node *parent = self.parent();
	if(parent){
		node = PythonDataCollector::findPyObject(parent->id());
	}
	
	return node;
}

boost::python::object node_slicer(Node &self){
	boost::python::object node;

	Node *slicer = self.slicer();
	if(slicer){
		node = PythonDataCollector::findPyObject(slicer->id());
	}

	return node;
}

boost::python::object node_nodeAt(Node &self, int position){
	boost::python::object nodeObj;
	
	Node *node = self.nodeAt(position);
	if(node){
		nodeObj = PythonDataCollector::findPyObject(node->id());
	}
	
	return nodeObj;
}

boost::python::object node_inputAttributeAt(Node &self, int position){
	boost::python::object attrObj;
	
	Attribute *attribute = self.inputAttributeAt(position);
	if(attribute){
		attrObj = PythonDataCollector::findPyObject(attribute->id());
	}
	
	return attrObj;
}

boost::python::object node_outputAttributeAt(Node &self, int position){
	boost::python::object attrObj;
	
	Attribute *attribute = self.outputAttributeAt(position);
	if(attribute){
		attrObj = PythonDataCollector::findPyObject(attribute->id());
	}
	
	return attrObj;
}

void node_addNodeCallback(Node *self, Node *node){
	if(PythonDataCollector::hasCallback("node_addNode")){
		if(PythonDataCollector::hasPyObject(self->id() && node->id()))
			PythonDataCollector::findCallback("node_addNode")(PythonDataCollector::findPyObject(self->id()), PythonDataCollector::findPyObject(node->id()));
	}
}

void node_removeNodeCallback(Node *self, Node *node){
	if(PythonDataCollector::hasCallback("node_removeNode")){
		if(PythonDataCollector::hasPyObject(self->id()) && PythonDataCollector::hasPyObject(node->id()))
			PythonDataCollector::findCallback("node_removeNode")(PythonDataCollector::findPyObject(self->id()), PythonDataCollector::findPyObject(node->id()));
	}
}

void node_addInputAttributeCallback(Node *self, Attribute *attribute){
	if(PythonDataCollector::hasCallback("node_addInputAttribute")){
		if(PythonDataCollector::hasPyObject(self->id()) && PythonDataCollector::hasPyObject(attribute->id()))
			PythonDataCollector::findCallback("node_addInputAttribute")(PythonDataCollector::findPyObject(self->id()), PythonDataCollector::findPyObject(attribute->id()));
	}
}

void node_addOutputAttributeCallback(Node *self, Attribute *attribute){
	if(PythonDataCollector::hasCallback("node_addOutputAttribute")){
		if(PythonDataCollector::hasPyObject(self->id()) && PythonDataCollector::hasPyObject(attribute->id()))
			PythonDataCollector::findCallback("node_addOutputAttribute")(PythonDataCollector::findPyObject(self->id()), PythonDataCollector::findPyObject(attribute->id()));
	}
}

void node_connectionChangedCallback(Node *self, Attribute *attribute){
	if(PythonDataCollector::hasCallback("node_connectionChanged")){
		if(PythonDataCollector::hasPyObject(self->id()) && PythonDataCollector::hasPyObject(attribute->id()))
			PythonDataCollector::findCallback("node_connectionChanged")(PythonDataCollector::findPyObject(self->id()), PythonDataCollector::findPyObject(attribute->id()));
	}
}

boost::python::object node_findNode(Node &self, const std::string &name){
	Node *node = self.findNode(name);
	
	boost::python::object nodeObj;
	if(node){
		nodeObj = PythonDataCollector::findPyObject(node->id());
	}
	
	return nodeObj;
}

boost::python::object node_findAttribute(Node &self, const std::string &name){
	Attribute *attribute = self.findAttribute(name);
	
	boost::python::object attributeObj;
	if(attribute){
		attributeObj = PythonDataCollector::findPyObject(attribute->id());
	}
	
	return attributeObj;
}

void node__del__(Node &self){
	if(!self.isDeleted()){
		ObjectAccessor::_setIsDeleted(self, true);
		self.deleteIt();
	}
}

void node_deleteItCallback(Node *self){
	if(PythonDataCollector::hasCallback("node_deleteIt")){
		if(PythonDataCollector::hasPyObject(self->id()))
			PythonDataCollector::findCallback("node_deleteIt")(PythonDataCollector::findPyObject(self->id()));
	}
}

void node_removeAttributeCallback(Node *self, Attribute *attribute){
	if(PythonDataCollector::hasCallback("node_removeAttribute")){
		if(PythonDataCollector::hasPyObject(self->id()) && PythonDataCollector::hasPyObject(attribute->id()))
			PythonDataCollector::findCallback("node_removeAttribute")(PythonDataCollector::findPyObject(self->id()), PythonDataCollector::findPyObject(attribute->id()));
	}
}

void node_setAttributeAffect(Node &self, Attribute *source, Attribute *destination){
	NodeAccessor::_setAttributeAffect(self, source, destination);
}

void node_setAttributeIsClean(Node &self, Attribute *attribute, bool value){
	NodeAccessor::_setAttributeIsClean(self, attribute, value);
}

void node_setIsInvalid(Node &self, bool value, const std::string &message){
	NodeAccessor::_setIsInvalid(self, value, message);
}

void node_setAttributeAllowedSpecializations(Node &self, Attribute *attribute, boost::python::list stringList){
	std::vector<std::string> specialization;
	for(int i = 0; i < boost::python::len(stringList); ++i){
		std::string spec = boost::python::extract<std::string>(stringList[i]);
		if(!spec.empty()){
			specialization.push_back(spec);
		}
	}
	
	if(specialization.size()){
		NodeAccessor::_setAttributeAllowedSpecializations(self, attribute, specialization);
	}
}

void node_setAllowDynamicAttributes(Node &self, bool value){
	NodeAccessor::_setAllowDynamicAttributes(self, value);
}

void node_catchAttributeDirtied(Node &self, Attribute *attribute, bool value){
	NodeAccessor::_catchAttributeDirtied(self, attribute, value);
}

void node_setSliceable(Node &self, bool value){
	NodeAccessor::_setSliceable(self, value);
}

std::vector<Attribute*> node_dynamicAttributes(Node &self){
	return self.dynamicAttributes();
}

std::vector<Attribute*> node_inputAttributes(Node &self){
	return self.inputAttributes();
}

std::vector<Attribute*> node_outputAttributes(Node &self){
	return self.outputAttributes();
}

void node_updateAttributeSpecialization(Node &self, Attribute *attribute){
	NodeAccessor::_updateAttributeSpecialization(self, attribute);
}

void node_addAttributeSpecializationLink(Node &self, Attribute *attributeA, Attribute *attributeB){
	NodeAccessor::_addAttributeSpecializationLink(self, attributeA, attributeB);
}

void node_setUpdateEnabled(Node &self, bool value){
	NodeAccessor::_setUpdateEnabled(self, value);
}

void node_postInit(Node &self){
	NodeAccessor::_setConstructorDone(self, true);
	if(PythonDataCollector::hasCallback("node_created")){
		PythonDataCollector::findCallback("node_created")(PythonDataCollector::findPyObject(self.id()));
	}
}

void nodeWrapper(){
	boost::python::register_ptr_to_python<boost::shared_ptr<Node> >();
	
	boost::python::class_<NodeWrapper, boost::shared_ptr<NodeWrapper>, boost::python::bases<NestedObject>, boost::noncopyable>("Node", boost::python::no_init)
		.def("__init__", pythonWrapperUtils::__init__<NodeWrapper, const std::string, Node*>)
		.def("_postInit", node_postInit)
		.def("__del__", node__del__)
		.def("createUnwrapped", pythonWrapperUtils::createUnwrapped2<Node, const std::string, Node*>)
		.staticmethod("createUnwrapped")
		.def("addInputAttribute", &Node::addInputAttribute)
		.def("addOutputAttribute", &Node::addOutputAttribute)
		.def("addNode", &Node::addNode)
		.def("parent", node_parent)
		.def("setParent", &Node::setParent)
		.def("nodeAt", node_nodeAt)
		.def("inputAttributeAt", node_inputAttributeAt)
		.def("outputAttributeAt", node_outputAttributeAt)
		.def("removeNode", &Node::removeNode)
		.def("containsNode", &Node::containsNode)
		.def("update", &Node::update, &NodeWrapper::update_default)
		.def("nodes", &Node::nodes)
		.def("inputAttributes", node_inputAttributes)
		.def("outputAttributes", node_outputAttributes)
		.def("findNode", node_findNode)
		.def("findAttribute", node_findAttribute)
		.def("deleteIt", &Node::deleteIt, &NodeWrapper::deleteIt_default)
		.def("isInvalid", &Node::isInvalid)
		.def("invalidityMessage", &Node::invalidityMessage)
		.def("removeAttribute", &Node::removeAttribute)
		.def("attributes", &Node::attributes)
		.def("contentAsScript", &Node::contentAsScript)
		.def("asScript", &Node::asScript, &NodeWrapper::asScript_default)
		.def("_setAttributeAffect", node_setAttributeAffect)
		.def("_setAttributeIsClean", node_setAttributeIsClean)
		.def("_setIsInvalid", node_setIsInvalid)
		.def("_setAttributeAllowedSpecializations", node_setAttributeAllowedSpecializations)
		.def("setName", &Node::setName, &NodeWrapper::setName_default)
		.def("isValid", &Node::isValid)
		.def("updateEnabled", &Node::updateEnabled)
		.def("_setUpdateEnabled", node_setUpdateEnabled)
		.def("debugInfo", &Node::debugInfo)
		.def("computeTimeTicks", &Node::computeTimeTicks)
		.def("computeTimeMilliseconds", &Node::computeTimeMilliseconds)
		.def("computeTimeSeconds", &Node::computeTimeSeconds)
		.def("addDynamicAttribute", &Node::addDynamicAttribute, &NodeWrapper::addDynamicAttribute_default)
		.def("dynamicAttributes", node_dynamicAttributes)
		.def("_setAllowDynamicAttributes", node_setAllowDynamicAttributes)
		.def("allowDynamicAttributes", &Node::allowDynamicAttributes)
		.def("_updateAttributeSpecialization", node_updateAttributeSpecialization)
		.def("_addAttributeSpecializationLink", node_addAttributeSpecializationLink)
		.def("updateSpecializationLink", &Node::updateSpecializationLink, &NodeWrapper::updateSpecializationLink_default)
		.def("specializationPresets", &Node::specializationPresets)
		.def("enableSpecializationPreset", &Node::enableSpecializationPreset)
		.def("enabledSpecializationPreset", &Node::enabledSpecializationPreset)
		.def("_catchAttributeDirtied", node_catchAttributeDirtied)
		.def("attributeDirtied", &Node::attributeDirtied, &NodeWrapper::attributeDirtied_default)
		.def("attributeSpecializationPreset", &Node::attributeSpecializationPreset)
		.def("sliceable", &Node::sliceable)
		.def("_setSliceable", node_setSliceable)
		.def("slicer", &node_slicer)
		.def("shortDebugInfo", &Node::shortDebugInfo, &NodeWrapper::shortDebugInfo_default)
	;
	
	Node::_addNodeCallback = node_addNodeCallback;
	Node::_removeNodeCallback = node_removeNodeCallback;
	Node::_addInputAttributeCallback = node_addInputAttributeCallback;
	Node::_addOutputAttributeCallback = node_addOutputAttributeCallback;
	Node::_removeAttributeCallback = node_removeAttributeCallback;
	Node::_deleteItCallback = node_deleteItCallback;
	Node::_connectionChangedCallback = node_connectionChangedCallback;
}

#endif
