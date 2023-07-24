#ifndef OBJLOAD_H
#define OBJLOAD_H
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>

#if defined(WIN32)
#define SSCANF sscanf_s
#elif defined(__linux__) || defined(__APPLE__)
#define SSCANF sscanf
#endif

namespace OBJFile {
	struct fileVec2 {
		float x;
		float y;
	};

	struct fileVec3 {
		float x;
		float y;
		float z;
	};

	struct fileVertex_all {
		// position
		float x;
		float y;
		float z;

		// texture coords
		float tx;
		float ty;

		// normals
		float nx;
		float ny;
		float nz;
	};

	struct fileVertex_pos {
		// position
		float x;
		float y;
		float z;
	};

	struct fileVertex_tex {
		// position
		float x;
		float y;
		float z;

		// texture coords
		float tx;
		float ty;
	};

	struct fileVertex_norm {
		// position
		float x;
		float y;
		float z;

		// normals
		float nx;
		float ny;
		float nz;
	};

	struct fileInd {
		int pos;
		int tex;
		int norm;
	};

	struct mtlMaterial {
		fileVec3 ka;
		fileVec3 kd;
		fileVec3 ks;
		float ns;
		float d;
		float tr; // = 1 - d
		float ni;
		float illum;
	};

	enum dataLayout {
		ERROR,
		POS,
		POS_TEX,
		POS_NORM,
		POS_TEX_NORM
	};

	struct loadInfo {
		dataLayout layout;
		unsigned long long floatsWritten;
	};

	class OBJLoader {
	public:
		OBJLoader();
		~OBJLoader();

		// writes data to a vector and returns its format
		loadInfo load(const char* iPath, std::vector<float>& iVector);
		loadInfo load(const char* iPath, std::vector<float>& iVector, dataLayout iDesiredLayout);

		// extract materials from mtl file
		void loadMtlFile(const char* iPath, std::vector<mtlMaterial>& iVector);
	};
}
#endif
