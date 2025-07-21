/*
  AP_BlinkAnLED.ino - WebGUI Library LED Blink Control Example (Access Point Mode)
  
  DESCRIPTION:
  This example demonstrates controlling the blinking behavior of an LED using toggle and slider controls.
  The toggle switch enables/disables blinking, and the slider controls the blink rate (delay between blinks).
  Your Arduino creates its own WiFi network that you can connect to directly.
  
  FEATURES:
  - Toggle switch to enable/disable LED blinking
  - Slider to control blink rate (50ms to 2000ms delay)
  - Real-time blink rate adjustment
  - Smooth blinking using millis() timing method
  - Access Point Mode (creates its own WiFi network)
  
  HARDWARE REQUIRED:
  - Arduino UNO R4 WiFi, Arduino Nano 33 IoT, or ESP32
  - Built-in LED (no external components needed)
  
  NETWORK SETUP:
  1. Upload this sketch to your Arduino
  2. Open Serial Monitor to see WiFi network name and password
  3. Connect your phone/computer to the Arduino's WiFi network
  4. Open a web browser and navigate to http://192.168.4.1 (fixed IP in AP mode)
  
  NETWORK DETAILS:
  - Access Point Mode creates a fixed IP address: 192.168.4.1
  - Default WiFi name: "Arduino-BlinkLED" (customizable in code)
  - Default password: "blink123" (customizable in code)
  
  This example code is in the public domain and can be used as a starting
  point for your own projects.
*/

#include <WebGUI.h>

// Pin definitions
const int BLINK_LED_PIN = LED_BUILTIN;         // LED for blinking control

// Network configuration - customize these for your project
const char* AP_NAME = "Arduino-BlinkLED";      // WiFi network name
const char* AP_PASSWORD = "blink123";          // WiFi password (minimum 8 characters)

// Web interface configuration
const char* PAGE_TITLE = "LED Blink Controller";

// Control elements
Toggle blinkToggle("Enable Blinking", 20, 50, 120);              // Toggle: enable/disable blinking
Slider blinkRateSlider("Blink Rate (ms)", 20, 100, 50, 2000, 500); // Slider: blink delay 50-2000ms

// Blinking control variables
bool blinkEnabled = false;            // Is blinking currently enabled?
unsigned long previousMillis = 0;     // Track timing for LED blinking
bool ledState = false;                // Current LED state (on/off)
int blinkDelay = 500;                 // Current blink delay in milliseconds

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  Serial.println("Starting WebGUI LED Blink Control Example...");
  
  // Configure LED pin as output and turn it off initially
  pinMode(BLINK_LED_PIN, OUTPUT);
  digitalWrite(BLINK_LED_PIN, LOW);
  ledState = false;
  
  // Configure WiFi Access Point
  // This creates a WiFi network that others can connect to
  GUI.startAP(AP_NAME, AP_PASSWORD);
  
  // Configure the web interface appearance
  GUI.setTitle(PAGE_TITLE);
  
  // Add control elements to the web interface
  GUI.addElement(&blinkToggle);
  GUI.addElement(&blinkRateSlider);
  
  // Start the web server
  GUI.begin();
  
  // Print connection information to Serial Monitor
  Serial.println();
  Serial.println("===============================================");
  Serial.println("WebGUI LED Blink Control - Access Point Mode");
  Serial.println("===============================================");
  Serial.println("WiFi Network Name: " + String(AP_NAME));
  Serial.println("WiFi Password: " + String(AP_PASSWORD));
  Serial.println("Web Interface: http://192.168.4.1 (fixed IP)");
  Serial.println("===============================================");
  Serial.println("Connect your device to the WiFi network above,");
  Serial.println("then open the web address in your browser.");
  Serial.println("===============================================");
  Serial.println("LED Blink Controller ready!");
  Serial.println();
}

void loop() {
  // CRITICAL: Always call GUI.update() in your main loop
  // This processes web requests and updates control values
  GUI.update();
  
  // Check if blink toggle was changed
  if (blinkToggle.wasToggled()) {
    blinkEnabled = blinkToggle.isOn();
    Serial.println("Blinking: " + String(blinkEnabled ? "ENABLED" : "DISABLED"));
    
    // If blinking is disabled, turn LED off immediately
    if (!blinkEnabled) {
      digitalWrite(BLINK_LED_PIN, LOW);
      ledState = false;
    }
  }
  
  // Get current blink rate from slider
  blinkDelay = blinkRateSlider.getIntValue();
  
  // Handle LED blinking behavior
  updateBlinkBehavior();
  
  // Small delay to prevent overwhelming the system
  delay(10);
}

/*
  Function: updateBlinkBehavior()
  
  This function handles the LED blinking logic using millis() for non-blocking timing.
  It only blinks the LED if blinking is enabled, and uses the current blink delay setting.
*/
void updateBlinkBehavior() {
  if (blinkEnabled) {
    // Check if enough time has passed to toggle the LED
    unsigned long currentMillis = millis();
    
    if (currentMillis - previousMillis >= blinkDelay) {
      // Save the last time we toggled the LED
      previousMillis = currentMillis;
      
      // Toggle LED state
      ledState = !ledState;
      digitalWrite(BLINK_LED_PIN, ledState);
    }
  }
}
