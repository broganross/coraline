
#include "KernelNode.h"
#include "../src/stringUtils.h"

using namespace coral;

namespace {
	std::string oclErrorString(cl_int error)
	{
	    static const char* errorString[] = {
	        "CL_SUCCESS",
	        "CL_DEVICE_NOT_FOUND",
	        "CL_DEVICE_NOT_AVAILABLE",
	        "CL_COMPILER_NOT_AVAILABLE",
	        "CL_MEM_OBJECT_ALLOCATION_FAILURE",
	        "CL_OUT_OF_RESOURCES",
	        "CL_OUT_OF_HOST_MEMORY",
	        "CL_PROFILING_INFO_NOT_AVAILABLE",
	        "CL_MEM_COPY_OVERLAP",
	        "CL_IMAGE_FORMAT_MISMATCH",
	        "CL_IMAGE_FORMAT_NOT_SUPPORTED",
	        "CL_BUILD_PROGRAM_FAILURE",
	        "CL_MAP_FAILURE",
	        "",
	        "",
	        "",
	        "",
	        "",
	        "",
	        "",
	        "",
	        "",
	        "",
	        "",
	        "",
	        "",
	        "",
	        "",
	        "",
	        "",
	        "CL_INVALID_VALUE",
	        "CL_INVALID_DEVICE_TYPE",
	        "CL_INVALID_PLATFORM",
	        "CL_INVALID_DEVICE",
	        "CL_INVALID_CONTEXT",
	        "CL_INVALID_QUEUE_PROPERTIES",
	        "CL_INVALID_COMMAND_QUEUE",
	        "CL_INVALID_HOST_PTR",
	        "CL_INVALID_MEM_OBJECT",
	        "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR",
	        "CL_INVALID_IMAGE_SIZE",
	        "CL_INVALID_SAMPLER",
	        "CL_INVALID_BINARY",
	        "CL_INVALID_BUILD_OPTIONS",
	        "CL_INVALID_PROGRAM",
	        "CL_INVALID_PROGRAM_EXECUTABLE",
	        "CL_INVALID_KERNEL_NAME",
	        "CL_INVALID_KERNEL_DEFINITION",
	        "CL_INVALID_KERNEL",
	        "CL_INVALID_ARG_INDEX",
	        "CL_INVALID_ARG_VALUE",
	        "CL_INVALID_ARG_SIZE",
	        "CL_INVALID_KERNEL_ARGS",
	        "CL_INVALID_WORK_DIMENSION",
	        "CL_INVALID_WORK_GROUP_SIZE",
	        "CL_INVALID_WORK_ITEM_SIZE",
	        "CL_INVALID_GLOBAL_OFFSET",
	        "CL_INVALID_EVENT_WAIT_LIST",
	        "CL_INVALID_EVENT",
	        "CL_INVALID_OPERATION",
	        "CL_INVALID_GL_OBJECT",
	        "CL_INVALID_BUFFER_SIZE",
	        "CL_INVALID_MIP_LEVEL",
	        "CL_INVALID_GLOBAL_WORK_SIZE",
	    };

	    const int errorCount = sizeof(errorString) / sizeof(errorString[0]);

	    const int index = -error;

	    return std::string((index >= 0 && index < errorCount) ? errorString[index] : "");
	}

	int findMinorInputSize(const std::vector<Attribute*> &attrs, const std::vector<bool> &useSize, std::map<int, Numeric*> &values){
		int minorSize = INT_MAX;
		int size = 0;
		for(int i = 0; i < attrs.size(); ++i){
			Attribute *attr = attrs[i];
			if(attr->isInput() && useSize[i]){
				Numeric *num = values[attr->id()];
				if(num->isArray()){
					int currentSize = num->size();
					if(currentSize < minorSize){
						minorSize = currentSize;
						size = minorSize;
					}
				}
			}
		}

		return size;
	}

	typedef struct{
		float x;
		float y;
		float z;
	} CLVec3;

	void createInputIntBuffer(Numeric *value, cl::Buffer &buffer, int size, cl::Context &context, cl::CommandQueue &queue, cl::Event &event){
		size_t bufferSize = sizeof(int) * size;
		buffer = cl::Buffer(context, CL_MEM_READ_WRITE, bufferSize, NULL);
		queue.enqueueWriteBuffer(buffer, CL_TRUE, 0, bufferSize, &value->intValues()[0], NULL, &event);
	}

	void createInputFloatBuffer(Numeric *value, cl::Buffer &buffer, int size, cl::Context &context, cl::CommandQueue &queue, cl::Event &event){
		size_t bufferSize = sizeof(float) * size;
		buffer = cl::Buffer(context, CL_MEM_READ_WRITE, bufferSize, NULL);
		queue.enqueueWriteBuffer(buffer, CL_TRUE, 0, bufferSize, &value->floatValues()[0], NULL, &event);
	}

