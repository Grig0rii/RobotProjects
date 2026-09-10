#include <Servo.h>

 
// TINY IR RECEIVER
 

#define IR_RECEIVE_PIN 3
#define NO_LED_RECEIVE_FEEDBACK_CODE

#include <TinyIRReceiver.hpp>


 
// PINS
 

#define ML_Ctrl 4
#define ML_PWM  6
#define MR_Ctrl 2
#define MR_PWM  5

#define TRIG_PIN 12
#define ECHO_PIN 13

#define SCL_Pin A5
#define SDA_Pin A4

#define LED_PIN 9
#define SERVO_PIN 10


Servo servo;


 
// SELECT PROJECT
 

byte currentProject = 1;


 
// MATRIX
 

unsigned char move[] = {
  0x00, 0x7e, 0x12, 0x12, 0x7e, 0x00, 0x7e, 0x42,
  0x42, 0x42, 0x00, 0x4e, 0x2a, 0x1a, 0x7e, 0x00
};

unsigned char smile[] = {
  0x00, 0x7e, 0x12, 0x12, 0x7e, 0x00, 0x7e, 0x42,
  0x42, 0x42, 0x00, 0x4e, 0x2a, 0x1a, 0x7e, 0x00
};

unsigned char forward_move[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x0c, 0x7e,
  0x7e, 0x0c, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char back_move[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x30, 0x7e,
  0x7e, 0x30, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00
};

unsigned char icon_right[] = {
  0x00, 0x18, 0x3c, 0x7e, 0x5a, 0x5a, 0x5a, 0x18,
  0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00
};

unsigned char icon_left[] = {
  0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
  0x18, 0x5a, 0x5a, 0x7e, 0x3c, 0x18, 0x00, 0x00
};

unsigned char eye_normal[] = {
  0x00, 0x18, 0x24, 0x24, 0x18, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x18, 0x24, 0x24, 0x18, 0x00
};

unsigned char alarm_signal[] = {
  0x00, 0x00, 0xbf, 0x00, 0x00, 0xbf, 0x00, 0xbf,
  0xbf, 0x00, 0xbf, 0x00, 0x00, 0xbf, 0x00, 0x00
};


 
// PROJECT 1
 

int oldExtended = 0;
int isExtended = 0;
int sensorValue = 0;
int outputValue = 0;
int sensorValue1 = 0;
int outputValue1 = 0;



// PROJECT 2


int lastTurn = 0;



// PROJECT 3
 

int centerAngle = 45;
int currentAngle = 45;
bool scanningLeft = true;

const int DIST_TOO_CLOSE = 10;
const int DIST_FOLLOW = 40;


 
// PROJECT 4
 

int servo_pos = 45;


 
// PROJECT 5
 

bool alarm = false;
bool ledState = false;

unsigned long prevMillis = 0;
unsigned long routeStart = 0;


 
// FUNCTION DECLARATIONS
 

float getDistance();

void stop();

void project1();
void project2();
void project3();
void project4();
void project5();

void changeProject(byte newProject);
void checkBluetooth();

void forw(byte speed = 100);
void back(byte speed = 200);
void left(byte speed = 110);
void right(byte speed = 110);


 
// SETUP
 

void setup()
{
  Serial.begin(9600);

  // Servo
  servo.attach(SERVO_PIN, 350, 2450);
  servo.write(45);

  // Ultrasonic
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Matrix
  pinMode(SCL_Pin, OUTPUT);
  pinMode(SDA_Pin, OUTPUT);

  // Motors
  pinMode(ML_Ctrl, OUTPUT);
  pinMode(ML_PWM, OUTPUT);

  pinMode(MR_Ctrl, OUTPUT);
  pinMode(MR_PWM, OUTPUT);

  // LED
  pinMode(LED_PIN, OUTPUT);

  // IR
  pinMode(IR_RECEIVE_PIN, INPUT);

  // Stop motors
  stop();

  // Start Tiny IR receiver
  initPCIInterruptForTinyIRReceiver();

  matrix_display(move);

  Serial.println("ROBOT READY");
  Serial.println("PROJECT_1 ... PROJECT_5");
  Serial.println("TINY IR RECEIVER READY");
}


 
// BLUETOOTH / UART
 

void checkBluetooth()
{
  if (Serial.available() == 0)
    return;

  String command = Serial.readStringUntil('\n');

  command.trim();
  command.toUpperCase();

  if (command == "PROJECT_1")
  {
    changeProject(1);
  }
  else if (command == "PROJECT_2")
  {
    changeProject(2);
  }
  else if (command == "PROJECT_3")
  {
    changeProject(3);
  }
  else if (command == "PROJECT_4")
  {
    changeProject(4);
  }
  else if (command == "PROJECT_5")
  {
    changeProject(5);
  }
  else if (command == "STOP")
  {
    stop();
  }
}


 
// CHANGE PROJECT
 

void changeProject(byte newProject)
{
  stop();

  alarm = false;
  ledState = false;

  digitalWrite(LED_PIN, LOW);

  currentProject = newProject;

  currentAngle = 45;
  scanningLeft = true;

  servo_pos = 45;
  servo.write(45);

  matrix_display(move);

  Serial.print("PROJECT_CHANGED:");
  Serial.println(currentProject);
}


 
// MAIN LOOP
 

void loop()
{
  checkBluetooth();

  switch (currentProject)
  {
    case 1:
      project1();
      break;

    case 2:
      project2();
      break;

    case 3:
      project3();
      break;

    case 4:
      project4();
      break;

    case 5:
      project5();
      break;
  }
}


 
// MOTORS
 

void forw(byte speed)
{
  digitalWrite(ML_Ctrl, HIGH);
  analogWrite(ML_PWM, speed);

  digitalWrite(MR_Ctrl, HIGH);
  analogWrite(MR_PWM, speed);
}


void back(byte speed)
{
  digitalWrite(ML_Ctrl, LOW);
  analogWrite(ML_PWM, speed);

  digitalWrite(MR_Ctrl, LOW);
  analogWrite(MR_PWM, speed);
}


void left(byte speed)
{
  digitalWrite(ML_Ctrl, LOW);
  analogWrite(ML_PWM, speed);

  digitalWrite(MR_Ctrl, HIGH);
  analogWrite(MR_PWM, speed);
}


void right(byte speed)
{
  digitalWrite(ML_Ctrl, HIGH);
  analogWrite(ML_PWM, speed);

  digitalWrite(MR_Ctrl, LOW);
  analogWrite(MR_PWM, speed);
}


void stop()
{
  analogWrite(ML_PWM, 0);
  analogWrite(MR_PWM, 0);

  digitalWrite(ML_Ctrl, LOW);
  digitalWrite(MR_Ctrl, LOW);
}


 
// ULTRASONIC
 

float getDistance()
{
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(3);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);

  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration =
    pulseIn(ECHO_PIN, HIGH, 30000);

  if (duration == 0)
  {
    return 400.0;
  }

  float distance = duration / 58.0;

  if (distance < 2.0 || distance > 400.0)
  {
    return 400.0;
  }

  return distance;
}


 
// PROJECT 1
 

