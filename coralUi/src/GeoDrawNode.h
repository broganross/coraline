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

#ifndef CORALGEODRAWNODE_H
#define CORALGEODRAWNODE_H

#include <cstdio>
#include <string.h>

#include <coral/src/Node.h>
#include <coral/src/Geo.h>
#include <coral/src/GeoAttribute.h>
#include <coral/src/ImageAttribute.h>
#include <coral/src/BoolAttribute.h>
#include <coral/src/NumericAttribute.h>
#include "DrawNode.h"
#include "coralUiDefinitions.h"

#include <GL/glew.h>

namespace coralUi{

class CORALUI_EXPORT GeoDrawNode : public DrawNode{
public:
	GeoDrawNode(const std::string &name, coral::Node *parent);
	~GeoDrawNode();
	void attributeDirtied(coral::Attribute *attribute);
	void draw();
	void initGL();

private:
	coral::GeoAttribute *_geo;
	coral::BoolAttribute *_smooth;
	coral::BoolAttribute *_flat;
	coral::BoolAttribute *_wireframe;
	coral::BoolAttribute *_points;
	coral::BoolAttribute *_normals;
	// coral::BoolAttribute *_ids;
	coral::NumericAttribute *_colors;
	coral::ImageAttribute *_image;

	bool _shouldUpdateGeoVBO;
	bool _shouldUpdateColorVBO;
	bool _shouldUpdateTexture;
	
	void updateGeoVBO();
	void updateColorVBO();
	void updateTexture();
	void updateVBOWireframeIndex(coral::Geo *geo);
	void updateShaders(coral::Geo *geo);
	void drawNormals(coral::Geo *geo, bool shouldDrawFlat);
	void drawWireframe(coral::Geo *geo);
	void drawSurface(coral::Geo *geo, bool smooth);
	void drawPoints(coral::Geo *geo);
	// void drawPointIds(coral::Geo *geo);

	// OpenGL
	GLuint _vtxBuffer;		// buffer of vertices: {0.35, 0.76, 0.48, 0.56, 0.37, etc...}
	GLuint _nrmBuffer;		// buffer of normales: {0.0, 0.0, 1.0, 0.0, 0.0, 1.0, etc...}
	GLuint _uvBuffer;
	GLuint _colBuffer;		// buffer of color4: {0.5, 0.5, 0.5, 1.0, 0.5, 0.5, 0.5, 1.0, etc...}
	GLuint _idxBuffer;		// buffer of indices: {0,1,2,3, 3,4,5,0, 4,6,7,5 etc...}
	GLuint _texture;

	GLsizei _vtxCount;		// the number of vtx/normal/col send (used for allocation optimization)
	GLsizei _nrmCount;
	GLsizei _uvCount;
	GLsizei _colCount;		// col count is acutally a little special (more infos in the code)
	GLsizei _idxCount;
};

}

#endif