	void createInputVec3Buffer(Numeric *value, cl::Buffer &buffer, int size, cl::Context &context, cl::CommandQueue &queue, cl::Event &event){
		size_t bufferSize = sizeof(CLVec3) * size;
		buffer = cl::Buffer(context, CL_MEM_READ_WRITE, bufferSize, NULL);
		queue.enqueueWriteBuffer(buffer, CL_TRUE, 0, bufferSize, &value->vec3Values()[0], NULL, &event);
	}

	void createOutputIntBuffer(Numeric *value, cl::Buffer &buffer, int size, cl::Context &context, cl::CommandQueue &queue, cl::Event &event){
		size_t bufferSize = sizeof(int) * size;
		buffer = cl::Buffer(context, CL_MEM_READ_WRITE, bufferSize, NULL);
	}

	void createOutputFloatBuffer(Numeric *value, cl::Buffer &buffer, int size, cl::Context &context, cl::CommandQueue &queue, cl::Event &event){
		size_t bufferSize = sizeof(float) * size;
		buffer = cl::Buffer(context, CL_MEM_READ_WRITE, bufferSize, NULL);
	}

	void createOutputVec3Buffer(Numeric *value, cl::Buffer &buffer, int size, cl::Context &context, cl::CommandQueue &queue, cl::Event &event){
		size_t bufferSize = sizeof(CLVec3) * size;
		buffer = cl::Buffer(context, CL_MEM_READ_WRITE, bufferSize, NULL);
	}

	void readIntBuffer(Numeric *value, cl::Buffer &buffer, int size, cl::CommandQueue &queue, cl::Event &event){
		size_t bufferSize = sizeof(int) * size;
		std::vector<int> outVal(size);
		queue.enqueueReadBuffer(buffer, CL_TRUE, 0, bufferSize, &outVal[0], NULL, &event);
		value->setIntValues(outVal);
	}

	void readFloatBuffer(Numeric *value, cl::Buffer &buffer, int size, cl::CommandQueue &queue, cl::Event &event){
		size_t bufferSize = sizeof(float) * size;
		std::vector<float> outVal(size);
		queue.enqueueReadBuffer(buffer, CL_TRUE, 0, bufferSize, &outVal[0], NULL, &event);
		value->setFloatValues(outVal);
	}

	void readVec3Buffer(Numeric *value, cl::Buffer &buffer, int size, cl::CommandQueue &queue, cl::Event &event){
		size_t bufferSize = sizeof(CLVec3) * size;
		std::vector<Imath::V3f> outVal(size);
		queue.enqueueReadBuffer(buffer, CL_TRUE, 0, bufferSize, &outVal[0], NULL, &event);
		value->setVec3Values(outVal);
	}

	void collectNumericValues(const std::vector<Attribute*> &attributes, std::map<int, Numeric*> &numericValues){
		for(int i = 0; i < attributes.size(); ++i){
			Attribute *attr = attributes[i];
			if(attr->isInput()){
				numericValues[attr->id()] = (Numeric*)attr->value();
			}
			else{
				numericValues[attr->id()] = (Numeric*)attr->outValue();
			}
		}
	}

	void collectKernelFunctionNames(const std::string &kernelSource, std::vector<std::string> &kernelFunctionNames){
		std::vector<std::string> kernels;
		stringUtils::split(kernelSource, kernels, "__kernel");

		for(int i = 1; i < kernels.size(); ++i){
			std::string &kernelFunc = kernels[i];
			std::vector<std::string> funcElements;
			stringUtils::split(kernelFunc, funcElements, "(");

			if(funcElements.size()){
				std::string &kernelFuncDecl = funcElements[0];
				std::vector<std::string> funcDeclElements;
				stringUtils::split(kernelFuncDecl, funcDeclElements, " ");

				for(int j = funcDeclElements.size() - 1; j >= 0 ; --j){
					std::string funcDeclElement = funcDeclElements[j];
					if(funcDeclElement != " "){
						kernelFunctionNames.push_back(funcDeclElement);
						break;
					}
				}
			}
		}
	}

	void deviceEnumChanged(Node *node, Enum *){
		KernelNode *kernelNode = (KernelNode*)node;
		kernelNode->updateDeviceUsed();
	}
}

KernelNode::KernelNode(const std::string &name, Node *parent): 
Node(name, parent){
	setAllowDynamicAttributes(true);

	_device = new EnumAttribute("device", this);
	_kernelSource = new StringAttribute("_kernelSource", this);
	_useSize = new BoolAttribute("_useSize", this);

	addInputAttribute(_device);
	addInputAttribute(_kernelSource);
	addInputAttribute(_useSize);

	setAttributeAllowedSpecialization(_useSize, "BoolArray");

	catchAttributeDirtied(_kernelSource);

	_device->outValue()->addEntry(0, "cpu");
	_device->outValue()->addEntry(1, "gpu");
	_device->outValue()->setCurrentIndex(0);
	_device->outValue()->setCurrentIndexChangedCallback(this, deviceEnumChanged);

	initCL();
}

