#pragma once

#include<opencv2/opencv.hpp>
#include <windows.h>
#include "oRiUtils.h"

using namespace std;
using namespace cv;

class InputEmulator
{
public:
	shared_ptr<Point> globalPos;

	void PressKey(int Key)
	{
			keybd_event(Key, 0, 0, 0);
			keybd_event(Key, 0, KEYEVENT_KEYUP, 0);
	}

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

	void moveCursorToCell(Point Cell, Point2f internalCellOffset)
	{
		Point pixPos = ORiUtils::CellPixelPosition(Cell.x + 1, Cell.y + 1);
		int xPos = pixPos.x + globalOffsetX + internalCellOffset.x;
		int yPos = pixPos.y + globalOffsetY + internalCellOffset.y;
		SetCursorPos(xPos, yPos);
	};


	void followRoute(vector<Point>& route, Point dollPoss, Point2f internalCellOffset)
	{

		if (route.size() <= 1)
		{
			route.clear(); 
			ReleaseLeftClick();
			return;
		}

		Point posOffset;

		while (route.size() > 1)
		{
			posOffset = route.back() - dollPoss;

			if (abs(posOffset.x) >= 6 || abs(posOffset.y) >= 6)
			{
				route.clear();
				ReleaseLeftClick();
				return;
			}

			if (abs(posOffset.x) < 6 && abs(posOffset.y) < 6)
				route.pop_back();
			else
				break;
		}


		posOffset = route.back() - dollPoss;
		
		Point pixPos = ORiUtils::CellPixelPositionByOffset(posOffset.x, posOffset.y);
		if (pixPos == Point(-1, -1))
		{
			route.clear();
			return;
		}

		int xPos = pixPos.x +globalOffsetX + internalCellOffset.x;
		int yPos = pixPos.y +globalOffsetY + internalCellOffset.y;

		//cout << posOffset << "==" << Point(xPos, yPos) << "==" << Point(xPos2,yPos2) <<endl;

		SetCursorPos(xPos, yPos);
		holdLeftClick();
	};

	void holdLeftClick()
	{
		INPUT    Input = { 0 };
		// left down 
		Input.type = INPUT_MOUSE;
		Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		::SendInput(1, &Input, sizeof(INPUT));
	};

	void PressLeftClick()
	{
		INPUT    Input = { 0 };
		// left down 
		Input.type = INPUT_MOUSE;
		Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		::SendInput(1, &Input, sizeof(INPUT));
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		// left up
		::ZeroMemory(&Input, sizeof(INPUT));
		Input.type = INPUT_MOUSE;
		Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		::SendInput(1, &Input, sizeof(INPUT));
	};

	void ReleaseLeftClick()
	{
		INPUT    Input = { 0 };

		::ZeroMemory(&Input, sizeof(INPUT));
		Input.type = INPUT_MOUSE;
		Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		::SendInput(1, &Input, sizeof(INPUT));
	};

	void PressRighttClick()
	{
		INPUT    Input = { 0 };
		// right down 
		Input.type = INPUT_MOUSE;
		Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
		::SendInput(1, &Input, sizeof(INPUT));
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		// right up
		::ZeroMemory(&Input, sizeof(INPUT));
		Input.type = INPUT_MOUSE;
		Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
		::SendInput(1, &Input, sizeof(INPUT));
	};
private:
	int globalOffsetX = 0;
	int globalOffsetY = 0; 
	const int KEYEVENT_KEYUP = 0x02;
};