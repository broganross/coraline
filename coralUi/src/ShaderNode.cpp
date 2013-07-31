
#include <iostream>
#include <fstream>
#include <algorithm>

#include "ShaderNode.h"
#include <coral/src/stringUtils.h>
#include <coral/src/NetworkManager.h>
#include <coral/src/Numeric.h>
#include <coral/src/ImageAttribute.h>
#include <coral/src/Image.h>

using namespace coral;
using namespace coralUi;

namespace {
	bool notAscii(int i){ 
		return !isascii(i);
	}

	template <class T>
	void fillIfEmptyValuesInBuffer(const std::vector<T> &values, int pointsCount, const T &filler){
		int valuesCount = values.size();
		if(valuesCount < pointsCount){
			int emptyCount = pointsCount - valuesCount;
			std::vector<T> emptyArray;
			T lastVal;
			if(valuesCount){
				lastVal = values[valuesCount - 1];
			}
			else{
				lastVal = filler;
			}

			emptyArray.resize(emptyCount, lastVal);

			GLintptr offset = sizeof(T)*valuesCount;
			GLsizeiptr size = sizeof(T)*emptyCount;
			glBufferSubData(GL_ARRAY_BUFFER, offset, size, (GLvoid*)&emptyArray[0]);
		}
	}

	class ShaderAttributeFloat: public BaseShaderAttribute{
	public:
		ShaderAttributeFloat(NumericAttribute *attribute): BaseShaderAttribute(attribute){
		}

		void update(int pointsCount){
			const std::vector<float> &values = _attribute->value()->floatValues();

			glBindBuffer(GL_ARRAY_BUFFER, _vbo);
			glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*pointsCount, (GLvoid*)&values[0], GL_STATIC_DRAW);

			float filler = 0.0;
			fillIfEmptyValuesInBuffer<float>(values, pointsCount, filler);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}

		void enable(){
			glBindBuffer(GL_ARRAY_BUFFER, _vbo);
			glVertexAttribPointer(_attributeLocation, 1, GL_FLOAT, 0, 0, 0);
			glEnableVertexAttribArray(_attributeLocation);
		}
	};

	class ShaderUniformFloat: public BaseShaderUniform{
	public:
		ShaderUniformFloat(Attribute *attribute): BaseShaderUniform(attribute){
		}

		void update(){
			Numeric *value = (Numeric*)_attribute->value();
			glUniform1f(_uniformLocation, value->floatValueAt(0));
		}
	};

	class ShaderUniformVec3: public BaseShaderUniform{
	public:
		ShaderUniformVec3(Attribute *attribute): BaseShaderUniform(attribute){
		}

		void update(){
			Numeric *value = (Numeric*)_attribute->value();
			Imath::V3f vec3 = value->vec3ValueAt(0);
			glUniform3f(_uniformLocation, vec3.x, vec3.y, vec3.z);
		}
	};

	class ShaderUniformVec4: public BaseShaderUniform{
	public:
		ShaderUniformVec4(Attribute *attribute): BaseShaderUniform(attribute){
		}

		void update(){
			Numeric *value = (Numeric*)_attribute->value();
			Imath::Color4f col4 = value->col4ValueAt(0);
			glUniform4f(_uniformLocation, col4.r, col4.g, col4.b, col4.a);
		}
	};

	class ShaderUniformSampler2D: public BaseShaderUniform{
	public:
		ShaderUniformSampler2D(Attribute *attribute): BaseShaderUniform(attribute){
		}

		void init(GLuint shaderProgram){
			BaseShaderUniform::init(shaderProgram);
			glGenTextures(1, &_texture);

			dirtied();
		}

		void update(){
			glActiveTexture(GL_TEXTURE0 + 0);

			GLuint unifLocation = glGetUniformLocation(_shaderProgram, _attribute->name().data());
			glUniform1i(unifLocation, 0);

			glBindTexture(GL_TEXTURE_2D, _texture);
		}
		
		void dirtied(){
			glActiveTexture(GL_TEXTURE0 + 0);
			glBindTexture(GL_TEXTURE_2D, _texture);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			Image *image = (Image*)_attribute->value();
			glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB, image->width(), image->height(), 0, GL_RGB, GL_FLOAT, image->pixels());
		}

		void destroy(){
			glDeleteTextures(1, &_texture);
		}

	private:
		GLuint _texture;
	};
}

