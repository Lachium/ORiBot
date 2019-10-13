#pragma once

#include<opencv2/opencv.hpp>
#include <windows.h>

using namespace std;
using namespace cv;

class InputEmulator
{
public:
	shared_ptr<Point> globalPos;
	int type;
	InputEmulator(/*shared_ptr<Point> pGlobalPos*/)
	{
		//globalPos = pGlobalPos;
	};

	void SetNumLock(BOOL bState)
	{
		BYTE keyState[256];

		GetKeyboardState((LPBYTE)& keyState);
		if ((bState && !(keyState[VK_NUMLOCK] & 1)) ||
			(!bState && (keyState[VK_NUMLOCK] & 1)))
		{
			// Simulate a key press
			keybd_event(VK_NUMLOCK,
				0x45,
				KEYEVENTF_EXTENDEDKEY | 0,
				0);

			// Simulate a key release
			keybd_event(VK_NUMLOCK,
				0x45,
				KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP,
				0);
		}
	};

private:
};