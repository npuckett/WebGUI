#include <WebGUI.h>

/*
  BasicControls Example - WebGUI Library
  
  This example demonstrates the basic features of the WebGUI library:
  - Creating buttons and sliders
  - Setting up a WiFi access point
  - Controlling an LED through the web interface
  
  Compatible Hardware:
  - Arduino UNO R4 WiFi
  - Arduino Nano 33 IoT
  - ESP32 boards
  
  Instructions:
  1. Upload this sketch to your Arduino
  2. Connect to the WiFi network "Arduino-GUI" with password "password123"
  3. Open a web browser and go to the IP address shown in Serial Monitor
  4. Use the buttons and sliders to control the LED
  
  Created by WebGUI Library Contributors
  This example code is in the public domain.
*/

// Pin definitions
const int LED_PIN = 2;

// Create GUI controls with proper spacing
Button toggleLED("Toggle LED", 20, 50);                          // (20, 50)
Button resetButton("Reset All", 140, 50);                       // (140, 50)

// Sliders spaced 70px apart vertically to avoid overlap
Slider brightness("LED Brightness", 20, 100, 0, 255, 128);      // (20, 100)
Slider servoPosition("Servo Position", 20, 170, 0, 180, 90);    // (20, 170) 
Slider updateRate("Update Rate (ms)", 20, 240, 50, 1000, 100);  // (20, 240)

// State variables
bool ledState = false;
int currentBrightness = 128;
int currentServoPos = 90;
int currentUpdateRate = 100;
unsigned long lastUpdate = 0;

void setup() {
  Serial.begin(115200);
  
  // Setup pins
  pinMode(LED_PIN, OUTPUT);
  
  // Start WiFi Access Point
  GUI.startAP("Arduino-GUI", "password123");
  
  // Configure page title and heading
  GUI.setTitle("LED Controller v1.0");           // Browser tab title
  GUI.setHeading("Arduino LED Control Panel");   // Page heading
  
  // Optional: Set a custom theme
  // GUI.setTheme(WEBGUI_DARK_THEME);
  
  // Optional: Add custom CSS
  // GUI.setCustomCSS(".webgui-button.primary { background: linear-gradient(145deg, #ff6b6b, #ee5a52); }");
  
  // Add controls to the GUI
  GUI.addElement(&toggleLED);
  GUI.addElement(&resetButton);
  GUI.addElement(&brightness);
  GUI.addElement(&servoPosition);
  GUI.addElement(&updateRate);
  
  // Start the web server
  GUI.begin();
  
  Serial.println("=== Arduino WebGUI Example ===");
  Serial.println("WiFi Access Point started");
  Serial.println("SSID: Arduino-GUI");
  Serial.println("Password: password123");
  Serial.println("GUI available at: http://" + GUI.getIP());
  Serial.println("Page Title: LED Controller v1.0");
  Serial.println("===============================");
}

void loop() {
  // Update the GUI (process web requests)
  GUI.update();
  
  // Check for button presses
  if (toggleLED.wasPressed()) {
    ledState = !ledState;
    Serial.println("LED toggled: " + String(ledState ? "ON" : "OFF"));
  }
  
  if (resetButton.wasPressed()) {
    Serial.println("Reset button pressed - resetting all values");
    ledState = false;
    brightness.setValue(128);
    servoPosition.setValue(90);
    updateRate.setValue(100);
  }
  
  // Get current slider values
  currentBrightness = brightness.getIntValue();
  currentServoPos = servoPosition.getIntValue();
  currentUpdateRate = updateRate.getIntValue();
  
  // Apply LED control
  if (ledState) {
    analogWrite(LED_PIN, currentBrightness);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  
  // Print status periodically
  if (millis() - lastUpdate > currentUpdateRate) {
    Serial.println("Status - LED: " + String(ledState ? "ON" : "OFF") + 
                   ", Brightness: " + String(currentBrightness) + 
                   ", Servo: " + String(currentServoPos) + "°");
    lastUpdate = millis();
  }
  
  delay(10);
}

/*
  POSITIONING GUIDELINES:
  
  Button spacing:
  - Buttons: 30-40px height, can be placed side by side with 20px gap
  - Example: Button1(20,50), Button2(140,50), Button3(260,50)
  
  Slider spacing:
  - Sliders: ~60px total height (label + slider + padding)
  - Space sliders 70px apart vertically
  - Example: Slider1(20,100), Slider2(20,170), Slider3(20,240)
  
  Grid Layout Alternative:
  - Use CSS Grid for automatic spacing
  - Add elements without specific positioning
  - Let CSS handle the layout
  
  Custom Styling:
  - Use GUI.setTheme() for color schemes
  - Use GUI.setCustomCSS() for specific overrides
  - Modify WebGUIStyles.h for global changes
*/
