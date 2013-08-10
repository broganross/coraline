
#ifndef POLYNODESWRAPPER_H
#define POLYNODESWRAPPER_H

#include "../src/Node.h"
#include "../src/PolyAttribute.h"
#include "../src/PolyValue.h"
#include "../builtinNodes/PolyNodes.h"

void polyNodesWrapper(){
	pythonWrapperUtils::pythonWrapper<ConstantArray, Node>("ConstantArray");
	pythonWrapperUtils::pythonWrapper<BuildArray, Node>("BuildArray")
		.def("addAttribute", &BuildArray::addAttribute);
	pythonWrapperUtils::pythonWrapper<GetArrayElement, Node>("GetArrayElement");
	pythonWrapperUtils::pythonWrapper<SetArrayElement, Node>("SetArrayElement");

}

#endif
