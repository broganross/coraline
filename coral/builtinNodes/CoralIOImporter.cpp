#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include "CoralIOImporter.h"
#include "../src/stringUtils.h"
#include "../src/Numeric.h"
#include "../src/NetworkManager.h"

using namespace coral;

namespace {
	void resetMatrixScaling(Imath::M44f &matrix){
		Imath::V3f axis;
		for(int i = 0; i < 3; ++i){
			axis.x = matrix.x[i][0];
			axis.y = matrix.x[i][1];
			axis.z = matrix.x[i][2];

			axis.normalize();

			matrix.x[i][0] = axis.x;
			matrix.x[i][1] = axis.y;
			matrix.x[i][2] = axis.z;
		}
	}

	void parseCIOTransforms(const std::string &filename, std::string &version, std::string &type, int &transforms, int &frames, std::vector<Imath::M44f> &matrixValues, std::vector<Imath::V3f> &vec3Values){
		std::ifstream t;
		t.open(filename.data());
		std::string buffer;
		std::string line;
		
		int lineNumber = 0;
		while(std::getline(t, line)){
			
			if(lineNumber == 0){
				std::vector<std::string> result;
				stringUtils::split(line, result, ":");
				if(result.size() == 2){
					if(result[0] == "coralIO"){
						version = result[1];
					}
				}
			}
			else if(lineNumber == 1){
				std::vector<std::string> result;
				stringUtils::split(line, result, ":");
				if(result.size() == 2){
					if(result[0] == "type"){
						type = result[1];
					}
				}
			}
			else if(lineNumber == 2){
				std::vector<std::string> result;
				stringUtils::split(line, result, ":");
				if(result.size() == 2){
					if(result[0] == "transforms"){
						transforms = stringUtils::parseInt(result[1]);
					}
				}
			}
			else if(lineNumber == 3){
				std::vector<std::string> result;
				stringUtils::split(line, result, ":");
				if(result.size() == 2){
					if(result[0] == "frames"){
						frames = stringUtils::parseInt(result[1]);
					}
				}
			}
			else{
				if(type == "transform"){
					std::vector<std::string> result;
					stringUtils::split(line, result, ",");
					
					Imath::M44f matrix;
					for(int i = 0; i < result.size(); ++i){
						float val = stringUtils::parseFloat(result[i]);
						int row = i / 4;
						int col= i % 4;
						matrix.x[row][col] = val;
					}
					
					resetMatrixScaling(matrix);

					matrixValues.push_back(matrix);
				}
			}
			
			lineNumber ++;
		}
		t.close();
	}

	void cacheMatrixValues(int frames, int transforms, const std::vector<Imath::M44f> &matrixValues, std::map<int, std::vector<Imath::M44f> > &cachedMatrixValues){
		int matrixCounter = 0;
		for(int t = 0; t < frames; ++t){
			cachedMatrixValues[t].resize(transforms);
			for(int i = 0; i < transforms; ++i){
				cachedMatrixValues[t][i] = matrixValues[matrixCounter];
				
				matrixCounter += 1;
			}
		}
	}