void project1()
{
  matrix_display(smile);

  sensorValue = analogRead(A1);
  sensorValue1 = analogRead(A2);

  outputValue = map(sensorValue, 0, 1023, 0, 255);
  outputValue1 = map(sensorValue1, 0, 1023, 0, 255);

  float distance = getDistance();

  if (distance > 0 && distance <= 20)
  {
    do
    {
      isExtended = random(1, 4);
    }
    while (isExtended == oldExtended);

    oldExtended = isExtended;

    if (isExtended == 1)
      servo.write(90);
    else if (isExtended == 2)
      servo.write(45);
    else
      servo.write(0);
  }

  if (sensorValue > 500 || sensorValue1 > 500)
  {
    digitalWrite(LED_PIN, LOW);
  }
  else
  {
    digitalWrite(LED_PIN, HIGH);
  }

  delay(50);
}


 
// PROJECT 2
 

void project2()
{
  matrix_display(move);

  servo.write(47);
  delay(50);

  float distance = getDistance();

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


 
// PROJECT 3
 

void project3()
{
  servo.write(currentAngle);
  delay(35);

  float distance = getDistance();

  if (distance > DIST_TOO_CLOSE &&
      distance <= DIST_FOLLOW)
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
  else if (distance <= DIST_TOO_CLOSE &&
           distance > 1.0)
  {
    back(115);
    delay(40);
  }
  else
  {
    stop();

    if (scanningLeft)
    {
      currentAngle += 4;

      if (currentAngle >= 75)
        scanningLeft = false;
    }
    else
    {
      currentAngle -= 4;

      if (currentAngle <= 15)
        scanningLeft = true;
    }
  }

  matrix_display(eye_normal);
}


 
// PROJECT 4 - IR CONTROL
 

void project4()
{
  if (TinyIRReceiverDecode())
  {
    byte command = TinyIRReceiverData.Command;

    Serial.print("IR CODE: 0x");
    Serial.println(command, HEX);

    switch (command)
    {
      // FORWARD
      case 0x46:

        matrix_display(forward_move);
        forw(100);

        break;


      // BACK
      case 0x15:

        matrix_display(back_move);
        back(200);

        break;


      // LEFT
      case 0x44:

        matrix_display(icon_left);
        left(200);

        break;


      // RIGHT
      case 0x43:

        matrix_display(icon_right);
        right(200);

        break;


      // STOP
      case 0x40:

        matrix_display(move);
        stop();

        break;


      // SERVO RIGHT
      case 0x16:

        servo_pos += 15;

        if (servo_pos > 180)
          servo_pos = 180;

        servo.write(servo_pos);

        break;


      // SERVO LEFT
      case 0x19:

        servo_pos -= 15;

        if (servo_pos < 0)
          servo_pos = 0;

        servo.write(servo_pos);

        break;
    }
  }
}


 
// PROJECT 5
 

void project5()
{
  float distance = getDistance();

  if (!alarm &&
      distance > 0 &&
      distance < 40)
  {
    alarm = true;

    Serial.println("ALARM!");

    routeStart = millis();
  }

  if (alarm)
  {
    unsigned long t = millis() - routeStart;

    if (t < 2500)
    {
      back(200);
    }
    else if (t < 4200)
    {
      left(200);
    }
    else if (t < 17800)
    {
      forw(100);
    }
    else
    {
      stop();
    }

    if (millis() - prevMillis >= 500)
    {
      prevMillis = millis();

      ledState = !ledState;

      digitalWrite(LED_PIN, ledState);

      if (ledState)
      {
        matrix_display(alarm_signal);
        servo.write(0);
      }
      else
      {
        matrix_clear();
        servo.write(90);
      }
    }
  }
  else
  {
    stop();

    matrix_display(move);

    digitalWrite(LED_PIN, LOW);

    servo.write(45);
  }
}


 
// MATRIX
 

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


void matrix_clear()
{
  IIC_start();

  IIC_send(0xc0);

  for (int i = 0; i < 16; i++)
  {
    IIC_send(0x00);
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
    if (send_data & mask)
      digitalWrite(SDA_Pin, HIGH);
    else
      digitalWrite(SDA_Pin, LOW);

    delayMicroseconds(3);

    digitalWrite(SCL_Pin, HIGH);

    delayMicroseconds(3);

    digitalWrite(SCL_Pin, LOW);
  }
}
