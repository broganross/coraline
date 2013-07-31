
#ifndef CORAL_KERNELNODE_H
#define CORAL_KERNELNODE_H

#include "../src/Node.h"
#include "../src/NumericAttribute.h"
#include "../src/StringAttribute.h"
#include "../src/BoolAttribute.h"
#include "../src/EnumAttribute.h"

#define __CL_ENABLE_EXCEPTIONS

#include <stdlib.h>
#include <map>
#include <vector>
#include <CL/cl.hpp>

namespace coral{

class KernelNode: public Node{
public:
	KernelNode(const std::string &name, Node *parent);
	void addDynamicAttribute(Attribute *attribute);
	void removeDynamicAttribute(Attribute *attribute);
	void attributeDirtied(Attribute *attribute);
	void update(Attribute *attribute);
	void attributeSpecializationChanged(Attribute *attribute);
	void updateDeviceUsed();

	std::string buildInfo();
	void buildKernelSource();

private:
	void initCL();
	void cacheBufferReadWrite(Attribute *attribute);
	void writeBuffers(std::map<int, Numeric*> &values, std::map<int, cl::Buffer> &buffers, int size, const std::vector<Attribute*> &dynAttrs, cl::Event &event);
	cl_int executeKernel(cl::Kernel &kernel, int size, cl::Event &event);
	void readBuffers(std::map<int, Numeric*> &values, std::map<int, cl::Buffer> &buffers, int size, cl::Event &event);

	EnumAttribute *_device;
	StringAttribute *_kernelSource;
	BoolAttribute *_useSize;
	cl::Context _context;
	cl::CommandQueue _queue;
	std::vector<cl::Kernel> _kernels;
	std::vector<cl::Device> _devices;
	std::map<int, void(*)(Numeric *, cl::Buffer &, int, cl::Context &, cl::CommandQueue &, cl::Event &)> _writeBuffer;
	std::map<int, void(*)(Numeric *, cl::Buffer &, int, cl::CommandQueue &, cl::Event &)> _readBuffer;
	std::string _buildMessage;
};

}

#endif
