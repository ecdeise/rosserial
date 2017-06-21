#include <ros.h>
#include <ros/time.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <std_msgs/String.h>
#include <std_msgs/Float32.h>
#include "geometry_msgs/Point.h"
#include <std_msgs/Int8.h>
#include <std_msgs/Int16.h>
#include "geometry_msgs/Point.h"
#include <stdio.h>
#include <stdlib.h>
#include <std_msgs/Empty.h>

#define ULTRASONIC_MAX_WAIT_COUNT 5000

const int ir_pin = 0;
const int TRIGGER_PIN = 10;
const int ECHO_PIN = 12;
long duration = 0;

const int encoder_pin_right = 2;
const int encoder_pin_left = 3;

int encoderValue_R = 0;
int encoderValue_L = 0; 

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

// void messageCb( const std_msgs::Empty& toggle_msg){
//   digitalWrite(8, HIGH-digitalRead(8)); 
//   digitalWrite(7, HIGH-digitalRead(7));   // blink the led
// }

//Subscribers
void ledToggleCallback( const std_msgs::Int8 &toggle_msg){

  char str[8];
  sprintf(str, "%d", toggle_msg.data);
  
  if (strcmp(str, "1") == 0)
  {
    //nh.loginfo(str);
    digitalWrite(9, HIGH-digitalRead(9));
  } 
  else if (strcmp(str, "2") == 0)
  {
    //nh.loginfo(str);
    digitalWrite(8, HIGH-digitalRead(8));
  }
  else if (strcmp(str, "3") == 0)
  {
    //nh.loginfo(str);
    digitalWrite(9, HIGH-digitalRead(9));   // blink the led
    digitalWrite(8, HIGH-digitalRead(8));   // blink the led
  }
  else if (strcmp(str, "4") == 0) 
  {
    //nh.loginfo(str);
    digitalWrite(9, HIGH-digitalRead(9));   // blink the led
    digitalWrite(8, HIGH-digitalRead(8));   // blink the led
  } 
  else 
  {
    nh.loginfo(str);
  }
 
}


ros::Subscriber<std_msgs::Int8> sub("toggle_led", &ledToggleCallback);

//ros::Subscriber<std_msgs::Empty> sub("toggle_led", &messageCb )

void countR(){
  encoderValue_R++;
}

void countL(){
  encoderValue_L++;
}

void setup()
{
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(encoder_pin_right, INPUT);
  pinMode(encoder_pin_left, INPUT);

  // attachInterrupt(digitalPinToInterrupt(encoder_pin_right), countA, FALLING);
  // attachInterrupt(digitalPinToInterrupt(encoder_pin_left), countB, FALLING);
  
  attachInterrupt(digitalPinToInterrupt(encoder_pin_right), countR, FALLING);
  attachInterrupt(digitalPinToInterrupt(encoder_pin_left), countL, FALLING);

  nh.initNode();
  nh.advertise(ir_data_raw);
  nh.advertise(mag_data_raw);
  nh.advertise(ultrasonic_data_raw);
  nh.advertise(encoder_data_right);
  nh.advertise(encoder_data_left);
  randomSeed(analogRead(0));

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

void getEncoderDataRight(std_msgs::Int16 &encoderRight){

  encoderRight.data = encoderValue_R;

}

void getEncoderDataLeft(std_msgs::Int16 &encoderLeft){

  encoderLeft.data = encoderValue_L;

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

  getEncoderDataRight(encoderRight);
  encoder_data_right.publish(&encoderRight);

  getEncoderDataLeft(encoderLeft);
  encoder_data_left.publish(&encoderLeft);


  nh.spinOnce();
  delay(1000);
}

