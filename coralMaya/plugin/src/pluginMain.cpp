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

#include <maya/MFnPlugin.h>
#include <maya/MGlobal.h>
#include <maya/MObject.h>

#include "CoralNetworkNode.h"
#include "CoralMayaCmd.h"
#include "CoralDrawNode.h"

#ifdef CORAL_PARALLEL_TBB
	#include <tbb/task_scheduler_init.h>
#endif

namespace{
	#ifdef CORAL_PARALLEL_TBB
		tbb::task_scheduler_init _tbbinit;
	#endif
}

MStatus initializePlugin(MObject obj)
{
	MFnPlugin plugin(obj);
	
	plugin.registerCommand("coralMayaCmd", CoralMayaCmd::creator);
    plugin.registerNode("coralNetworkNode", CoralNetworkNode::id, CoralNetworkNode::creator, CoralNetworkNode::initialize);
	plugin.registerNode("coralDrawNode", CoralDrawNode::id, CoralDrawNode::creator, CoralDrawNode::initialize, MPxNode::kLocatorNode);

	// tbb init is only required for old tbb versions, maya still needs it.
	#ifdef CORAL_PARALLEL_TBB
		_tbbinit = tbb::task_scheduler_init::deferred;
		_tbbinit.initialize();
	#endif
   	
	return MS::kSuccess;
}

MStatus uninitializePlugin(MObject obj)
{
	MStatus status;
	MFnPlugin plugin(obj);
	
	plugin.deregisterCommand("coralMayaCmd");
    plugin.deregisterNode(CoralNetworkNode::id);
	plugin.deregisterNode(CoralDrawNode::id);

	#ifdef CORAL_PARALLEL_TBB
		_tbbinit.terminate();
	#endif
    
	return MS::kSuccess;
}
