#include "thought.h"


void Thought::lookAtMapConents(Mat& world)
{
	//Mat eyes = Mat::zeros(world.rows, world.cols, CV_8UC4);
	
	//Mapping
	vector<vector<MapElement>> viewedMapContents;
	for (int row = 0; row < world.rows; row++)
	{
		vector<MapElement> viewedMapContentsCol;
		for (int col = 0; col < world.cols; col++)
		{
			Vec4b worldColorPoint = (world.at<Vec4b>(Point(col, row)));
			viewedMapContentsCol.push_back(mapElementCollection.searchMapElementByColor(Vec3b(worldColorPoint[0], worldColorPoint[1], worldColorPoint[2])));
			Vec3b color = viewedMapContentsCol.back().color;
			//eyes.at<Vec4b>(Point(col, row)) = Vec4b(color[0], color[1], color[2], 255);
		}
		viewedMapContents.push_back(viewedMapContentsCol);
	}
	appendToMap(viewedMapContents);
	//drawMap(mapPiece, "piece");
	//drawMap(gridMap, "grid");

	//for (const auto& key : mapElementCollection.mapElements)
	//	cout << key.second.name << " " << key.second.mapCount << endl;

	mapElementCollection.clearCounts();

	//cv::resize(eyes, eyes, cv::Size(), 6, 6, INTER_NEAREST);
	//imshow("Eyes", eyes);


};

void Thought::appendToMap(vector<vector<MapElement>> & mapPiece)
{
	bool matchFound = false;
	const int border = 3;

	if (gridMap.size() == 0)
	{
		gridMap = mapPiece;
		return;
	}
	else
	{
		//Look for match
		int matches = 0;
		int negativeMatch = 0;
		for (int foundRow = 0; foundRow < gridMap.size() - (mapPiece.size() - border *2); foundRow++)
			for (int foundCol = 0; foundCol < gridMap.front().size() - (mapPiece.front().size() - border * 2); foundCol++)
				if (gridMap.at(foundRow).at(foundCol).name == mapPiece.at(border).at(border).name || gridMap.at(foundRow).at(foundCol).type == 1 || mapPiece.at(border).at(border).type == 1)
				{
					matches = 1;
					negativeMatch = 0;
					for (int row = 1; ((row + foundRow) < gridMap.size() && row < (mapPiece.size() - border * 2)); row++)
					{
						for (int col = 1; ((col + foundCol) < gridMap.front().size() && col < (mapPiece.front().size() - border * 2)); col++)
						{
							if (gridMap.at((row + foundRow)).at((col + foundCol)).name == mapPiece.at(row + border).at(col + border).name ||
								(gridMap.at((row + foundRow)).at((col + foundCol)).name == "WalkableA" && mapPiece.at(row + border).at(col + border).name == "WalkableB") ||
								(gridMap.at((row + foundRow)).at((col + foundCol)).name == "WalkableB" && mapPiece.at(row + border).at(col + border).name == "WalkableA") ||
								gridMap.at((row + foundRow)).at((col + foundCol)).type == 1 ||
								mapPiece.at(row + border).at(col + border).type == 1)
							{
								matches++;
							}
							else
							{
								negativeMatch++;
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
						//cout << "Merge Point: " << "(" << foundRow << "," << foundCol << ")";

						vector<vector<MapElement>> newGridMap;


						//1A
						if (foundRow < 0 && foundCol < 0)
						{
							//cout << " 1A" << endl;
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
							//cout << " 1B" << endl;
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
							//cout << " 1C" << endl;
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
							//cout << " 2A" << endl;
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
							//cout << " 2B" << endl;
							const int rowSize = gridMap.size();
							const int colSize = gridMap.front().size();
							for (int row = 0; row < rowSize; row++)
							{
								vector<MapElement> newGridMapLine;
								for (int col = 0; col < colSize; col++)
								{
									const bool pPartCod = (row >= foundRow && col >= foundCol && row < foundRow + mapPiece.size() && col < foundCol + mapPiece.front().size());
									const int pPartRow = row - foundRow;
									const int pPartCol = col - foundCol;
									const bool gPartCod = (true);
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
							//cout << " 2C" << endl;
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
							//cout << " 3A" << endl;
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
							//cout << "3B" << endl;
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
							//cout << "3C" << endl;
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

MapElement Thought::innerConditions(const bool pPartCod, const bool gPartCod, const int pPartRow, const int pPartCol, const int gPartRow, const int gPartCol, vector<vector<MapElement>>& mapPiece, vector<vector<MapElement>>& gridMap)
{
	if (pPartCod && gPartCod) //CC
	{
		if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
			return (mapPiece.at(pPartRow).at(pPartCol));

		if (gridMap.at(gPartRow).at(gPartCol).type == 0)
			return (gridMap.at(gPartRow).at(gPartCol));


		return(MapElement("Unseen", Vec3b(0, 255, 255), 2));
	}

	if (pPartCod) //P
	{
		if (mapPiece.at(pPartRow).at(pPartCol).type == 0)
			return (mapPiece.at(pPartRow).at(pPartCol));


		return(MapElement("Unseen", Vec3b(0, 255, 255), 2));
	}

	if (gPartCod) //G
	{
		if (gridMap.at(gPartRow).at(gPartCol).type == 0)
			return(gridMap.at(gPartRow).at(gPartCol));


		return(MapElement("Unseen", Vec3b(0, 255, 255), 2));
	}

	return(MapElement("Unseen", Vec3b(0, 255, 255), 2));
};

void Thought::drawMap(vector<vector<MapElement>>& map, string windowName)
{
	Mat mapImg = Mat::zeros(map.size(), map.front().size(), CV_8UC3);
	for (int row = 0; row < map.size(); row++)
		for (int col = 0; col < map.front().size(); col++)
		{
			mapImg.at<Vec3b>(Point(col, row)) = map.at(row).at(col).color;
		}
	//cv::resize(mapImg, mapImg, cv::Size(), 6, 6, INTER_NEAREST);
	imshow(windowName, mapImg);
};