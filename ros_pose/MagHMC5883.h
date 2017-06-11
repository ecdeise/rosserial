#ifndef Maghmc5883_h
#define Maghmc5883_h

class MagHMC5883
{
	public:
	 MagHMC5883(Adafruit_HMC5883_Unified mag);
	 void displaySensorDetails();
	 void init();
	 float getHeading();


	Adafruit_HMC5883_Unified magcompass;
};

#endif
