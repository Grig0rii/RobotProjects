#include <Servo.h>
#include <IRremote.hpp>
Servo servo;
#define IR_RECEIVE_PIN 3 
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

unsigned char forward_move[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x0c, 0x7e, 0x7e, 0x0c, 0x18, 0x00,
    0x00, 0x00, 0x00, 0x00
};
unsigned char back_move[] =
{
    0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x30, 0x7e, 0x7e, 0x30, 0x18, 0x00,
    0x00, 0x00, 0x00, 0x00
};
unsigned char icon_right[] = {
    0x00, 0x18, 0x3c, 0x7e, 0x5a, 0x5a, 0x5a, 0x18, 0x18, 0x18, 0x18, 0x18,
    0x18, 0x18, 0x18, 0x00
};
unsigned char icon_left[] = {
    0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x5a, 0x5a, 0x7e,
    0x3c, 0x18, 0x00, 0x00
};
int servo_pos = 45;
unsigned long ir_rec = 0;
void setup()
{
  Serial.begin(9600);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); 
  servo.attach(10);
  servo.write(servo_pos);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(SCL_Pin, OUTPUT);
  pinMode(SDA_Pin, OUTPUT);
  pinMode(ML_Ctrl, OUTPUT);
  pinMode(ML_PWM, OUTPUT);
  pinMode(MR_Ctrl, OUTPUT);
  pinMode(MR_PWM, OUTPUT);
  matrix_display(move);
}
void forw()
{
    matrix_display(forward_move);
    digitalWrite(ML_Ctrl, HIGH);
    analogWrite(ML_PWM, 100);
    digitalWrite(MR_Ctrl, HIGH);
    analogWrite(MR_PWM, 100);
    delay(10);
} 
void back()
{
    matrix_display(back_move);
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
    matrix_display(move);
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
  if (IrReceiver.decode()) 
  {
    ir_rec = IrReceiver.decodedIRData.command;
    Serial.print("Код кнопки: 0x");
    Serial.println(ir_rec, HEX);
    switch (ir_rec) 
    {
      case 0x46: forw();  break;
      case 0x15: back();  break;
      case 0x44: left();  break;
      case 0x43: right(); break;
      case 0x40: stop();  break;

      case 0x16:
        servo_pos += 15;
        if (servo_pos > 180) servo_pos = 180;
        servo.write(servo_pos);
        break;
        
      case 0x19:
        servo_pos -= 15;
        if (servo_pos < 0) servo_pos = 0;
        servo.write(servo_pos);
        break;

      default: 
        break;
    }

    IrReceiver.resume();
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