ShaderNode::ShaderNode(const std::string &name, Node *parent):
_initialized(false),
_recompileShaderLog("Ready to do some shading!"),
DrawNode(name, parent){
	_geo = new GeoAttribute("geo", this);
	_vertexShaderFilename = new StringAttribute("vertexShaderFilename", this);
	_fragmentShaderFilename = new StringAttribute("fragmentShaderFilename", this);

	addInputAttribute(_geo);
	addInputAttribute(_vertexShaderFilename);
	addInputAttribute(_fragmentShaderFilename);

	catchAttributeDirtied(_geo);
	catchAttributeDirtied(_vertexShaderFilename);
	catchAttributeDirtied(_fragmentShaderFilename);

	if(glContextExists()){
		initGL();
	}

	// make sure attributeDirtied is called for these two after a network is loaded
	setAttributeIsClean(_vertexShaderFilename, true);
	setAttributeIsClean(_fragmentShaderFilename, true);
}

ShaderNode::~ShaderNode(){
	if(glContextExists()){
		glDeleteBuffers(1, &_vtxBuffer);
		glDeleteBuffers(1, &_nrmBuffer);
		glDeleteBuffers(1, &_idxBuffer);
		glDeleteBuffers(1, &_uvBuffer);

		clearDynamicGLData();
	}
}

std::string ShaderNode::recompileShaderLog(){
	return _recompileShaderLog;
}

void ShaderNode::initGL(){
	glGenBuffers(1, &_vtxBuffer);
	glGenBuffers(1, &_nrmBuffer);
	glGenBuffers(1, &_idxBuffer);
	glGenBuffers(1, &_uvBuffer);

	recompileShader();
}

void ShaderNode::updateUniformsFromShader(const std::string &filename){
	std::ifstream shaderSource(filename.data());

	// read attribs from shader
	std::vector<std::string> uniformsType;
	std::vector<std::string> uniformsName;

	std::string line;
	while(std::getline(shaderSource, line)){
		if(stringUtils::startswith(line, "uniform")){
			std::vector<std::string> result;
			stringUtils::split(line, result, " ");
			if(result.size() >= 2){
				std::vector<std::string> result2;
				stringUtils::split(result[2], result2, ";");
				if(result2.size() > 0){
					uniformsType.push_back(result[1]);
					uniformsName.push_back(result2[0]);
				}
			}
		}
	}

	// update attributes
	for(int i = 0; i < uniformsType.size(); ++i){
		const std::string &name = uniformsName[i];
		const std::string &type = uniformsType[i];

		Attribute *dynamicAttr = 0;
		BaseShaderUniform *shaderUniform = 0;
		if(type == "float"){
			dynamicAttr = new NumericAttribute(name, this);
			setAttributeAllowedSpecialization(dynamicAttr, "Float");

			shaderUniform = new ShaderUniformFloat(dynamicAttr);
		}
		else if(type == "vec3"){
			dynamicAttr = new NumericAttribute(name, this);
			setAttributeAllowedSpecialization(dynamicAttr, "Vec3");
			
			shaderUniform = new ShaderUniformVec3(dynamicAttr);
		}
		else if(type == "vec4"){
			dynamicAttr = new NumericAttribute(name, this);
			setAttributeAllowedSpecialization(dynamicAttr, "Col4");
			
			shaderUniform = new ShaderUniformVec4(dynamicAttr);
		}
		else if(type == "sampler2D"){
			dynamicAttr = new ImageAttribute(name, this);
			shaderUniform = new ShaderUniformSampler2D(dynamicAttr);

			_dirtiedUniforms[dynamicAttr] = shaderUniform;
		}

		if(dynamicAttr){
			_shaderUniforms.push_back(shaderUniform);

			addInputAttribute(dynamicAttr);
			addDynamicAttribute(dynamicAttr);
		}
	}

	shaderSource.close();
}

