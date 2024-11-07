/* Code for receiver using ESP-NOW and L298N Motor Driver
  Made by: NIHAL T P
  GitHub: nihaltp
*/

#include <ESP8266WiFi.h>
#include <espnow.h>

const int L1  = D0; // Left  1
const int L2  = D1; // Left  2
const int ENL = D2; // Left  Enable
const int R1  = D3; // Right 1
const int R2  = D4; // Right 2
const int ENR = D5; // Right Enable

const int DEAD_ZONE = 10; // To Avoid Unnecessary movements when joystick close to center // TODO: Change
const int JS_MAX = 1023; // Maximum value of Joystick
int X_CENTER = 0; // Value of X axis when joystick is untouched
int Y_CENTER = 0; // Value of Y axis when joystick is untouched
const int WORKING_RANGE = 255; // Maximum value of PWM
boolean SERIAL_PORT = true;  // Change to false if Serial monitor is not needed // TODO: Change
const int numSamples = 100; // Number of samples to average to find X_CENTER and Y_CENTER // TODO: Change

typedef struct packetData {
  int x;
  int y;
} packetData;

packetData controls;

void onReceive(uint8_t *mac_addr, uint8_t *incomingData, uint8_t len) {
  memcpy(&controls, incomingData, sizeof(controls));
  if (SERIAL_PORT) {
    Serial.print("Received commands:");  Serial.print("\t");
    Serial.print(controls.x);            Serial.print("\t");
    Serial.println(controls.y);
  }
  
  if (abs(controls.x - X_CENTER) < DEAD_ZONE) {
    controls.x = X_CENTER;
  }
  if (abs(controls.y - Y_CENTER) < DEAD_ZONE) {
    controls.y = Y_CENTER;
  }
  simpleMovements();
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  
  // Initialize ESP-NOW
  while (esp_now_init() != 0) {
    Serial.print(".");
  } Serial.println("ESP-NOW Initialized Successfully");
  
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(onReceive);
  
  calibrateJoystick();
  
  pinMode(L1,  OUTPUT);
  pinMode(L2,  OUTPUT);
  pinMode(ENL, OUTPUT);
  pinMode(R1,  OUTPUT);
  pinMode(R2,  OUTPUT);
  pinMode(ENR, OUTPUT);
}

void loop() {}

void calibrateJoystick() {
  int xArray[numSamples];      // Array to store X-axis values
  int yArray[numSamples];      // Array to store Y-axis values
  long xSum = 0, ySum = 0;     // Sum of X-axis and Y-axis values
  int xError = 0, yError = 0;  // Counter for X-axis and Y-axis Error
  
  for (int i = 0; i < numSamples; i++) {
    xArray[i] = controls.x;
    yArray[i] = controls.y;
    delay(10);
  }
  for (int i = 0; i < numSamples; i++) {
    xSum += xArray[i];
    ySum += yArray[i];
  }
  
  int xMean = xSum / numSamples;  // Mean for X-axis
  int yMean = ySum / numSamples;  // Mean for Y-axis
  xSum = 0;
  ySum = 0;
  
  for (int i = 0; i < numSamples; i++) {
    if (abs(xArray[i] - xMean) < DEAD_ZONE) {
      xSum += xArray[i];
    } else {
      xError++;
    }
    if (abs(yArray[i] - yMean) < DEAD_ZONE) {
      ySum += yArray[i];
    } else {
      yError++;
    }
  }
  
  X_CENTER = xSum / (numSamples - xError);
  Y_CENTER = ySum / (numSamples - yError);
  
  if (SERIAL_PORT) {
    Serial.println("Calibration complete.");
    Serial.print("X_CENTER: ");  Serial.print(X_CENTER);
    Serial.print("\tY_CENTER: ");  Serial.println(Y_CENTER);
  }
}

void simpleMovements () {
  int xMapped, yMapped;
  int left, right;
  
  handleJoystickInput(controls.x, controls.y, xMapped, yMapped);
  
  if (SERIAL_PORT) {
    Serial.print("Mapped values:");  Serial.print("\t");
    Serial.print(xMapped);           Serial.println("\t");
    Serial.println(yMapped);
  }
  
  calculateMotorSpeeds(xMapped, yMapped, left, right);
  
  if (SERIAL_PORT) {
    Serial.print("Motor speeds:");  Serial.print("\t");
    Serial.print(left);             Serial.print("\t");
    Serial.println(right);
  }
  rotateMotors(left, right);
}

void handleJoystickInput(int x, int y, int &xMapped, int &yMapped) {
  xMapped = (x > X_CENTER) ? map(x, X_CENTER, JS_MAX, 0, WORKING_RANGE) :
            (x < X_CENTER) ? map(x, 0, X_CENTER, -WORKING_RANGE, 0) : 0;
  yMapped = (y > Y_CENTER) ? map(y, Y_CENTER, JS_MAX, 0, WORKING_RANGE) :
            (y < Y_CENTER) ? map(y, 0, Y_CENTER, -WORKING_RANGE, 0) : 0;
}

void calculateMotorSpeeds(int x, int y, int &left, int &right) {
  if (x == 0 && y == 0) {
    left = right = 0;
    return;
  }
  if (y == 0) {
    left = right = x;
    return;
  }
  if (x == 0) {
    left = (y > 0) ? y : 0;
    right = (y < 0) ? y : 0;
    return;
  }
  
  int speed = (x > 0) ? 
    calculateForwardSpeed(x, 0) :
    calculateBackwardSpeed(x, 0);
  
  if (x > 0) {
    left = (y > 0) ?
    calculateForwardSpeed(x, y) :
    speed;
    right = (y < 0) ?
    calculateForwardSpeed(x, -y) :
    speed;
  } else if (x < 0) {
    left = speed;
    right = (y > 0) ?
      calculateBackwardSpeed(x, -y) :
      calculateBackwardSpeed(x, y);
  }
}

int calculateForwardSpeed(int a, int b) {
  return map(a + b, 0, WORKING_RANGE * 2, 0, WORKING_RANGE);
}

int calculateBackwardSpeed(int a, int b) {
  return map(a + b, WORKING_RANGE * 2, 0, -WORKING_RANGE, 0);
}

void rotateMotors(int x, int y) {
  digitalWrite(L1, (x > 0) ? HIGH : LOW);
  digitalWrite(L2, (x < 0) ? HIGH : LOW);
  analogWrite(ENL, abs(x));
  digitalWrite(R1, (y > 0) ? HIGH : LOW);
  digitalWrite(R2, (y < 0) ? HIGH : LOW);
  analogWrite(ENR, abs(y));
}