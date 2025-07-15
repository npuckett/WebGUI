#include <WebGUI.h>

/*
  WiFi Station Mode Example - WebGUI Library
  
  This example demonstrates how to connect the WebGUI to an
  existing WiFi network instead of creating an access point.
  Perfect for integrating with home networks or IoT systems.
  
  Features:
  - Connects to existing WiFi network
  - DHCP IP assignment
  - Network status monitoring
  - Remote access capability
  
  Hardware connections:
  - LED on pin 2 (built-in LED)
  - Button sensor on pin 4 (with pullup)
  
  Created by WebGUI Library Contributors
  This example code is in the public domain.
*/

// Network credentials - CHANGE THESE FOR YOUR NETWORK
const char* WIFI_SSID = "YourWiFiNetwork";
const char* WIFI_PASSWORD = "YourWiFiPassword";

// Hardware pins
const int LED_PIN = 2;
const int BUTTON_PIN = 4;

// GUI Elements
Button networkLED("🌐 Network LED", 50, 80, 150, 50);
Button reconnectWiFi("🔄 Reconnect WiFi", 220, 80, 150, 50);
Button showNetworkInfo("ℹ️ Network Info", 50, 150, 150, 50);

Slider signalStrength("Signal Strength", 50, 200, -100, 0, -50, 200);
Slider connectionTime("Connected (min)", 50, 280, 0, 1440, 0, 200);

// Network monitoring
unsigned long connectStartTime = 0;
bool networkConnected = false;
bool ledState = false;

void setup() {
  Serial.begin(115200);
  
  // Initialize hardware
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Start WiFi in Station mode
  Serial.println("=== WebGUI WiFi Station Mode ===");
  Serial.println("Connecting to: " + String(WIFI_SSID));
  
  if (GUI.startStation(WIFI_SSID, WIFI_PASSWORD)) {
    networkConnected = true;
    connectStartTime = millis();
    Serial.println("✅ Connected successfully!");
    Serial.println("📍 IP Address: " + GUI.getIP());
    Serial.println("🌐 Access GUI at: http://" + GUI.getIP());
  } else {
    Serial.println("❌ Connection failed!");
    Serial.println("🔄 Will retry connection...");
    networkConnected = false;
  }
  
  // Configure interface
  GUI.setTitle("WiFi Station Controller");
  GUI.setHeading("🌐 Network Connected Device");
  
  // Add network-themed styling
  GUI.setCustomCSS(
    "body { background: linear-gradient(135deg, #74b9ff 0%, #0984e3 100%); } "
    ".container { background: rgba(255,255,255,0.95); border-radius: 15px; box-shadow: 0 8px 32px rgba(0,0,0,0.1); } "
    "h1 { color: #2d3436; text-align: center; margin-bottom: 10px; } "
    
    // Add network status indicator
    ".container::before { "
    "  content: '" + String(networkConnected ? "🟢 ONLINE" : "🔴 OFFLINE") + "'; "
    "  display: block; text-align: center; padding: 5px; "
    "  background: " + String(networkConnected ? "#00b894" : "#e17055") + "; "
    "  color: white; font-weight: bold; border-radius: 10px 10px 0 0; "
    "} "
    
    // Style buttons with network theme
    "#element_0 { background: linear-gradient(145deg, #00b894, #00a085); border-radius: 10px; } "
    "#element_1 { background: linear-gradient(145deg, #fdcb6e, #e17055); border-radius: 10px; } "
    "#element_2 { background: linear-gradient(145deg, #74b9ff, #0984e3); border-radius: 10px; } "
    
    // Style monitoring displays
    "#element_3_container, #element_4_container { "
    "  background: #f8f9fa; border-left: 4px solid #74b9ff; border-radius: 8px; "
    "} "
  );
  
  // Add all controls
  GUI.addElement(&networkLED);
  GUI.addElement(&reconnectWiFi);
  GUI.addElement(&showNetworkInfo);
  GUI.addElement(&signalStrength);
  GUI.addElement(&connectionTime);
  
  // Start web server
  GUI.begin();
  
  Serial.println("================================");
}

