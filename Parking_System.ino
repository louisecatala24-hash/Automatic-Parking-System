#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <string.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
Servo gateServo;

const byte IR1 = 2;
const byte IR2 = 3;
const byte SERVO_PIN = 4;
const byte LDR_PIN = A0;
const byte LED1 = 8;
const byte LED2 = 9;
const byte LED3 = 10;
const int CAPACITY = 4;
const int OPEN_ANGLE = 0;
const int CLOSED_ANGLE = 100;
const int DARK_THRESHOLD = 450;
const int BRIGHT_THRESHOLD = 550;
const unsigned long CLEAR_TIME = 500;

int availableSlots = CAPACITY;
enum GateState { IDLE, ENTERING, EXITING, WAIT_ENTRY_CLEAR, WAIT_EXIT_CLEAR };
GateState state = IDLE;
bool lightsOn = false;
bool clearTimerRunning = false;
unsigned long clearStarted = 0;
const char* lastMessage = "";
int lastDisplayedSlots = -1;

void showStatus(const char* message) {
  if (strcmp(lastMessage, message) == 0 &&
      lastDisplayedSlots == availableSlots) return;
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print(message);
  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("Slots: ");
  lcd.print(availableSlots);
  lcd.print(" / ");
  lcd.print(CAPACITY);
  lastMessage = message;
  lastDisplayedSlots = availableSlots;
}

void updateLights() {
  int reading = analogRead(LDR_PIN);
  if (reading < DARK_THRESHOLD) lightsOn = true;
  else if (reading > BRIGHT_THRESHOLD) lightsOn = false;
  digitalWrite(LED1, lightsOn ? HIGH : LOW);
  digitalWrite(LED2, lightsOn ? HIGH : LOW);
  digitalWrite(LED3, lightsOn ? HIGH : LOW);
}

bool sensorsClearFor(bool outside, bool inside, unsigned long interval) {
  if (outside || inside) {
    clearTimerRunning = false;
    return false;
  }
  if (!clearTimerRunning) {
    clearStarted = millis();
    clearTimerRunning = true;
  }
  return millis() - clearStarted >= interval;
}

void closeGate() {
  gateServo.write(CLOSED_ANGLE);
  state = IDLE;
  clearTimerRunning = false;
  showStatus("Parking Ready");
}

void setup() {
  Serial.begin(9600);
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  gateServo.attach(SERVO_PIN);
  gateServo.write(CLOSED_ANGLE);
  lcd.init();
  lcd.backlight();
  showStatus("Parking Ready");
}

void loop() {
  updateLights();
  bool outside = digitalRead(IR1) == LOW;
  bool inside = digitalRead(IR2) == LOW;
  switch (state) {
    case IDLE:
      if (outside && !inside) {
        if (availableSlots > 0) {
          gateServo.write(OPEN_ANGLE);
          state = ENTERING;
          clearTimerRunning = false;
          showStatus("Car Entering");
        } else showStatus("Parking Full");
      } else if (inside && !outside) {
        gateServo.write(OPEN_ANGLE);
        state = EXITING;
        clearTimerRunning = false;
        showStatus("Car Leaving");
      } else if (!outside && !inside) showStatus("Parking Ready");
      break;
    case ENTERING:
      if (inside) {
        state = WAIT_ENTRY_CLEAR;
        clearTimerRunning = false;
      } else if (sensorsClearFor(outside, inside, 2000)) closeGate();
      break;
    case EXITING:
      if (outside) {
        state = WAIT_EXIT_CLEAR;
        clearTimerRunning = false;
      } else if (sensorsClearFor(outside, inside, 2000)) closeGate();
      break;
    case WAIT_ENTRY_CLEAR:
      if (sensorsClearFor(outside, inside, CLEAR_TIME)) {
        if (availableSlots > 0) availableSlots--;
        closeGate();
      }
      break;
    case WAIT_EXIT_CLEAR:
      if (sensorsClearFor(outside, inside, CLEAR_TIME)) {
        if (availableSlots < CAPACITY) availableSlots++;
        closeGate();
      }
      break;
  }
}
