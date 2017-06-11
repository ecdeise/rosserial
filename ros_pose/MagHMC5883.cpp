#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include "MagHMC5883.h"


MagHMC5883::MagHMC5883(Adafruit_HMC5883_Unified mag) {

	Adafruit_HMC5883_Unified magcompass = mag;
}

void MagHMC5883::init() {
	if(!magcompass.begin())
	  {
	    Serial.println("No HMC5883 detected ... Check the wiring!");
	    while(1);
	  }
	displaySensorDetails();
}

void MagHMC5883::displaySensorDetails(void)
{
  sensor_t sensor;
  magcompass.getSensor(&sensor);
  // Serial.println("------------------------------------");
  // Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  // Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  // Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  // Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");
  // Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");
  // Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");
  // Serial.println("------------------------------------");
  // Serial.println("");
  delay(500);
}

float MagHMC5883::getHeading() {
	/* Get a new sensor event */
	 sensors_event_t event;
	 magcompass.getEvent(&event);

	//Serial.print("X: "); Serial.print(event.magnetic.x); Serial.print("  ");
  	//Serial.print("Y: "); Serial.print(event.magnetic.y); Serial.print("  ");
  	//Serial.print("Z: "); Serial.print(event.magnetic.z); Serial.print("  ");Serial.println("uT");

	// Hold the module so that Z is pointing 'up' and you can measure the heading with x&y
 	// Calculate heading when the magnetometer is level, then correct for signs of axis.
	float heading = atan2(event.magnetic.y, event.magnetic.x);

	// Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
	// Find yours here: http://www.magnetic-declination.com/
	// Mine is: -13* 2' W, which is ~13 Degrees, or (which we need) 0.22 radians
	// If you cannot find your Declination, comment out these two lines, your compass will be slightly off.

	float declinationAngle = 0.25; //for boston
	heading += declinationAngle;

	// Correct for when signs are reversed.
	if(heading < 0)
	  heading += 2*PI;

	// Check for wrap due to addition of declination.
	if(heading > 2*PI)
	  heading -= 2*PI;

	// Convert radians to degrees for readability.
	float headingDegrees = heading * 180/M_PI;

	Serial.print("Heading (degrees): "); Serial.println(headingDegrees);
	return headingDegrees;
}
