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

#include <sys/stat.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/subgraph.hpp>
#include <boost/graph/depth_first_search.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/topological_sort.hpp>

#include "NetworkManager.h"
#include "Node.h"
#include "Attribute.h"
#include "ErrorObject.h"
#include "containerUtils.h"
#include "stringUtils.h"

#ifdef WIN32
#define OS_SEP "\\"
#else
#define OS_SEP "/"
#endif

using namespace coral;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS> Graph;
typedef boost::graph_traits<Graph>::vertex_descriptor GraphVertex;

Graph _graph;

int NetworkManager::_nextAvailableId = 0;
std::map<int, Object *> NetworkManager::_objectsById;
std::vector<std::string> NetworkManager::_searchPaths;

namespace {
	int fileExist(const std::string &filename){
	  struct stat buffer;
	  return (stat (filename.data(), &buffer) == 0);
	}
}

class DownstreamVisitor : public boost::default_dfs_visitor{
public:
	void discover_vertex(GraphVertex v, const Graph& g){
		Attribute *attr = (Attribute*)NetworkManager::findObjectById(v);
		
		collectedAttributes->push_back(attr);
	}
	
	std::vector<Attribute*> *collectedAttributes;
};

class UpstreamVisitor : public boost::default_dfs_visitor{
public:
	void finish_vertex(GraphVertex v, const boost::reverse_graph<Graph>& g){
		Attribute *attr = (Attribute*)NetworkManager::findObjectById(v);
		collectedAttributes->push_back(attr);
	}
	
	std::vector<Attribute*> *collectedAttributes;
};

void NetworkManager::getDownstreamChain(Attribute *attribute, std::vector<Attribute*> &downstreamChain){
	downstreamChain.clear();
	
	int nvertices = boost::num_vertices(_graph);
	
	if(nvertices){
		DownstreamVisitor visitor;
		visitor.collectedAttributes = &downstreamChain;
		

		std::vector<boost::default_color_type> color_map(nvertices);
		boost::depth_first_visit(_graph, boost::vertex(attribute->id(), _graph), visitor, 
			boost::make_iterator_property_map(color_map.begin(), boost::get(boost::vertex_index, _graph), color_map[0]));
	}
}

void NetworkManager::getUpstreamChain(Attribute *attribute, std::vector<Attribute*> &upstreamChain){
	upstreamChain.clear();
	
	int nvertices = boost::num_vertices(_graph);
	
	if(nvertices){
		UpstreamVisitor visitor;
		visitor.collectedAttributes = &upstreamChain;
		
		std::vector<boost::default_color_type> color_map(nvertices);
		boost::reverse_graph<Graph> reverseGraph(_graph);
		boost::depth_first_visit(reverseGraph, boost::vertex(attribute->id(), _graph), visitor, 
			boost::make_iterator_property_map(color_map.begin(), boost::get(boost::vertex_index, _graph), color_map[0]));
	}
}

void NetworkManager::collectParentNodeConnectedInputs(Attribute *attribute, Node *parentNode, std::vector<Attribute*> &attributes){
	std::vector<Attribute*> chain;
	getUpstreamChain(attribute, chain);
	
	for(int i = 0; i < parentNode->_inputAttributes.size(); ++i){
		Attribute *attr = parentNode->_inputAttributes[i];
		if(std::find(chain.begin(), chain.end(), attr) != chain.end()){
			attributes.push_back(attr);
		}
	}
}

void NetworkManager::getCleanChain(Attribute *attribute, std::map<int, std::vector<Attribute*> > &cleanChain, std::map<int, std::vector<Attribute*> > &affectedInputs){
	Node *startContainerNode = attribute->parent();
	if(startContainerNode){
		startContainerNode = startContainerNode->parent();
	}

	std::vector<Attribute*> attributes;
	getUpstreamChain(attribute, attributes);
	
	cleanChain.clear();
	affectedInputs.clear();
	
	int nvertices = boost::num_vertices(_graph);
	std::vector<int> attrDependencyOrder(nvertices, 0);
	
	// sort the chain in parallel slices
	std::vector<Node*> processedNodes;
	int offset = 0;
	for(int i = 0; i < attributes.size(); ++i){
		Attribute *attr = attributes[i];
		int attrId = attr->id();
		
		if(in_degree(attrId, _graph) > 0){
			Graph::in_edge_iterator j, j_end;
			int maxdist = 0;
			
			for(boost::tie(j, j_end) = boost::in_edges(attrId, _graph); j != j_end; ++j){
				maxdist = std::max(attrDependencyOrder[source(*j, _graph)], maxdist);
			}
			
			attrDependencyOrder[attrId] = maxdist + 1;
		}
		
		if(attr->isOutput()){
			Node *parentNode = attr->parent();
			if(parentNode){
				Node *containerNode = parentNode->parent();

				if(containerUtils::elementInContainer(parentNode, processedNodes)){ // make sure the same node doesn't fall in the same slice more than once.
					offset++;
				}

				int dependencyOrder = attrDependencyOrder[attrId] + offset;
				std::vector<Attribute*> &slice = cleanChain[dependencyOrder];
				slice.push_back(attr);
				
				processedNodes.push_back(parentNode);
				
				std::vector<Attribute*> &inputs = affectedInputs[attr->id()];
				collectParentNodeConnectedInputs(attr, parentNode, inputs);
			}
		}
	}
}


