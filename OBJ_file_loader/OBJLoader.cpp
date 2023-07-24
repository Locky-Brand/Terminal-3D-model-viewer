#include "OBJLoader.h"

namespace OBJFile {
	OBJLoader::OBJLoader() {

	}
	OBJLoader::~OBJLoader() {

	}

	loadInfo OBJLoader::load(const char* iPath, std::vector<float>& iVector) {
		std::ifstream file(iPath, std::ios_base::binary);
		if (file.is_open()) {
			std::vector<fileVec3> verts;
			std::vector<fileVec3> norms;
			std::vector<fileVec2> tex;
			std::vector<fileInd> inds;
			std::vector<fileInd> assembledInds;

			bool hasTex = false;
			bool hasNorm = false;

			// extraction of data
			std::string currentLine;
			while (!file.eof()) {
				std::getline(file, currentLine);
				switch (currentLine[0]) {
				case 'v':
				{
					switch (currentLine[1]) {
					case 'n':
					{
						fileVec3 vec;
						SSCANF(currentLine.c_str(), "vn %f %f %f\n", &vec.x, &vec.y, &vec.z);
						norms.push_back(vec);
					}
					break;
					case 't':
					{
						fileVec2 vec;
						SSCANF(currentLine.c_str(), "vt %f %f\n", &vec.x, &vec.y);
						tex.push_back(vec);
					}
					break;
					default:
					{
						fileVec3 vec;
						SSCANF(currentLine.c_str(), "v %f %f %f\n", &vec.x, &vec.y, &vec.z);
						verts.push_back(vec);
					}
					break;
					}
				}
				break;
				case 'f':
				{
					fileInd ind = { 0, 0, 0 };
					unsigned int slashCount = 0;
					currentLine.append(1, '\n');
					std::string indStr = "";
					for (auto i = 2; i < currentLine.size(); ++i) {
						if (currentLine[i] == '/') {
							switch (slashCount)
							{
							case 0:
								ind.pos = atoi(indStr.c_str());
								indStr.clear();
								if (currentLine[i + 1] != '/') {
									hasTex = true;
								}
								break;
							case 1:
								ind.tex = atoi(indStr.c_str());
								indStr.clear();
								hasNorm = true;
								break;
							default:
								break;
							}
							++slashCount;
						}
						else if (currentLine[i] == ' ') {
							ind.norm = atoi(indStr.c_str());
							indStr.clear();
							inds.push_back(ind);
							slashCount = 0;
						}
						else if (currentLine[i] == '\n') {
							ind.norm = atoi(indStr.c_str());
							indStr.clear();
							inds.push_back(ind);
							slashCount = 0;

							// triangulate all file inds in struct form
							fileInd pinInd = inds[0];
							for (int n = 2; n < inds.size(); ++n) {
								assembledInds.push_back(pinInd);
								assembledInds.push_back(inds[n - 1]);
								assembledInds.push_back(inds[n]);
							}
							inds.clear();
						}
						else if (((currentLine[i] >= '0') && (currentLine[i] <= '9')) || (currentLine[i] == '-')) {
							indStr.append(1, currentLine[i]);
						}
					}
				}
				break;
				}
			}
			file.close();

			// triangulation and assembly of data
			if (hasTex && hasNorm) {
				std::vector <fileVertex_all> assembledVerts;

				// get all verticies assembled
				for (auto& i : assembledInds) {
					assembledVerts.push_back({ verts[i.pos - 1].x, verts[i.pos - 1].y, verts[i.pos - 1].z, tex[i.tex - 1].x, tex[i.tex - 1].y, norms[i.norm - 1].x, norms[i.norm - 1].y, norms[i.norm - 1].z });
				}

				// push all floats into the input buffer
				for (auto i = 0; i < assembledVerts.size(); ++i) {
					iVector.push_back(assembledVerts[i].x);
					iVector.push_back(assembledVerts[i].y);
					iVector.push_back(assembledVerts[i].z);

					iVector.push_back(assembledVerts[i].tx);
					iVector.push_back(assembledVerts[i].ty);

					iVector.push_back(assembledVerts[i].nx);
					iVector.push_back(assembledVerts[i].ny);
					iVector.push_back(assembledVerts[i].nz);
				}

				return { dataLayout::POS_TEX_NORM, iVector.size() };
			}
			else if (hasTex && !hasNorm) {
				std::vector <fileVertex_tex> assembledVerts;

				for (auto& i : assembledInds) {
					assembledVerts.push_back({ verts[i.pos - 1].x, verts[i.pos - 1].y, verts[i.pos - 1].z, tex[i.tex - 1].x, tex[i.tex - 1].y });
				}

				for (auto i = 0; i < assembledVerts.size(); ++i) {
					iVector.push_back(assembledVerts[i].x);
					iVector.push_back(assembledVerts[i].y);
					iVector.push_back(assembledVerts[i].z);

					iVector.push_back(assembledVerts[i].tx);
					iVector.push_back(assembledVerts[i].ty);
				}

				return { dataLayout::POS_TEX, iVector.size() };
			}
			else if (!hasTex && hasNorm) {
				std::vector <fileVertex_norm> assembledVerts;

				for (auto& i : assembledInds) {
					assembledVerts.push_back({ verts[i.pos - 1].x, verts[i.pos - 1].y, verts[i.pos - 1].z, norms[i.norm - 1].x, norms[i.norm - 1].y, norms[i.norm - 1].z });
				}

				for (auto i = 0; i < assembledVerts.size(); ++i) {
					iVector.push_back(assembledVerts[i].x);
					iVector.push_back(assembledVerts[i].y);
					iVector.push_back(assembledVerts[i].z);

					iVector.push_back(assembledVerts[i].nx);
					iVector.push_back(assembledVerts[i].ny);
					iVector.push_back(assembledVerts[i].nz);
				}

				return { dataLayout::POS_NORM, iVector.size() };
			}
			else {
				std::vector <fileVertex_pos> assembledVerts;

				for (auto& i : assembledInds) {
					assembledVerts.push_back({ verts[i.pos - 1].x, verts[i.pos - 1].y, verts[i.pos - 1].z });
				}

				for (auto i = 0; i < assembledVerts.size(); ++i) {
					iVector.push_back(assembledVerts[i].x);
					iVector.push_back(assembledVerts[i].y);
					iVector.push_back(assembledVerts[i].z);
				}

				return { dataLayout::POS, iVector.size() };
			}
		}
		else {
			std::cout << "error: file path not found" << '\n';
			return { dataLayout::ERROR, 0 };
		}
	}
	loadInfo OBJLoader::load(const char* iPath, std::vector<float>& iVector, dataLayout iDesiredLayout){
		std::ifstream file(iPath, std::ios_base::binary);
		if (file.is_open()) {
			std::vector<fileVec3> verts;
			std::vector<fileVec3> norms;
			std::vector<fileVec2> tex;
			std::vector<fileInd> inds;
			std::vector<fileInd> assembledInds;

			bool hasTex = false;
			bool hasNorm = false;

			bool wantTex = (iDesiredLayout == POS_TEX) || (iDesiredLayout == POS_TEX_NORM);
			bool wantNorm = (iDesiredLayout == POS_NORM) || (iDesiredLayout == POS_TEX_NORM);

			// extraction of data
			std::string currentLine;
			while (!file.eof()) {
				std::getline(file, currentLine);
				switch (currentLine[0]) {
				case 'v':
				{
					switch (currentLine[1]) {
					case 'n':
					{
						if(wantNorm){
							fileVec3 vec;
							SSCANF(currentLine.c_str(), "vn %f %f %f\n", &vec.x, &vec.y, &vec.z);
							norms.push_back(vec);
						}
					}
					break;
					case 't':
					{
						if(wantTex){
							fileVec2 vec;
							SSCANF(currentLine.c_str(), "vt %f %f\n", &vec.x, &vec.y);
							tex.push_back(vec);
						}
					}
					break;
					default:
					{
						fileVec3 vec;
						SSCANF(currentLine.c_str(), "v %f %f %f\n", &vec.x, &vec.y, &vec.z);
						verts.push_back(vec);
					}
					break;
					}
				}
				break;
				case 'f':
				{
					fileInd ind = { 0, 0, 0 };
					unsigned int slashCount = 0;
					currentLine.append(1, '\n');
					std::string indStr = "";
					for (auto i = 2; i < currentLine.size(); ++i) {
						if (currentLine[i] == '/') {
							switch (slashCount)
							{
							case 0:
								ind.pos = atoi(indStr.c_str());
								indStr.clear();
								if (currentLine[i + 1] != '/') {
									hasTex = true && wantTex;
								}
								break;
							case 1:
								ind.tex = atoi(indStr.c_str());
								indStr.clear();
								hasNorm = true && wantNorm;
								break;
							default:
								break;
							}
							++slashCount;
						}
						else if (currentLine[i] == ' ') {
							ind.norm = atoi(indStr.c_str());
							indStr.clear();
							inds.push_back(ind);
							slashCount = 0;
						}
						else if (currentLine[i] == '\n') {
							ind.norm = atoi(indStr.c_str());
							indStr.clear();
							inds.push_back(ind);
							slashCount = 0;

							// triangulate all file inds in struct form
							fileInd pinInd = inds[0];
							for (int n = 2; n < inds.size(); ++n) {
								assembledInds.push_back(pinInd);
								assembledInds.push_back(inds[n - 1]);
								assembledInds.push_back(inds[n]);
							}
							inds.clear();
						}
						else if (((currentLine[i] >= '0') && (currentLine[i] <= '9')) || (currentLine[i] == '-')) {
							indStr.append(1, currentLine[i]);
						}
					}
				}
				break;
				}
			}
			file.close();

			// triangulation and assembly of data
			if (hasTex && hasNorm) {
				std::vector <fileVertex_all> assembledVerts;

				// get all verticies assembled
				for (auto& i : assembledInds) {
					assembledVerts.push_back({ verts[i.pos - 1].x, verts[i.pos - 1].y, verts[i.pos - 1].z, tex[i.tex - 1].x, tex[i.tex - 1].y, norms[i.norm - 1].x, norms[i.norm - 1].y, norms[i.norm - 1].z });
				}

				// push all floats into the input buffer
				for (auto i = 0; i < assembledVerts.size(); ++i) {
					iVector.push_back(assembledVerts[i].x);
					iVector.push_back(assembledVerts[i].y);
					iVector.push_back(assembledVerts[i].z);

					iVector.push_back(assembledVerts[i].tx);
					iVector.push_back(assembledVerts[i].ty);

					iVector.push_back(assembledVerts[i].nx);
					iVector.push_back(assembledVerts[i].ny);
					iVector.push_back(assembledVerts[i].nz);
				}

				return { dataLayout::POS_TEX_NORM, iVector.size() };
			}
			else if (hasTex && !hasNorm) {
				std::vector <fileVertex_tex> assembledVerts;

				for (auto& i : assembledInds) {
					assembledVerts.push_back({ verts[i.pos - 1].x, verts[i.pos - 1].y, verts[i.pos - 1].z, tex[i.tex - 1].x, tex[i.tex - 1].y });
				}

				for (auto i = 0; i < assembledVerts.size(); ++i) {
					iVector.push_back(assembledVerts[i].x);
					iVector.push_back(assembledVerts[i].y);
					iVector.push_back(assembledVerts[i].z);

					iVector.push_back(assembledVerts[i].tx);
					iVector.push_back(assembledVerts[i].ty);
				}

				return { dataLayout::POS_TEX, iVector.size() };
			}
			else if (!hasTex && hasNorm) {
				std::vector <fileVertex_norm> assembledVerts;

				for (auto& i : assembledInds) {
					assembledVerts.push_back({ verts[i.pos - 1].x, verts[i.pos - 1].y, verts[i.pos - 1].z, norms[i.norm - 1].x, norms[i.norm - 1].y, norms[i.norm - 1].z });
				}

				for (auto i = 0; i < assembledVerts.size(); ++i) {
					iVector.push_back(assembledVerts[i].x);
					iVector.push_back(assembledVerts[i].y);
					iVector.push_back(assembledVerts[i].z);

					iVector.push_back(assembledVerts[i].nx);
					iVector.push_back(assembledVerts[i].ny);
					iVector.push_back(assembledVerts[i].nz);
				}

				return { dataLayout::POS_NORM, iVector.size() };
			}
			else {
				std::vector <fileVertex_pos> assembledVerts;

				for (auto& i : assembledInds) {
					assembledVerts.push_back({ verts[i.pos - 1].x, verts[i.pos - 1].y, verts[i.pos - 1].z });
				}

				for (auto i = 0; i < assembledVerts.size(); ++i) {
					iVector.push_back(assembledVerts[i].x);
					iVector.push_back(assembledVerts[i].y);
					iVector.push_back(assembledVerts[i].z);
				}

				return { dataLayout::POS, iVector.size() };
			}
		}
		else {
			std::cout << "error: file path not found" << '\n';
			return { dataLayout::ERROR, 0 };
		}
	}
	void OBJLoader::loadMtlFile(const char* iPath, std::vector<mtlMaterial>& iVector) {
		bool clear = false;
		std::ifstream mtlFile(iPath, std::ios_base::binary);
		if (mtlFile.is_open()) {
			std::string currentLine;
			while (!mtlFile.eof()) {
				std::getline(mtlFile, currentLine);

				mtlMaterial newMat;
				
				std::string prop = currentLine.substr(0, 2);
				switch (prop[0]) {
				case 'K':
				{
					std::string vecString = currentLine.substr(3, currentLine.size());
					fileVec3 vecs;
					SSCANF(vecString.c_str(), "%f %f %f\n", &vecs.x, &vecs.y, &vecs.z);
					switch (prop[1]) {
					case 'a':
						newMat.ka = vecs;
						break;
					case 'd':
						newMat.kd = vecs;
						break;
					case 's':
						newMat.ks = vecs;
						break;
					}
				}
					break;
				case 'N':
				{
					std::string vecString = currentLine.substr(3, currentLine.size());
					float num;
					SSCANF(vecString.c_str(), "%f\n", &num);
					switch (prop[1]) {
					case 's':
						newMat.ns = num;
						break;
					case 'i':
						newMat.ni = num;
						break;
					}
					break;
				}
				case 'd':
				{
					std::string vecString = currentLine.substr(2, currentLine.size());
					SSCANF(vecString.c_str(), "%f\n", &newMat.d);
				}
					break;
				case 'i':
				{
					std::string vecString = currentLine.substr(6, currentLine.size());
					SSCANF(vecString.c_str(), "%f\n", &newMat.illum);
				}
					break;
				}
				if (currentLine == "") {
					if (clear) {
						iVector.push_back(newMat);
					}
					clear = true;
				}
			}

			mtlFile.close();
		}
		else {
			std::cout << "error: mtlFile not found" << '\n';
		}
	}
}
