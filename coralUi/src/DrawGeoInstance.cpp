
#include "DrawGeoInstance.h"

using namespace coral;
using namespace coralUi;

DrawGeoInstance::DrawGeoInstance(const std::string &name, Node *parent):
DrawNode(name, parent),
_matrixAttrLoc(-1),
_geoInstanceDirtied(true){

	_geoInstance = new GeoInstanceArrayAttribute("geoInstance", parent);
	_colors = new NumericAttribute("colors", parent);

	addInputAttribute(_geoInstance);
	addInputAttribute(_colors);

	std::vector<std::string> colorSpecializations;
	colorSpecializations.push_back("Col4");
	colorSpecializations.push_back("Col4Array");
	setAttributeAllowedSpecializations(_colors, colorSpecializations);

	_colors->outValue()->setCol4ValueAt(0, Imath::Color4f(0.5, 0.5, 0.5, 1.0));

	if(glContextExists()){
		initGL();
	}
}

DrawGeoInstance::~DrawGeoInstance(){
	if(glContextExists()){
		glDeleteBuffers(1, &_vtxBuffer);
		glDeleteBuffers(1, &_nrmBuffer);
		glDeleteBuffers(1, &_idxBuffer);
	}
}

void DrawGeoInstance::initShader(){
	std::string vertexShaderSource = 
		"\
		attribute mat4 location;\n\
		void main() {\n\
			vec3 normal = normalize(gl_NormalMatrix * gl_Normal);\n\
			gl_Position = gl_ModelViewMatrix * gl_Vertex;\n\
			gl_Position = gl_Position / gl_Position.w;\n\
			float NdotL;\n\
			float HdotN;\n\
			vec3 lightDir;\n\
			vec3 halfVector;\n\
			vec4 diffuseC;\n\
			vec4 specularC;\n\
			int numLightSources = 1;\n\
			gl_FrontColor = gl_FrontMaterial.ambient;\n\
			for(int i = 0; i < numLightSources; i++) {\n\
				lightDir = normalize(vec3(gl_LightSource[i].position));\n\
				NdotL = max(dot(normal, lightDir), 0.0);\n\
				diffuseC = gl_FrontMaterial.diffuse * gl_LightSource[i].diffuse * NdotL;\n\
				gl_FrontColor += diffuseC;\n\
				if (NdotL > 0.0) {\n\
					halfVector = normalize(lightDir - normalize(gl_Position.xyz));\n\
					HdotN = max(0.0, dot(halfVector,  normal));\n\
					specularC = gl_FrontMaterial.specular * gl_LightSource[i].specular * pow (HdotN, gl_FrontMaterial.shininess);\n\
					gl_FrontColor += specularC;\n\
				}\n\
			}\n\
			gl_Position = (gl_ModelViewProjectionMatrix * location) * gl_Vertex;\n\
		}";

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

	GLchar *glVertexShaderSource = (GLchar*)vertexShaderSource.data();
	glShaderSource(vertexShader, 1, (const GLchar**) &glVertexShaderSource, NULL);
	glCompileShader(vertexShader);

	int shaderStatus;
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &shaderStatus);
	if(!shaderStatus){
		std::cout << "error while compiling vertex shader" << std::endl; 
	}

	_shaderProgram = glCreateProgram();
	glAttachShader(_shaderProgram, vertexShader);

	glLinkProgram(_shaderProgram);
}

void DrawGeoInstance::initGL(){
	catchAttributeDirtied(_geoInstance);

	glGenBuffers(1, &_vtxBuffer);
	glGenBuffers(1, &_nrmBuffer);
	glGenBuffers(1, &_idxBuffer);
	glGenBuffers(1, &_locationsBuffer);

	initShader();
}

void DrawGeoInstance::attributeDirtied(Attribute *attribute){
	DrawNode::attributeDirtied(attribute);
	
	if(attribute == _geoInstance){
		_geoInstanceDirtied = true;
	}
}

