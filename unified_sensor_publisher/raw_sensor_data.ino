#include <ros.h>
#include <ros/time.h>
//#include <Wire.h>
#include <Adafruit_Sensor.h>
//#include <Adafruit_HMC5883_U.h>
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
//Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

//register the ros node
ros::NodeHandle nh;

// Publishers
std_msgs::Float32 ultraSonicRangeData;
ros::Publisher ultrasonic_data_raw("ultrasonic_data_raw", &ultraSonicRangeData);

// geometry_msgs::Point magDataRaw;
// ros::Publisher mag_data_raw("mag_data_raw", &magDataRaw);

std_msgs::Float32 irRangeRaw;
ros::Publisher ir_data_raw("ir_data_raw", &irRangeRaw);

//rostopic pub motor_left std_msgs/Int16 0 --once

std_msgs::Int16 encoderRight;
ros::Publisher encoder_data_right("encoder_data_right", &encoderRight);

std_msgs::Int16 encoderLeft;
ros::Publisher encoder_data_left("encoder_data_left", &encoderLeft);



void motor_left( const std_msgs::Int16& msg){

  int enA = 10;
  int in1 = 8;
  int in2 = 9;

    if(msg.data == 0) {
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);  
    } else if (msg.data > 0) {
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH); 
        analogWrite(enA, msg.data); 
    } else {
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW); 
        analogWrite(enA, -msg.data);
    }



    
    // if(msg.data == 0)
    // {
    //   nh.loginfo("motor_left == 0");
    //   digitalWrite(5, LOW); //
    //   digitalWrite(4, LOW); //
    //   digitalWrite(6, HIGH); //
    //   //digitalWrite(STBY, HIGH); // STYBY - HIGH
    // }
    // else if (msg.data > 0)
    // {
    //   nh.loginfo("motor_left > 0");
    //   digitalWrite(4, HIGH);
    //   digitalWrite(5, LOW);
    //   //digitalWrite(STBY, HIGH); // STYBY - HIGH
    //   analogWrite(6, msg.data);
    // }
    // else
    // {
    //   nh.loginfo("motor_left < 0");
    //   digitalWrite(4, LOW);
    //   digitalWrite(5, HIGH);
    //   //digitalWrite(STBY, HIGH); // STYBY - HIGH
    //   analogWrite(6, -msg.data);
    // }

}

void motor_right( const std_msgs::Int16& msg){
  
  // motor two
  int enB = 6;
  int in3 = 4;
  int in4 = 5;

  if(msg.data == 0) {
      digitalWrite(in3, LOW);
      digitalWrite(in4, LOW);  
    } else if (msg.data > 0) {
      digitalWrite(in3, LOW);
      digitalWrite(in4, HIGH);
      analogWrite(enB, msg.data); 
    } else {
      digitalWrite(in3, HIGH);
      digitalWrite(in4, LOW);
      analogWrite(enB, -msg.data); 
    }

 // if(msg.data == 0)
 //    {
 //      nh.loginfo("motor_right == 0");
 //      digitalWrite(8, LOW); //
 //      digitalWrite(9, LOW); //
 //      digitalWrite(10, HIGH); //
 //      //digitalWrite(STBY, HIGH); // STYBY - HIGH
 //    }
 //    else if (msg.data > 0)
 //    {
 //      nh.loginfo("motor_right > 0");
 //      digitalWrite(8, HIGH);
 //      digitalWrite(9, LOW);
 //      //digitalWrite(STBY, HIGH); // STYBY - HIGH
 //      analogWrite(10, msg.data);
 //    }
 //    else
 //    {
 //      nh.loginfo("motor_right < 0");
 //      digitalWrite(8, LOW);
 //      digitalWrite(9, HIGH);
 //      //digitalWrite(STBY, HIGH); // STYBY - HIGH
 //      analogWrite(10, -msg.data);
 //    }

}


ros::Subscriber<std_msgs::Int16> sub_right("motor_right", motor_right);

ros::Subscriber<std_msgs::Int16> sub_left("motor_left", motor_left);


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

  pinMode(10, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(2), count_encoder_A, FALLING);
  attachInterrupt(digitalPinToInterrupt(3), count_encoder_B, FALLING);

  nh.initNode();
  nh.advertise(ir_data_raw);
  //nh.advertise(mag_data_raw);
  nh.advertise(ultrasonic_data_raw);
  nh.advertise(encoder_data_right);
  nh.advertise(encoder_data_left);
  randomSeed(analogRead(0));

  nh.subscribe(sub_left);
  nh.subscribe(sub_right);
}

void getIrRawData(std_msgs::Float32 &irRangeRaw)
{
  irRangeRaw.data = analogRead(0);
}


// void getRawMagnetometerData(sensors_event_t event, geometry_msgs::Point &magDataRaw){

//   magDataRaw.x = event.magnetic.x;
//   magDataRaw.y = event.magnetic.y;
//   magDataRaw.z = event.magnetic.z;

// }

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

  // sensors_event_t event;
  // mag.getEvent(&event);

  //raw IR data
  getIrRawData(irRangeRaw);
  ir_data_raw.publish(&irRangeRaw);

  //get magnetometer (compass) pose data
  //getRawMagnetometerData(event, magDataRaw);
  //mag_data_raw.publish(&magDataRaw);

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

