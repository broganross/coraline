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
#include "ObjImporter.h"
#include "../src/GeoAttribute.h"
#include "../src/StringAttribute.h"
#include "../src/NetworkManager.h"

#include <fstream>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#if defined(WIN64) || defined(_WIN64) || defined(WIN32) || defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/gl.h>
#endif
#endif

using namespace coral;

ObjImporter::ObjImporter(const std::string &name, Node *parent) : Node(name, parent){
	_fileName = new StringAttribute("fileName", this);
	_geo = new GeoAttribute("geo", this);

	addInputAttribute(_fileName);
	addOutputAttribute(_geo);

	setAttributeAffect(_fileName, _geo);
}

ObjImporter::~ObjImporter(){
}

int _pattern = -1;

bool retrievePattern(std::istream &stream){

/*
	pattern 0: v v v ...					(0 slashes)
	pattern 1: v/vt v/vt v/vt ...			(1 slashes)
	pattern 2: v/vt/vn v/vt/vn v/vt/vn ...	(2 slashes)
	pattern 3: v//vn v//vn v//vn ...		(2 slashes)
*/

	std::string s;
	stream >> s;

	int slash0 = s.find("/");
	if(slash0 != std::string::npos){
		int slash1 = s.find("/", slash0 + 1);

		if(slash1 != std::string::npos){
			// 2 consecutive slashes mean pattern 3(v//vn), otherwise pattern 2 (v/vt/vn)
			if(slash1 - 1 == slash0)
				_pattern = 3;
			else
				_pattern = 2;
		}
		else
			// 1 slash (v/vt)
			_pattern = 1;
	}
	else
		// 0 slash (v)
		_pattern = 0;

	if(!s.empty())
		for(unsigned int u = s.size(); u > 0; --u){
			stream.putback(s[u-1]);
		}

	return _pattern != -1;
}

void skipLine(std::istream& stream){
	stream >> std::noskipws;
    char c;
	while((stream >> c) && (c != '\n'));	// stop skipping at new line
}

bool readLine(std::istream& stream, std::vector<Imath::V3f> &vertices, std::vector<Imath::V3f> &normals, std::vector<Imath::V2f> &uvs, std::vector<std::vector<int> > &faces){
	char c;

	while(stream >> std::skipws >> c && c == '#'){
		skipLine(stream);
	}
	stream.putback(c);

	std::string s;
	if(!(stream >> s))
		return false;

	else if(s == "v"){
		float x, y, z;
		stream >> x >> y >> z;
		vertices.push_back(Imath::V3f(x, y, z));
	}

	else if(s == "vt"){
		float u, v;
		stream >> u >> v;
		uvs.push_back(Imath::V2f(u, v));
	}

	else if(s == "vn"){
		float x, y, z;
		stream >> x >> y >> z;

		if(!stream.good()){		// -1#IND00
			x = y = z = 0.f;
			stream.clear();
			skipLine(stream);
		}
		normals.push_back(Imath::V3f(x, y, z));
	}

	else if(s == "f"){
		
		if(_pattern == -1)
			if(!retrievePattern(stream))
				return false;

		int v, vt, vn;
		std::vector<int> faceVertices;

		while(stream.good()){
			/*
			pattern 0: v v v ...					(0 slashes)
			pattern 1: v/vt v/vt v/vt ...			(1 slashes)
			pattern 2: v/vt/vn v/vt/vn v/vt/vn ...	(2 slashes)
			pattern 3: v//vn v//vn v//vn ...		(2 slashes)
			*/

			// v
			stream >> v;
			faceVertices.push_back(v-1);	// Obj indices start from 1

			// let's skip uv & normal ID as we don't need them
			if(_pattern == 1){
				// /vt
				stream >> c;
				stream >> vt;
			}
			else if(_pattern == 2){
				// /vt/vn
				stream >> c;
				stream >> vt;
				stream >> c;
				stream >> vn;
			}
			else if(_pattern == 3){
				// //vn
				stream >> c;
				stream >> c;
				stream >> vn;
			}

			// end of line (face) or another entry (vertex) ?
			stream >> std::noskipws;

			do{
				stream >> c;
			}
			while(c == ' ');

			stream >> std::skipws;

			if(c == '\n')
				break;
			else
				stream.putback(c);
		}

		faces.push_back(faceVertices);

		stream.clear();
	}
	else
		skipLine(stream);	// skip every command ("o, g..") but commands "v, vt, vn, f"

	return true;
}

void ObjImporter::updateSlice(Attribute *attribute, unsigned int slice){

	// read
	std::string filename = _fileName->value()->stringValue();
	filename = NetworkManager::resolveFilename(filename);

	std::ifstream stream(filename.c_str(), std::ios::in | std::ios::ate);
	if(!stream || !stream.tellg()){
		_geo->outValue()->clear();
		return;
	}
	
	stream.seekg(0, std::ios::beg);
	
	std::vector<Imath::V3f> vertices;
	std::vector<Imath::V3f> normals;
	std::vector<Imath::V2f> uvs;
	std::vector<std::vector<int> > faces;

	_pattern = -1;

	while(readLine(stream, vertices, normals, uvs, faces));
	stream.close();
	
	_geo->outValue()->build(vertices, faces, uvs);
	if(normals.size()){
		_geo->outValue()->setVerticesNormals(normals);
	}
}
