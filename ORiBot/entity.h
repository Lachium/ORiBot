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
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
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

	static void F4_SkillCast_On_Target(InputEmulator& inputEmulator, Point CellPos, Point2f internalCellOffset)
	{
		inputEmulator.moveCursorToCell(CellPos, internalCellOffset);
		inputEmulator.ReleaseLeftClick();
		inputEmulator.PressKey(VK_F4);
		inputEmulator.PressLeftClick();
	};

	static void F2_SkillCast_On_Target_TP_if_Close(InputEmulator& inputEmulator, Point CellPos, Point2f internalCellOffset)
	{
		if (ORiUtils::getDistanceBins(CellPos, ORiUtils::getDollDefaultPos()) < 5)
		{
			Doll_Actions::Teleport(inputEmulator, CellPos, internalCellOffset);
			return;
		}
		inputEmulator.moveCursorToCell(CellPos, internalCellOffset);
		inputEmulator.ReleaseLeftClick();
		inputEmulator.PressKey(VK_F2);
		inputEmulator.PressLeftClick();
	};

	static void F3_SkillCast_On_Target_TP_if_Close(InputEmulator& inputEmulator, Point CellPos, Point2f internalCellOffset)
	{
		if (ORiUtils::getDistanceBins(CellPos, ORiUtils::getDollDefaultPos()) < 5)
		{
			Doll_Actions::Teleport(inputEmulator, CellPos, internalCellOffset);
			return;
		}
		inputEmulator.moveCursorToCell(CellPos, internalCellOffset);
		inputEmulator.ReleaseLeftClick();
		inputEmulator.PressKey(VK_F3);
		inputEmulator.PressLeftClick();
	};

	static void F4_SkillCast_On_Target_TP_if_Close(InputEmulator& inputEmulator, Point CellPos, Point2f internalCellOffset)
	{
		if (ORiUtils::getDistanceBins(CellPos, ORiUtils::getDollDefaultPos()) < 6)
		{
			Doll_Actions::Teleport(inputEmulator, CellPos, internalCellOffset);
			return;
		}
		inputEmulator.moveCursorToCell(CellPos, internalCellOffset);
		inputEmulator.ReleaseLeftClick();
		inputEmulator.PressKey(VK_F4);
		inputEmulator.PressLeftClick();
	};

	static void F5_SkillCast_On_Target_TP_if_Close(InputEmulator& inputEmulator, Point CellPos, Point2f internalCellOffset)
	{
		if (ORiUtils::getDistanceBins(CellPos, ORiUtils::getDollDefaultPos()) < 5)
		{
			Doll_Actions::Teleport(inputEmulator, CellPos, internalCellOffset);
			return;
		}
		inputEmulator.moveCursorToCell(CellPos, internalCellOffset);
		inputEmulator.ReleaseLeftClick();
		inputEmulator.PressKey(VK_F5);
		inputEmulator.PressLeftClick();
	};

	static void F6_SkillCast_On_Target_TP_if_Close(InputEmulator& inputEmulator, Point CellPos, Point2f internalCellOffset)
	{
		cout << ORiUtils::getDistanceBins(CellPos, ORiUtils::getDollDefaultPos());
		if (ORiUtils::getDistanceBins(CellPos, ORiUtils::getDollDefaultPos()) < 5)
		{
			Doll_Actions::Teleport(inputEmulator, CellPos, internalCellOffset);
			return;
		}
		inputEmulator.moveCursorToCell(CellPos, internalCellOffset);
		inputEmulator.ReleaseLeftClick();
		inputEmulator.PressKey(VK_F6);
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
			return Doll_Actions::F4_SkillCast_On_Target_Then_Normal_Attack;
		case(eggyra):
		case(elder_wilow):
		case(poporing):
		case(kobold_archer):
			return Doll_Actions::F2_SkillCast_On_Target;
		case(bigfoot):
			return Doll_Actions::F3_SkillCast_On_Target;
		case(caramel):
			return Doll_Actions::F2_SkillCast_On_Target_Then_Normal_Attack;
		case(wild_rose):
		case(dustiness):
		case(jakk):
			return Doll_Actions::F4_SkillCast_On_Target;
		case(creamy):
		case(peco_egg):
		case(pupa):
		case(horn):
		case(stainer):
			return Doll_Actions::F3_SkillCast_On_Target_Then_Normal_Attack;
		case(mandragora):
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
		case(porcelio):
			return Doll_Actions::F2_SkillCast_On_Target;
		case(flora):
			return Doll_Actions::F3_SkillCast_On_Target_TP_if_Close;
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
		case(rocker):
		case(horn):
		case(stainer):
			return 0;
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
		case(poring):
		case(eggyra):
		case(bigfoot):
		case(elder_wilow):
			return 0;
		case(jakk):
			return 5;
		case(dustiness):
		case(wild_rose):
			return 0;
		case(kobold_archer):
			return 0;
		case(porcelio):
		case(creamy):
			return 0;
		case(flora):
			return 0;
		case(andre):
		case(piere):
		case(caramel):
		case(pecopeco):
			return 0;
		case(sasquatch):
			return 0;
		case(garm_baby_MVP):
			return 0;
		case(garm_MVP):
		case(vocal):
			return 0;
		}
		ORiUtils::ConsoleLog("Entity action unspecified: " + to_string(name));
		return 0;
	}

}