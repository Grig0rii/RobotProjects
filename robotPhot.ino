#include <Servo.h>

Servo servo;
#define TRIG_PIN 12
#define ECHO_PIN 13
#define SCL_Pin  A5
#define SDA_Pin  A4
unsigned char smile[] = {0x00, 0x7e, 0x12, 0x12, 0x7e, 0x00, 0x7e, 0x42, 0x42, 0x42, 0x00, 0x4e,
    0x2a, 0x1a, 0x7e, 0x00};
void setup() {
  servo.attach(10);
  servo.write(0);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(9, OUTPUT);
  pinMode(A1, INPUT);
  pinMode(A2, INPUT);
  pinMode(SCL_Pin, OUTPUT);
  pinMode(SDA_Pin, OUTPUT);
}

float getDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  return (float)duration / 29 / 2;
}
int oldExtended = 0;
int isExtended = 0;
int sensorValue = 0;
int outputValue = 0;
int sensorValue1 = 0;
int outputValue1 = 0;

void loop() {
  matrix_display(smile);
  sensorValue = analogRead(A1);
  sensorValue1 = analogRead(A2);
  outputValue = map(sensorValue, 0, 1023, 0, 255);
  outputValue1 = map(sensorValue1, 0, 1023, 0, 255);
  float distance = getDistance();

  if (distance > 0 && distance <= 20)
  {
    do {
      isExtended = random(1, 4); 
    } 
    while (isExtended == oldExtended);

    oldExtended = isExtended; 
    if (isExtended == 1) 
    {
      servo.write(90);
    } 
    else if (isExtended == 2)
    {
      servo.write(45);
    }
    else 
    {
      servo.write(0);
    }
    while (getDistance() <= 20) {
      delay(50); 
    }

    delay(200);
  }
  if (sensorValue > 500 || sensorValue1 > 500)
  {
    digitalWrite(9, LOW);
  }
  else
  {
    digitalWrite(9, HIGH);
  }
  delay(50);
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