void KernelNode::updateDeviceUsed(){
	initCL();
	buildKernelSource();
}

void KernelNode::attributeDirtied(Attribute *attribute){
	buildKernelSource();
}

void KernelNode::addDynamicAttribute(Attribute *attribute){
	Node::addDynamicAttribute(attribute);
	
	const std::vector<Attribute*> dynAttrs = dynamicAttributes();
	Bool *useSize = _useSize->outValue();
	useSize->resize(dynAttrs.size());

	if(attribute->isOutput()){
		std::vector<Attribute*> inputAttrs = inputAttributes();
		for(int i = 0; i < inputAttrs.size(); ++i){
			setAttributeAffect(inputAttrs[i], attribute);
		}

		useSize->setBoolValueAt(dynAttrs.size() - 1, false);
	}
	else{
		useSize->setBoolValueAt(dynAttrs.size() - 1, true);
	}
	
	cacheBufferReadWrite(attribute);
}

void KernelNode::removeDynamicAttribute(Attribute *attribute){
	if(findObject("_useSize")){ // during deletion this overloaded method might be invoked after the deletion of _useSize
		const std::vector<Attribute*> &dynAttrs = dynamicAttributes();
		Bool *useSize = _useSize->outValue();

		std::map<int, bool> useSizeMap;
		for(int i = 0; i < dynAttrs.size(); ++i){
			useSizeMap[dynAttrs[i]->id()] = useSize->boolValueAt(i);
		}

		Node::removeDynamicAttribute(attribute);

		const std::vector<Attribute*> &dynAttrs2 = dynamicAttributes();
		useSize->resize(dynAttrs2.size());
		for(int i = 0; i < dynAttrs2.size(); ++i){
			useSize->setBoolValueAt(i, useSizeMap[dynAttrs2[i]->id()]);
		}

		_readBuffer.erase(attribute->id());
		_writeBuffer.erase(attribute->id());
	}
	else{
		Node::removeDynamicAttribute(attribute);
	}
}

void KernelNode::initCL(){
	std::vector<cl::Platform> platforms;
    cl_int err = cl::Platform::get(&platforms);
    if(err){
    	std::cerr << "OpenCL error: " << oclErrorString(err) << std::endl;
    	return;
    }

    if(platforms.size() == 0){
    	std::cerr << "OpenCL error: no platforms available." << std::endl;
        return;
    }

	cl_context_properties properties[] = {CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0};

	cl_int device;
	int deviceIndex = _device->outValue()->currentIndex();
	if(deviceIndex == 0){
		device = CL_DEVICE_TYPE_CPU;
	}
	else if(deviceIndex == 1){
		device = CL_DEVICE_TYPE_GPU;
	}

	_context = cl::Context(device, properties);
	_devices = _context.getInfo<CL_CONTEXT_DEVICES>();

	if(_devices.size() == 0){
		std::cerr << "OpenCL error: no devices available." << std::endl;
		return;
	}

	_queue = cl::CommandQueue(_context, _devices[0], 0, &err);

	if(err){
		std::cerr << "OpenCL error creating CommandQueue: " << oclErrorString(err) << std::endl;
		return;
	}
}

std::string KernelNode::buildInfo(){
	return _buildMessage;
}

void KernelNode::buildKernelSource(){
	_kernels.clear();

	_buildMessage.clear();

	cl_int err = 0;
	
	std::string kernelSource = _kernelSource->value()->stringValue();
	if(kernelSource.empty()){
		return;
	}

	cl::Program program;
	try{
        cl::Program::Sources source(1, std::make_pair(kernelSource.c_str(), kernelSource.size()));
        program = cl::Program(_context, source);
    }
    catch(cl::Error er){
    	_buildMessage = "OpenCL error: " + std::string(er.what()) + " " + oclErrorString(er.err());
        std::cerr << _buildMessage << std::endl;
        return;
    }

    try{
    	err = program.build(_devices);
    }
    catch(cl::Error er){
    	_buildMessage = "OpenCL error: " + oclErrorString(er.err()) + "\n" + program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(_devices[0]);
    	std::cerr << _buildMessage << std::endl;
    	return;
    }

    std::vector<std::string> kernelFunctionNames;
    collectKernelFunctionNames(kernelSource, kernelFunctionNames);
    for(int i = 0; i < kernelFunctionNames.size(); ++i){
    	std::string &kernelFunctionName = kernelFunctionNames[i];

    	cl::Kernel kernel;
    	try{
			kernel = cl::Kernel(program, kernelFunctionName.data(), &err);
    	}
    	catch(cl::Error er){
    		std::string errStr = oclErrorString(er.err());
    		_buildMessage = "OpenCL error while parsing source:\n" + std::string(er.what()) + "\n" + errStr;
    		return;
    	}

    	if(!err){
    		_kernels.push_back(kernel);
    	}
    }

    _buildMessage = "Build successful.";
}

