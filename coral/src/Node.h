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


#ifndef CORAL_NODE_H
#define CORAL_NODE_H

#include <vector>
#include <map>
#include <iostream>
#include "NestedObject.h"
#include "Value.h"

namespace coral{
class Attribute;
class NodeAccessor;
class SpecializationLink;
class node_parallelUpdate;


//! The base class to all nodes.
class CORAL_EXPORT Node: public NestedObject{
public:
	Node(const std::string &name, Node *parent);
	virtual ~Node();

	void addInputAttribute(Attribute *attribute);
	void addOutputAttribute(Attribute *attribute);
	void addNode(Node *node);
	Node *parent();
	void setParent(Node *parent);
	Node *nodeAt(int position);
	Attribute *inputAttributeAt(int position);
	Attribute *outputAttributeAt(int position);
	void removeNode(Node *node);
	void removeAttribute(Attribute *attribute);
	bool containsNode(Node *node);
	std::vector <Node*> nodes();
	const std::vector<Attribute*> &inputAttributes();
	const std::vector<Attribute*> &outputAttributes();
	Node *findNode(const std::string &name);
	Attribute *findAttribute(const std::string &name);
	bool isInvalid();
	bool isValid();
	std::string invalidityMessage();
	std::vector<Attribute*> attributes();
	bool updateEnabled();
	int computeTimeTicks();
	int computeTimeMilliseconds();
	int computeTimeSeconds();
	const std::vector<Attribute*> &dynamicAttributes();
	bool allowDynamicAttributes();
	void enableSpecializationPreset(const std::string &preset);
	std::string enabledSpecializationPreset();

	//! Indicates if this node can be nested under a slicer node such as a ForLoop node, this value is false by default.
	bool sliceable();

	//! The number of slices is 1 by default, this value can change before any update if this node is nested under a slicer node such as a ForLoop node. 
	unsigned int slices();

	//! Returns the parent node in charge of imposing the number of slices such as a ForLoop node, if there's no slicer this value is NULL.
	Node *slicer();

	//! Returns all the available presets for this node.
	std::vector<std::string> specializationPresets();

	//! Returns the specialization for attribute should this node be set on preset.
	std::string attributeSpecializationPreset(const std::string &preset, Attribute *attribute);	

	//! Returns a python script to recreate all the nodes contained within this node.
	//! This method will invoke the asScript() virtual method for each contained node, in order to recreate the content of this node.
	std::string contentAsScript();

	//! The default implementation of this method will return the python code needed to create this node,
	//! you should reimplement this if you need a more complex script to recreate this node.
	virtual std::string asScript();

	//! This method is invoked by a child attribute when it gets dirtied.
	//! To enable it for a particular attribute this node must call catchAttributeDirtied(childAttr, true) from its constructor.
	virtual void attributeDirtied(Attribute *attribute);
	
	virtual void setName(const std::string &name);
	virtual void deleteIt();
	virtual void update(Attribute *attribute);
	virtual void updateSpecializationLink(Attribute *attributeA, Attribute *attributeB, std::vector<std::string> &specializationA, std::vector<std::string> &specializationB);
	virtual void attributeConnectionChanged(Attribute *attribute);
	virtual void attributeSpecializationChanged(Attribute *attribute);
	virtual std::string debugInfo();
	virtual std::string shortDebugInfo();
	virtual void addDynamicAttribute(Attribute *attribute);
	virtual void removeDynamicAttribute(Attribute *attribute);
	virtual void updateSlice(Attribute *attribute, unsigned int slice);

	//! This method is invoked before updateSlice if there is a change in the number of slices imposed by the slicer.
	//! Overriding this method is often handy when a node has some internal data that needs to be sliced accordingly. 
	virtual void resizedSlices(unsigned int slices);

	//! This method should be reimplemented by slicer nodes such as the ForLoop node, 
	//! every node contained in a slicer node will ask its slicer node for the number of slices before to start computing.
	virtual unsigned int computeSlices();

	static void(*_addNodeCallback)(Node *self, Node *node);
	static void(*_removeNodeCallback)(Node *self, Node *node);
	static void(*_addInputAttributeCallback)(Node *self, Attribute *attribute);
	static void(*_addOutputAttributeCallback)(Node *self, Attribute *attribute);
	static void(*_removeAttributeCallback)(Node *self, Attribute *attribute);
	static void(*_deleteItCallback)(Node *self);
	static void(*_connectionChangedCallback)(Node *self, Attribute *attribute);

protected:
	void setAttributeAffect(Attribute *source, Attribute *destination);
	void setAttributeIsClean(Attribute *attribute, bool value);
	void setIsInvalid(bool value, const std::string &message);
	void addAttributeSpecializationLink(Attribute *attributeA, Attribute *attributeB);
	void setAttributeAllowedSpecializations(Attribute *attribute, const std::vector<std::string> &specializations);
	void setAttributeAllowedSpecialization(Attribute *attribute, const std::string &specialization);
	void setUpdateEnabled(bool value);
	void setAllowDynamicAttributes(bool value);
	void updateAttributeSpecialization(Attribute *attribute);
	void setSpecializationPreset(const std::string &presetName, Attribute *attribute, const std::string &specialization);
	void catchAttributeDirtied(Attribute *attribute, bool value = true);

	//! In order for this node to be nested under a slicer node, such as a ForLoop node, it has to be declared as sliceable(true).
	//! Once a node is declared as sliceable the updateSlice(attribute, slice) method needs to be overridden instead of the usual update(attribute) method.
	void setSliceable(bool value);
	
	//! Loop nodes such as the ForLoop node are marked as setIsSlicer(true) and they compute a slicing count for their children nodes by overriding the 
	//! virtual method unsigned int computeSlices().
	void setIsSlicer(bool value);

private:
	friend class NodeAccessor;
	friend class NetworkManager;
	friend class Attribute;
	friend class node_parallelUpdate;
	
	std::string saveContentRecursive(bool thisIsRoot);
	std::string saveNodeConnectionsScript(Node *node);
	void doUpdate(Attribute *attribute);
	std::string attrsVectorToStr(const std::vector<Attribute*> &vec);
	void _attributeConnectionChanged(Attribute *attribute);
	Node *findParentSlicer();

	std::vector<Attribute*> _outputAttributes;
	std::vector<Attribute*> _inputAttributes;
	std::vector<Attribute*> _dynamicAttributes;
	std::vector<Node*> _nodes;
	std::map<std::string, std::map<int, std::string> > _specializationPresets; // _specializationPresets["presetName"][attr->id()] = "Int"
	bool _isInvalid;
	bool _updateEnabled;
	std::string _invalidityMessage;
	int _computeTimeSeconds;
	int _computeTimeMilliseconds;
	int _computeTimeTicks;
	bool _allowDynamicAttributes;
	bool _constructorDone;
	std::string _specializationPreset;
	unsigned int _slices;
	bool _isSlicer;
	Node *_slicer;
	bool _sliceable;

	Node();
	Node(const Node &other);
	Node &operator =(const Node &other);
};
}
#endif
