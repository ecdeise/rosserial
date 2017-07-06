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

int encoderValue_A = 0;
int encoderValue_B = 0; 
long ultrasonic_duration = 0;

//https://www.ez-robot.com/Tutorials/UserTutorials/189/1 encoder on D2 and D3

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

std_msgs::Int16 encoderRight;
ros::Publisher encoder_data_right("encoder_data_right", &encoderRight);

std_msgs::Int16 encoderLeft;
ros::Publisher encoder_data_left("encoder_data_left", &encoderLeft);


void count_encoder_A(){
  encoderValue_A++;
}

void count_encoder_B(){
  encoderValue_B++;
}

void setup()
{
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(2), count_encoder_A, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), count_encoder_B, FALLING);

  nh.initNode();
  nh.advertise(ir_data_raw);
  nh.advertise(mag_data_raw);
  nh.advertise(ultrasonic_data_raw);
  nh.advertise(encoder_data_right);
  nh.advertise(encoder_data_left);
  randomSeed(analogRead(0));

  nh.subscribe(sub);
}

void getIrRawData(std_msgs::Float32 &irRangeRaw)
{
  irRangeRaw.data = analogRead(0);
}


void getRawMagnetometerData(sensors_event_t event, geometry_msgs::Point &magDataRaw){

  magDataRaw.x = event.magnetic.x;
  magDataRaw.y = event.magnetic.y;
  magDataRaw.z = event.magnetic.z;

}

void getRawUltraSonicData(std_msgs::Float32 &ultraSonicRangeData) {

  pinMode(10, OUTPUT);
  pinMode(12,INPUT);
  digitalWrite(10, LOW);
  delayMicroseconds(2);
  digitalWrite(10, HIGH);
  delayMicroseconds(10);
  digitalWrite(10, LOW);

  //Get ultrasonic_duration it takes to receive echo
  ultrasonic_duration = pulseIn(12, HIGH);

  //Convert ultrasonic_duration into distance
  ultraSonicRangeData.data = ultrasonic_duration; // /29/2;
 
}

void getEncoderDataRight(std_msgs::Int16 &encoderRight){

  encoderRight.data = encoderValue_A;

}

void getEncoderDataLeft(std_msgs::Int16 &encoderLeft){

  encoderLeft.data = encoderValue_B;

}

void loop()
{

  sensors_event_t event;
  mag.getEvent(&event);

  //raw IR data
  getIrRawData(irRangeRaw);
  ir_data_raw.publish(&irRangeRaw);

  //get magnetometer (compass) pose data
  getRawMagnetometerData(event, magDataRaw);
  mag_data_raw.publish(&magDataRaw);

  //get the ultrasonic ranger 
  getRawUltraSonicData(ultraSonicRangeData);
  ultrasonic_data_raw.publish(&ultraSonicRangeData);

  getEncoderDataRight(encoderRight);
  encoder_data_right.publish(&encoderRight);

  getEncoderDataLeft(encoderLeft);
  encoder_data_left.publish(&encoderLeft);


  nh.spinOnce();
  delay(1000);
}

