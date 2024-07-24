#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
// wifiHandler.ino has all the code used for wifi setup - it is automatically concatinated to this file

const char* apSSID = "TicTacToe";
const char* apPassword = "12345678";

ESP8266WebServer server(80);

bool startedMqttConnect = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Started");

  // Set up Access Point
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

  if(getWiFi().isConnected() && !startedMqttConnect){
    startedMqttConnect = true;
    connectToMqtt();
  }

  if(mqttClientIsConnected()) {
    String message = "Hej - ";
    message += counter;
    publishMessage("lol", message, false);
    counter++;
    delay(500);
  }

}
