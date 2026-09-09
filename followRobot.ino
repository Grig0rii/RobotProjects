#include <Servo.h>

Servo servo;

#define ML_Ctrl 4 
#define ML_PWM  6 
#define MR_Ctrl 2 
#define MR_PWM  5

#define TRIG_PIN 12
#define ECHO_PIN 13

#define SCL_Pin  A5
#define SDA_Pin  A4

int centerAngle = 45;
int currentAngle = 45;
bool scanningLeft = true;

const int DIST_TOO_CLOSE = 10;
const int DIST_FOLLOW    = 40;

unsigned char eye_normal[] = {
    0x00, 0x18, 0x24, 0x24, 0x18, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x18, 0x24, 0x24, 0x18, 0x00
};

void setup()
{
  Serial.begin(9600);
  servo.attach(10);
  servo.write(centerAngle);
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(SCL_Pin, OUTPUT);
  pinMode(SDA_Pin, OUTPUT);
  pinMode(ML_Ctrl, OUTPUT);
  pinMode(ML_PWM, OUTPUT);
  pinMode(MR_Ctrl, OUTPUT);
  pinMode(MR_PWM, OUTPUT);
  
  matrix_display(eye_normal);
}

void forw(byte speed = 95) {
    digitalWrite(ML_Ctrl, HIGH); analogWrite(ML_PWM, speed);
    digitalWrite(MR_Ctrl, HIGH); analogWrite(MR_PWM, speed);
} 

void back(byte speed = 110) {
    digitalWrite(ML_Ctrl, LOW); analogWrite(ML_PWM, speed);
    digitalWrite(MR_Ctrl, LOW); analogWrite(MR_PWM, speed);
} 

void left(byte speed = 110) {
    digitalWrite(ML_Ctrl, LOW);  analogWrite(ML_PWM, speed);
    digitalWrite(MR_Ctrl, HIGH); analogWrite(MR_PWM, speed);
}

void right(byte speed = 110) {
    digitalWrite(ML_Ctrl, HIGH); analogWrite(ML_PWM, speed);
    digitalWrite(MR_Ctrl, LOW);  analogWrite(MR_PWM, speed);
}

void stop() {
    analogWrite(ML_PWM, 0);
    analogWrite(MR_PWM, 0);
}

float getDistance() 
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); 
  if (duration == 0) return 400.0;
  return (float)duration / 29.0 / 2.0;
}

void loop()
{
  servo.write(currentAngle);
  delay(35);
  
  float distance = getDistance();
  
  if (distance > DIST_TOO_CLOSE && distance <= DIST_FOLLOW) 
  {
    if (currentAngle > centerAngle + 8) 
    {
      left(105);
      delay(30); 

      currentAngle -= 3; 
    } 
    else if (currentAngle < centerAngle - 8) 
    {
      right(105);
      delay(30);
      currentAngle += 3; 
    } 
    else 
    {
      forw(90);
    }
  } 
  else if (distance <= DIST_TOO_CLOSE && distance > 1.0) 
  {
    back(115);
    delay(40);
  } 
  else 
  {
    stop();
    
    if (scanningLeft) {
      currentAngle += 4;
      if (currentAngle >= 75) scanningLeft = false;
    } else {
      currentAngle -= 4;
      if (currentAngle <= 15) scanningLeft = true;
    }
  }
}

void matrix_display(unsigned char matrix_value[])
{
  IIC_start();
  IIC_send(0xc0);
  for (int i = 0; i < 16; i++) { IIC_send(matrix_value[i]); }
  IIC_end();
  IIC_start();
  IIC_send(0x8A);
  IIC_end();
}

void IIC_start()
{
  digitalWrite(SDA_Pin, HIGH); digitalWrite(SCL_Pin, HIGH); delayMicroseconds(3);
  digitalWrite(SDA_Pin, LOW);  delayMicroseconds(3); digitalWrite(SCL_Pin, LOW);
}

void IIC_end()
{
  digitalWrite(SCL_Pin, LOW);  digitalWrite(SDA_Pin, LOW);  delayMicroseconds(3);
  digitalWrite(SCL_Pin, HIGH); delayMicroseconds(3); digitalWrite(SDA_Pin, HIGH); delayMicroseconds(3);
}

void IIC_send(unsigned char send_data)
{
  for (byte mask = 0x01; mask != 0; mask <<= 1)
  {
    digitalWrite(SDA_Pin, (send_data & mask) ? HIGH : LOW);
    delayMicroseconds(3);
    digitalWrite(SCL_Pin, HIGH); delayMicroseconds(3); digitalWrite(SCL_Pin, LOW);
  }
}