void NetworkManager::addEdge(Attribute *attributeA, Attribute *attributeB){
	boost::add_edge(attributeA->id(), attributeB->id(), _graph);
}

void NetworkManager::removeEdge(Attribute *attributeA, Attribute *attributeB){
	boost::remove_edge(attributeA->id(), attributeB->id(), _graph);
}

int NetworkManager::useNextAvailableId(){
	_nextAvailableId += 1;
	
	return _nextAvailableId;
}

void NetworkManager::storeObject(int id, Object *object){
	_objectsById[id] = object;
}

void NetworkManager::removeObject(int id){
	std::map<int, Object *>::iterator it = _objectsById.find(id);
	if(it != _objectsById.end()){
		_objectsById.erase(it);
	}
}

int NetworkManager::objectCount(){
	return (int)_objectsById.size();
}

Object *NetworkManager::findObjectById(int id){
	Object *foundObject = 0;
	
	std::map<int, Object *>::iterator it = _objectsById.find(id);
	if(it != _objectsById.end()){
		foundObject = _objectsById[id];
	}
	
	return foundObject;
}

bool NetworkManager::isCycle(Attribute *attribute, Attribute *input){

	if(attribute && input){
		bool inputNeedsRecursion = false;
		bool affectedByNeedsRecursion = false;

		if(attribute->_isInput && attribute->_input){
			if(attribute->_input == input)
				return true;
			else
				inputNeedsRecursion = true;
		}
		// check attribute->affectedBy() too before starting an input recursion
		else if(attribute->_isOutput && attribute->_affectedBy.empty() == false){
			if(attribute->isAffectedBy(input))
				return true;
			else
				affectedByNeedsRecursion = true;
		}

		if(inputNeedsRecursion)
			if(isCycle(attribute->_input, input))
				return true;

		if(affectedByNeedsRecursion){
			for(unsigned int u = 0; u < attribute->_affectedBy.size(); ++u){
				if(isCycle(attribute->_affectedBy[u], input)){
					return true;
				}
			}
		}
	}

	return false;
}

bool NetworkManager::allowConnection(Attribute *sourceAttribute, Attribute *destinationAttribute, ErrorObject *errorObject){
	if(!sourceAttribute){
		errorObject->setMessage(sourceAttribute->fullName() + " is not a valid object");

		return false;
	}

	if(!destinationAttribute){
		errorObject->setMessage(destinationAttribute->fullName() + " is is not a valid object");

		return false;
	}

	if(sourceAttribute == destinationAttribute){
		errorObject->setMessage(sourceAttribute->fullName() + " can't be connected to itself");

		return false;
	}

	if(sourceAttribute->parent() == 0 || containerUtils::elementInContainer(sourceAttribute, sourceAttribute->parent()->attributes()) == false){
		errorObject->setMessage("no parent node found for " + sourceAttribute->fullName());

		return false;
	}
	
	if(destinationAttribute->parent() == 0 || containerUtils::elementInContainer(destinationAttribute, destinationAttribute->parent()->attributes()) == false){
		errorObject->setMessage("no parent node found for " + destinationAttribute->fullName());

		return false;
	}

	if(destinationAttribute->_input == sourceAttribute){
		std::string message(sourceAttribute->fullName() + " already connected to ");
		errorObject->setMessage(message + destinationAttribute->fullName());

		return false;
	}
	
	if(destinationAttribute->_isOutput == true && destinationAttribute->_passThrough == false){
		errorObject->setMessage(destinationAttribute->fullName() + " is output");

		return false;
	}

	if(isCycle(sourceAttribute, destinationAttribute)){	// connection/affect
		std::string message("cycle : p");
		errorObject->setMessage(message);

		return false;
	}
	
	if(!sourceAttribute->allowConnectionTo(destinationAttribute)){
		std::string message("types don't match : /");
		errorObject->setMessage(message);
		
		return false;
	}


	return true;
}

bool NetworkManager::connect(Attribute *sourceAttribute, Attribute *destinationAttribute, ErrorObject *errorObject){
	bool success = false;
		
	if(!errorObject){
		errorObject = new ErrorObject();
	}
	
	errorObject->addReference();
	
	if(allowConnection(sourceAttribute, destinationAttribute, errorObject)){
		success = sourceAttribute->connectTo(destinationAttribute, errorObject);
	}

	errorObject->removeReference();
	
	return success;
}

std::string NetworkManager::resolveFilename(const std::string &filename){
	std::string resolvedFilename;
	if(fileExist(filename)){
		resolvedFilename = filename;
	}
	else{
		std::string sep(OS_SEP);

		for(int i = 0; i < _searchPaths.size(); ++i){
			std::string newPath = _searchPaths[i] + sep + filename;
			stringUtils::replace(newPath, sep + sep, sep); //fix possible crap
		  	if(fileExist(newPath)){
		  		resolvedFilename = newPath;
		  		break;
		  	}
		}
	}

	return resolvedFilename;
}

void NetworkManager::addSearchPath(const std::string &path){
	if(!containerUtils::elementInContainer(path, _searchPaths)){
		_searchPaths.push_back(path);
	}
}

void NetworkManager::removeSearchPath(const std::string &path){
	containerUtils::eraseElementInContainer(path, _searchPaths);
}
