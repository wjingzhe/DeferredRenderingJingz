#pragma once

enum VoxelState
{
	Unactivated = 0,
	Active = 1,
};

struct Voxel
{
	VoxelState State;
};