void KernelNode::cacheBufferReadWrite(Attribute *attribute){
	Numeric *val = (Numeric*)attribute->value();
	int attrId = attribute->id();

	Numeric::Type type = val->type();
	if(type == Numeric::numericTypeInt || type == Numeric::numericTypeIntArray){
		if(attribute->isInput()){
			_writeBuffer[attrId] = createInputIntBuffer;
		}
		else{
			_writeBuffer[attrId] = createOutputIntBuffer;
			_readBuffer[attrId] = readIntBuffer;
		}
	}
	else if(type == Numeric::numericTypeFloat || type == Numeric::numericTypeFloatArray){
		if(attribute->isInput()){
			_writeBuffer[attrId] = createInputFloatBuffer;
		}
		else{
			_writeBuffer[attrId] = createOutputFloatBuffer;
			_readBuffer[attrId] = readFloatBuffer;
		}
	}
	else if(type == Numeric::numericTypeVec3 || type == Numeric::numericTypeVec3Array){
		if(attribute->isInput()){
			_writeBuffer[attrId] = createInputVec3Buffer;
		}
		else{
			_writeBuffer[attrId] = createOutputVec3Buffer;
			_readBuffer[attrId] = readVec3Buffer;
		}
	}
	else{
		if(_writeBuffer.find(attrId) != _writeBuffer.end()){
			_writeBuffer.erase(attrId);
		}
		if(_readBuffer.find(attrId) != _readBuffer.end()){
			_readBuffer.erase(attrId);
		}
	}
}

void KernelNode::attributeSpecializationChanged(Attribute *attribute){
	cacheBufferReadWrite(attribute);
}

void KernelNode::writeBuffers(std::map<int, Numeric*> &values, std::map<int, cl::Buffer> &buffers, int size, const std::vector<Attribute*> &dynAttrs, cl::Event &event){
	for(int i = 0; i < dynAttrs.size(); ++i){
		Attribute *attr = dynAttrs[i];
		int attrId = attr->id();

		if(_writeBuffer.find(attrId) != _writeBuffer.end()){
			cl::Buffer &attrBuffer = buffers[attrId];

			_writeBuffer[attrId](values[attrId], attrBuffer, size, _context, _queue, event);

			for(int j = 0; j < _kernels.size(); ++j){
				_kernels[j].setArg(i, attrBuffer);
			}
		}
	}
}

cl_int KernelNode::executeKernel(cl::Kernel &kernel, int size, cl::Event &event){
	cl_int err = 0;

	try{
		err = _queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(size), cl::NullRange, NULL, &event);
	}
	catch(cl::Error er){
		std::cerr << "OpenCL enqueueNDRangeKernel error: " << oclErrorString(err) << std::endl;
		_queue.finish();
	}

	return err;
}

void KernelNode::readBuffers(std::map<int, Numeric*> &values, std::map<int, cl::Buffer> &buffers, int size, cl::Event &event){
	const std::vector<Attribute*> &outAttrs = outputAttributes();
	for(int i = 0; i < outAttrs.size(); ++i){
		Attribute *attr = outAttrs[i];
		int attrId = attr->id();

		cl::Buffer &buffer = buffers[attrId];
		Numeric *val = values[attrId];

		if(_readBuffer.find(attrId) != _readBuffer.end()){
			_readBuffer[attrId](val, buffer, size, _queue, event);
			setAttributeIsClean(attr, true);
		}
	}
}

void KernelNode::update(Attribute *attribute){
	if(_kernels.size()){
		const std::vector<Attribute*> &dynAttrs = dynamicAttributes();

		if(_writeBuffer.size() != dynAttrs.size()){
			return;
		}

		std::map<int, Numeric*> values;
		collectNumericValues(dynAttrs, values);

		int size = findMinorInputSize(dynAttrs, _useSize->value()->boolValues(), values);
		if(size == 0){
			return;
		}

		cl::Event event;
		std::map<int, cl::Buffer> buffers;
		writeBuffers(values, buffers, size, dynAttrs, event);

		for(int i = 0; i < _kernels.size(); ++i){
			cl_int err = executeKernel(_kernels[i], size, event);
			if(err){
				return;
			}
		}

		readBuffers(values, buffers, size, event);

		_queue.finish();
	}
}