void DrawGeoInstance::updateGeoVBO(Geo *geo){
	const std::vector<Imath::V3f> &points = geo->points();
	const std::vector<Imath::V3f> &vtxNormals = geo->verticesNormals();
	const std::vector<std::vector<int> > &faces = geo->rawFaces();

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


	// clean OpenGL states
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void DrawGeoInstance::updateLocationsVBO(const std::vector<Imath::M44f> &locations){
	glBindBuffer(GL_ARRAY_BUFFER, _locationsBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Imath::M44f) * locations.size(), &locations[0].x, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DrawGeoInstance::draw(){
	GeoInstanceArray *geoInstance = _geoInstance->value();
	const std::vector<Geo*> &sourceGeos = geoInstance->sourceGeos();
	const std::vector<std::vector<Imath::M44f> > &selectedLocations = geoInstance->selectedLocations();
	const std::vector<Imath::Color4f> &colors = _colors->value()->col4Values();

	int sourceGeosSize = sourceGeos.size();
	int colorsSize = colors.size();

	for(int i = 0; i < sourceGeosSize; ++i){
		Geo *geo = sourceGeos[i];
		const std::vector<int> &indexCounts = geo->rawIndexCounts();
		const std::vector<Imath::M44f> &locations = selectedLocations[i];
		
		if(indexCounts.size() && locations.size()){
			glPushAttrib(GL_POLYGON_BIT | GL_LIGHTING_BIT | GL_LINE_BIT | GL_CURRENT_BIT | GL_POINT_BIT);

			updateLocationsVBO(locations);
			updateGeoVBO(geo);

			glUseProgram(_shaderProgram);

			glBindBuffer(GL_ARRAY_BUFFER, _locationsBuffer);

			// setup location attribute
			_matrixAttrLoc = glGetAttribLocation(_shaderProgram, "location");
			glEnableVertexAttribArray(_matrixAttrLoc);
			glEnableVertexAttribArray(_matrixAttrLoc + 1);
			glEnableVertexAttribArray(_matrixAttrLoc + 2);
			glEnableVertexAttribArray(_matrixAttrLoc + 3);

			glVertexAttribPointer(_matrixAttrLoc, 4, GL_FLOAT, GL_FALSE, sizeof(Imath::M44f), (void*)(0));
			glVertexAttribPointer(_matrixAttrLoc + 1, 4, GL_FLOAT, GL_FALSE, sizeof(Imath::M44f), (void*)(sizeof(float) * 4));
			glVertexAttribPointer(_matrixAttrLoc + 2, 4, GL_FLOAT, GL_FALSE, sizeof(Imath::M44f), (void*)(sizeof(float) * 8));
			glVertexAttribPointer(_matrixAttrLoc + 3, 4, GL_FLOAT, GL_FALSE, sizeof(Imath::M44f), (void*)(sizeof(float) * 12));

			glVertexAttribDivisorARB(_matrixAttrLoc, 1);
			glVertexAttribDivisorARB(_matrixAttrLoc + 1, 1);
			glVertexAttribDivisorARB(_matrixAttrLoc + 2, 1);
			glVertexAttribDivisorARB(_matrixAttrLoc + 3, 1);

			// setup render
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.f, 1.f);

			glEnable(GL_LIGHTING);
			glShadeModel(GL_SMOOTH);

			glBindBuffer(GL_ARRAY_BUFFER, _vtxBuffer);
			glVertexPointer(3, GL_FLOAT, 0, NULL);

			glBindBuffer(GL_ARRAY_BUFFER, _nrmBuffer);
			glNormalPointer(GL_FLOAT, 0, NULL);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _idxBuffer);

			glEnableClientState(GL_VERTEX_ARRAY);
			glEnableClientState(GL_NORMAL_ARRAY);

			glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
			glEnable(GL_COLOR_MATERIAL);

			if(colorsSize == 0){
				glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
			}
			else if(i <= colorsSize - 1){
				const Imath::Color4f &color = colors[i];
				glColor4f(color.r, color.g, color.b, color.a);
			}
			else{
				const Imath::Color4f &color = colors[colorsSize - 1];
				glColor4f(color.r, color.g, color.b, color.a);
			}
			
			// render
			int idOffset = 0;
			for(int i = 0; i < indexCounts.size(); ++i){
				glDrawElementsInstancedARB(GL_POLYGON, indexCounts[i], GL_UNSIGNED_INT, (GLuint*)NULL+idOffset, locations.size());

				idOffset += indexCounts[i];	// 4+3+4+4+4+4+etc...
			}

			// clean OpenGL states
			glDisable(GL_COLOR_MATERIAL);

			glDisable(GL_POLYGON_OFFSET_FILL);

			glDisable(GL_LIGHTING);

			glDisableClientState(GL_VERTEX_ARRAY);
			glDisableClientState(GL_NORMAL_ARRAY);

			// disable every attribute location
			glDisableVertexAttribArray(_matrixAttrLoc);
			glDisableVertexAttribArray(_matrixAttrLoc + 1);
			glDisableVertexAttribArray(_matrixAttrLoc + 2);
			glDisableVertexAttribArray(_matrixAttrLoc + 3);

			// don't forget to set divisors to zero!
			glVertexAttribDivisorARB(_matrixAttrLoc, 0);
			glVertexAttribDivisorARB(_matrixAttrLoc + 1, 0);
			glVertexAttribDivisorARB(_matrixAttrLoc + 2, 0);
			glVertexAttribDivisorARB(_matrixAttrLoc + 3, 0);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			glUseProgram(0);

			glPopAttrib();
		}
	}
}
