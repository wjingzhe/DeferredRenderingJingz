#pragma once

#include <vector>

#include "Voxel.h"
#include "d3d11.h"
#include "VertexStructs.h"

class Chunk
{
public:
	Chunk();
	~Chunk();

	void Update();
	Voxel* GetVoxel(const int x, const int y, const int z);
	static const int CHUNK_SIZE = 16;

	int m_X;
	int m_y;
	int m_z;

protected:
	Voxel*** m_pVoxels;

	std::vector<VertexPositionTextureNormal> m_VertexLists;

	// Pre-allocated vertex and index arrays
	
	//void ComputeBuffers();
	//void CreateVoxels();


private:
};

