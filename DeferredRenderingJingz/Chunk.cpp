#pragma once
#include "Chunk.h"

Chunk::Chunk()
{
	m_pVoxels = new Voxel**[CHUNK_SIZE*CHUNK_SIZE*CHUNK_SIZE];
}

Chunk::~Chunk()
{
	delete[] m_pVoxels;
}

void Chunk::Update()
{
	//empty
}

Voxel * Chunk::GetVoxel(const int x, const int y, const int z)
{
	return &m_pVoxels[x][y][z];
}

