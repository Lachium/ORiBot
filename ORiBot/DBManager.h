#pragma once
#include <iostream>
#include <string>
#include <mysql.h>

class DBManager
{
public:
	MYSQL* conn = mysql_init(0);
	MYSQL_ROW row;
	MYSQL_RES* res;

	DBManager()
	{
		conn = mysql_real_connect(conn, "192.168.1.59", "monty", "some_pass", "spider", 3306, NULL, 0);
		//conn = mysql_real_connect(conn, "localhost", "root", "root", "spider", 3306, NULL, 0);
		if (conn)
			cout << "DB Connected\n";
		else
			cout << "No DB Connection\n";

	}

	int timeSinceLastRecording(string tabel)
	{
		string sql = "select (select NOW() - (select max(creationdate) a from  " + tabel + "))";
		const char* q = sql.c_str();
		int qstate = mysql_query(conn, q);
		if (!qstate)
		{
			res = mysql_store_result(conn);
			while (row = mysql_fetch_row(res))
				return stoi(row[0]);
		}
		else
			ORiUtils::ConsoleLog("[QUERY FAILED] -> " + sql);

		return -1;
	}

	int getMapPlayerCountStat(string mapName)
	{		
		string sql = "select playercount, NOW() - creationdate, creationdate from spider.mapstatistics where name = \"" + mapName + "\" order by creationdate desc limit 1";
		const char* q = sql.c_str();
		int qstate = mysql_query(conn, q);
		if (!qstate)
		{
			res = mysql_store_result(conn);
			while (row = mysql_fetch_row(res))
			{
				if (stoi(row[1]) > 70)
					return 0;
				else
					return stoi(row[0]);
			}

			return 0;
		}
		else
			ORiUtils::ConsoleLog("[QUERY FAILED] -> " + sql);

		return -1;
	}

	int getMapPlayerCountStatWithiTimeLimit(string mapName, int timeLimit)
	{
		int playerCount = getMapPlayerCountStat(mapName);

		int timeSinceRecording = timeSinceLastRecording("spider.mapstatistics");

		if (playerCount == -1 || timeSinceRecording == -1)
			return -1;

		if (timeSinceRecording <= timeLimit)
			return playerCount;

		return -1;
	}
};