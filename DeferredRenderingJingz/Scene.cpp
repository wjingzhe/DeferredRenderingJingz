#include "Scene.h"

Scene::Scene(ID3D11Device* pDevice)
{
	m_Models.push_back(new Model(pDevice));

	srand(0);
	for (int x =0;x<64;++x)
	{
		for (int y = 0;y<10;++y)
		{
			for (int z = 0;z<64;++z)
			{
				if (rand()%2 ==0)
				{
					m_Voxels[x][y][z].State = Active;
				}
				else
				{
					m_Voxels[x][y][z].State = Unactivated;
				}
			}
		}
	}

}

Scene::~Scene()
{
}
