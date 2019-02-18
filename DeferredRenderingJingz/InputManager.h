#pragma once

#include <dinput.h>

class InputManager
{
public:

	static InputManager* Instance();
	void Update();
	bool IsKeyDown(int keyChar);
	void GetMouseLocation(int& mouseX, int& mouseY);
	void GetMouseMove(int& deltaMouseX, int& deltaMouseY);
	bool IsMouseDown(int keyChar);

	void SetScreenSolution(int iScreenWidth, int iScreenHeight);

protected:
private:
	InputManager(void);
	~InputManager(void);

	static InputManager* m_pInstance;

	IDirectInput8 *m_pDirectInput;
	IDirectInputDevice8 * m_pKeyboard;
	IDirectInputDevice8 * m_pMouse;

	unsigned char m_KeyboardStates[256];
	DIMOUSESTATE m_MouseStates;

	int m_iScreenWidth;
	int m_iScreenHeight;

	int m_iPosMouseX;
	int m_iPosMouseY;

	int m_iDeltaMouseX;
	int m_iDeltaMouseY;
};