void loop() {
  GUI.update();
  
  // Check network status
  checkNetworkStatus();
  
  // Handle button controls
  if (networkLED.wasPressed()) {
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    Serial.println("🌐 Network LED: " + String(ledState ? "ON" : "OFF"));
  }
  
  if (reconnectWiFi.wasPressed()) {
    Serial.println("🔄 Attempting WiFi reconnection...");
    reconnectNetwork();
  }
  
  if (showNetworkInfo.wasPressed()) {
    printNetworkInfo();
  }
  
  // Update monitoring displays
  updateNetworkMonitoring();
  
  // Read physical button (if connected)
  static bool lastButtonState = HIGH;
  bool currentButtonState = digitalRead(BUTTON_PIN);
  if (lastButtonState == HIGH && currentButtonState == LOW) {
    // Button pressed - toggle LED
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    Serial.println("📱 Physical button pressed - LED: " + String(ledState ? "ON" : "OFF"));
    delay(50); // Debounce
  }
  lastButtonState = currentButtonState;
  
  delay(10);
}

void checkNetworkStatus() {
  static unsigned long lastCheck = 0;
  if (millis() - lastCheck < 5000) return; // Check every 5 seconds
  
  bool currentStatus = GUI.isConnected();
  if (currentStatus != networkConnected) {
    networkConnected = currentStatus;
    if (networkConnected) {
      connectStartTime = millis();
      Serial.println("✅ Network reconnected!");
      Serial.println("📍 New IP: " + GUI.getIP());
    } else {
      Serial.println("❌ Network disconnected!");
    }
  }
  
  lastCheck = millis();
}

void reconnectNetwork() {
  Serial.println("Disconnecting from current network...");
  GUI.disconnect();
  delay(2000);
  
  Serial.println("Attempting to reconnect...");
  if (GUI.startStation(WIFI_SSID, WIFI_PASSWORD)) {
    networkConnected = true;
    connectStartTime = millis();
    Serial.println("✅ Reconnection successful!");
    Serial.println("📍 IP Address: " + GUI.getIP());
  } else {
    networkConnected = false;
    Serial.println("❌ Reconnection failed!");
  }
}

void updateNetworkMonitoring() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 2000) return; // Update every 2 seconds
  
  if (networkConnected) {
    // Update signal strength (simulate or use actual RSSI)
    int rssi = GUI.getSignalStrength(); // This would need to be implemented in WebGUI
    if (rssi == 0) rssi = random(-80, -30); // Simulate for now
    signalStrength.setValue(rssi);
    
    // Update connection time
    unsigned long connectedMinutes = (millis() - connectStartTime) / 60000;
    connectionTime.setValue(min((int)connectedMinutes, 1440));
  } else {
    signalStrength.setValue(-100); // No signal
    connectionTime.setValue(0);
  }
  
  lastUpdate = millis();
}

void printNetworkInfo() {
  Serial.println("=== Network Information ===");
  Serial.println("📡 SSID: " + String(WIFI_SSID));
  Serial.println("📍 IP Address: " + GUI.getIP());
  Serial.println("🌐 Gateway: " + GUI.getGateway()); // Would need implementation
  Serial.println("🔧 DNS: " + GUI.getDNS()); // Would need implementation
  Serial.println("📶 Signal: " + String(signalStrength.getIntValue()) + " dBm");
  Serial.println("⏱️ Connected: " + String(connectionTime.getIntValue()) + " minutes");
  Serial.println("🔗 Status: " + String(networkConnected ? "CONNECTED" : "DISCONNECTED"));
  Serial.println("🌐 Web GUI: http://" + GUI.getIP());
  Serial.println("==========================");
}
