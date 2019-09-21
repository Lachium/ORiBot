#pragma once
#include<opencv2/opencv.hpp>
#include<string>
#include <iostream>
#include "mob.h"

using namespace std;
using namespace cv;

class Thought
{
	MapElementCollection mapElementCollection;
public:
	Thought() {};
	void printMapConents(Mat & world)
	{
		Mat eyes = Mat::zeros(world.rows, world.cols, CV_8UC4);


		for(int row= 0; row < world.rows-1; row++)
			for (int col = 0; col < world.cols - 1; col++)
			{
				Vec4b worldColorPoint = (world.at<Vec4b>(Point(col, row)));
				//cout << tileLists.getTileNameByColor(Vec3b(worldColorPoint[0], worldColorPoint[1], worldColorPoint[2])) << endl;
				Vec3b color = mapElementCollection.searchMapElementByColor(Vec3b(worldColorPoint[0], worldColorPoint[1], worldColorPoint[2])).color;
					eyes.at<Vec4b>(Point(col, row)) = Vec4b(color[0], color[1], color[2], 255);
			}


		cout << "\x1B[2J\x1B[H";
		for (const auto& key : mapElementCollection.mapElements)
			cout << key.second.name << " " << key.second.mapCount << endl;

		mapElementCollection.clearCounts();

		cv::resize(eyes, eyes, cv::Size(), 10, 10, INTER_NEAREST);
		imshow("Eyes", eyes);
	};
};

