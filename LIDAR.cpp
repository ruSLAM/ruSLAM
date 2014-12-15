#include "LIDAR.h"
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <stdlib.h>
#include <time.h>

using namespace std;


	LIDAR::LIDAR()
	{
		TurnOn = "BM";
		Getswipe = "GD0044072500";
		LidarData.open("/dev/ttyACM0");
		LidarComand.open("/dev/ttyACM0");
	}
	LIDAR::~LIDAR()
	{
		LidarData.close();
		LidarComand.close();
	}



	void LIDAR::flushLidar()
	{
		string DataString = "";
		int lines = 0;

		while (LidarData.good())
		{ 								// flush first sweep it's often not good

			getline(LidarData,DataString);
			cout << DataString << endl;

			if (DataString == "" && lines > 5)			// Getline goes weird when the last line is empty
				break;

			lines++;
			DataString = "";

		}
		// test lidar
		if (lines > 5)
			cout << "Lidar OK" << endl;


	}
	
	string LIDAR::getData()
	{

		int lines = 0;
		string DataString = "";
		string DataColector = "";

		while ( LidarData.good() )
	{
		getline(LidarData,DataString);

		if (DataString == "" && lines > 5)				// Getline goes weird when the last line is empty
			break;

		if (DataString.length() > 64)
			DataString = DataString.substr(0,64); 			// start at 0, length 64 cut the checksum off

		if (lines > 2)	 						// first 3 lines are echo and checksums
			DataColector += DataString; 				// collect the lines in to one variable for decoding

		lines++;
		DataString = "";

		}
		return DataColector;
	}
	void LIDAR::Decoder(int Data[], string Data_string)
	{
		// This is a decoder function that decods the serial data in to lengths


		int tmp=0;
		int subt = 0x30;  //Subtractor: subtracts 0x30 from the ascii value of serial data. They are shifted to get printable characters
		int mesure [2100];

		for (int i = 0; i< Data_string.length() ;i++)
		{ //shift the asciis
			mesure[i] = Data_string[i] - subt;
		}

		for (int i = 0; i< 682 ;i++)
		{	//3 car encoding takes 3 pyts and pusels the to gether by shifting the first 12 to whrigt  next 6 top rhigt and then ands all of the to gether
			Data[i] = (mesure[tmp] << 12) | (mesure[tmp+1] << 6) | mesure[tmp+2];
			tmp+=3;
		}


	}
void LIDAR::start()
{
		LidarComand << TurnOn << endl; 						// Start the Lidar

}
void LIDAR::getSwipe()
{
		LidarComand << Getswipe << endl; 	
}