void ShaderNode::initShaderAttributes(const std::string &filename){
	std::ifstream shaderSource(filename.data());

	// read attribs from shader
	std::vector<std::string> attributesType;
	std::vector<std::string> attributesName;

	std::string line;
	while(std::getline(shaderSource, line)){
		if(stringUtils::startswith(line, "attribute")){
			std::vector<std::string> result;
			stringUtils::split(line, result, " ");
			if(result.size() >= 2){
				std::vector<std::string> result2;
				stringUtils::split(result[2], result2, ";");
				if(result2.size() > 0){
					attributesType.push_back(result[1]);
					attributesName.push_back(result2[0]);
				}
			}
		}
	}

	// update shader attributes
	for(int i = 0; i < attributesType.size(); ++i){
		const std::string &name = attributesName[i];
		const std::string &type = attributesType[i];

		NumericAttribute *dynamicAttr = new NumericAttribute(name, this);
		std::vector<std::string> spec(2);
		BaseShaderAttribute *shaderAttribute;
		if(type == "float"){
			shaderAttribute = new ShaderAttributeFloat(dynamicAttr);
			spec[0] = "Float";
			spec[1] = "FloatArray";
		}

		if(shaderAttribute){
			_shaderAttributes.push_back(shaderAttribute);

			setAttributeAllowedSpecializations(dynamicAttr, spec);
			addInputAttribute(dynamicAttr);
			addDynamicAttribute(dynamicAttr);
		}
	}

	shaderSource.close();
}

void ShaderNode::clearDynamicAttributes(){
	const std::vector<Attribute*> dynAttrs = dynamicAttributes();
	for(int i = 0; i < dynAttrs.size(); ++i){
		removeAttribute(dynAttrs[i]);
	}
}

void ShaderNode::clearDynamicGLData(){
	for(int i = 0; i < _shaderAttributes.size(); ++i){
		_shaderAttributes[i]->destroy();
		delete _shaderAttributes[i];
	}
	_shaderAttributes.clear();

	for(int i = 0; i < _shaderUniforms.size(); ++i){
		_shaderUniforms[i]->destroy();
		delete _shaderUniforms[i];
	}
	_shaderUniforms.clear();
	_dirtiedUniforms.clear();
}

void ShaderNode::cacheAndRemoveConnections(std::vector<std::pair<Attribute*, std::string> > &inputConnections){
	const std::vector<Attribute*> &dynAttrs = dynamicAttributes();
	for(int i = 0; i < dynAttrs.size(); ++i){
		Attribute *dynAttr = dynAttrs[i];
		Attribute *inputAttr = dynAttr->input();
		if(inputAttr){
			inputConnections.push_back(std::pair<Attribute*, std::string>(inputAttr, dynAttr->name()));
			dynAttr->disconnectInput();
		}
	}
}

void ShaderNode::restoreConnections(std::vector<std::pair<Attribute*, std::string> > &inputConnections){
	for(int i = 0; i < inputConnections.size(); ++i){
		Attribute *sourceAttr = inputConnections[i].first;
		Attribute *destAttr = findAttribute(inputConnections[i].second);

		if(sourceAttr && destAttr){
			NetworkManager::connect(sourceAttr, destAttr);
		}
	}
}

void ShaderNode::cacheVaues(std::map<std::string, std::string> &values){
	const std::vector<Attribute*> &dynAttrs = dynamicAttributes();
	for(int i = 0; i < dynAttrs.size(); ++i){
		Attribute *dynAttr = dynAttrs[i];
		if(!dynAttr->input()){
			std::string valStr = dynAttr->value()->asString();
			if(!valStr.empty()){
				values[dynAttr->name()] = valStr;
			}
		}
	}
}

void ShaderNode::restoreValues(std::map<std::string, std::string> &values){
	const std::vector<Attribute*> &dynAttrs = dynamicAttributes();
	for(int i = 0; i < dynAttrs.size(); ++i){
		Attribute *dynAttr = dynAttrs[i];
		if(!dynAttr->input()){
			std::string attrName = dynAttr->name();
			if(values.find(attrName) != values.end()){
				dynAttr->value()->setFromString(values[attrName]);
			}
		}
	}
}

