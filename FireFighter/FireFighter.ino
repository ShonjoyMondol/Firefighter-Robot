// Code By Shonjoy09
#include <SPI.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ================= LCD =================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= MOTOR PINS =================
#define LF 2
#define LB 3
#define RF 4
#define RB 5

#define EN1 10
#define EN2 11

// ================= OTHER PINS =================
#define RELAY_PIN 7
#define SERVO_PIN 9

#define FRONT_LED A1
#define BACK_LED  A2
#define BUZZER    A3

#define FLAME_SENSOR A0

Servo myservo;

char cmd;
int speedValue = 255;

bool extraMode = false;
bool shown = false;

unsigned long servoTimer = 0;
int servoPos = 0;
int servoStep = 2;

// ===================================================
void setup()
{
  Serial.begin(9600);

  pinMode(LF, OUTPUT);
  pinMode(LB, OUTPUT);
  pinMode(RF, OUTPUT);
  pinMode(RB, OUTPUT);

  pinMode(EN1, OUTPUT);
  pinMode(EN2, OUTPUT);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);   // OFF at startup

  pinMode(FRONT_LED, OUTPUT);
  pinMode(BACK_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  pinMode(FLAME_SENSOR, INPUT);

  analogWrite(EN1, speedValue);
  analogWrite(EN2, speedValue);

  myservo.attach(SERVO_PIN);
  myservo.write(90);

  // LCD
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("firefighter");
}
// ===================================================
void loop()
{


  // Bluetooth receive
  if (Serial.available())
  {
    cmd = Serial.read();
    processCommand(cmd);
  }

  // Servo sweep when Extra ON
  if (extraMode)
  {
    if (millis() - servoTimer > 20)
    {
      servoTimer = millis();

      servoPos += servoStep;

      if (servoPos >= 180)
      {
        servoPos = 180;
        servoStep = -2;
      }

      if (servoPos <= 0)
      {
        servoPos = 0;
        servoStep = 2;
      }

      myservo.write(servoPos);
    }
  }

  // Flame detection
  flameCheck();
}

// ===================================================
void processCommand(char c)
{
  switch(c)
  {
    // -------- Movement --------
    case 'F':
      forward();
      break;

    case 'B':
      backward();
      break;

    case 'L':
      left();
      break;

    case 'R':
      right();
      break;

    case 'S':
      stopCar();
      break;

    // -------- Front LED --------
    case 'W':
      digitalWrite(FRONT_LED, HIGH);
      break;

    case 'w':
      digitalWrite(FRONT_LED, LOW);
      break;

    // -------- Back LED --------
    case 'U':
      digitalWrite(BACK_LED, HIGH);
      break;

    case 'u':
      digitalWrite(BACK_LED, LOW);
      break;

    // -------- Buzzer --------
    case 'V':
      digitalWrite(BUZZER, HIGH);
      break;

    case 'v':
      digitalWrite(BUZZER, LOW);
      break;

    // -------- Extra --------
case 'X':
  digitalWrite(RELAY_PIN, LOW);   // Relay ON
  extraMode = true;
  break;

case 'x':
  digitalWrite(RELAY_PIN, HIGH);  // Relay OFF
  extraMode = false;
  myservo.write(90);
  break;

    // -------- Speed --------
    case '0': setSpeed(0);   break;
    case '1': setSpeed(25);  break;
    case '2': setSpeed(50);  break;
    case '3': setSpeed(75);  break;
    case '4': setSpeed(100); break;
    case '5': setSpeed(125); break;
    case '6': setSpeed(150); break;
    case '7': setSpeed(175); break;
    case '8': setSpeed(200); break;
    case '9': setSpeed(230); break;
    case 'q': setSpeed(255); break;
  }
}

// ===================================================
void setSpeed(int pwm)
{
  speedValue = pwm;

  analogWrite(EN1, speedValue);
  analogWrite(EN2, speedValue);
}

// ===================================================
void forward()
{
  digitalWrite(LF, HIGH);
  digitalWrite(LB, LOW);

  digitalWrite(RF, HIGH);
  digitalWrite(RB, LOW);
}

// ===================================================
void backward()
{
  digitalWrite(LF, LOW);
  digitalWrite(LB, HIGH);

  digitalWrite(RF, LOW);
  digitalWrite(RB, HIGH);
}

// ===================================================
void left()
{
  digitalWrite(LF, LOW);
  digitalWrite(LB, HIGH);

  digitalWrite(RF, HIGH);
  digitalWrite(RB, LOW);
}

// ===================================================
void right()
{
  digitalWrite(LF, HIGH);
  digitalWrite(LB, LOW);

  digitalWrite(RF, LOW);
  digitalWrite(RB, HIGH);
}

// ===================================================
void stopCar()
{
  digitalWrite(LF, LOW);
  digitalWrite(LB, LOW);

  digitalWrite(RF, LOW);
  digitalWrite(RB, LOW);
}

// ===================================================
void flameCheck()
{
  if (digitalRead(FLAME_SENSOR) == LOW)
  {
    shown = false;

    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Fire Alert !");

    for (int i = 0; i < 2; i++)
    {
      digitalWrite(BUZZER, HIGH);
      digitalWrite(BACK_LED, HIGH);
      delay(300);

      digitalWrite(BUZZER, LOW);
      digitalWrite(BACK_LED, LOW);
      delay(300);
    }
  }
  else
  {
    if (!shown)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Rescue and");
      lcd.setCursor(0, 1);
      lcd.print("Firefighter Robot");
  delay(2500);
      shown = true;
    }
  }
}
