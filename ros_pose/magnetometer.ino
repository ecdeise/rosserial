#include <ros.h>
#include <ros/time.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <std_msgs/Float64.h>

/*

*** THIS IS A SUCCESSFUL TEST FOR Adafruit_HMC5883 COMPASS ***

note: occasionally we need to run the sample script ... before the rossieral code will work.  Why? who knows...

for wiring
https://learn.adafruit.com/adafruit-hmc5883l-breakout-triple-axis-magnetometer-compass-sensor/wiring-and-test

*/

/* Assign a unique ID to this sensor at the same time */
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

ros::NodeHandle  nh;

std_msgs::Float64 myData;

ros::Publisher pose("pose", &myData);


void setup(void)
{

  nh.initNode();
  nh.advertise(pose);
  randomSeed(analogRead(0));

}

void loop(void)
{
  /* Get a new sensor event */
  sensors_event_t event;
  mag.getEvent(&event);

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

  //myData.data = headingDegrees;
  /* -- test that the loop isn't broken
  long randNumber = random(10, 20);
  myData.data = randNumber;
  */

  myData.data  = headingDegrees;
  pose.publish(&myData);

  nh.spinOnce();
  delay(500);
}
