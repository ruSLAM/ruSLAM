#include "Filter.h"

	Filter::Filter()
	{
		FirstFlag = true;
		count = 683;
		MapFile.open("Map.txt");
	}
	Filter::~Filter()
	{
		MapFile.close();
	}
	void Filter::SetToCord(int MapCord[][683], int data[683], int x, int y, double ang)
	{
		double angle[683];

		for (int i = 0; i < 683; ++i)
		{
			angle[i] = -2.0944	+ (0.0061 * i);
		}

		for(int	i = 0 ; i < 683; i++)
		{
			 MapCord[0][i] = (data[i] * cos(angle[i] + ang + 3.1416/2) ) + x;
			 MapCord[1][i] = (data[i] * sin(angle[i] + ang + 3.1416/2) ) + y;
		}
	}
	void Filter::SetMap(int MapCord[][683], int Map[][5000], int Grid)
	{
		bool flag = false;
		int x,y;

		if(FirstFlag)
		{
			for (int i = 0; i < count; ++i)
			{
				Map[0][i] = MapCord[0][i];
				Map[1][i] = MapCord[1][i];
				Map[2][i] = 1;
			}
		}
		FirstFlag = false;
		if(!FirstFlag)
		{
				for (int j = 0; j < 683; j++)
					{
					x = MapCord[0][j];
					y = MapCord[1][j];


					for (int k = 0; k < count; k++)
					{
						if(abs(x) > 0 || abs(y) > 0)
							if (abs(x-Map[0][k]) < Grid && abs(y-Map[1][k]) < Grid)
							{
								Map[2][k]++;
								flag = true;
							}

						if(abs(x) > 100000 || abs(y) > 100000)
						{
							x = 0;
							y = 0;
						}

					}

					if(!flag)
					{
						Map[0][count] = x;
						Map[1][count] = y;
						Map[2][count] = 1;
						count++;
					}
					flag = false;
			}
		}
		//FirstFlag = false;
	}

	void Filter::FilterOut(int Map[][5000] , int Cluster)
	{
		for (int i = 0; i < count; ++i)
			{
				if (Map[2][i] < Cluster)
				 {
					Map[0][i] = 0;
					Map[1][i] = 0;
				    Map[2][i] = 0;
				 } 
			}
	}

	void Filter::SaveMap(int Map[][5000])
	{
		MapFile.close();
		MapFile.open("Map.txt");
		//system(" > Map.txt");
		for(int i = 0; i < count ; i++)
		{
			MapFile << Map[0][i] << "#";
			MapFile << Map[1][i] << "#";
			MapFile << Map[2][i] << endl;
		}
	}
