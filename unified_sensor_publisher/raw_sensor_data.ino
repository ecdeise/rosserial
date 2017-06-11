
#include <ros.h>
#include <ros/time.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <std_msgs/String.h>
#include <std_msgs/Float32.h>
#include "geometry_msgs/Point.h"
#include <std_msgs/Int16.h>
//#include <sensor_msgs/Range.h>
#include <stdio.h>
#include <stdlib.h>
#include <std_msgs/Empty.h>

#define ULTRASONIC_MAX_WAIT_COUNT 5000


Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

ros::NodeHandle nh;

std_msgs::Float32 ultraSonicRangeData;
ros::Publisher ultrasonic_data_raw("ultrasonic_data_raw", &ultraSonicRangeData);

geometry_msgs::Point magDataRaw;
ros::Publisher mag_data_raw("mag_data_raw", &magDataRaw);

std_msgs::Float32 irRangeRaw;
ros::Publisher ir_data_raw("ir_data_raw", &irRangeRaw);

void messageCb( const std_msgs::Empty& toggle_msg){
  digitalWrite(13, HIGH-digitalRead(13));   // blink the led
}


ros::Subscriber<std_msgs::Empty> sub("toggle_led", &messageCb );


const int ir_pin = 0;
const int TRIGGER_PIN = 10;
const int ECHO_PIN = 12;
long duration = 0;


void setup()
{
  nh.initNode();
  nh.advertise(ir_data_raw);
  nh.advertise(mag_data_raw);
  nh.advertise(ultrasonic_data_raw);
  randomSeed(analogRead(0));

  pinMode(13, OUTPUT);
  nh.subscribe(sub);
}


// char * getHeading() {
//   sensors_event_t event;
//   mag.getEvent(&event);

//   // Hold the module so that Z is pointing 'up' and you can measure the heading with x&y
//   // Calculate heading when the magnetometer is level, then correct for signs of axis.
//   float heading = atan2(event.magnetic.y, event.magnetic.x);

//   // Once you have your heading, you must then add your 'Declination Angle', which is the 'Error' of the magnetic field in your location.
//   // Find yours here: http://www.magnetic-declination.com/
//   // Mine is: -13* 2' W, which is ~13 Degrees, or (which we need) 0.22 radians
//   // If you cannot find your Declination, comment out these two lines, your compass will be slightly off.
//   float declinationAngle = 0.25; //for boston
//   heading += declinationAngle;

//   // Correct for when signs are reversed.
//   if(heading < 0)
//     heading += 2*PI;

//   // Check for wrap due to addition of declination.
//   if(heading > 2*PI)
//     heading -= 2*PI;

//   // Convert radians to degrees for readability.
//   float headingDegrees = heading * 180/M_PI;


//   char buffer[5];
//   String s = dtostrf(headingDegrees, 1, 4, buffer);

//   return buffer;
// }



void getIrRawData(int pin, std_msgs::Float32 &irRangeRaw)
{
  irRangeRaw.data = analogRead(pin);
}


void getRawMagnetometerData(sensors_event_t event, geometry_msgs::Point &magDataRaw){

  // nh.loginfo("pub raw mag data");

  // char result_x[8]; // Buffer big enough for 7-character float
  // char result_y[8];
  // char result_z[8];
  // dtostrf(event.magnetic.x, 6, 2, result_x);
  // dtostrf(event.magnetic.y, 6, 2, result_y);
  // dtostrf(event.magnetic.z, 6, 2, result_z);

  // nh.loginfo(result_x);
  // nh.loginfo(result_y);
  // nh.loginfo(result_z);


  magDataRaw.x = event.magnetic.x;
  magDataRaw.y = event.magnetic.y;
  magDataRaw.z = event.magnetic.z;

}

// void getSonarDataRaw(int trigpin, int echopin, std_msgs::Float32 &range){
//   pinMode(trigpin, OUTPUT);
//   pinMode(echopin,INPUT);
//   digitalWrite(trigpin, LOW);
//   delayMicroseconds(2);
//   digitalWrite(trigpin, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(trigpin, LOW);
//   //Get duration it takes to receive echo
//   range.data = pulseIn(echopin, HIGH);
// }

void getRawUltraSonicData(int trigger_pin, int echo_pin, 
  std_msgs::Float32 &ultraSonicRangeData) {


// Send out PING))) signal pulse
  pinMode(trigger_pin, OUTPUT);
  pinMode(echo_pin,INPUT);
  digitalWrite(trigger_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigger_pin, LOW);
  //Get duration it takes to receive echo
  duration = pulseIn(echo_pin, HIGH);
  //Convert duration into distance
  ultraSonicRangeData.data = duration; // /29/2;

  
  // unsigned long low_start_time, high_start_time, uptime, c;

  // pinMode(trigger_pin, OUTPUT);
  // pinMode(echo_pin, INPUT);
  // int _trigger_pin = trigger_pin;
  // int _echo_pin = echo_pin;

  // digitalWrite(_trigger_pin, LOW);
  // delayMicroseconds(2);

  // // Pulse for 10 us
  // digitalWrite(_trigger_pin, HIGH);
  // delayMicroseconds(10);
  // digitalWrite(_trigger_pin, LOW);


  // // Measure echo signal uptime in microseconds.
  // c = 0;
  // low_start_time = micros();
  // while (digitalRead(_echo_pin) == LOW && c++ < 1000) {}
  // if (micros()-low_start_time < 400) {
  //   return 4000;
  // }


  // c = 0;
  // high_start_time = micros();
  // while(digitalRead(_echo_pin) == HIGH && c++ < ULTRASONIC_MAX_WAIT_COUNT) {}
  // uptime = micros() - high_start_time;


  // ultraSonicRangeData.data = uptime;

  // Distance in meters = (duration in seconds) * (speed of sound m/s) / 2
  // Distance in cm = (t * 1e-6) * (340 * 1e2) / 2 = t * 17/1000
  // Distance in millimeters = (t * 1e-6) * (340 * 1e3) / 2 = t * 17/100

  // Return distance in mm, sensor is supposedly accurate to 0.3cm = 3mm
  // Clamp to 4000mm, which is 4m or maximum effective range of this sensor.

  //return min(4000, uptime * 17 / 1000);
}


void loop()
{

  sensors_event_t event;
  mag.getEvent(&event);

  //raw IR data
  getIrRawData(ir_pin, irRangeRaw);
  ir_data_raw.publish(&irRangeRaw);

  //get magnetometer (compass) pose data
  getRawMagnetometerData(event, magDataRaw);
  mag_data_raw.publish(&magDataRaw);

  getRawUltraSonicData(TRIGGER_PIN, ECHO_PIN, ultraSonicRangeData);
  ultrasonic_data_raw.publish(&ultraSonicRangeData);


  nh.spinOnce();
  delay(1000);
}



