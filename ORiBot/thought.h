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

			//Look for match
			int matches = 0;
			int negativeMatch = 0;
			for (int foundRow = 0; foundRow < gridMap.size() - cutMpPiece.size(); foundRow++)
				for (int foundCol = 0; foundCol < gridMap.front().size() - cutMpPiece.front().size(); foundCol++)
					if (gridMap.at(foundRow).at(foundCol).name == cutMpPiece.front().front().name || gridMap.at(foundRow).at(foundCol).type == 1 || cutMpPiece.front().front().type == 1)
					{
						matches = 1;
						negativeMatch = 0;
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
									negativeMatch++;
									//cout << "nextGrid" << endl;
									if (negativeMatch > 5)
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
							//cout << "Complete Match: (" << matches << ") from: (" << foundRow << "," << foundCol << ")" << endl;
							cout << "Merge Point: " << "(" << foundRow << "," << foundCol << ")";

							vector<vector<MapElement>> newGridMap;


							//1A
							if (foundRow < 0 && foundCol < 0)
							{
								cout << " 1A" << endl;
								const int rowSize = -foundRow + gridMap.size();
								const int colSize = -foundCol + gridMap.front().size();
								for (int row = 0; row < rowSize; row++)
								{
									vector<MapElement> newGridMapLine;
									for (int col = 0; col < colSize; col++)
									{
										const bool pPartCod = (row < mapPiece.size() && col < mapPiece.front().size());
										const int pPartRow = row;
										const int pPartCol = col;
										const bool gPartCod = (row >= -foundRow && col >= -foundCol);
										const int gPartRow = row + foundRow;
										const int gPartCol = col + foundCol;
										newGridMapLine.push_back(innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
									}
									newGridMap.push_back(newGridMapLine);
								}
								gridMap = newGridMap;
							}
							//1B
							else if (foundRow < 0 && foundCol >= 0 &&
								foundCol + mapPiece.front().size() <= gridMap.front().size())
							{
								cout << " 1B" << endl;
								const int rowSize = -foundRow + gridMap.size();
								const int colSize = gridMap.front().size();
								for (int row = 0; row < rowSize; row++)
								{
									vector<MapElement> newGridMapLine;
									for (int col = 0; col < colSize; col++)
									{
										const bool pPartCod = (row < -foundRow && col >= foundCol && col < foundCol + mapPiece.front().size());
										const int pPartRow = row;
										const int pPartCol = col - foundCol;
										const bool gPartCod = (row >= -foundRow);
										const int gPartRow = row + foundRow;
										const int gPartCol = col;
										newGridMapLine.push_back(innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
									}
									newGridMap.push_back(newGridMapLine);
								}
								gridMap = newGridMap;
							}
							//1C
							else if (foundRow < 0 && foundCol > 0 &&
								foundCol + mapPiece.front().size() >= gridMap.front().size())
							{
								cout << " 1C" << endl;
								const int rowSize = -foundRow + gridMap.size();
								const int colSize = foundCol + mapPiece.front().size();
								for (int row = 0; row < rowSize; row++)
								{
									vector<MapElement> newGridMapLine;
									for (int col = 0; col < colSize; col++)
									{
										const bool pPartCod = (row < mapPiece.size() && col >= foundCol);
										const int pPartRow = row;
										const int pPartCol = col - foundCol;
										const bool gPartCod = (row >= -foundRow && col < gridMap.front().size());
										const int gPartRow = row + foundRow;
										const int gPartCol = col;
										newGridMapLine.push_back(innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
									}
									newGridMap.push_back(newGridMapLine);
								}
								gridMap = newGridMap;
							}
							//2A
							else if (foundRow >= 0 && foundCol < 0 &&
								foundRow + mapPiece.size() <= gridMap.size())
							{
								cout << " 2A" << endl;
								const int rowSize = gridMap.size();
								const int colSize = -foundCol + gridMap.front().size();
								for (int row = 0; row < rowSize; row++)
								{
									vector<MapElement> newGridMapLine;
									for (int col = 0; col < colSize; col++)
									{
										const bool pPartCod = (row >= foundRow && col < mapPiece.front().size() && row < foundRow + mapPiece.size());
										const int pPartRow = row - foundRow;
										const int pPartCol = col;
										const bool gPartCod = (col >= -foundCol);
										const int gPartRow = row;
										const int gPartCol = col + foundCol;
										newGridMapLine.push_back(innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
									}
									newGridMap.push_back(newGridMapLine);
								}
								gridMap = newGridMap;
							}
							//2B
							else if (foundRow >= 0 && foundCol >= 0 &&
								foundRow + mapPiece.size() <= gridMap.size() &&
								foundCol + mapPiece.front().size() <= gridMap.front().size())
							{
								cout << " 2B" << endl;
								const int rowSize = gridMap.size();
								const int colSize = gridMap.front().size();
								for (int row = 0; row < rowSize; row++)
								{
									vector<MapElement> newGridMapLine;
									for (int col = 0; col < colSize; col++)
									{
										const bool pPartCod = (row >= foundRow && col >= foundCol && row < foundRow + mapPiece.size() && col < mapPiece.front().size());
										const int pPartRow = row - foundRow;
										const int pPartCol = col - foundCol;
										const bool gPartCod = (!(row >= foundRow && col >= foundCol && row < foundRow + mapPiece.size() && col < mapPiece.front().size()));
										const int gPartRow = row;
										const int gPartCol = col;
										newGridMapLine.push_back(innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
									}
									newGridMap.push_back(newGridMapLine);
								}
								gridMap = newGridMap;
							}
							//2C
							else if (foundRow >= 0, foundCol > 0 &&
								foundRow + mapPiece.size() <= gridMap.size() &&
								foundCol + mapPiece.front().size() > gridMap.front().size())
							{
							cout << " 2C" << endl;
								const int rowSize = gridMap.size();
								const int colSize = foundCol + mapPiece.front().size();
								for (int row = 0; row < rowSize; row++)
								{
									vector<MapElement> newGridMapLine;
									for (int col = 0; col < colSize; col++)
									{
										const bool pPartCod = (row >= foundRow && col >= foundCol && row < foundRow + mapPiece.size());
										const int pPartRow = row - foundRow;
										const int pPartCol = col - foundCol;
										const bool gPartCod = (col < gridMap.front().size());
										const int gPartRow = row;
										const int gPartCol = col;
										newGridMapLine.push_back(innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
									}
									newGridMap.push_back(newGridMapLine);
								}
								gridMap = newGridMap;
							}
							//3A
							else if (foundRow > 0 && foundCol < 0 &&
							foundRow + mapPiece.size() > gridMap.size())
							{
							cout << " 3A" << endl;
							const int rowSize = foundRow + mapPiece.size();
							const int colSize = -foundCol + gridMap.front().size();
							for (int row = 0; row < rowSize; row++)
							{
								vector<MapElement> newGridMapLine;
								for (int col = 0; col < colSize; col++)
								{
									const bool pPartCod = (row >= foundRow && col < mapPiece.front().size());
									const int pPartRow = row - foundRow;
									const int pPartCol = col;
									const bool gPartCod = (row < gridMap.size() && col >= -foundCol);
									const int gPartRow = row;
									const int gPartCol = col + foundCol;
									newGridMapLine.push_back(innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
								}
								newGridMap.push_back(newGridMapLine);
							}
							gridMap = newGridMap;
							}
							//3B
							else if (foundRow > 0 && foundCol >= 0 && 
							foundCol + mapPiece.front().size() <= gridMap.front().size() && 
							foundRow + mapPiece.size() > gridMap.size())
							{
							cout << "3B" << endl;
							const int rowSize = foundRow + mapPiece.size();
							const int colSize = gridMap.front().size();
							for (int row = 0; row < rowSize; row++)
							{
								vector<MapElement> newGridMapLine;
								for (int col = 0; col < colSize; col++)
								{
									const bool pPartCod = (row >= foundRow && col >= foundCol && col < foundCol + mapPiece.front().size());
									const int pPartRow = row - foundRow;
									const int pPartCol = col - foundCol;
									const bool gPartCod = (row < gridMap.size());
									const int gPartRow = row;
									const int gPartCol = col;
									newGridMapLine.push_back(innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
								}
								newGridMap.push_back(newGridMapLine);
							}
							gridMap = newGridMap;
							}
							//3C
							else if (foundRow > 0 && foundCol > 0 &&
							foundCol + mapPiece.front().size() > gridMap.front().size() &&
							foundRow + mapPiece.size() > gridMap.size())
							{
							cout << "3C" << endl;
							const int rowSize = foundRow + mapPiece.size();
							const int colSize = foundCol + mapPiece.front().size();
							for (int row = 0; row < rowSize; row++)
							{
								vector<MapElement> newGridMapLine;
								for (int col = 0; col < colSize; col++)
								{
									const bool pPartCod = (row >= foundRow && col >= foundCol);
									const int pPartRow = row - foundRow;
									const int pPartCol = col - foundCol;
									const bool gPartCod = (row < gridMap.size() && col < gridMap.front().size());
									const int gPartRow = row;
									const int gPartCol = col;
									newGridMapLine.push_back(innerConditions(pPartCod, gPartCod, pPartRow, pPartCol, gPartRow, gPartCol, mapPiece, gridMap));
								}
								newGridMap.push_back(newGridMapLine);
							}
							gridMap = newGridMap;
							}
							drawMap(gridMap, "grid");
							return;
						}
					}
		}
	};

	MapElement innerConditions(const bool pPartCod, const bool gPartCod, const int pPartRow, const int pPartCol, const int gPartRow, const int gPartCol, vector<vector<MapElement>>& mapPiece, vector<vector<MapElement>>& gridMap)
	{
		if (pPartCod && gPartCod) //CC
		{
			if (mapPiece.at(pPartRow).at(pPartRow).type != 0 && gridMap.at(gPartRow).at(gPartCol).type == 0)
				return (gridMap.at(gPartRow).at(gPartCol));
			else
				return (mapPiece.at(pPartRow).at(pPartCol));
		}
		else if (pPartCod) //P
			return (mapPiece.at(pPartRow).at(pPartCol));
		else if (gPartCod) //G
			return(gridMap.at(gPartRow).at(gPartCol));
		else
			return(MapElement("Unseen", Vec3b(0, 255, 255), 2));
	};

	void drawMap(vector<vector<MapElement>>& map, string windowName)
	{
		Mat mapImg = Mat::zeros(map.size(), map.front().size(), CV_8UC3);
		for (int row = 0; row < map.size(); row++)
			for (int col = 0; col < map.front().size(); col++)
			{
				mapImg.at<Vec3b>(Point(col, row)) = map.at(row).at(col).color;
			}
		cv::resize(mapImg, mapImg, cv::Size(), 6, 6, INTER_NEAREST);
		imshow(windowName, mapImg);
	};


};

