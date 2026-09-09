#include <Servo.h>
Servo servo;
#define ML_Ctrl 4
#define ML_PWM 6
#define MR_Ctrl 2
#define MR_PWM 5
#define TRIG_PIN 12
#define ECHO_PIN 13
#define SCL_Pin  A5
#define SDA_Pin  A4
unsigned char move[] = {
    0x00, 0x7e, 0x12, 0x12, 0x7e, 0x00, 0x7e, 0x42, 0x42, 0x42, 0x00, 0x4e,
    0x2a, 0x1a, 0x7e, 0x00
};

unsigned char icon_right[] = {
    0x00, 0x18, 0x3c, 0x7e, 0x5a, 0x5a, 0x5a, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x00
};
unsigned char icon_left[] = {
    0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x5a, 0x5a, 0x7e,
    0x3c, 0x18, 0x00, 0x00
};
void setup()
{
  servo.attach(10);
  servo.write(45);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(SCL_Pin, OUTPUT);
  pinMode(SDA_Pin, OUTPUT);
  pinMode(ML_Ctrl, OUTPUT);
  pinMode(ML_PWM, OUTPUT);
  pinMode(MR_Ctrl, OUTPUT);
  pinMode(MR_PWM, OUTPUT);
}
void forw()
{
     digitalWrite(ML_Ctrl, HIGH);
    analogWrite(ML_PWM, 100);
    digitalWrite(MR_Ctrl, HIGH);
    analogWrite(MR_PWM, 100);
    delay(10);
} 
void back()
{
    digitalWrite(ML_Ctrl, LOW);
    analogWrite(ML_PWM, 200);
    digitalWrite(MR_Ctrl, LOW);
    analogWrite(MR_PWM, 200);
    delay(10);
} 
void left()
{
    matrix_display(icon_left);
    digitalWrite(ML_Ctrl, LOW);
    analogWrite(ML_PWM, 200);
    digitalWrite(MR_Ctrl, HIGH);
    analogWrite(MR_PWM, 55);
    delay(10);
}
void right()
{
    matrix_display(icon_right);
    digitalWrite(ML_Ctrl, HIGH);
    analogWrite(ML_PWM, 55);
    digitalWrite(MR_Ctrl, LOW);
    analogWrite(MR_PWM, 200);
    delay(10);
}
void stop()
{
    digitalWrite(ML_Ctrl, LOW);
    analogWrite(ML_PWM, 0);
    digitalWrite(MR_Ctrl, LOW);
    analogWrite(MR_PWM, 0);
    delay(10);
}
float getDistance() 
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  return (float)duration / 29 / 2;
}
void loop()
{
    matrix_display(move);
    
    servo.write(45); 
    delay(50);
    float distance = getDistance();

    if (distance <= 30)
    {
        stop();
        
        servo.write(90);
        delay(300);
        float distanceLeft = getDistance();
        
        servo.write(0);
        delay(300);
        float distanceRight = getDistance();
        
        servo.write(45);
        
        if (distanceRight > distanceLeft)
        {
            right();
            delay(500);
        }
        else
        {
            left();
            delay(500);
        }
        stop();
    }
    else
    {
        forw();
    }
}

void matrix_display(unsigned char matrix_value[])
{
  IIC_start();
  IIC_send(0xc0);

  for (int i = 0; i < 16; i++)
  {
    IIC_send(matrix_value[i]);
  }

  IIC_end();

  IIC_start();
  IIC_send(0x8A);
  IIC_end();
}

void IIC_start()
{
  digitalWrite(SDA_Pin, HIGH);
  digitalWrite(SCL_Pin, HIGH);
  delayMicroseconds(3);
  digitalWrite(SDA_Pin, LOW);
  delayMicroseconds(3);
  digitalWrite(SCL_Pin, LOW);
}

void IIC_end()
{
  digitalWrite(SCL_Pin, LOW);
  digitalWrite(SDA_Pin, LOW);
  delayMicroseconds(3);
  digitalWrite(SCL_Pin, HIGH);
  delayMicroseconds(3);
  digitalWrite(SDA_Pin, HIGH);
  delayMicroseconds(3);
}

void IIC_send(unsigned char send_data)
{
  for (byte mask = 0x01; mask != 0; mask <<= 1)
  {
    if (send_data & mask) {
      digitalWrite(SDA_Pin, HIGH);
    } else {
      digitalWrite(SDA_Pin, LOW);
    }
    delayMicroseconds(3);
    digitalWrite(SCL_Pin, HIGH);
    delayMicroseconds(3);
    digitalWrite(SCL_Pin, LOW);
  }
}