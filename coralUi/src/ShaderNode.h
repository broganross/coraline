
#ifndef CORAL_SHADERNODE_H
#define CORAL_SHADERNODE_H

#include <coral/src/Node.h>
#include <coral/src/Attribute.h>
#include <coral/src/NumericAttribute.h>
#include <coral/src/BoolAttribute.h>
#include <coral/src/StringAttribute.h>
#include <coral/src/GeoAttribute.h>
#include <coral/src/Geo.h>

#include "DrawNode.h"
#include "coralUiDefinitions.h"

#include <GL/glew.h>

namespace coralUi{

class BaseShaderAttribute{
public:
	BaseShaderAttribute(coral::NumericAttribute *attribute){
		_attribute = attribute;
	}

	virtual void init(GLuint shaderProgram){
		_shaderProgram = shaderProgram;
		glGenBuffers(1, &_vbo);
		_attributeLocation = glGetAttribLocation(_shaderProgram, _attribute->name().data());
	};
	virtual void update(int pointsCount){};
	virtual void enable(){};
	virtual void disable(){
		glDisableVertexAttribArray(_attributeLocation);
	};
	virtual void destroy(){
		glDeleteBuffers(1, &_vbo);
	};

protected:
	coral::NumericAttribute *_attribute;
	GLuint _shaderProgram;
	GLuint _vbo;
	GLuint _attributeLocation;
};

class BaseShaderUniform{
public:
	BaseShaderUniform(coral::Attribute *attribute){
		_attribute = attribute;
	}

	virtual void init(GLuint shaderProgram){
		_shaderProgram = shaderProgram;
		_uniformLocation = glGetUniformLocation(_shaderProgram, _attribute->name().data());
	};

	virtual void update(){};
	virtual void dirtied(){};
	virtual void destroy(){};

protected:
	coral::Attribute *_attribute;
	GLuint _shaderProgram;
	GLuint _uniformLocation;
};

class ShaderNode : public DrawNode{
public:
	ShaderNode(const std::string &name, coral::Node *parent);
	~ShaderNode();
	void draw();
	void initGL();
	void attributeDirtied(coral::Attribute *attribute);
	void addDynamicAttribute(coral::Attribute *attribute);
	void recompileShader();
	std::string recompileShaderLog();

private:
	coral::GeoAttribute *_geo;
	coral::StringAttribute *_vertexShaderFilename;
	coral::StringAttribute *_fragmentShaderFilename;
	GLuint _shaderProgram;
	GLuint _vtxBuffer;
	GLuint _nrmBuffer;
	GLuint _idxBuffer;
	GLuint _uvBuffer;
	bool _initialized;
	std::string _recompileShaderLog;
	std::vector<BaseShaderAttribute*> _shaderAttributes;
	std::vector<BaseShaderUniform*> _shaderUniforms;
	std::map<coral::Attribute*, BaseShaderUniform*> _dirtiedUniforms;

	void updateGeoVBO(coral::Geo *geo);
	void initShaderAttributes(const std::string &filename);
	void updateShaderAttributes(coral::Geo *geo);
	void enableShaderAttributes();
	void disableShaderAttributes();
	void clearDynamicAttributes();
	void clearDynamicGLData();
	void cacheAndRemoveConnections(std::vector<std::pair<coral::Attribute*, std::string> > &inputConnections);
	void restoreConnections(std::vector<std::pair<coral::Attribute*, std::string> > &inputConnections);
	void updateDynamicDataFromShaders(const std::string &vertexShaderFilename, const std::string &fragmentShaderFilename);
	void updateUniformsFromShader(const std::string &filename);
	void updateUniforms();
	void cacheVaues(std::map<std::string, std::string> &values);
	void restoreValues(std::map<std::string, std::string> &values);
};

}

#endif
