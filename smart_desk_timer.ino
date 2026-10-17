#include <WiFi.h>
#include <WebServer.h>

// --- UPDATE YOUR WI-FI DETAILS HERE ---
const char* ssid     = "WIFI-NAME";     // Enter your wifi name
const char* password = "WIFI-PASSWORD"; // Enter your wifi password

// --- GPIO PIN CONFIGURATION ---
int greenPins[3] = {12, 13, 14};
int bluePins[3]  = {15, 18, 19};
int redPins[4]   = {21, 22, 23, 25};

WebServer server(80);

// Default mode set to AUTO Wi-Fi Monitoring
String currentMode = "AUTO"; 
unsigned long lastSignalCheck = 0;

void turnAllOff() {
  for (int p : greenPins) digitalWrite(p, LOW);
  for (int p : bluePins) digitalWrite(p, LOW);
  for (int p : redPins) digitalWrite(p, LOW);
}

void setBank(int pins[], int size) {
  turnAllOff();
  for (int i = 0; i < size; i++) digitalWrite(pins[i], HIGH);
}

// Automatically checks Wi-Fi RSSI strength and updates LEDs
void updateWifiSignalGauge() {
  int rssi = WiFi.RSSI(); 

  if (rssi >= -55) {
    setBank(greenPins, 3); // Strong Signal -> Green
  } else if (rssi >= -75) {
    setBank(bluePins, 3);  // Medium Signal -> Blue
  } else {
    setBank(redPins, 4);   // Weak Signal -> Red
  }
}

void handleRoot() {
  int rssi = WiFi.RSSI();
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body{font-family:Arial; text-align:center; padding:20px; background-color:#121212; color:white;}";
  html += ".btn{display:block; width:100%; max-width:280px; margin:10px auto; padding:12px; font-size:16px; border:none; border-radius:8px; color:white; cursor:pointer;}";
  html += ".btn-auto{background:#17a2b8;} .btn-green{background:#28a745;} .btn-blue{background:#007bff;} .btn-red{background:#dc3545;} .btn-off{background:#6c757d;}</style></head><body>";
  html += "<h2>ESP32 Smart Network Monitor</h2>";
  html += "<p>Live Signal Strength: <b>" + String(rssi) + " dBm</b></p>";
  html += "<h3>Active Mode: <span style='color:#00ffcc;'>" + currentMode + "</span></h3>";
  html += "<a href='/auto'><button class='btn btn-auto'>Auto Wi-Fi Monitor Mode</button></a>";
  html += "<a href='/green'><button class='btn btn-green'>Force Green (Available)</button></a>";
  html += "<a href='/blue'><button class='btn btn-blue'>Force Blue (In a Call)</button></a>";
  html += "<a href='/red'><button class='btn btn-red'>Force Red (Do Not Disturb)</button></a>";
  html += "<a href='/off'><button class='btn btn-off'>Turn Off</button></a>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);

  for (int p : greenPins) pinMode(p, OUTPUT);
  for (int p : bluePins) pinMode(p, OUTPUT);
  for (int p : redPins) pinMode(p, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWi-Fi Connected!");
  Serial.print("Dashboard Address: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/auto",  []() { currentMode = "AUTO";  server.sendHeader("Location", "/"); server.send(303); });
  server.on("/green", []() { currentMode = "GREEN"; setBank(greenPins, 3); server.sendHeader("Location", "/"); server.send(303); });
  server.on("/blue",  []() { currentMode = "BLUE";  setBank(bluePins, 3);  server.sendHeader("Location", "/"); server.send(303); });
  server.on("/red",   []() { currentMode = "RED";   setBank(redPins, 4);   server.sendHeader("Location", "/"); server.send(303); });
  server.on("/off",   []() { currentMode = "OFF";   turnAllOff();          server.sendHeader("Location", "/"); server.send(303); });

  server.begin();
}

void loop() {
  server.handleClient();

  // In AUTO mode, re-check Wi-Fi signal strength every 2 seconds
  if (currentMode == "AUTO") {
    if (millis() - lastSignalCheck > 2000) {
      lastSignalCheck = millis();
      updateWifiSignalGauge();
    }
  }
}