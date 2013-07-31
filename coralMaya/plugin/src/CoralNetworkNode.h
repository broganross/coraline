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

#ifndef CORALNETWORKNODE_H
#define CORALNETWORKNODE_H

#include <map>
#include <sstream>
#include <string>

#include <maya/MIOStream.h>
#include <maya/MPxNode.h> 
#include <maya/MFnNumericAttribute.h>
#include <maya/MString.h> 
#include <maya/MTypeId.h> 
#include <maya/MPlug.h>
#include <maya/MVector.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MGlobal.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MStringArray.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MPlugArray.h>
#include <maya/MGlobal.h>

#include <coral/src/Object.h>
#include <coral/src/Node.h>
#include <coral/src/Attribute.h>
#include <coral/src/NetworkManager.h>
#include <coral/src/containerUtils.h>

#include <CoralMayaAttribute.h>

/*
 This node contains a network of coral nodes, it uses this network to form user defined algorithms that can ultimately be used to manipulate 
 data in a maya scene.
 This node is associated with a MayaContextNode (via its id CoralNetworkNode::coralNodeId), each CoralMayaNode found under the MayaContextNode
 is associated to one of the input or output attributes of this node.
*/
class CoralNetworkNode: public MPxNode
{
public:
    CoralNetworkNode();
	virtual ~CoralNetworkNode(); 

	virtual MStatus compute(const MPlug& plug, MDataBlock& data);
	virtual MStatus setDependentsDirty(MPlug const &inPlug, MPlugArray &affectedPlugs);
	virtual MStatus shouldSave(const MPlug &plug, bool &isSaving);
	virtual void copyInternalData(MPxNode *node);
	
	static void* creator();
	static MStatus initialize();

	static MTypeId id; 
	static MObject coralNodeId;
	static MObject saveData;
	
	void updateCoralAttributeMap();
	
private:
	std::map<std::string, int> _coralAttributeMap;
	std::vector<MPlug> _inputPlugs;
	MPlugArray _outputPlugs;
	
	std::vector<coral::Attribute*> findCoralMayaAttributes(coral::Node *coralNode);
	void coralCanDirtyOutAttrs(bool value);
};

#endif
