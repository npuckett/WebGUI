/*
  Station_ToggleAnLED.ino - WebGUI Library Simple LED Toggle Example (Station Mode)
  
  DESCRIPTION:
  This is the simplest possible WebGUI example - just toggle the built-in LED on and off.
  Based on the Quick Start code from the README, this demonstrates the absolute basics
  of creating a web interface in Station mode (connecting to existing WiFi).
  
  FEATURES:
  - Single LED toggle control for built-in LED
  - Station mode (connects to your home/office WiFi)
  - Minimal code - perfect for learning the basics
  - No external components required
  - Fallback to Access Point mode if WiFi connection fails
  
  HARDWARE REQUIRED:
  - Arduino UNO R4 WiFi, Arduino Nano 33 IoT, or ESP32
  - Built-in LED only (no additional components needed)
  
  NETWORK SETUP:
  1. Update WiFi credentials below (WIFI_SSID and WIFI_PASSWORD)
  2. Upload this sketch to your Arduino
  3. Open Serial Monitor to see the IP address assigned by your router
  4. Open a web browser and navigate to the IP address shown
  
  NETWORK DETAILS:
  - Station Mode connects to your existing WiFi network
  - IP address is assigned by your router (DHCP)
  - Check Serial Monitor for the actual IP address
  - If WiFi connection fails, falls back to Access Point mode
  
  This example code is in the public domain and can be used as a starting
  point for your own projects. This is adapted from the README Quick Start.
*/

#include <WebGUI.h>

// *** WIFI CREDENTIALS - UPDATE THESE ***
const char* WIFI_SSID = "YourWiFiName";       // Replace with your WiFi network name
const char* WIFI_PASSWORD = "YourWiFiPassword"; // Replace with your WiFi password

// Create a simple LED toggle control
Toggle ledToggle("Built-in LED", 20, 50, 120);

void setup() {
  Serial.begin(115200);
  
  // Configure built-in LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  // Try to connect to existing WiFi network
  Serial.println("Attempting to connect to WiFi...");
  if (GUI.connectWiFi(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("Connected to WiFi network: " + String(WIFI_SSID));
    Serial.println("Web interface: http://" + GUI.getIP());
  } else {
    // Fallback to Access Point mode if WiFi connection fails
    Serial.println("WiFi connection failed - starting backup Access Point");
    GUI.startAP("LED-Controller", "password123");
    Serial.println("Connect to WiFi: LED-Controller");
    Serial.println("Web interface: http://192.168.4.1");
  }
  
  // Configure interface
  GUI.setTitle("LED Controller");
  
  // Add toggle to GUI
  GUI.addElement(&ledToggle);
  
  // Start web server
  GUI.begin();
  
  Serial.println("LED Controller ready!");
}

void loop() {
  GUI.update();  // Always call this in loop()
  
  // Check if toggle was switched
  if (ledToggle.wasToggled()) {
    bool isOn = ledToggle.isOn();
    digitalWrite(LED_BUILTIN, isOn);
    Serial.println("Built-in LED: " + String(isOn ? "ON" : "OFF"));
  }
}
