#ifndef ArduinoCom_h
#define ArduinoCom_h
#include <iostream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <cstring>

using namespace std;

class ArduinoCom
{
	private:
		ifstream Data;
		ofstream Comand;
		string Pos;
		int xpos,ypos;
		double theta;
		void cutPos();
		string intToString(int number);
	public:

		ArduinoCom();
		~ArduinoCom();
		string GetPos();
		int Xpos();
		int Ypos();
		double angle();
		void DriveForward();
		void DriveBack();
		void TurnLeft(int angle);
		void TurnRight(int angle);
		void Stop();
		void ResetPos();
		void Boot();
};
#endif
