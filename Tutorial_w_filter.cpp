#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <time.h>
#include "LIDAR.h"
#include "ArduinoCom.h"
#include "LIDAR.cpp"
#include "ArduinoCom.cpp"
#include "Filter.cpp"
#include "Filter.h"

using namespace std;


int main(){
/* 
	Initializing variables data structure and data stream
*/

	int turnCounter = 0 ,turnTime = 0;			// For turning
	bool Yflag = true;							// Flag so the initial position and first swipe are saved
	
	ofstream DataLog("Datalog.txt");			// Datafile all location and mapping data are saved to this file
	
	int Xpos = 0, Ypos = 0, Data[682];			// 
	double Angle;								// 
	string Pos,DataString;						// 

	int Map[3][5000],Cord[2][683];				// This is for the filter Map keeps track of all points inn the map
	 
	int LastLength = 0, SwipeCounter = 0;		//
	
	ArduinoCom Roomba;							// Roomba object handles all communications with the Roomba and the optical flow sensor
	LIDAR Radar;								// Radar object handles all communications with the LIDAR
	Filter Filter;								// This is a test filter and is only here to show an example of how to implement one

	Radar.start();								// Sends the command to turn on the laser of the LIDAR and to start taking measurements 
	Radar.getSwipe();							// getSwipe sends asks for a one swipe from the LIDAR and stores it in a private variable. To get the data use the Radar.getData(); command
	Radar.flushLidar();							// Sometimes the first swipe from the LDAR is incorrect so this command dumps the data from the first swipe

	Roomba.Boot();								// This command boots up the Link between the Raspberry and the Arduino 
	Roomba.DriveForward();						// Drives the Roomba forward until told otherwise


	/*
		The this loop will run until the platform (Roomba) is one meter from its starting point
	*/	
	while(sqrt(abs(Xpos)^2 + abs(Ypos)^2) < 1000)
	{

		/*
			When the platform has gone 0.5m from its starting point this tells it to turn 90° to the left.
			The turnCounter keeps track the order of the commands to execute 
		*/
		if(abs(Ypos) > 500 && turnCounter == 0)
		{
			turnTime = 0;
			Roomba.TurnLeft(90);
			turnCounter++;
		}

		/*
			This will tell the platform to keep driving forward after finishing the 90° left turn.
			The turnTime makes sure that some time has passed between the TurnLeft command and the DriveForward 
			so that the Arduino will be ready for a new command.
		*/
		if(turnCounter == 1 && turnTime == 10)
		{
			Roomba.DriveForward();
			turnCounter++;
		}

		/*
			When the Roomba has gone 0.5m on X axis  it will turn 90° to the Right
		*/
		if(abs(Xpos) > 500 && turnCounter == 2)
		{
			turnTime = 0;
			Roomba.TurnRight(90);
			turnCounter++;
		}

		/*
			When the platform is done with the Right turn it will drive backwards until the end of the script 
			(until the radius from the starting point is < 1m)	
		*/
		if(turnCounter == 3 && turnTime == 10)
		{
			Roomba.DriveBack();
			turnCounter++;
		}

		DataString = "";						// Clear the dataString so it is ready fot the data from the LIDAR 
		Pos = Roomba.GetPos();					// Gets the position from the optical flow sensor. Pos is raw data and is not used here

		// Sets the position in to the correct parameters
		Xpos = Roomba.Xpos();					// X coordinate extracted from Pos
		Ypos = Roomba.Ypos();					// Y coordinate extracted from Pos
		Angle = Roomba.angle();					// yaw of the Create extracted from Pos

		/*
			Here new data will be added to a map every 0.1m the platform moves 
		*/
		if(abs(LastLength - sqrt(abs(Xpos)^2 + abs(Ypos)^2)) > 100 || Yflag)
		{
			Radar.getSwipe();								//	Tells the LIDAR to send the data from the last swipe
			DataString = Radar.getData();					//	Gets the Data from the private variable 
			Radar.Decoder(Data,DataString);					//	Decodes the data from getData

			Filter.SetToCord(Cord,Data,Xpos,Ypos,Angle);	// Sets the data points to Cartesian coordinates
			Filter.SetMap(Cord,Map,25);						// Filters and saves the data to the map

			Filter.SaveMap(Map);							// saves the map into in to a txt file

			cout << "Data has bin saved in file" << endl;	// 
			Yflag = false;									// This is a boolean variable so the data from the starting point will be saved 
			LastLength = sqrt(abs(Xpos)^2 + abs(Ypos)^2);	//	
			SwipeCounter++;									//

			/*
				Some filter stuff 
			*/
			if(SwipeCounter > 3)
			{
				Filter.FilterOute(Map,2);					
				SwipeCounter = 0;
			}
		}

		turnTime++;											// 

		usleep(50000);										// This needs to be here to give the Arduino time to get ready for the next command

		cout << Xpos << " " << Ypos << " " << Angle << endl;
	}

	Roomba.Stop();											// When the route has ended stop the Roomba									
	Pos = Roomba.GetPos();									// Gets the last position from the optical flow sensor 
	cout << Pos << endl;									// For debugging 
	DataLog.close();										// Close file

	return 0;
}
