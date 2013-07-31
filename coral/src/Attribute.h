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


#ifndef CORAL_ATTRIBUTE_H
#define CORAL_ATTRIBUTE_H

#include <vector>
#include <map>
#include <time.h>
#include <iostream>

#include "NestedObject.h"

namespace coral{
class Node;
class Value;
class AttributeAccessor;
class NetworkManager;
class SpecializationLink;
class ErrorObject;
class attribute_parallelClean;
class Attribute;

struct SpecializationLink{
	// Helper struct to hold two linked attributes together, not exposed to public API
	Attribute *attributeA;
	Attribute *attributeB;
};

//! The base class for customized attributes.
//
//! Internally it stores a pointer to a Value, 
//! the scope of this class is to expose any kind of data as interface of a node to then manipulate the data.
class CORAL_EXPORT Attribute: public NestedObject{
public:
	Attribute(const std::string &name, Node *parent);
	virtual ~Attribute();
	
	template<class T>
    friend std::ostream& operator<<(std::ostream& str, Attribute *attribute);
	
	Attribute *input();
	const std::vector<Attribute*> &outputs();
	bool isClean();
	bool isConnectedTo(Attribute *attribute);
	bool isAffectedBy(Attribute *attribute);
	const std::vector<Attribute*> &affectedBy();
	const std::vector<Attribute*> &affecting();
	Node* parent();
	bool isPassThrough();
	void disconnectInput();
	void disconnectOutput(Attribute *attribute);
	bool isInput();
	bool isOutput();
	bool allowConnectionTo(Attribute *attribute);
	void setSpecializationOverride(const std::string &specialization);
	std::string specializationOverride();
	void removeSpecializationOverride();
	void forceSpecializationUpdate();
	void forceDirty();

	/*! Returns the first connected attribute found that is not a passThrough.
		This method should be used whenever trying to access the internal value of this attribute wihtout knowing if it's a passThrough.*/
	Attribute *connectedNonPassThrough();

	/*! An array os strings that form the currently active specialization for this attribute.*/
	std::vector<std::string> specialization();
	
	/*! This array of strings represents all the available specializations for this attribute. 
		To know the currently active specialization use specialization() */
	std::vector<std::string> allowedSpecialization();
	
	std::vector<Attribute*> specializationLinkedTo();
	std::vector<Attribute*> specializationLinkedBy();
	
	/*! Call this method after you perform modifications to the internal value of this attribute.
		example: 
		attr->outValue()->setSomething(1);
		attr->valueChanged();*/
	void valueChanged();
	
	virtual std::string asScript();
	virtual void deleteIt();
	virtual void onDirtied();
	virtual void onSettingSpecialization(const std::vector<std::string> &specialization);
	virtual std::string shortDebugInfo();
	virtual std::string debugInfo();
	
	/*! This method returns a pointer to a clean Value, this Value will come from the first input connected in the chain, 
		if ther's no input connected then the Value of this attribute is returned.
		If you want to be sure to access this attribute's value (for example to modify the data of an output attribute) 
		then you must use outValue() instead.*/
	Value *value();
	
	/*! Return this attribute's value without cleaning it, use this method to access the Value for modifying its internal data.*/
	Value *outValue();
	
	// UI\external observers stuff
	static void(*_connectToCallback)(Attribute *self, Attribute *other);
	static void(*_disconnectInputCallback)(Attribute *self);
	static void(*_disconnectOutputCallback)(Attribute *self, Attribute *other);
	static void(*_deleteItCallback)(Attribute *self);
	static void(*_specializationCallBack)(Attribute *self);
	// static void(*_valueChangedCallback)(Attribute *self);
	static void queueDirtyingDoneCallback(void(*callback)(Attribute *));
	
protected:
	void setValuePtr(Value *value);
	void setPassThrough(bool value);
	void setIsOutput(bool value);
	void setIsInput(bool value);
	void setIsClean(bool value);
	void setAllowedSpecialization(const std::vector<std::string> &specialization);

private:
	friend class AttributeAccessor;
	friend class attribute_parallelClean;
	friend class Node;
	friend class NetworkManager;

	bool connectTo(Attribute *attribute, ErrorObject *errorObject);
	void addAffectedFrom(Attribute *attribute);
	void removeAffectFrom(Attribute *attribute);
	void setInput(Attribute *attribute);
	void dirty(bool force = false);
	void clean();
	void addAffect(Attribute *attribute);
	Attribute *connectedInputNonPassThrough(Attribute *attribute);
	void removeAffect(Attribute *attribute);
	void setParent(Node *parent);
	void resetInputValuesInChain();
	void removeSpecializationLink(SpecializationLink *specializationLink);
	bool updateBranchSpecializations(bool reset);
	void setSpecialization(const std::vector<std::string> &specialization);
	bool specializationContainedOne(const std::vector<std::string> &specialization1, const std::vector<std::string> &specialization2);
	void linkSpecializationTo(Attribute *attribute);
	void cacheEvaluationChain();
	void cacheDirtyChainUpstream();
	void cacheCleanChainDownstream();
	void cleanSelf();
	void processDirtyingDoneCallbackQueue();
	Attribute *findFirstOutputNotPassThrough();
	void initValueFromPassThroughFirstOutput(Attribute *attribute);
	void setNotifyParentNodeOnDirty(bool value);
	void collectSpecializationBranch(std::vector<std::pair<Attribute*, Attribute*> > &specializationPairs, std::vector<std::pair<Attribute*, Attribute*> > &specializationLinks, std::map<int, std::vector<std::string> > &specializationMap, bool reset);
	bool updateSpecialization(std::vector<std::pair<Attribute*, Attribute*> > &specializationPairs, std::vector<std::pair<Attribute*, Attribute*> > &specializationLinks, std::map<int, std::vector<std::string> > &specializationMap);

	Attribute *_input;
	std::vector<Attribute*> _outputs;
	std::vector<Attribute*> _affect;
	std::vector<Attribute*> _affectedBy;
	bool _isClean;
	bool _isOutput;
	bool _isInput;
	bool _passThrough;
	bool _valueObserved;
	bool _notifyParentNodeOnDirty;
	Value *_value;
	Value *_inputValue;
	std::vector<std::string> _allowedSpecialization;
	std::vector<std::string> _specialization;
	std::vector<std::string> _specializationOverride;
	std::vector<SpecializationLink*> _specializationLinks;
	int _computeTimeSeconds;
	int _computeTimeMilliseconds;
	std::vector<Attribute*> _dirtyChain;
	std::map<int, std::vector<Attribute*> > _cleanChain;
	std::map<int, std::vector<Attribute*> > _inputsCleanChain;
	
	Attribute();
	Attribute(const Attribute &other);
	Attribute &operator =(const Attribute &other);
};

template<class T>
std::ostream& operator<<(std::ostream& str, Attribute *attribute){
    str << attribute->fullName().data();
}

}
#endif
