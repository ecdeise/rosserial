#include <ros.h>
#include <ros/time.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <std_msgs/String.h>
#include <std_msgs/Float32.h>
#include "geometry_msgs/Point.h"
#include <std_msgs/Int16.h>
#include <stdio.h>
#include <stdlib.h>
#include <std_msgs/Empty.h>

#define ULTRASONIC_MAX_WAIT_COUNT 5000

const int ir_pin = 0;
const int TRIGGER_PIN = 10;
const int ECHO_PIN = 12;
long duration = 0;

//register the mag compass
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

//register the ros node
ros::NodeHandle nh;

// Publishers
std_msgs::Float32 ultraSonicRangeData;
ros::Publisher ultrasonic_data_raw("ultrasonic_data_raw", &ultraSonicRangeData);

geometry_msgs::Point magDataRaw;
ros::Publisher mag_data_raw("mag_data_raw", &magDataRaw);

std_msgs::Float32 irRangeRaw;
ros::Publisher ir_data_raw("ir_data_raw", &irRangeRaw);

//Subscribers
void messageCb( const std_msgs::Empty& toggle_msg){
  digitalWrite(13, HIGH-digitalRead(13));   // blink the led
}


ros::Subscriber<std_msgs::Empty> sub("toggle_led", &messageCb );



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



void getIrRawData(int pin, std_msgs::Float32 &irRangeRaw)
{
  irRangeRaw.data = analogRead(pin);
}


void getRawMagnetometerData(sensors_event_t event, geometry_msgs::Point &magDataRaw){

  magDataRaw.x = event.magnetic.x;
  magDataRaw.y = event.magnetic.y;
  magDataRaw.z = event.magnetic.z;

}



void getRawUltraSonicData(int trigger_pin, int echo_pin, 
  std_msgs::Float32 &ultraSonicRangeData) {

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

  //get the ultrasonic ranger 
  getRawUltraSonicData(TRIGGER_PIN, ECHO_PIN, ultraSonicRangeData);
  ultrasonic_data_raw.publish(&ultraSonicRangeData);


  nh.spinOnce();
  delay(1000);
}



