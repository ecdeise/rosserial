#include <Adafruit_Sensor.h>
/*
 * rosserial Ultrasound Example
 * using seeestudio ultrasound sensor
 * (compatible with PING))))
 */

#include <ros.h>
#include <ros/time.h>
#include <sensor_msgs/Range.h>

#define ULTRASONIC_MAX_WAIT_COUNT 5000

ros::NodeHandle  nh;

sensor_msgs::Range range_msg;
ros::Publisher pub_range1("/ultrasound1", &range_msg);
//ros::Publisher pub_range2("/ultrasound2", &range_msg);
// "/arduino_ultrasound"

char frameid[] = "/ultrasound";

long duration;
// float getRange_Ultrasound(int pin_num)
// {
//   // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
//   // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
//   pinMode(pin_num, OUTPUT);
//   digitalWrite(pin_num, LOW);
//   delayMicroseconds(2);
//   digitalWrite(pin_num, HIGH);
//   delayMicroseconds(10);
//   digitalWrite(pin_num, LOW);

//   // The same pin is used to read the signal from the PING))): a HIGH
//   // pulse whose duration is the time (in microseconds) from the sending
//   // of the ping to the reception of its echo off of an object.
//   pinMode(pin_num, INPUT);
//   duration = pulseIn(pin_num, HIGH);

//   // convert the time into a distance
//   return duration/58; //    duration/29/2, return centimeters
// }

unsigned long GetRawDistance(int trigger_pin, int echo_pin) {
  unsigned long low_start_time, high_start_time, uptime, c;

  pinMode(trigger_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  int _trigger_pin = trigger_pin;
  int _echo_pin = echo_pin;

  digitalWrite(_trigger_pin, LOW);
  delayMicroseconds(2);

  // Pulse for 10 us
  digitalWrite(_trigger_pin, HIGH);
  delayMicroseconds(10);
  digitalWrite(_trigger_pin, LOW);


  // Measure echo signal uptime in microseconds.
  c = 0;
  low_start_time = micros();
  while (digitalRead(_echo_pin) == LOW && c++ < 1000) {}
  if (micros()-low_start_time < 400) {
    return 4000;
  }


  c = 0;
  high_start_time = micros();
  while(digitalRead(_echo_pin) == HIGH && c++ < ULTRASONIC_MAX_WAIT_COUNT) {}
  uptime = micros() - high_start_time;


  // Distance in meters = (duration in seconds) * (speed of sound m/s) / 2
  // Distance in cm = (t * 1e-6) * (340 * 1e2) / 2 = t * 17/1000
  // Distance in millimeters = (t * 1e-6) * (340 * 1e3) / 2 = t * 17/100

  // Return distance in mm, sensor is supposedly accurate to 0.3cm = 3mm
  // Clamp to 4000mm, which is 4m or maximum effective range of this sensor.

  return min(4000, uptime * 17 / 1000);
}

void setup()
{
  nh.initNode();
  nh.advertise(pub_range1);
  //nh.advertise(pub_range2);

  range_msg.radiation_type = sensor_msgs::Range::ULTRASOUND;
  range_msg.header.frame_id =  frameid;
  range_msg.field_of_view = 0.1;  // fake
  range_msg.min_range = 0.0;
  range_msg.max_range = 2.0;
}

long range_time;

void loop()
{
  //publish the adc value every 50 milliseconds
  //since it takes that long for the sensor to stabilize
  if ( millis() >= range_time ){
    range_msg.range = GetRawDistance(2, 4);
    range_msg.header.stamp = nh.now();
    pub_range1.publish(&range_msg);

//    range_msg.range = getRange_Ultrasound(6);
//    range_msg.header.stamp = nh.now();
//    pub_range2.publish(&range_msg);

    range_time =  millis() + 50;
  }
  delay(1000);
  nh.spinOnce();
}
