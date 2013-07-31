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

#ifndef CORAL_COREPARALLELALGOS_H
#define CORAL_COREPARALLELALGOS_H

#ifdef CORAL_PARALLEL_TBB

#include <tbb/blocked_range.h>
#include <vector>
#include "Attribute.h"
#include "Node.h"

namespace coral{
	
class attribute_parallelClean{
public:
	attribute_parallelClean(std::vector<Attribute*> *cleanChain): _cleanChain(cleanChain){ 
	}
	
	void operator() (const tbb::blocked_range<size_t> &r) const{
		for(size_t i = r.begin(); i != r.end(); ++i){
			_cleanChain->at(i)->cleanSelf();
		}
	}

private:
	std::vector<Attribute*> *_cleanChain;
};

class node_parallelUpdate{
public:
	node_parallelUpdate(Node *node, Attribute* attribute): _node(node), _attribute(attribute){ 
	}
	
	void operator() (const tbb::blocked_range<size_t> &r) const{
		for(size_t i = r.begin(); i != r.end(); ++i){
			_node->updateSlice(_attribute, i);
		}
	}

private:
	Node *_node;
	Attribute *_attribute;
};

}

#endif // tbb

#endif
