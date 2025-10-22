#include <Servo.h>

Servo servoX;      // horizontal movement
Servo servoY;      // vertical movement
Servo servoBlink;  // eyelid servo

int servoXPin = 5;
int servoYPin = 6;
int servoBlinkPin = 9; // eyelid pin
int blinkLEDPin = 4;

int servoXAngle = 0;
int servoYAngle = 0;
int lastServoXAngle = 0;

bool isBlinking = false;        
unsigned long blinkStartTime = 0;
int blinkDuration = 200;        
int eyelidOffset = 10;          

unsigned long lastAutoBlink = 0;
int autoBlinkInterval = 5000;   

// Buffer for serial input
char inputBuffer[32];
byte inputPos = 0;

void setup() {
  Serial.begin(9600);

  servoX.attach(servoXPin);
  servoY.attach(servoYPin);
  servoBlink.attach(servoBlinkPin);

  servoX.write(90);
  servoY.write(90);
  servoBlink.write(0); // eyelid open

  pinMode(blinkLEDPin, OUTPUT);
  delay(1000);
}

void loop() {
  unsigned long currentMillis = millis();

  // --- Handle serial data (using cstring) ---
  while (Serial.available() > 0) {
    char c = Serial.read();

    if (c == '\n' || c == '\r') {
      if (inputPos > 0) {
        inputBuffer[inputPos] = '\0'; // null-terminate string
        processSerialData(inputBuffer);
        inputPos = 0; // reset for next line
      }
    } else if (inputPos < sizeof(inputBuffer) - 1) {
      inputBuffer[inputPos++] = c;
    }
  }

  // --- Automatic blink every few seconds ---
  if (!isBlinking && currentMillis - lastAutoBlink >= autoBlinkInterval) {
    isBlinking = true;
    blinkStartTime = currentMillis;
    servoBlink.write(90);  // close eyelid
    lastAutoBlink = currentMillis;
  }

  // --- Blink animation timing ---
  if (isBlinking && currentMillis - blinkStartTime > blinkDuration) {
    isBlinking = false;
  }

  // --- Eyelid follows Y if not blinking ---
  if (!isBlinking) {
    int eyelidAngle = servoYAngle - eyelidOffset;
    eyelidAngle = constrain(eyelidAngle, 0, 90);
    servoBlink.write(eyelidAngle);
  }
}

// --- Parse and apply servo angles from serial data ---
void processSerialData(char *data) {
  // Example data: "350,200"
  char *comma = strchr(data, ',');
  if (comma) {
    *comma = '\0'; // split at comma
    int cx = atoi(data);
    int cy = atoi(comma + 1);

    servoXAngle = map(cx, 0, 1000, 40, 120);
    servoYAngle = map(cy, 0, 500, 180, 0);
    servoXAngle = constrain(servoXAngle, 0, 180);
    servoYAngle = constrain(servoYAngle, 100, 180);

    unsigned long currentMillis = millis();

    // Trigger blink if X movement is fast
    if (abs(lastServoXAngle - servoXAngle) > 10) {
      digitalWrite(blinkLEDPin, HIGH);
      if (!isBlinking) {
        isBlinking = true;
        blinkStartTime = currentMillis;
        servoBlink.write(90); // close eyelid
      }
    } else {
      digitalWrite(blinkLEDPin, LOW);
    }

    servoX.write(servoXAngle);
    servoY.write(servoYAngle);
    lastServoXAngle = servoXAngle;
    delay(10);
  }
}
