
#include <iostream>
#include <dinput.h>
#include <conio.h>
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

using namespace std;

LPDIDEVICEINSTANCE instance;
LPDIRECTINPUTDEVICE8 joystick;
LPDIRECTINPUT8 directInput;
HRESULT hr;
DIDEVCAPS caps;
DIJOYSTATE2 joystate;
INPUT input;

HRESULT poll(DIJOYSTATE2 &joystate)
{
	HRESULT hr;

	hr = joystick->Poll();
	if (FAILED(hr))
	{
		hr = joystick->Acquire();
		while (hr == DIERR_INPUTLOST) hr = joystick->Acquire();
		if ((hr == DIERR_INVALIDPARAM) || (hr == DIERR_NOTINITIALIZED)) return E_FAIL;
	}
	if (FAILED(hr = joystick->GetDeviceState(sizeof(DIJOYSTATE2), &joystate))) return hr;

	SetCursorPos(joystate.lX, joystate.lY);

	printf("Os X : %ld\n", joystate.lX);
	printf("Os Y : %ld\n", joystate.lY);
	printf("Os Z : %ld\n", joystate.lZ);
	printf("Przyciski: ");
	for (int i = 0; i < 128; i++)
		if (joystate.rgbButtons[i] != NULL)
			printf("%2d ", i);
	if (joystate.rgbButtons[0] != NULL)
	{
		input.type = INPUT_MOUSE;
		input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		::SendInput(1, &input, sizeof(INPUT));
		::ZeroMemory(&input, sizeof(INPUT));
		input.type = INPUT_MOUSE;
		input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		::SendInput(1, &input, sizeof(INPUT));
	}

	return S_OK;
}

BOOL CALLBACK enumCallback(const DIDEVICEINSTANCE *instance, VOID* context)
{
	hr = directInput->CreateDevice(instance->guidInstance, &joystick, NULL);
	if (FAILED(hr))	return DIENUM_CONTINUE;
	return DIENUM_STOP;
}

BOOL CALLBACK enumAxeCallback(const DIDEVICEOBJECTINSTANCE *instance, VOID* context)
{
	HWND hDLG = (HWND)context;
	DIPROPRANGE propRange;
	propRange.diph.dwSize = sizeof(DIPROPRANGE);
	propRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	propRange.diph.dwHow = DIPH_BYID;
	propRange.diph.dwObj = instance->dwType;
	propRange.lMin = 0;
	propRange.lMax = +3000;
	if (FAILED(joystick->SetProperty(DIPROP_RANGE, &propRange.diph))) return DIENUM_STOP;
	return DIENUM_CONTINUE;
}


int main(int argc, char * argv[])
{

	if (FAILED(hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&directInput, NULL))) return hr;
	if (FAILED(hr = directInput->EnumDevices(DI8DEVCLASS_GAMECTRL, enumCallback, NULL, DIEDFL_ATTACHEDONLY))) return hr;
	if (joystick == NULL) {
		cout << "Joystick NIE zostal znaleziony\n";
		_getch();
		return E_FAIL;
	}
	else
		cout << "OK. Joystick zostal znaleziony\n";
	hr = joystick->SetDataFormat((DIDATAFORMAT*)&c_dfDIJoystick2);
	hr = joystick->SetCooperativeLevel(NULL, DISCL_EXCLUSIVE | DISCL_FOREGROUND);
	caps.dwSize = sizeof(DIDEVCAPS);
	hr = joystick->GetCapabilities(&caps);
	hr = joystick->EnumObjects(enumAxeCallback, NULL, DIDFT_AXIS);
	while (true) {
		system("cls");
		hr = poll(joystate);
	}

	if (joystick) joystick->Unacquire();
}
