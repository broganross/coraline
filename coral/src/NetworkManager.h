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


#ifndef NODEMANAGER_H
#define NODEMANAGER_H

#include <string>
#include <vector>
#include <map>
#include "coralDefinitions.h"

namespace coral{

class Object;
class Attribute;
class Node;
class ErrorObject;

//! In charge of managing lifetime and connections of each Object in the network.
class CORAL_EXPORT NetworkManager{
public:
	static int objectCount();
	static Object *findObjectById(int id);
	static bool allowConnection(Attribute *sourceAttribute, Attribute *destinationAttribute, ErrorObject *errorObject);
	static bool connect(Attribute *sourceAttribute, Attribute *destinationAttribute, ErrorObject *errorObject = 0);
	static bool isCycle(Attribute *sourceAttribute, Attribute *destinationAttribute);
	static void getDownstreamChain(Attribute *attribute, std::vector<Attribute*> &downstreamChain);
	static void getUpstreamChain(Attribute *attribute, std::vector<Attribute*> &upstreamChain);
	static std::string resolveFilename(const std::string &filename);
	static void addSearchPath(const std::string &path);
	static void removeSearchPath(const std::string &path);

private:
	friend class Object;
	friend class Attribute;
	friend class Node;
	
	static int useNextAvailableId();
	static void storeObject(int id, Object *object);
	static void removeObject(int id);
	static void addEdge(Attribute *attributeA, Attribute *attributeB);
	static void removeEdge(Attribute *attributeA, Attribute *attributeB);
	static void getCleanChain(Attribute *attribute, std::map<int, std::vector<Attribute*> > &cleanChain, std::map<int, std::vector<Attribute*> > &affectedInputs);
	static void collectParentNodeConnectedInputs(Attribute *attribute, Node *parentNode, std::vector<Attribute*> &attributes);

	static int _nextAvailableId;
	static std::map<int, Object *> _objectsById;
	static std::vector<std::string> _searchPaths;
};

}
#endif
