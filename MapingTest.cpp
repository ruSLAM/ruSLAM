#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "LIDAR.h"
#include "ArduinoCom.h"
#include "LIDAR.cpp"
#include "ArduinoCom.cpp"
#include <time.h>

using namespace std;


int main()
{	int turnCounter = 0 ,turnTime = 0;
	bool Yflag = true;
	ofstream DataLog("Datalog.txt");
	int Xpos = 0,Ypos = 0, Data[682];
	double Angle, LastAngle = 0;
	int Map[3][5000],Cord[2][683];
	string Pos,DataString;
	int LastLengthX = 0,LastLengthY = 0;
	ArduinoCom Rumba;
	LIDAR Radar;

	Radar.Start();
	Radar.GetSwipe();
	Radar.FlushLidar();

	Rumba.Boot();
	Rumba.DriveForward();

	while(abs(Ypos) + abs(Xpos) < 5000)
	{
		if(abs(Ypos) > 2000 && turnCounter == 0)
		{
			turnTime = 0;
			Rumba.TurnLeft(90);
			turnCounter++;
		}

		if(turnCounter == 1 && turnTime == 10)
		{
			Rumba.DriveForward();
			turnCounter++;
		}
		
		DataString = "";

		Pos = Rumba.GetPos();
		Xpos = Rumba.Xpos();
		Ypos = Rumba.Ypos();
		Angle = Rumba.Angle();

		if(abs(Xpos) - LastLengthX > 100 || abs(Ypos) - LastLengthY > 100 || Yflag)
		{
			Radar.GetSwipe();
			DataString = Radar.GetData();
			Radar.Decoder(Data,DataString);

			DataLog << "New Data" << endl;
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

		}
		turnTime++;

		usleep(50000);
		cout << Xpos << " " << Ypos << " " << Angle << endl;
	}
	Rumba.Stop();
	Pos= Rumba.GetPos();
	cout << Pos << endl;
	DataLog.close();								// closes the txt doc

	return 0;
}
