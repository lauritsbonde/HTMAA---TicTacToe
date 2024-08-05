#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "./enums.h"
// wifiHandler.ino has all the code used for wifi setup - it is automatically concatinated to this file

const char* apSSID = "_TicTacToe";
const char* apPassword = "12345678";

ESP8266WebServer server(80);

void setup() {
  Serial.begin(115200);
  Serial.println("Started");

  setupGantry(); // gantryHandler.ino

  // Set up Access Point
  WiFi.setAutoReconnect(true);
  WiFi.setSleepMode(WIFI_NONE_SLEEP);
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(apSSID, apPassword);
  Serial.println("Access Point started");

  // Start the web server
  server.on("/", handleRoot); // wifiHandler.ino function
  server.on("/scan", handleScan); // wifiHandler.ino function
  server.on("/connect", handleConnect); // wifiHandler.ino function
  server.on("/disconnect", handleDisconnect); // wifiHandler.ino function
  server.begin();
  Serial.println("Web server started");
}

int counter = 0;

void loop() {
  server.handleClient();

  if(checkWiFiConnected() && !mqttClientIsConnected()){
    connectToMqtt();
  }

  if(mqttClientIsConnected()) {
    mqttLoop();
    gameLoop();
  }

}
