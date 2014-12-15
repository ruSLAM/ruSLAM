#ifndef Filter_h
#define Filter_h
#include <iostream>
#include <string>
#include <cmath>
#include <fstream>
#include <stdlib.h>

using namespace std;
class Filter
{
	private:
		bool FirstFlag;
		int count;
		ofstream MapFile;
	public:
		Filter();
		~Filter();
		void SetToCord(int MapCord[][683], int data[683], int x, int y, double ang);
		void SetMap(int MapCord[][683], int Map[][5000], int Grid);
		void FilterOute(int Map[][5000], int Cluster);
		void SaveMap(int Map[][5000]);
};
#endif
