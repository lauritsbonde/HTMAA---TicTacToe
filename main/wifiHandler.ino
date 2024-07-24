#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

extern ESP8266WebServer server;


ESP8266WiFiClass getWiFi() {
  return WiFi;
}

bool checkConnected() {
  return WiFi.isConnected();
}

void handleDisconnect() {
  WiFi.disconnect();
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "Redirecting to /");
}

String isConnectedPage() {
  String ssid = WiFi.SSID();
  String page = "<html><body>";
  page += "<h1>Currently Connected to: ";
  page += ssid;
  page += "</h1>";
  page += "<form action=\"/disconnect\" method=\"post\">";
  page += "<input type=\"submit\" value=\"Disconnect\">";
  page += "</form>";
  page += "</body></html>";
  return page;
}

void handleRoot() {
  if (checkConnected()) {
    server.send(200, "text/html", isConnectedPage());
    return;
  }
  String page = "<html><body>";
  page += "<h1>ESP8266 Network Scanner</h1>";
  page += "<p><a href=\"/scan\">Scan for Networks</a></p>";
  page += "</body></html>";
  server.send(200, "text/html", page);
}

void handleScan() {
  if (checkConnected()) {
    server.send(200, "text/html", isConnectedPage());
    return;
  }
  String page = "<html><body>";
  page += "<h1>Available Networks</h1>";
  int n = WiFi.scanNetworks();
  if (n == 0) {
    page += "<p>No networks found</p>";
  } else {
    for (int i = 0; i < n; ++i) {
      page += "<form action=\"/connect\" method=\"get\">";
      page += "SSID: " + WiFi.SSID(i) + " (RSSI: " + String(WiFi.RSSI(i)) + ")";
      page += "<input type=\"hidden\" name=\"ssid\" value=\"" + WiFi.SSID(i) + "\">";
      page += "<input type=\"password\" name=\"password\" placeholder=\"Enter Password\">";
      page += "<input type=\"submit\" value=\"Connect\">";
      page += "</form><br>";
    }
  }
  page += "<p><a href=\"/\">Back</a></p>";
  page += "</body></html>";
  server.send(200, "text/html", page);
}

void handleConnect() {
  if (checkConnected()) {
    server.send(200, "text/html", isConnectedPage());
    return;
  }
  Serial.println("Handle connect:");
  String ssid = server.arg("ssid");
  String password = server.arg("password");

  Serial.print("Connecting to: ");
  Serial.print(ssid);

  WiFi.begin(ssid.c_str(), password.c_str());

  String page = "<html><body>";
  page += "<h1>Connecting to ";
  page += ssid;
  page += "</h1>";

  int timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 20) {
    delay(500);
    Serial.print(".");
    timeout++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    page += "<p>Connected successfully!</p>";
    Serial.println(" Connected successfully!");
  } else {
    page += "<p>Connection failed. Please try again.</p>";
    Serial.println("Connection failed.");
  }

  page += "<p><a href=\"/\">Back</a></p>";
  page += "</body></html>";

  server.send(200, "text/html", page);
}
