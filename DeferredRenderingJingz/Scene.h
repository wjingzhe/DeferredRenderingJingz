#pragma once
#include "Model.h"
#include "Voxel.h"
#include <vector>

class Scene
{
public:
	Scene(ID3D11Device* pDevice);
	~Scene();

	std::vector<Model*> GetModels()
	{
		return m_Models;
	}

	Voxel m_Voxels[64][10][64];

private:
	std::vector<Model*> m_Models;

};
