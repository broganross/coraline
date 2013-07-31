
#ifndef CORAL_DRAWGEOINSTANCE_H
#define CORAL_DRAWGEOINSTANCE_H

#include <cstdio>
#include <string.h>

#include <coral/src/Node.h>
#include <coral/src/GeoInstanceArrayAttribute.h>
#include <coral/src/Geo.h>
#include <coral/src/Attribute.h>
#include <coral/src/NumericAttribute.h>

#include "DrawNode.h"
#include "coralUiDefinitions.h"

#include <GL/glew.h>

namespace coralUi{

class CORALUI_EXPORT DrawGeoInstance : public DrawNode{
public:
	DrawGeoInstance(const std::string &name, coral::Node *parent);
	~DrawGeoInstance();
	void draw();
	void initGL();
	void attributeDirtied(coral::Attribute *attribute);

private:
	coral::GeoInstanceArrayAttribute *_geoInstance;
	coral::NumericAttribute *_colors;

	GLuint _shaderProgram;
	GLint _matrixAttrLoc;	// the location of matrix

	GLuint _locationsBuffer;	// vbos
	GLuint _vtxBuffer;
	GLuint _nrmBuffer;
	GLuint _idxBuffer;

	bool _geoInstanceDirtied;

	void updateGeoVBO(coral::Geo *geo);
	void updateLocationsVBO(const std::vector<Imath::M44f> &locations);
	void initShader();
};

}

#endif
