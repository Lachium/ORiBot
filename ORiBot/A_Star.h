
#include <iostream>
#include <iomanip>
#include <queue>
#include <string>
#include <math.h>
#include <ctime>


using namespace std;

class node
{
	int dx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	int dy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
	// current position
	int xPos;
	int yPos;
	// total distance already travelled to reach the node
	int level;
	// priority=level+remaining distance estimate
	int priority;  // smaller: higher priority

public:
	node(int xp, int yp, int d, int p)
	{
		xPos = xp; yPos = yp; level = d; priority = p;
	}

	int getxPos() const { return xPos; }
	int getyPos() const { return yPos; }
	int getLevel() const { return level; }
	int getPriority() const { return priority; }

	void updatePriority(const int& xDest, const int& yDest)
	{
		priority = level + estimate(xDest, yDest) * 10; //A*
	}

	// give better priority to going strait instead of diagonally
	void nextLevel(const int& i) // i: direction
	{
		level += (8 == 8 ? (i % 2 == 0 ? 10 : 14) : 10);
	}

	// Estimation function for the remaining distance to the goal.
	const int& estimate(const int& xDest, const int& yDest) const
	{
		static int xd, yd, d;
		xd = xDest - xPos;
		yd = yDest - yPos;

		// Euclidian Distance
		d = static_cast<int>(sqrt(xd * xd + yd * yd));

		// Manhattan distance
		//d=abs(xd)+abs(yd);

		// Chebyshev distance
		//d=max(abs(xd), abs(yd));

		return(d);
	}
};

// Determine priority (in the priority queue)
bool operator<(const node& a, const node& b)
{
	return a.getPriority() > b.getPriority();
}

class A_Star
{
	int dx[8] = { 1, 1, 0, -1, -1, -1, 0, 1 };
	int dy[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };

public:
	A_Star()
	{
	};

	vector<Point> doPathdinding(int rowA, int colA, int rowB, int colB, const deque<deque<MapTile>>& gridMap)
	{
		vector<vector<int>> mapArr;
		srand(time(NULL));

		// create map
		for (size_t row = 0; row < gridMap.size(); row++)
		{
			vector<int> mapArrLine;
			for (size_t col = 0; col < gridMap.at(row).size(); col++)
			{
				if (gridMap.at(row).at(col).mapElement->type == 1)
					mapArrLine.push_back(0);
				else
					mapArrLine.push_back(1);
			}
			mapArr.push_back(mapArrLine);
		}

		return pathFind(colA, rowA, colB, rowB, mapArr);
	};

	// A-star algorithm.
	// The route returned is a string of direction digits.
	vector<Point> pathFind(const int& xStart, const int& yStart,
		const int& xFinish, const int& yFinish,
		vector<vector<int>> & mapArr)
	{
		vector<Point> foundPath;
		static priority_queue<node> pq[2]; // list of open (not-yet-tried) nodes
		static int pqi; // pq index
		static node* n0;
		static node* m0;
		static int i, j, x, y, xdx, ydy;
		static char c;
		pqi = 0;
		
		vector<vector<int>> closed_nodes_map(mapArr.size(), std::vector<int>(mapArr.front().size()));
		vector<vector<int>> open_nodes_map(mapArr.size(), std::vector<int>(mapArr.front().size()));
		vector<vector<int>> dir_map(mapArr.size(), std::vector<int>(mapArr.front().size()));
		// reset the node maps

		// create the start node and push into list of open nodes
		n0 = new node(xStart, yStart, 0, 0);
		n0->updatePriority(xFinish, yFinish);
		pq[pqi].push(*n0);
		open_nodes_map[y][x] = n0->getPriority(); // mark it on the open nodes map

		// A* search
		while (!pq[pqi].empty())
		{
			// get the current node w/ the highest priority
			// from the list of open nodes
			n0 = new node(pq[pqi].top().getxPos(), pq[pqi].top().getyPos(),
				pq[pqi].top().getLevel(), pq[pqi].top().getPriority());

			x = n0->getxPos(); y = n0->getyPos();

			pq[pqi].pop(); // remove the node from the open list
			open_nodes_map[y][x] = 0;
			// mark it on the closed nodes map
			closed_nodes_map[y][x] = 1;

			// quit searching when the goal state is reached
			//if((*n0).estimate(xFinish, yFinish) == 0)
			if (x == xFinish && y == yFinish)
			{
				// generate the path from finish to start
				// by following the directions
				while (!(x == xStart && y == yStart))
				{
					j = dir_map[y][x];
					foundPath.emplace_back(Point(y,x));
					x += dx[j];
					y += dy[j];
				}

				// garbage collection
				delete n0;
				// empty the leftover nodes
				while (!pq[pqi].empty()) pq[pqi].pop();
				return foundPath;
			}

			// generate moves (child nodes) in all possible directions
			for (i = 0; i < 8; i++)
			{
				xdx = x + dx[i]; ydy = y + dy[i];

				if (!(xdx<0 || xdx>=mapArr.front().size() -1 || ydy<0 || ydy>=mapArr.size() -1 || mapArr[ydy][xdx] == 1
					|| closed_nodes_map[ydy][xdx] == 1))
				{
					// generate a child node
					m0 = new node(xdx, ydy, n0->getLevel(),
						n0->getPriority());
					m0->nextLevel(i);
					m0->updatePriority(xFinish, yFinish);

					// if it is not in the open list then add into that
					if (open_nodes_map[ydy][xdx] == 0)
					{
						open_nodes_map[ydy][xdx] = m0->getPriority();
						pq[pqi].push(*m0);
						// mark its parent node direction
						dir_map[ydy][xdx] = (i + 8 / 2) % 8;
					}
					else if (open_nodes_map[ydy][xdx] > m0->getPriority())
					{
						// update the priority info
						open_nodes_map[ydy][xdx] = m0->getPriority();
						// update the parent direction info
						dir_map[ydy][xdx] = (i + 8 / 2) % 8;

						// replace the node
						// by emptying one pq to the other one
						// except the node to be replaced will be ignored
						// and the new node will be pushed in instead
						if (pq[pqi].size() > 0)
						{
							while (pq[pqi].size() > 0 && !(pq[pqi].top().getxPos() == xdx && pq[pqi].top().getyPos() == ydy))
							{
								pq[1 - pqi].push(pq[pqi].top());
								pq[pqi].pop();
								if (pq[pqi].size() <= 0)
									break;
							}
							if (pq[pqi].size() > 0)
								pq[pqi].pop(); // remove the wanted node
						}
						// empty the larger size pq to the smaller one
						if (pq[pqi].size() > pq[1 - pqi].size()) pqi = 1 - pqi;
						while (!pq[pqi].empty())
						{
							pq[1 - pqi].push(pq[pqi].top());
							pq[pqi].pop();
						}
						pqi = 1 - pqi;
						pq[pqi].push(*m0); // add the better node instead
					}
					else delete m0; // garbage collection
				}
			}
			delete n0; // garbage collection
		}
		return foundPath; // no route found
	};
};