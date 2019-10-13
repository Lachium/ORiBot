#pragma once

using namespace std;

class navagation
{
public:
	shared_ptr<Point> globalPos;
	int type;
	navagation(shared_ptr<Point> pGlobalPos)
	{
		globalPos = pGlobalPos;

	};

private:
};