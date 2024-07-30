#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

extern ESP8266WebServer server;


ESP8266WiFiClass getWiFi() {
  return WiFi;
}

bool checkWiFiConnected() {
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
  if (checkWiFiConnected()) {
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
  if (checkWiFiConnected()) {
    server.send(200, "text/html", isConnectedPage());
    return;
  }
  
  String page = "<html><body>";
  page += "<h1>Available Networks</h1>";
  int n = WiFi.scanNetworks();
  
  if (n == 0) {
    page += "<p>No networks found</p>";
  } else {
    // Arrays to store SSID and RSSI values
    String ssids[n];
    int rssis[n];
    int count = 0;
    
    // Insertion sort while inserting networks
    for (int i = 0; i < n; ++i) {
      int rssi = WiFi.RSSI(i);
      String ssid = WiFi.SSID(i);

      // Find the position to insert the new network
      int j = count - 1;
      while (j >= 0 && rssis[j] < rssi) {
        rssis[j + 1] = rssis[j];
        ssids[j + 1] = ssids[j];
        j--;
      }
      // Insert the new network
      rssis[j + 1] = rssi;
      ssids[j + 1] = ssid;
      count++;
    }
    
    // Generate HTML with sorted SSID and RSSI values
    for (int i = 0; i < count; ++i) {
      page += "<form action=\"/connect\" method=\"get\">";
      page += "SSID: " + ssids[i] + " (RSSI: " + String(rssis[i]) + ")";
      page += "<input type=\"hidden\" name=\"ssid\" value=\"" + ssids[i] + "\">";
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
  if (checkWiFiConnected()) {
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
