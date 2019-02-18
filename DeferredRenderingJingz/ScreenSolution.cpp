#include "ScreenSolution.h"

ScreenSolution::ScreenSolution(float width, float height)
{
	SetScreenSize(width, height);
}

void ScreenSolution::SetScreenSize(float width, float height)
{
	m_fWidth = width;
	m_fHeight = height;

	m_fScreenRatioWidthToHeight = m_fWidth / m_fHeight;
}
