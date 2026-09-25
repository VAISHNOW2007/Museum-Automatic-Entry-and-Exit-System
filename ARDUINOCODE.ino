#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define ENTRY_IR  2
#define EXIT_IR   3
#define PIR_PIN   5
#define BUZZER    8
#define SERVO_PIN 9

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo gate;

// ================= VARIABLES =================

int people = 0;
const int maxPeople = 10;

bool entryReady = true;
bool exitReady = true;

unsigned long lastLCD = 0;
unsigned long lastPIR = 0;

byte screen = 0;


// ================= SETUP =================

void setup() {

  Serial.begin(9600);

  pinMode(ENTRY_IR, INPUT);
  pinMode(EXIT_IR, INPUT);
  pinMode(PIR_PIN, INPUT);
  pinMode(BUZZER, OUTPUT);

  noTone(BUZZER);

  // Servo
  gate.attach(SERVO_PIN);
  gate.write(0);

  // LCD
  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("VCET MUSEUM");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  delay(2000);

  lcd.clear();
}


// ================= OPEN GATE =================

void openGate() {

  Serial.println("GATE OPEN");

  gate.write(90);

  delay(1500);

  gate.write(0);

  Serial.println("GATE CLOSED");
}


// ================= ENTRY =================

void checkEntry() {

  int state = digitalRead(ENTRY_IR);

  // LOW = object detected

  if (state == LOW && entryReady) {

    entryReady = false;

    Serial.println("ENTRY DETECTED");

    if (people < maxPeople) {

      people++;

      Serial.print("ENTERED: ");
      Serial.println(people);

      // Entry beep
      tone(BUZZER, 1500);
      delay(100);
      noTone(BUZZER);

      // Open gate
      openGate();
    }

    else {

      Serial.println("MUSEUM FULL");

      gate.write(0);
    }
  }

  // Sensor clear
  if (state == HIGH) {

    entryReady = true;
  }
}


// ================= EXIT =================

void checkExit() {

  int state = digitalRead(EXIT_IR);

  // LOW = object detected

  if (state == LOW && exitReady) {

    exitReady = false;

    Serial.println("EXIT DETECTED");

    if (people > 0) {

      people--;

      Serial.print("EXITED: ");
      Serial.println(people);

      // Open gate for exit
      openGate();
    }
  }

  // Sensor clear
  if (state == HIGH) {

    exitReady = true;
  }
}


// ================= FULL BUZZER =================

void checkBuzzer() {

  if (people >= maxPeople) {

    // Continuous buzzer when 10 people reached
    tone(BUZZER, 2000);
  }

  else {

    noTone(BUZZER);
  }
}


// ================= PIR =================

void checkPIR() {

  int motion = digitalRead(PIR_PIN);

  if (motion == HIGH) {

    if (millis() - lastPIR > 2000) {

      lastPIR = millis();

      Serial.println("MOTION DETECTED");
    }
  }
}


// ================= LCD =================

void updateLCD() {

  if (millis() - lastLCD < 2000) {
    return;
  }

  lastLCD = millis();

  lcd.clear();

  if (screen == 0) {

    lcd.setCursor(0, 0);
    lcd.print("VCET MUSEUM");

    lcd.setCursor(0, 1);
    lcd.print("People:");
    lcd.print(people);
    lcd.print("/");
    lcd.print(maxPeople);

    screen = 1;
  }

  else {

    lcd.setCursor(0, 0);

    if (people >= maxPeople) {
      lcd.print("MUSEUM FULL");
    }
    else {
      lcd.print("ENTRY AVAILABLE");
    }

    lcd.setCursor(0, 1);
    lcd.print("Count: ");
    lcd.print(people);

    screen = 0;
  }
}


// ================= LOOP =================

void loop() {

  checkEntry();

  checkExit();

  checkPIR();

  checkBuzzer();

  updateLCD();
}