	void interpolateMatrix(const Imath::M44f &matrixA, const Imath::M44f &matrixB, float blend, Imath::M44f &outMatrix){
		Imath::V3f matrixAX(matrixA[0][0], matrixA[0][1], matrixA[0][2]);
		Imath::V3f matrixAY(matrixA[1][0], matrixA[1][1], matrixA[1][2]);
		Imath::V3f matrixAZ(matrixA[2][0], matrixA[2][1], matrixA[2][2]);
		
		Imath::V3f matrixBX(matrixB[0][0], matrixB[0][1], matrixB[0][2]);
		Imath::V3f matrixBY(matrixB[1][0], matrixB[1][1], matrixB[1][2]);
		Imath::V3f matrixBZ(matrixB[2][0], matrixB[2][1], matrixB[2][2]);
		
		Imath::V3f interpX = matrixAX + ((matrixBX - matrixAX) * blend);
		Imath::V3f interpY = matrixAY + ((matrixBY - matrixAY) * blend);
		Imath::V3f interpZ = matrixAZ + ((matrixBZ - matrixAZ) * blend);
		
		Imath::V3f matrixApos(matrixA[3][0], matrixA[3][1], matrixA[3][2]);
	    Imath::V3f matrixBpos(matrixB[3][0], matrixB[3][1], matrixB[3][2]);
		Imath::V3f interpPos = matrixApos + ((matrixBpos - matrixApos) * blend);
		
		interpX.normalize();
		interpY.normalize();
		interpZ.normalize();
		
		outMatrix.x[0][0] = interpX.x;
		outMatrix.x[0][1] = interpX.y;
		outMatrix.x[0][2] = interpX.z;
		outMatrix.x[0][3] = 0.0;
		
		outMatrix.x[1][0] = interpY.x;
		outMatrix.x[1][1] = interpY.y;
		outMatrix.x[1][2] = interpY.z;
		outMatrix.x[1][3] = 0.0;
		
		outMatrix.x[2][0] = interpZ.x;
		outMatrix.x[2][1] = interpZ.y;
		outMatrix.x[2][2] = interpZ.z;
		outMatrix.x[3][3] = 0.0;
		
		outMatrix.x[3][0] = interpPos.x;
		outMatrix.x[3][1] = interpPos.y;
		outMatrix.x[3][2] = interpPos.z;
		outMatrix.x[3][3] = 1.0;
	}

	void interpolateMatrixValues(const std::vector<Imath::M44f> &matrixValuesA, const std::vector<Imath::M44f> &matrixValuesB, float blend, std::vector<Imath::M44f> &outMatrixValues){
		int size = matrixValuesA.size();
		if(size != matrixValuesB.size())
			return;
		
		outMatrixValues.resize(size);
		for(int i = 0; i < size; ++i){
			interpolateMatrix(matrixValuesA[i], matrixValuesB[i], blend, outMatrixValues[i]);
		}
	}

	void parseCIOSkinWeights(const std::string &filename, std::string &version, std::string &type, std::vector<int> &vertices, std::vector<int> &deformers, std::vector<float> &weights){
		std::ifstream t;
		t.open(filename.data());
		std::string buffer;
		std::string line;
		
		int lineNumber = 0;
		while(std::getline(t, line)){
			
			if(lineNumber == 0){
				std::vector<std::string> result;
				stringUtils::split(line, result, ":");
				if(result.size() == 2){
					if(result[0] == "coralIO"){
						version = result[1];
					}
				}
			}
			else if(lineNumber == 1){
				std::vector<std::string> result;
				stringUtils::split(line, result, ":");
				if(result.size() == 2){
					if(result[0] == "type"){
						type = result[1];
					}
				}
			}
			else if(lineNumber == 2){
				//vertices
			}
			else if(lineNumber == 3){
				//deformers
			}
			else{
				if(type == "skinWeight"){
					std::vector<std::string> sep;
					stringUtils::split(line, sep, ",");
					if(sep.size() == 3){
						std::vector<std::string> vertex;
						stringUtils::split(sep[0], vertex, ":");

						std::vector<std::string> deformer;
						stringUtils::split(sep[1], deformer, ":");

						std::vector<std::string> weight;
						stringUtils::split(sep[2], weight, ":");

						if(vertex.size() == 2 && deformer.size() == 2 && weight.size() == 2){
							int vertexVal = stringUtils::parseInt(vertex[1]);
							int deformerVal = stringUtils::parseInt(deformer[1]);
							float weightVal = stringUtils::parseFloat(weight[1]);

							vertices.push_back(vertexVal);
							deformers.push_back(deformerVal);
							weights.push_back(weightVal);
						}
					}
				}
			}
			
			lineNumber ++;
		}
		t.close();
	}

}

