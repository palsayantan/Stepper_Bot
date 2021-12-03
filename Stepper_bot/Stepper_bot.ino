#include <Wire.h>
#include <VL53L0X.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include "index.h"

//Enter your SSID and PASSWORD
const char* ssid = "SSID";
const char* password = "PASSWORD";

#define led   2

int en[4] = {25, 14, 17, 19};
int dir[4] = {32, 26, 4, 5};
int step[4] = {33, 27, 16, 18};
int xshut[4] = {12, 23, 15, 13};
int inputs[4] = {36, 39, 34, 35};

WebServer server(80);

VL53L0X sensor1;
VL53L0X sensor2;
VL53L0X sensor3;
VL53L0X sensor4;

String state = "stop";
char distVal[128];
DynamicJsonDocument doc(256);

void handleRoot() {
  String page = MAIN_page;
  server.send(200, "text/html", page); //Send web page
}

void handleStepper() {
  state = server.arg("state");
  Serial.println(state);
}

void handleDistance() {
  doc["v1"] = sensor4.readRangeContinuousMillimeters();
  doc["v2"] = sensor2.readRangeContinuousMillimeters();
  doc["v3"] = sensor3.readRangeContinuousMillimeters();
  doc["v4"] = sensor1.readRangeContinuousMillimeters();
  serializeJson(doc, distVal);
  server.send(200, "application/json", distVal);
}

void setup() {
  Serial.begin(115200);
  Wire.begin();

  pinMode(led, OUTPUT);
  for (int i = 0; i < 4; i++) {
    pinMode(en[i], OUTPUT);
    pinMode(dir[i], OUTPUT);
    pinMode(step[i], OUTPUT);
    pinMode(xshut[i], OUTPUT);
    pinMode(inputs[i], OUTPUT);
    digitalWrite(xshut[i], LOW);
    digitalWrite(en[i], HIGH);
  }

  init_VL53L0X();

  //ESP As Server
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  int count;
  //Wait for WiFi to connect
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(led, !digitalRead(led));
    delay(300);
    count++;
    if (count > 10) ESP.restart();
  }

  init_OTA();

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP

  server.on("/", handleRoot);
  server.on("/stepper", handleStepper);
  server.on("/distance", handleDistance);
  server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  wifi_check();
  ArduinoOTA.handle();
  server.handleClient();
  controlStepper();
}

void controlStepper() {
  if (state == "stop") {
    for (int i = 0; i < 4; i++) {
      digitalWrite(en[i], HIGH);
    }
  }
  else {
    for (int i = 0; i < 4; i++) {
      digitalWrite(en[i], LOW);
    }
  }
  if (state == "front") {
    for (int i = 0; i < 4; i++) {
      digitalWrite(dir[i], HIGH);
      digitalWrite(step[i], HIGH);
      delayMicroseconds(1000);
      digitalWrite(step[i], LOW);
      delayMicroseconds(1000);
    }
  }
  if (state == "back") {
    for (int i = 0; i < 4; i++) {
      digitalWrite(dir[i], LOW);
      digitalWrite(step[i], HIGH);
      delayMicroseconds(1000);
      digitalWrite(step[i], LOW);
      delayMicroseconds(1000);
    }
  }
  if (state == "left") {
    digitalWrite(dir[0], HIGH);
    digitalWrite(dir[1], HIGH);
    digitalWrite(dir[2], LOW);
    digitalWrite(dir[3], LOW);
    for (int i = 0; i < 4; i++) {
      digitalWrite(step[i], HIGH);
      delayMicroseconds(1000);
      digitalWrite(step[i], LOW);
      delayMicroseconds(1000);
    }
  }
  if (state == "right") {
    digitalWrite(dir[0], LOW);
    digitalWrite(dir[1], LOW);
    digitalWrite(dir[2], HIGH);
    digitalWrite(dir[3], HIGH);
    for (int i = 0; i < 4; i++) {
      digitalWrite(step[i], HIGH);
      delayMicroseconds(1000);
      digitalWrite(step[i], LOW);
      delayMicroseconds(1000);
    }
  }
}

void init_VL53L0X() {
  Serial.print("Setting VL53L0X addresses...");
  digitalWrite(xshut[0], HIGH);
  delay(100);
  sensor1.init(true);
  delay(100);
  sensor1.setAddress((uint8_t)01);
  delay(100);

  digitalWrite(xshut[1], HIGH);
  delay(100);
  sensor2.init(true);
  delay(100);
  sensor2.setAddress((uint8_t)02);
  delay(100);

  digitalWrite(xshut[2], HIGH);
  delay(100);
  sensor3.init(true);
  delay(100);
  sensor3.setAddress((uint8_t)03);
  delay(100);

  digitalWrite(xshut[3], HIGH);
  delay(100);
  sensor4.init(true);
  delay(100);
  sensor4.setAddress((uint8_t)04);
  delay(100);
  Serial.println("Done!");

  sensor1.startContinuous();
  sensor2.startContinuous();
  sensor3.startContinuous();
  sensor4.startContinuous();
}

void wifi_check() {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(led, HIGH);
  }
  if (WiFi.status() != WL_CONNECTED) {
    digitalWrite(led, !digitalRead(led));
    delay(300);
    WiFi.reconnect();
  }
}

void init_OTA() {
  // Initiate OTA updates
  ArduinoOTA
  .onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH)
      type = "sketch";
    else // U_SPIFFS
      type = "filesystem";

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  })
  .onEnd([]() {
    Serial.println("\nEnd");
  })
  .onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("><%u%%\r", (progress / (total / 100)));
  })
  .onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.begin();
}
