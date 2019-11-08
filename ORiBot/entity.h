#pragma once
#include<opencv2/opencv.hpp>
#include "mapElement.h"
#include "ORiUtils.h"
#include <vector>
#include <map>

using namespace std;
using namespace cv;

typedef void (*callback_function)(InputEmulator&, Point, Point2f);

class Entity {
public:
	int name;
	Point pos;

	Entity(int pName, Point pPos)
	{
		name = pName;
		pos = pPos;
	};

private:
	static auto getFunction(string pName)
	{
		return ORiUtils::draw;
	};
};

namespace  Doll_Actions
{
	static void do_nothing(InputEmulator& inputEmulator, Point CellPos, Point2f internalCellOffset) {};

	static void Teleport(InputEmulator& inputEmulator, Point CellPos, Point2f internalCellOffset)
	{
		inputEmulator.PressKey(VK_F1);
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		inputEmulator.PressKey(VK_RETURN);
		std::this_thread::sleep_for(std::chrono::milliseconds(800));
	};

	static void NormaAttack(InputEmulator& inputEmulator, Point CellPos, Point2f internalCellOffset)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(600));
		inputEmulator.moveCursorToCell(CellPos, internalCellOffset);
		inputEmulator.ReleaseLeftClick();
		inputEmulator.PressLeftClick();
	};

	static void F2_SkillCast_On_Target(InputEmulator& inputEmulator, Point CellPos, Point2f internalCellOffset)
	{
		inputEmulator.moveCursorToCell(CellPos, internalCellOffset);
		inputEmulator.ReleaseLeftClick();
		inputEmulator.PressKey(VK_F2);
		inputEmulator.PressLeftClick();
	};


	static void F2_SkillCast_On_Target_Then_Normal_Attack(InputEmulator& inputEmulator, Point CellPos, Point2f internalCellOffset)
	{
		inputEmulator.moveCursorToCell(CellPos, internalCellOffset);
		inputEmulator.ReleaseLeftClick();
		inputEmulator.PressKey(VK_F2);
		inputEmulator.PressLeftClick();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		NormaAttack(inputEmulator, CellPos, internalCellOffset);
	};

	static void F3_SkillCast_On_Target_Then_Normal_Attack(InputEmulator& inputEmulator, Point CellPos, Point2f internalCellOffset)
	{
		inputEmulator.moveCursorToCell(CellPos, internalCellOffset);
		inputEmulator.ReleaseLeftClick();
		inputEmulator.PressKey(VK_F3);
		inputEmulator.PressLeftClick();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		NormaAttack(inputEmulator, CellPos, internalCellOffset);
	};

	static void F4_SkillCast_On_Target_Then_Normal_Attack(InputEmulator& inputEmulator, Point CellPos, Point2f internalCellOffset)
	{
		inputEmulator.moveCursorToCell(CellPos, internalCellOffset);
		inputEmulator.ReleaseLeftClick();
		inputEmulator.PressKey(VK_F4);
		inputEmulator.PressLeftClick();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		NormaAttack(inputEmulator, CellPos, internalCellOffset);
	};

	static void F5_SkillCast_On_Target_Then_Normal_Attack(InputEmulator& inputEmulator, Point CellPos, Point2f internalCellOffset)
	{
		inputEmulator.moveCursorToCell(CellPos, internalCellOffset);
		inputEmulator.ReleaseLeftClick();
		inputEmulator.PressKey(VK_F5);
		inputEmulator.PressLeftClick();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		NormaAttack(inputEmulator, CellPos, internalCellOffset);
	};

	static void F6_SkillCast_On_Target_Then_Normal_Attack(InputEmulator& inputEmulator, Point CellPos, Point2f internalCellOffset)
	{
		inputEmulator.moveCursorToCell(CellPos, internalCellOffset);
		inputEmulator.ReleaseLeftClick();
		inputEmulator.PressKey(VK_F6);
		inputEmulator.PressLeftClick();
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		NormaAttack(inputEmulator, CellPos, internalCellOffset);
	};

	static void F3_SkillCast_On_Target(InputEmulator& inputEmulator, Point CellPos, Point2f internalCellOffset)
	{
		inputEmulator.moveCursorToCell(CellPos, internalCellOffset);
		inputEmulator.ReleaseLeftClick();
		inputEmulator.PressKey(VK_F3);
		inputEmulator.PressLeftClick();
	};

	static void F2_SkillCast_On_Target_TP_if_Close(InputEmulator& inputEmulator, Point CellPos, Point2f internalCellOffset)
	{
		if (ORiUtils::getDistanceBins(CellPos, ORiUtils::getDollDefaultPos()) < 2)
		{
			Doll_Actions::Teleport(inputEmulator, CellPos, internalCellOffset);
			return;
		}
		inputEmulator.moveCursorToCell(CellPos, internalCellOffset);
		inputEmulator.ReleaseLeftClick();
		inputEmulator.PressKey(VK_F2);
		inputEmulator.PressLeftClick();
	};

	static void F9_SkillCast_No_Target(InputEmulator& inputEmulator, Point CellPos, Point2f internalCellOffset)
	{
		inputEmulator.PressKey(VK_F9);
	};
	static callback_function getAction(int name)
	{
		switch (name) {
		case(Unseen):
		case(Unidentified):
		case(Shadow):
		case(Black):
		case(BlockedA):
		case(BlockedB):
		case(WalkableA):
		case(WalkableB):
		case(WalkableC):
		case(WalkableD):
		case(Water):
		{
			ORiUtils::ConsoleLog("Entity action Do_nothing: " + to_string(name));
			return Doll_Actions::do_nothing;
		}
		case(green_plant):
		case(shining_plant):
		case(yellow_plant):
		case(jakk):
			return Doll_Actions::F4_SkillCast_On_Target_Then_Normal_Attack;
		case(caramel):
			return Doll_Actions::F2_SkillCast_On_Target_Then_Normal_Attack;
		case(creamy):
			return Doll_Actions::F6_SkillCast_On_Target_Then_Normal_Attack;
		case(peco_egg):
		case(pupa):
			return Doll_Actions::F3_SkillCast_On_Target_Then_Normal_Attack;
		case(mandragora):
		case(poporing):
		case(smokie):
		case(drops):
		case(picky):
		case(picky_):
		case(poring):
		case(rocker):
			return Doll_Actions::F5_SkillCast_On_Target_Then_Normal_Attack;
		case(ant_egg):
		case(marin):
		case(piere):
		case(andre):
		case(pecopeco):
			return Doll_Actions::F3_SkillCast_On_Target_Then_Normal_Attack;
		case(bigfoot):
		case(dustiness):
			return Doll_Actions::F2_SkillCast_On_Target;
		case(sasquatch):
		case(garm_baby_MVP):
		case(vocal):
			return Doll_Actions::F2_SkillCast_On_Target_TP_if_Close;
		case(garm_MVP):
			return Doll_Actions::Teleport;
		}
		ORiUtils::ConsoleLog("Entity action unspecified: " + to_string(name));
		return Doll_Actions::Teleport;
	}

	static int getPriority(int name)
	{
		switch (name) {
		case(Unseen):
		case(Unidentified):
		case(Shadow):
		case(Black):
		case(BlockedA):
		case(BlockedB):
		case(WalkableA):
		case(WalkableB):
		case(WalkableC):
		case(WalkableD):
		case(Water):
		case(green_plant):
		case(shining_plant):
		case(yellow_plant):
		case(poring):
		case(rocker):
		case(creamy):
		case(poporing):
			return 0;
		case(mandragora):
		case(smokie):
		case(marin):
		case(peco_egg):
		case(pupa):
		case(picky):
		case(picky_):
		case(ant_egg):
		case(drops):
		case(jakk):
			return 10;
		case(andre):
		case(piere):
		case(bigfoot):
		case(dustiness):
		case(caramel):
		case(pecopeco):
			return 20;
		case(sasquatch):
			return 30;
		case(garm_baby_MVP):
			return 80;
		case(garm_MVP):
		case(vocal):
			return 100;
		}
		ORiUtils::ConsoleLog("Entity action unspecified: " + to_string(name));
		return 99;
	}

}