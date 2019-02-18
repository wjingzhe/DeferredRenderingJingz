#pragma once

class ScreenSolution
{
public:
	ScreenSolution(float width = 1440.0f, float height = 900.0f);
	
	float GetScreenWidth(void)
	{
		return m_fWidth;
	}

	float GetScreenHeight(void)
	{
		return m_fHeight;
	}

	float GetScreenRatio(void)
	{
		return m_fScreenRatioWidthToHeight;
	}

	void SetScreenSize(float width, float height);

protected:
private:
	float m_fWidth;
	float m_fHeight;
	float m_fScreenRatioWidthToHeight;
};