ImportCIOTransforms::ImportCIOTransforms(const std::string &name, Node *parent): Node(name, parent){
	_file = new StringAttribute("file", this);
	_time = new NumericAttribute("time", this);
	_out = new NumericAttribute("out", this);
	
	addInputAttribute(_file);
	addInputAttribute(_time);
	addOutputAttribute(_out);
	
	setAttributeAllowedSpecialization(_time, "Float");
	setAttributeAllowedSpecialization(_out, "Matrix44Array");
	
	setAttributeAffect(_file, _out);
	setAttributeAffect(_time, _out);
}

void ImportCIOTransforms::updateSlice(Attribute *attribute, unsigned int slice){
	std::string filename = _file->value()->stringValue();
	filename = NetworkManager::resolveFilename(filename);

	Numeric *time = _time->value();
	Numeric *out = _out->outValue();
	
	if(filename != _cachedFilename){
		_cachedFilename = filename;
		_cachedMatrixValues.clear();
		
		std::string version;
		std::string type;
		int transforms = 0;
		int frames = 0;
		std::vector<Imath::M44f> matrixValues;
		std::vector<Imath::V3f> vec3Values;
		
		parseCIOTransforms(filename, version, type, transforms, frames, matrixValues, vec3Values);
		if(version == "1.0"){
			if(type == "transform"){
				cacheMatrixValues(frames, transforms, matrixValues, _cachedMatrixValues);
			}
		}
	}
	
	int cachedMatrixFrames = _cachedMatrixValues.size();
	float timeVal = time->floatValueAt(0);
	int timeIndex = int(timeVal);
	if(timeIndex < cachedMatrixFrames && timeIndex >= 0){
		float mod = fmod(timeVal, 1.0f);
		if(mod > 0.0){
			std::vector<Imath::M44f> &prevMatrixValues = _cachedMatrixValues[timeIndex];
			std::vector<Imath::M44f> &nextMatrixValues = _cachedMatrixValues[timeIndex + 1];
			
			std::vector<Imath::M44f> matrixValues;
			interpolateMatrixValues(prevMatrixValues, nextMatrixValues, mod, matrixValues);
			out->setMatrix44Values(matrixValues);
		}
		else{
			out->setMatrix44Values(_cachedMatrixValues[timeIndex]);
		}
	}
}

ImportCIOSkinWeights::ImportCIOSkinWeights(const std::string &name, Node *parent): Node(name, parent){	
	_file = new StringAttribute("file", this);
	_vertices = new NumericAttribute("vertices", this);
	_deformers = new NumericAttribute("deformers", this);
	_weights = new NumericAttribute("weights", this);

	addInputAttribute(_file);
	addOutputAttribute(_vertices);
	addOutputAttribute(_deformers);
	addOutputAttribute(_weights);

	setAttributeAllowedSpecialization(_vertices, "IntArray");
	setAttributeAllowedSpecialization(_deformers, "IntArray");
	setAttributeAllowedSpecialization(_weights, "FloatArray");

	setAttributeAffect(_file, _vertices);
	setAttributeAffect(_file, _deformers);
	setAttributeAffect(_file, _weights);
}

void ImportCIOSkinWeights::updateSlice(Attribute *attribute, unsigned int slice){
	std::string filename = _file->value()->stringValue();
	filename = NetworkManager::resolveFilename(filename);

	std::string version;
	std::string type;
	std::vector<int> vertices;
	std::vector<int> deformers;
	std::vector<float> weights;

	parseCIOSkinWeights(filename, version, type, vertices, deformers, weights);
	
	if(version == "1.0"){
		if(type == "skinWeight"){
			_vertices->outValue()->setIntValues(vertices);
			_deformers->outValue()->setIntValues(deformers);
			_weights->outValue()->setFloatValues(weights);

			setAttributeIsClean(_vertices, true);
			setAttributeIsClean(_deformers, true);
			setAttributeIsClean(_weights, true);
		}
	}
}
