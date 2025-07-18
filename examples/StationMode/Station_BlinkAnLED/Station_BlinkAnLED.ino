/*
  Station_BlinkAnLED.ino - WebGUI Library LED Blink Control Example (Station Mode)
  
  DESCRIPTION:
  This example demonstrates controlling the blinking behavior of an LED using toggle and slider controls.
  The toggle switch enables/disables blinking, and the slider controls the blink rate (delay between blinks).
  Your Arduino connects to your existing WiFi network.
  
  FEATURES:
  - Toggle switch to enable/disable LED blinking
  - Slider to control blink rate (50ms to 2000ms delay)
  - Real-time blink rate adjustment
  - Smooth blinking using millis() timing method
  - Station Mode (connects to existing WiFi)
  
  HARDWARE REQUIRED:
  - Arduino UNO R4 WiFi, Arduino Nano 33 IoT, or ESP32
  - Built-in LED (no external components needed)
  
  NETWORK SETUP:
  1. Update WiFi credentials below (WIFI_SSID and WIFI_PASSWORD)
  2. Upload this sketch to your Arduino
  3. Open Serial Monitor to see the assigned IP address
  4. Open a web browser and navigate to the IP address shown
  
  This example code is in the public domain and can be used as a starting
  point for your own projects.
*/

#include <WebGUI.h>

// Function declarations
void updateBlinkBehavior();

// Pin definitions
const int BLINK_LED_PIN = LED_BUILTIN;         // LED for blinking control

// Network configuration - UPDATE THESE WITH YOUR WIFI CREDENTIALS
const char* WIFI_SSID = "YourWiFiName";        // Your WiFi network name
const char* WIFI_PASSWORD = "YourWiFiPassword"; // Your WiFi password

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
  
  // Connect to existing WiFi network
  Serial.println("Connecting to WiFi...");
  if (GUI.connectWiFi(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("Connected to WiFi: " + String(WIFI_SSID));
    Serial.println("Web interface: http://" + GUI.getIP());
  } else {
    Serial.println("WiFi connection failed - starting backup Access Point");
    GUI.startAP("Blink-Controller", "password123");
    Serial.println("Connect to WiFi: Blink-Controller");
    Serial.println("Web interface: http://192.168.4.1");
  }
  
  // Configure the web interface appearance
  GUI.setTitle(PAGE_TITLE);
  
  // Add control elements to the web interface
  GUI.addElement(&blinkToggle);
  GUI.addElement(&blinkRateSlider);
  
  // Start the web server
  GUI.begin();
  Serial.println("LED Blink Controller ready!");
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
