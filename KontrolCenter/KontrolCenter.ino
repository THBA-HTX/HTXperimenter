/*
* HTXperimenter - KontrolCenter
* The Dumb Version
*
*/
#define SW_1 4
#define SW_2 5
#define SW_3 10

#define LED_STATUS 13
#define RELAY_1 6
#define RELAY_2 7
#define RELAY_3 8
#define RELAY_4 9

#define TIMEOUT 1 * 60
#define ALARM_ON_TIME 8

#include <LCD_I2C.h>

LCD_I2C lcd(0x27, 16, 2);
enum State { IDLE,
             RUNNING,
             ALARM };
State currState = IDLE;

char buffer[16];
bool timeHasChanged = false;
unsigned long timeStamp;

int secondsTotal = TIMEOUT;
int minutes;
int seconds;
int numberOfRound = 1;

void setup() {
  pinMode(SW_1, INPUT_PULLUP);
  pinMode(SW_2, INPUT_PULLUP);
  pinMode(SW_3, INPUT_PULLUP);

  pinMode(LED_STATUS, OUTPUT);

  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);

  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  lcd.print("KontrolCenter ");
  lcd.setCursor(0, 1);
  lcd.print("v0.3b");
  delay(3000);
  lcd.clear();

  setStatus("IDLE.    ");
  timeHasChanged = true;

  timeStamp = millis();
}

void loop() {
  if (millis() - timeStamp > 1000 && currState != IDLE) {
    timeStamp = millis();
    secondsTotal--;
    timeHasChanged = true;
  }

  if (currState == IDLE) {           // STATE = IDLE
    if (digitalRead(SW_1) == LOW) {  // START TIMER
      delay(100);
      setStatus("RUNNING.");
      currState = RUNNING;
      delay(100);
    }

    if (digitalRead(SW_2) == LOW)  // RESET RESET
    {
      secondsTotal = TIMEOUT;
      timeHasChanged = true;
      delay(100);
    }
  }

  if (currState == ALARM) {  // ALARM STATE

    if (secondsTotal <= 0) {
      numberOfRound++;
      secondsTotal = TIMEOUT;
      timeHasChanged = true;
      relayAllOff();
      setStatus("RUNNING.  ");
      currState = RUNNING;
      Serial.println("ALARM -> RUNNING");
    }
  }

  if (currState == RUNNING) {        // STATE = RUNNING
    if (digitalRead(SW_1) == LOW) {  // GOTO IDLE ..!!
      delay(100);
      setStatus("IDLE.     ");
      currState = IDLE;
    }
    if (secondsTotal <= 0) {
      Serial.println("RUNNING -> ALARM.");
      setStatus("ALARM.   ");
      relayAllOn();
      secondsTotal = ALARM_ON_TIME;
      timeHasChanged = true;
      currState = ALARM;
    }
  }

  if (timeHasChanged == true) {
    displayTime();
    timeHasChanged = false;
  }
}

void displayTime() {
  minutes = secondsTotal / 60;
  seconds = secondsTotal % 60;
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  sprintf(buffer, "%02d)  %02d:%02d", numberOfRound, minutes, seconds);
  lcd.print(buffer);
}

void setStatus(char* ptr) {
  lcd.setCursor(0, 1);
  lcd.print(ptr);
}


void relayAllOff() {
  digitalWrite(RELAY_1, LOW);
  digitalWrite(RELAY_2, LOW);
  digitalWrite(RELAY_3, LOW);
  digitalWrite(RELAY_4, LOW);
}

void relayAllOn() {
  digitalWrite(RELAY_1, HIGH);
  digitalWrite(RELAY_2, HIGH);
  digitalWrite(RELAY_3, HIGH);
  digitalWrite(RELAY_4, HIGH);
}