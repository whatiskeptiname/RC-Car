#include"PPMReader.h"
#include<Servo.h>

int const ml1 = 7;
int const ml2 = 8;
int const mr1 = 9;
int const mr2 = 10;
int const pwm1 = 6;
int const pwm2 = 11;
int const interruptPin = 2;
int const servoPin1 = 3;
int const servoPin2 = 5;

boolean ch1_neutral = true, ch3_neutral =  true;
int channelAmount = 8;
int ch[8];
int pp1, pp2, pp3, pp4;
int var_maxPWM = 0, var_maxPWMTurn = 0;
int servo1, servo2;
Servo servo_1, servo_2;

PPMReader ppm(interruptPin, channelAmount);

void setup()
{
  pinMode(ml1, OUTPUT);
  pinMode(ml2, OUTPUT);
  pinMode(mr1, OUTPUT);
  pinMode(mr2, OUTPUT);
  pinMode(pwm1, OUTPUT);
  pinMode(pwm2, OUTPUT);
  servo_1.attach(servoPin1);
  servo_2.attach(servoPin2);

  Serial.begin(9600);
}

void readPPMData()
{
  for (int channel = 1; channel <= channelAmount; ++channel)
  {
    unsigned long value = ppm.latestValidChannelValue(channel, 0);
    ch[channel - 1] = value;
  }
}

void move_forward()
{
  digitalWrite(ml1, LOW);
  digitalWrite(ml2, HIGH);
  digitalWrite(mr1, HIGH);
  digitalWrite(mr2, LOW);
  analogWrite(pwm1, pp1);
  analogWrite(pwm2, pp2);
  Serial.println("FORWARD");
}

void move_backward()
{
  digitalWrite(ml1, HIGH);
  digitalWrite(ml2, LOW);
  digitalWrite(mr1, LOW);
  digitalWrite(mr2, HIGH);
  analogWrite(pwm1, pp1);
  analogWrite(pwm2, pp2);
  Serial.println("BACKWARD");
}

void move_left()
{
  digitalWrite(ml1, HIGH);
  digitalWrite(ml2, LOW);
  digitalWrite(mr1, HIGH);
  digitalWrite(mr2, LOW);
  analogWrite(pwm1, pp3);
  analogWrite(pwm2, pp4);
  Serial.println("LEFT");
}

void move_right()
{
  digitalWrite(ml1, LOW);
  digitalWrite(ml2, HIGH);
  digitalWrite(mr1, LOW);
  digitalWrite(mr2, HIGH);
  analogWrite(pwm1, pp3);
  analogWrite(pwm2, pp4);
  Serial.println("RIGHT");
}

void neutral()
{
  digitalWrite(ml1, LOW);
  digitalWrite(ml2, LOW);
  digitalWrite(mr1, LOW);
  digitalWrite(mr2, LOW);
  analogWrite(pwm1, 0);
  analogWrite(pwm2, 0);
  Serial.println("NEUTRAL");
}

bool system_alive()
{
  if (ch[6] > 1500)
    return 1;
  else
    return 0;
}

void servo_posn()
{
  servo1 = map(ch[4], 1000, 2000, 90, 0);
  servo2 = map(ch[9], 1000, 2000, 90, 0);
  servo_1.write(servo1);
  servo_2.write(servo2);
}

void servo_dance()
{
  if (ch[7] > 1500)
  {
    servo_1.write(servo1);
    delay(10);
    servo_1.write(servo1 + 20);
    delay(10);
    Serial.println("Activated Ultra Servo");
  }
}

void servo_mobilize()
{
  if (ch[9] > 1500)
  {
    servo_2.write(0);
    delay(10);
    servo_2.write(180);
    delay(10);
    Serial.println("Servo Mobilized");
  }
}

void get_gear()
{
  if (ch[5] < 1333)
  {
    var_maxPWM = 120;
    var_maxPWMTurn = 150;
    Serial.println("First Gear");
  }
  else if (ch[5] > 1334 && ch[5] < 1666)
  {
    var_maxPWM = 180;
    var_maxPWMTurn = 200;
    Serial.println("Second Gear");
  }
  else
  {
    var_maxPWM = 255;
    var_maxPWMTurn = 255;
    Serial.println("Third Gear");
  }
}

void mobilize()
{
  if (ch[1] > 1485 && ch[1] < 1515)
  {
    ch1_neutral = true;
  }
  else if (ch[1] > 1515)
  {
    ch1_neutral = false;
    pp1 = abs(map(ch[1], 1515, 2000, 0, var_maxPWM));
    pp2 = abs(map(ch[1], 1515, 2000, 0, var_maxPWM));
    move_backward();
  }
  else if (ch[1] < 1485)
  {
    ch1_neutral = false;
    pp1 = abs(map(ch[1], 1000, 1485, var_maxPWM, 0));
    pp2 = abs(map(ch[1], 1000, 1485, var_maxPWM, 0));
    move_forward();
  }
  if (ch[3] > 1485 && ch[3] < 1515)
  {
    ch3_neutral = true;
  }
  else if (ch[3] > 1515)
  {
    ch3_neutral = false;
    pp3 = abs(map(ch[3], 1515, 2000, 0, var_maxPWMTurn ));
    pp4 = abs(map(ch[3], 1515, 2000, 0, var_maxPWMTurn ));
    move_right();
  }
  else if (ch[3] < 1485)
  {
    ch3_neutral = false;
    pp3 = abs(map(ch[3], 1000, 1485, var_maxPWMTurn , 0));
    pp4 = abs(map(ch[3], 1000, 1485, var_maxPWMTurn , 0));
    move_left();
  }
  if (ch1_neutral && ch3_neutral)
  {
    neutral();
  }
}