void ShaderNode::updateDynamicDataFromShaders(const std::string &vertexShaderFilename, const std::string &fragmentShaderFilename){
	// temporarily enable dynamic attributes
	setAllowDynamicAttributes(true);

	std::map<std::string, std::string> values;
	cacheVaues(values);

	std::vector<std::pair<Attribute*, std::string> > inputConnections;
	cacheAndRemoveConnections(inputConnections);
	
	clearDynamicAttributes();
	clearDynamicGLData();
	initShaderAttributes(vertexShaderFilename);
	updateUniformsFromShader(vertexShaderFilename);
	updateUniformsFromShader(fragmentShaderFilename);
	
	restoreConnections(inputConnections);
	restoreValues(values);

	// disable dynamic attributes so they don't get saved in the network file and re-created on file load,
	// they will be re-created anyway by this node during the initialization 
	setAllowDynamicAttributes(false);
}

void ShaderNode::recompileShader(){
	std::string vertexShaderFilename = NetworkManager::resolveFilename(_vertexShaderFilename->value()->stringValue());
	std::string fragmentShaderFilename = NetworkManager::resolveFilename(_fragmentShaderFilename->value()->stringValue());
	
	if(vertexShaderFilename.empty()){
		_recompileShaderLog = "Error: Could not find the vertex file : p";
		return;
	}

	if(fragmentShaderFilename.empty()){
		_recompileShaderLog = "Error: Could not find the fragment file : |";
		return;
	}

	updateDynamicDataFromShaders(vertexShaderFilename, fragmentShaderFilename);

	if(glContextExists()){
		std::ifstream ifs(vertexShaderFilename.data());
	    std::ostringstream ss; ss << ifs.rdbuf();
	    std::ifstream ifs1(fragmentShaderFilename.data());
	    std::ostringstream ss1; ss1 << ifs1.rdbuf();
	    ifs.close(); 
	    ifs1.close();

	    std::string vertexShaderSource = ss.str();
	    vertexShaderSource.erase(std::remove_if(vertexShaderSource.begin(), vertexShaderSource.end(), notAscii), vertexShaderSource.end());

	    std::string fragmentShaderSource = ss1.str();
	    fragmentShaderSource.erase(std::remove_if(fragmentShaderSource.begin(), fragmentShaderSource.end(), notAscii), fragmentShaderSource.end());

	    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER_ARB);

		GLchar *glVertexShaderSource = (GLchar*)vertexShaderSource.data();
		glShaderSource(vertexShader, 1, (const GLchar**) &glVertexShaderSource, NULL);
		glCompileShader(vertexShader);

		int shaderStatus;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shaderStatus);
		if(!shaderStatus){
			_recompileShaderLog = "Error: failed compiling the vertex shader : /"; 
			return;
		}

		GLchar *glFragmentShaderSource = (GLchar*)fragmentShaderSource.data();
		glShaderSource(fragmentShader, 1, (const GLchar**) &glFragmentShaderSource, NULL);
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &shaderStatus);
		if(!shaderStatus){
			_recompileShaderLog = "Error: failed compiling the fragment shader : ("; 
			return;
		}

		_shaderProgram = glCreateProgram();
		glAttachShader(_shaderProgram, vertexShader);
		glAttachShader(_shaderProgram, fragmentShader);

		glLinkProgram(_shaderProgram);

		for(int i = 0; i < _shaderAttributes.size(); ++i){
			_shaderAttributes[i]->init(_shaderProgram);
		}

		for(int i = 0; i < _shaderUniforms.size(); ++i){
			_shaderUniforms[i]->init(_shaderProgram);
		}
	}

	_recompileShaderLog = "Your shader is running, cool!";
}

void ShaderNode::attributeDirtied(Attribute *attribute){
	DrawNode::attributeDirtied(attribute);

	if(!_initialized){
		if(attribute == _vertexShaderFilename || attribute == _fragmentShaderFilename){
			std::string vertexShaderFilename = _vertexShaderFilename->value()->stringValue();
			std::string fragmentShaderFilename = _fragmentShaderFilename->value()->stringValue();

			if(!vertexShaderFilename.empty() && !fragmentShaderFilename.empty()){
				recompileShader();
				_initialized = true;
			}
		}
	}
	else if(glContextExists()){
		if(_dirtiedUniforms.find(attribute) != _dirtiedUniforms.end()){
			_dirtiedUniforms[attribute]->dirtied();
		}
	}
	
}

