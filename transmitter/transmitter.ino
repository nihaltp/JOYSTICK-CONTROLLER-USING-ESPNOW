/*
  Code for transmitter using Joystick and ESP-NOW 
  Made by: NIHAL T P
  GitHub: nihaltp
*/

#include <ESP8266WiFi.h>
#include <espnow.h>

// Mac Address of the Receiver
uint8_t receiverMAC[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // TODO: Replace Mac Address

const int X_AXIS_PIN = D1;
const int Y_AXIS_PIN = D2;

const boolean SERIAL_PORT = true;  // TODO: Change

typedef struct packetData {
  int x;
  int y;
} packetData;

packetData controls;

void onDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  if (SERIAL_PORT) {
    Serial.print("Send Status:\t");
    Serial.println(sendStatus == 0 ? "Success" : "Fail");
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  while (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  } Serial.println("ESP-NOW Initialized Successfully");

  // Register the send callback
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(onDataSent);
  esp_now_add_peer(receiverMAC, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  pinMode(X_AXIS_PIN, INPUT);
  pinMode(Y_AXIS_PIN, INPUT);
}

void loop() {
  controls.x = analogRead(X_AXIS_PIN);
  controls.y = analogRead(Y_AXIS_PIN);

  if (SERIAL_PORT) {
    Serial.print("X Axis:");    Serial.print(controls.x);
    Serial.print("\tY Axis:");  Serial.println(controls.y);
  }
  esp_now_send(receiverMAC, (uint8_t *) &controls, sizeof(controls));
}