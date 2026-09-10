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
  Serial.begin(9600);
  servo.attach(10, 350, 2450);
  servo.write(47);
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

int lastTurn = 0;

void loop()
{
    matrix_display(move);
    
    servo.write(47); 
    delay(50);
    float distance = getDistance();
    Serial.print("Front: ");
    Serial.println(distance);
    if (distance > 30)
    {
        forw();
        return;
    }

    stop();

    servo.write(110);
    delay(300);
    float distanceLeft = getDistance();

    Serial.print("Left: ");
    Serial.println(distanceLeft);

    servo.write(0);
    delay(300);
    float distanceRight = getDistance();

    Serial.print("Right: ");
    Serial.println(distanceRight);

    servo.write(47);
    delay(100);

    Serial.print("Decision: ");
    
    if (distanceLeft <= 30 && distanceRight <= 30)
    {
        Serial.println("TURN AROUND");

        right();
        delay(1500);
        stop();

        lastTurn = 2;
    }

    else if (distanceRight > distanceLeft + 3)
    {
        Serial.println("RIGHT");

        right();
        delay(1000);
        stop();

        lastTurn = 2;
    }

    else if (distanceLeft > distanceRight + 3)
    {
        Serial.println("LEFT");

        left();
        delay(1000);
        stop();

        lastTurn = 1;
    }

    else
    {
        if (lastTurn == 1)
        {
            Serial.println("EQUAL -> LEFT");

            left();
            delay(1000);
            stop();

            lastTurn = 1;
        }
        else
        {
            Serial.println("EQUAL -> RIGHT");

            right();
            delay(1000);
            stop();

            lastTurn = 2;
        }
    }

    delay(100);
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
