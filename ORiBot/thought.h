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
	vector<vector<MapElement>> gridMap;
	//vector<vector<vector<MapElement>>> mapPieces;

	Thought() {};
	void printMapConents(Mat& world)
	{
		Mat eyes = Mat::zeros(world.rows, world.cols, CV_8UC4);


		//Mapping
		vector<vector<MapElement>> mapPiece;
		for (int row = 0; row < world.rows - 1; row++)
		{
			vector<MapElement> mapRow;
			for (int col = 0; col < world.cols - 1; col++)
			{
				Vec4b worldColorPoint = (world.at<Vec4b>(Point(col, row)));
				mapRow.push_back(mapElementCollection.searchMapElementByColor(Vec3b(worldColorPoint[0], worldColorPoint[1], worldColorPoint[2])));
				Vec3b color = mapRow.back().color;
				eyes.at<Vec4b>(Point(col, row)) = Vec4b(color[0], color[1], color[2], 255);
			}
			mapPiece.push_back(mapRow);
		}
		appendToMap(mapPiece);
		//drawMap(mapPiece, "piece");
		//drawMap(gridMap, "grid");

		//for (const auto& key : mapElementCollection.mapElements)
		//	cout << key.second.name << " " << key.second.mapCount << endl;

		mapElementCollection.clearCounts();

		cv::resize(eyes, eyes, cv::Size(), 10, 10, INTER_NEAREST);
		imshow("Eyes", eyes);


	};

	void appendToMap(vector<vector<MapElement>> mapPiece)
	{
		vector<vector<MapElement>> cutMpPiece;
		bool matchFound = false;
		const int border = 3;

		if (gridMap.size() == 0)
		{
			gridMap = mapPiece;
			return;
		}
		else
		{
			//Cut Piece Corners
			for (int rowPiece = border; rowPiece < mapPiece.size() - border; rowPiece++)
			{
				vector<MapElement> cutMpPieceLine;
				for (int colPiece = border; colPiece < mapPiece.front().size() - border; colPiece++)
					cutMpPieceLine.push_back(mapPiece.at(rowPiece).at(colPiece));
				cutMpPiece.push_back(cutMpPieceLine);
			}

			cutMpPiece;


			//Look for match
			int matches = 0;
			for (int foundRow = 0; foundRow < gridMap.size() - cutMpPiece.size(); foundRow++)
				for (int foundCol = 0; foundCol < gridMap.front().size() - cutMpPiece.front().size(); foundCol++)
					if (gridMap.at(foundRow).at(foundCol).name == cutMpPiece.front().front().name)
					{
						matches = 1;
						for (int row = 1; ((row + foundRow) < gridMap.size() && row < cutMpPiece.size()); row++)
						{
							for (int col = 1; ((col + foundCol) < gridMap.front().size() && col < cutMpPiece.front().size()); col++)
							{
								if (gridMap.at((row + foundRow)).at((col + foundCol)).name == cutMpPiece.at(row).at(col).name ||
									(gridMap.at((row + foundRow)).at((col + foundCol)).name == "WalkableA" && cutMpPiece.at(row).at(col).name == "WalkableB") ||
									(gridMap.at((row + foundRow)).at((col + foundCol)).name == "WalkableB" && cutMpPiece.at(row).at(col).name == "WalkableA") ||
									gridMap.at((row + foundRow)).at((col + foundCol)).type == 1 ||
									cutMpPiece.at(row).at(col).type == 1)
								{
									matches++;
									//cout << matches << endl;
									/*if (matches > 1000)
									{
										cout << "Threshold match from: (" << foundRow << "," << foundCol << ")" << endl;
										return;
									}*/
								}
								else
								{
									//cout << "nextGrid" << endl;
									goto nextGrid;
								}
							}
						}
						matchFound = true;
					nextGrid:
						//Append to Grid Map
						if (matchFound)
						{
							foundRow -= border;
							foundCol -= border;
							cout << "Complete Match: (" << matches << ") from: (" << foundRow << "," << foundCol << ")" << endl;

							cout << "Merge Point: " << "(" << foundRow << "," << foundCol << ")" << endl;

							vector<vector<MapElement>> newGridMap;
							for (int row = 0; row < foundRow + mapPiece.size(); row++)
							{
								vector<MapElement> newGridMapLine;
								for (int col = 0; col < foundCol + mapPiece.front().size(); col++)
								{
									//cout << "now: (" << row << "," << col << ")" << endl;
									if (row < gridMap.size() && col < gridMap.front().size())
									{
										//cout << "App Grid" << endl;
										newGridMapLine.push_back(gridMap.at(row).at(col));
									}
									else if (row >= foundRow && col >= foundCol)
									{
										//cout << "App mapP" << endl;
										//cout << "now: (" << row << "," << col << ")" << endl;
										//cout << "Fnow: (" << foundRow << "," << foundCol << ")" << endl;
										newGridMapLine.push_back(mapPiece.at(row - foundRow).at(col - foundCol));
									}
									else  if (row >= gridMap.size() && col < foundCol)
									{
										//cout << "App WH" << endl;
										newGridMapLine.push_back(MapElement("Unknown", Vec3b(255, 255, 255), 1));
									}
									else if (row < foundRow && col >= gridMap.front().size())
									{
										//cout << "App WH" << endl;
										newGridMapLine.push_back(MapElement("Unknown", Vec3b(255, 255, 255), 1));
									}
									else
									{
										//cout << "What now?" << endl;
										newGridMapLine.push_back(MapElement("Unknown", Vec3b(255, 255, 255), 1));
									}
								}
								newGridMap.push_back(newGridMapLine);
							}
							gridMap = newGridMap;
							drawMap(gridMap, "grid");
							return;
						}
						//cout << matches << endl;
					}
		}
		drawMap(gridMap, "grid");
	};

	void drawMap(vector<vector<MapElement>>& map, string windowName)
	{
		Mat mapImg = Mat::zeros(map.size(), map.front().size(), CV_8UC3);
		for (int row = 0; row < map.size(); row++)
			for (int col = 0; col < map.front().size(); col++)
			{
				mapImg.at<Vec3b>(Point(col, row)) = map.at(row).at(col).color;
			}
		cv::resize(mapImg, mapImg, cv::Size(), 10, 10, INTER_NEAREST);
		imshow(windowName, mapImg);
	};


};

