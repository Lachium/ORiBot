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
	InputEmulator() {};

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

	void setGlobalOffset(int pGlobalOffsetX, int pGlobalOffsetY)
	{
		globalOffsetX = pGlobalOffsetX;
		globalOffsetY = pGlobalOffsetY;
	};

	void moveCursorAndClick(Point CellPos, Point myPos)
	{
		Point posOffset = myPos - CellPos;
		SetCursorPos(CellPos.x + globalOffsetX, CellPos.y + globalOffsetY);
	};


	void followRoute(vector<Point>& route, Point myPos)
	{
		while (getDistance(myPos, route.back()) < 3)
			if (route.size() > 1)
				route.pop_back();
			else
			{
				ReleaseLeftClick();
				route.pop_back();
				return;
			}



		Point posOffset = route.back() - myPos;

		int c = 7;
		int xPos = gameWindownWidth / 2 + (posOffset.y * binHeight) + globalOffsetX;
		int yPos = gameWindownHeight / 2 + (posOffset.x * binWidth) + globalOffsetY;
		cout << posOffset << "==" << Point(xPos, yPos) << endl;
		SetCursorPos(xPos, yPos);
		PressLeftClick();
	};

	double getDistance(Point pointA, Point pointB)
	{
		return double(sqrt((pointA.x - pointB.x) * (pointA.x - pointB.x) + (pointA.y - pointB.y) * (pointA.y - pointB.y)));
	};

	void PressLeftClick()
	{
		INPUT    Input = { 0 };
		// left down 
		Input.type = INPUT_MOUSE;
		Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		::SendInput(1, &Input, sizeof(INPUT));
		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
		// left up
		//::ZeroMemory(&Input, sizeof(INPUT));
		//Input.type = INPUT_MOUSE;
		//Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		//::SendInput(1, &Input, sizeof(INPUT));
	};
	void ReleaseLeftClick()
	{
		INPUT    Input = { 0 };

		::ZeroMemory(&Input, sizeof(INPUT));
		Input.type = INPUT_MOUSE;
		Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		::SendInput(1, &Input, sizeof(INPUT));
	};
private:
	int globalOffsetX = 0;
	int globalOffsetY = 0;
};