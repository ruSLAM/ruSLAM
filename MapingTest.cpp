#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "LIDAR.h"
#include "ArduinoCom.h"
#include "LIDAR.cpp"
#include "ArduinoCom.cpp"
#include <time.h>
#include "Filter.cpp"
#include "Filter.h"

using namespace std;


int main()
{	int turnCounter = 0 ,turnTime = 0;
	bool Yflag = true;
	ofstream DataLog("Datalog.txt");
	int Xpos = 0,Ypos = 0, Data[682];
	double Angle, LastAngle = 0;
	int Map[3][5000],Cord[2][683];
	string Pos,DataString, e;
	int LastLengthX = 0,LastLengthY = 0, SwipeCounter = 0;
	ArduinoCom Rumba;
	LIDAR Radar;
	Filter Filter;

	Radar.start();
	Radar.getSwipe();
	Radar.flushLidar();

	Rumba.Boot();
	Rumba.DriveForward();

	e = "";
	while(abs(Ypos) + abs(Xpos) < 5000)
	{
		if(abs(Ypos) > 2000 && turnCounter == 0)
		{
			turnTime = 0;
			Rumba.TurnLeft(90);
			turnCounter++;
			e = "";
		}

		if(turnCounter == 1 && turnTime == 10)
		{
			Rumba.DriveForward();
			turnCounter++;
		}
		/*
		if(abs(Xpos) > 500 && turnCounter == 2)
		{
			turnTime = 0;
			Rumba.TurnRight(90);
			turnCounter++;
		}

		if(turnCounter == 3 && turnTime == 10)
		{
			Rumba.DriveBack();
			turnCounter++;
		}
		*/
		DataString = "";

		Pos = Rumba.GetPos();
		Xpos = Rumba.Xpos();
		Ypos = Rumba.Ypos();
		Angle = Rumba.angle();

		if(-LastLengthX + abs(Xpos) > 100 || -LastLengthY + abs(Ypos) > 100 || Yflag)
		{
			Radar.getSwipe();
			DataString = Radar.getData();
			Radar.Decoder(Data,DataString);

			//Filter.SetToCord(Cord,Data,Xpos,Ypos,Angle);
			//Filter.SetMap(Cord,Map,25);
			//Filter.SaveMap(Map);
			DataLog << "New Data" + e << endl;
			DataLog << Xpos << endl;
			DataLog << Ypos << endl;
			DataLog << Angle << endl;

			for(int i = 0 ; i < 682 ;i ++)
			{
				DataLog << Data[i] << endl;
			}

			cout << "Data has bin saved in file" << endl;
			Yflag = false;
			LastLengthX = abs(Xpos);
			LastLengthY = abs(Ypos);
			//SwipeCounter++;

			//if(SwipeCounter > 3)
			//{
				//Filter.FilterOute(Map,2);
			//	SwipeCounter = 0;
			//}
		}
		turnTime++;

		usleep(50000);
		//Pos = Rumba.GetPos();
		cout << Xpos << " " << Ypos << " " << Angle << endl;
	}
	Rumba.Stop();
	Pos= Rumba.GetPos();
	cout << Xpos << " "  << Ypos << " " << Angle << endl;
	DataLog.close();								// closes the txt doc



	return 0;
}
