/*
  AP_ToggleAnLED.ino - WebGUI Library Simple LED Toggle Example (Access Point Mode)
  
  DESCRIPTION:
  This is the simplest possible WebGUI example - just toggle the built-in LED on and off.
  Based on the Quick Start code from the README, this demonstrates the absolute basics
  of creating a web interface in Access Point mode.
  
  FEATURES:
  - Single LED toggle control for built-in LED
  - Access Point mode (creates its own WiFi network)
  - Minimal code - perfect for learning the basics
  - No external components required
  
  HARDWARE REQUIRED:
  - Arduino UNO R4 WiFi, Arduino Nano 33 IoT, or ESP32
  - Built-in LED only (no additional components needed)
  
  NETWORK SETUP:
  1. Upload this sketch to your Arduino
  2. Open Serial Monitor to see WiFi network name and password
  3. Connect your phone/computer to the Arduino's WiFi network
  4. Open a web browser and navigate to http://192.168.4.1 (fixed IP in AP mode)
  
  NETWORK DETAILS:
  - Access Point Mode creates a fixed IP address: 192.168.4.1
  - WiFi name: "My-Arduino" (you can change this in the code)
  - Password: "password123" (you can change this in the code)
  
  This example code is in the public domain and can be used as a starting
  point for your own projects. This is the exact code from the README Quick Start.
*/

#include <WebGUI.h>

// Create a simple LED toggle control
Toggle ledToggle("Built-in LED", 20, 50, 120);

void setup() {
  Serial.begin(115200);
  
  // Configure built-in LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  // Start WiFi Access Point
  GUI.startAP("My-Arduino", "password123");
  
  // Configure interface
  GUI.setTitle("LED Controller");
  
  // Add toggle to GUI
  GUI.addElement(&ledToggle);
  
  // Start web server
  GUI.begin();
  
  Serial.println("Connect to WiFi: My-Arduino");
  Serial.println("Web interface: http://" + GUI.getIP());
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
