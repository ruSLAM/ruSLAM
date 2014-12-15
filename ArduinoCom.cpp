#include "ArduinoCom.h"

ArduinoCom::ArduinoCom()
{
	Data.open("/dev/ttyACM1");
	Comand.open("/dev/ttyACM1");
}
ArduinoCom::~ArduinoCom()
{
	Data.close();
	Comand.close();
}

string ArduinoCom::GetPos()
{
	Comand << "XYA" << endl;

	alarm(1);
	getline(Data,Pos);
	alarm(0);

	if(Pos[0] == '#')
	{
		cutPos();
		return Pos;
	}
	else
		cout << "No data in Buffer" << endl;

	return "";
}

void ArduinoCom::Boot()
{
	cout << "BOOTING RUMBA" << endl;
	sleep(5);
	Comand << "RST" << endl;
	sleep(1);
	GetPos();
	sleep(1);
}

int ArduinoCom::Xpos()
{
	return xpos;
}

int ArduinoCom::Ypos()
{
	return ypos;
}

double ArduinoCom::angle()
{
	return theta;
}


void ArduinoCom::DriveForward()
{
	Comand << "DFW" << endl;
	sleep(1);
}
void ArduinoCom::DriveBack()
{
	Comand << "DBW" << endl;
	sleep(1);
}
void ArduinoCom::TurnLeft(int angle)
{
	string Left = "TLX";
	string ang = intToString(angle);

	Left = Left + ang;

	Comand << Left << endl;
}
void ArduinoCom::TurnRight(int angle)
{
	string Right = "TRX";
	string ang = intToString(angle);

	Right = Right + ang;

	Comand << Right << endl;
}

void ArduinoCom::Stop()
{
	Comand << "STP" << endl;
}
void ArduinoCom::ResetPos()
{
	Comand << "RST" << endl;
}



string ArduinoCom::intToString(int number)
{
	if(number > 360)
	{
		cout << "ERROR can not turn more thenn 360° " << endl;
		return "";
	}

	string s = "";
	 int b,c ;
	 char a[3] ;

	 for (int i = 0; i < 3; ++i)
	 {
		a[i] = '0';
	 }

	 if(number < 10)
	 {
	 	a[0] = number + 0x30;
	 }
	 if(number < 100 && number > 9)
	 {
	 		b = number%10;
	 		a[0] = b + 0x30;   
	 		a[1] = number/10 + 0x30;
	 }
	 if(number < 1000 && number > 99)
	 {
	 	    b = number%10;
	 	    c = (number/10)%10;
	 		a[0] = b + 0x30;   
	 		a[1] = c + 0x30;
	 		a[2] = number/100 + 0x30;
	 }


	  s = a[2];
	  s += a[1];
	  s += a[0];
	  return s;
}

void ArduinoCom::cutPos()
{
	string cutString = "";
	int Poslenth = Pos.length(); 							// length of data
	int flag = 1;

		for(int i = 1; i < Poslenth ; i++)
		{
         		if(Pos[i] != '#')
			{
				 cutString +=  Pos[i];                    	// cut out the first char
			}
			if(Pos[i] == '#' && flag == 1)
			{
				xpos = atoi(cutString.c_str());
				flag = 2;
				cutString = "";
			}
			if(Pos[i] == '#' && flag == 2)
			{
				ypos = atoi(cutString.c_str());
				cutString = "";
			}
                }

        theta = atoi(cutString.c_str())/1000.0;              			// Theta is [int], rad*1000

}
