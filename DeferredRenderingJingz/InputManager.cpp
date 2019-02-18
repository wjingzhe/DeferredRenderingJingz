#include "InputManager.h"


InputManager* InputManager::m_pInstance = nullptr;

InputManager * InputManager::Instance()
{
	if (!m_pInstance)
	{
		m_pInstance = new InputManager();
	}
	return m_pInstance;
}

void InputManager::Update()
{
	HRESULT hr = 0;
	hr = m_pKeyboard->Acquire();//输入设备绑定必须操作，具体没看懂
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Keyboard acquisition error.", L"Error", MB_OK);
	}

	hr = m_pKeyboard->GetDeviceState(sizeof(m_KeyboardStates), (LPVOID)&m_KeyboardStates);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Keyboard state error.", L"Error", MB_OK);
	}


	hr = m_pMouse->Acquire();

	if (FAILED(hr))
		MessageBox(NULL, L"Mouse acquisition error.", L"Error", MB_OK);

	hr = m_pMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_MouseStates);
	if (FAILED(hr))
	{
		MessageBox(NULL, L"Mouse state error.", L"Error", MB_OK);
	}

	//Update the location of the mouse cursor based on the change of the mouse location during the frame.
	m_iDeltaMouseX = m_MouseStates.lX;
	m_iDeltaMouseY = m_MouseStates.lY;

	m_iPosMouseX += m_iDeltaMouseX;
	m_iPosMouseY += m_iDeltaMouseY;

	//Ensure the mouse location doesn't exceed the screen width or height.
	if (m_iPosMouseX < 0)
	{
		m_iPosMouseX = 0;
	}
	if (m_iPosMouseX > m_iScreenWidth)
	{
		m_iPosMouseX = m_iScreenWidth;
	}
	if (m_iPosMouseY < 0)
	{
		m_iPosMouseY = 0;
	}
	if (m_iPosMouseY > m_iScreenHeight)
	{
		m_iPosMouseY = m_iScreenHeight;
	}

}

bool InputManager::IsKeyDown(int keyChar)
{
	if (m_KeyboardStates[keyChar] / 128)
	{
		return true;
	}

	return false;
}

void InputManager::GetMouseLocation(int & mouseX, int & mouseY)
{
	mouseX = m_iPosMouseX;
	mouseY = m_iPosMouseY;
}

void InputManager::GetMouseMove(int & deltaMouseX, int & deltaMouseY)
{
	deltaMouseX = m_iDeltaMouseX;
	deltaMouseY = m_iDeltaMouseY;
}

bool InputManager::IsMouseDown(int keyChar)
{
	if (m_MouseStates.rgbButtons[keyChar] / 128)
		return true;

	return false;
}

void InputManager::SetScreenSolution(int iScreenWidth, int iScreenHeight)
{
	m_iScreenWidth = iScreenWidth;
	m_iScreenHeight = iScreenHeight;
}

InputManager::InputManager(void):m_pDirectInput(nullptr), m_iPosMouseX(0), m_iPosMouseY(0)
{
	m_iScreenWidth = 1280;
	m_iScreenHeight = 800;

	//不熟
	DirectInput8Create(GetModuleHandle(0), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL);
	
	//看不懂
	//KEYBOARD
	m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL);
	m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	m_pKeyboard->SetCooperativeLevel(0, DISCL_FOREGROUND | DISCL_EXCLUSIVE);

	// Mouse
	m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse, NULL);
	m_pMouse->SetDataFormat(&c_dfDIMouse);
	m_pMouse->SetCooperativeLevel(0, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
}

InputManager::~InputManager(void)
{
	delete m_pDirectInput;
	delete m_pKeyboard;
	delete m_pMouse;
}