void ShaderNode::addDynamicAttribute(Attribute *attribute){
	DrawNode::addDynamicAttribute(attribute);
	catchAttributeDirtied(attribute);
}

// we might want to merge this and GeoDraw::updateGeoVBO()
void ShaderNode::updateGeoVBO(Geo *geo){
	const std::vector<Imath::V3f> &points = geo->points();
	const std::vector<Imath::V3f> &vtxNormals = geo->verticesNormals();
	const std::vector<std::vector<int> > &faces = geo->rawFaces();
	const std::vector<Imath::V2f> &rawUvs = geo->rawUvs();

	// vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, _vtxBuffer);
	glBufferData(GL_ARRAY_BUFFER, 3*sizeof(GLfloat)*points.size(), (GLvoid*)&points[0].x, GL_STATIC_DRAW);

	// normal buffer
	if(vtxNormals.empty() == false){
		glBindBuffer(GL_ARRAY_BUFFER, _nrmBuffer);
		glBufferData(GL_ARRAY_BUFFER, 3*sizeof(GLfloat)*vtxNormals.size(), (GLvoid*)&vtxNormals[0].x, GL_STATIC_DRAW);
	}

	// index buffer generation
	const std::vector<int> &indices = geo->rawIndices();
	// send to GPU!
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _idxBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int)*indices.size(), &indices[0], GL_STATIC_DRAW);

	if(rawUvs.empty() == false){
		glBindBuffer(GL_ARRAY_BUFFER, _uvBuffer);
		glBufferData(GL_ARRAY_BUFFER, 2*sizeof(GLfloat)*rawUvs.size(), (GLvoid*)&rawUvs[0].x, GL_STATIC_DRAW);
	}

	// clean OpenGL states
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void ShaderNode::updateShaderAttributes(Geo *geo){
	int pointsCount = geo->pointsCount();
	for(int i = 0; i < _shaderAttributes.size(); ++i){
		_shaderAttributes[i]->update(pointsCount);
	}
}

void ShaderNode::enableShaderAttributes(){
	for(int i = 0; i < _shaderAttributes.size(); ++i){
		_shaderAttributes[i]->enable();
	}
}

void ShaderNode::disableShaderAttributes(){
	for(int i = 0; i < _shaderAttributes.size(); ++i){
		_shaderAttributes[i]->disable();
	}
}

void ShaderNode::updateUniforms(){
	for(int i = 0; i < _shaderUniforms.size(); ++i){
		_shaderUniforms[i]->update();
	}
}

void ShaderNode::draw(){
	DrawNode::draw();

	Geo *geo = _geo->value();

	if(geo->facesCount()){
		updateGeoVBO(geo);
		updateShaderAttributes(geo);

		glPushAttrib(GL_POLYGON_BIT | GL_LIGHTING_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_POINT_BIT);

		const std::vector<int> &indexCounts = geo->rawIndexCounts();
		const std::vector<Imath::V3f> &vtxNormals = geo->verticesNormals();

		glUseProgram(_shaderProgram);

		enableShaderAttributes();

		updateUniforms();

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.f, 1.f);

		glEnable(GL_LIGHTING);
		glShadeModel(GL_SMOOTH);

		glBindBuffer(GL_ARRAY_BUFFER, _vtxBuffer);
		glVertexPointer(3, GL_FLOAT, 0, NULL);

		glBindBuffer(GL_ARRAY_BUFFER, _nrmBuffer);
		glNormalPointer(GL_FLOAT, 0, NULL);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _idxBuffer);

		glBindBuffer(GL_ARRAY_BUFFER, _uvBuffer);
		glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		//glEnable(GL_TEXTURE_2D);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);

		glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);

		int idOffset = 0;
		for(int i = 0; i < indexCounts.size(); ++i){
			glDrawElements(GL_POLYGON, indexCounts[i], GL_UNSIGNED_INT, (GLuint*)NULL+idOffset);
			idOffset += indexCounts[i];
		}

		glDisable(GL_COLOR_MATERIAL);

		glDisable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisableClientState(GL_COLOR_ARRAY);

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		disableShaderAttributes();

		glUseProgram(0);
		
		glPopAttrib();
	}
}
