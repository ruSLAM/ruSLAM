#ifndef LIDAR_H_
#define LIDAR_H_
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>


using namespace std;

class LIDAR
{
	private:
		ifstream LidarData;
		ofstream LidarComand;
		string TurnOn;
		string Getswipe;
	public:

		LIDAR();
		~LIDAR();
		int test();
		void flushLidar();
		string getData();
		void start();
		void getSwipe();
		void Decoder(int Data[], string Data_string);  
};
#endif