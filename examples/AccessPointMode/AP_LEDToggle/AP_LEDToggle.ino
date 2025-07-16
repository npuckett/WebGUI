/*
  AP_LEDToggle.ino - WebGUI Library LED Toggle Example
  
  DESCRIPTION:
  This example demonstrates controlling multiple LEDs using toggle switches in Access Point mode.
  Your Arduino creates its own WiFi network with a web interface featuring 4 toggle switches,
  each controlling an individual LED connected to pins 2-5.
  
  FEATURES:
  - 4 LED toggle switches (Pin 2 through Pin 5)
  - Horizontal layout for easy viewing
  - Individual control of each LED
  - Real-time status updates
  - Simple, clean interface
  
  HARDWARE REQUIRED:
  - Arduino UNO R4 WiFi, Arduino Nano 33 IoT, or ESP32
  - 4 LEDs connected to pins 2-5 (with appropriate resistors)
  - 4 current-limiting resistors (220Ω - 1kΩ)
  - Breadboard and jumper wires
  
  WIRING:
  Pin 2 -> LED 1 -> 220Ω Resistor -> GND
  Pin 3 -> LED 2 -> 220Ω Resistor -> GND
  Pin 4 -> LED 3 -> 220Ω Resistor -> GND
  Pin 5 -> LED 4 -> 220Ω Resistor -> GND
  
  NETWORK SETUP:
  1. Upload this sketch to your Arduino
  2. Open Serial Monitor to see WiFi network name and password
  3. Connect your phone/computer to the Arduino's WiFi network
  4. Open a web browser and navigate to http://192.168.4.1
  
  NETWORK DETAILS:
  - Access Point Mode creates a fixed IP address: 192.168.4.1
  - Default WiFi name: "Arduino-LEDToggle" (customizable in code)
  - Default password: "ledcontrol" (customizable in code)
  
  This example code is in the public domain and can be used as a starting
  point for your own projects.
*/

#include <WebGUI.h>

// Function declarations
void updateLEDStates();
void printLEDStatus();

// Pin definitions for LEDs - pins 2 through 5
const int LED_PIN_2 = 2;
const int LED_PIN_3 = 3;
const int LED_PIN_4 = 4;
const int LED_PIN_5 = 5;

// Network configuration - customize these for your project
const char* AP_NAME = "Arduino-LEDToggle";     // WiFi network name
const char* AP_PASSWORD = "ledcontrol";        // WiFi password (minimum 8 characters)

// Web interface configuration
const char* PAGE_TITLE = "LED Control";                  // Browser tab title
const char* PAGE_HEADING = "4-LED Controller";           // Main page heading

// LED Toggle switches - 4 LEDs for stable operation
// Toggle constructor: Toggle(label, x, y, width)
Toggle ledToggle2("Pin 2", 20, 50, 80);       // LED on pin 2
Toggle ledToggle3("Pin 3", 120, 50, 80);      // LED on pin 3  
Toggle ledToggle4("Pin 4", 220, 50, 80);      // LED on pin 4
Toggle ledToggle5("Pin 5", 320, 50, 80);      // LED on pin 5

// Global boolean variables to track LED states
bool led2State = false;  // Track state of LED on Pin 2
bool led3State = false;  // Track state of LED on Pin 3
bool led4State = false;  // Track state of LED on Pin 4
bool led5State = false;  // Track state of LED on Pin 5

// Variables to track system state
unsigned long lastStatusPrint = 0;        // Track when we last printed status
const int STATUS_UPDATE_RATE = 5000;     // Print status every 5 seconds

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  Serial.println("Starting WebGUI AP_LEDToggle Example...");
  
  // Configure LED pins as outputs and turn them off initially
  pinMode(LED_PIN_2, OUTPUT);
  pinMode(LED_PIN_3, OUTPUT);
  pinMode(LED_PIN_4, OUTPUT);
  pinMode(LED_PIN_5, OUTPUT);
  
  // Turn all LEDs off initially
  digitalWrite(LED_PIN_2, LOW);
  digitalWrite(LED_PIN_3, LOW);
  digitalWrite(LED_PIN_4, LOW);
  digitalWrite(LED_PIN_5, LOW);
  
  Serial.println("LED pins (2-5) configured as OUTPUT and set LOW");
  
  // Configure WiFi Access Point
  // This creates a WiFi network that others can connect to
  Serial.println("Starting Access Point...");
  GUI.startAP(AP_NAME, AP_PASSWORD);
  
  // Configure the web interface appearance
  GUI.setTitle(PAGE_TITLE);              // Browser tab title
  GUI.setHeading(PAGE_HEADING);          // Page heading
  GUI.setTheme(WEBGUI_DEFAULT_THEME);    // Use default light theme
  
  // Add LED toggle switches to the web interface
  GUI.addElement(&ledToggle2);
  GUI.addElement(&ledToggle3);
  GUI.addElement(&ledToggle4);
  GUI.addElement(&ledToggle5);
  
  Serial.println("Added 4 LED toggle switches to web interface");
  
  // Start the web server
  GUI.begin();
  Serial.println("Web server started successfully!");
  
  // Print connection information to Serial Monitor
  Serial.println();
  Serial.println("===============================================");
  Serial.println("WebGUI LED Toggle Example - Access Point Mode");
  Serial.println("===============================================");
  Serial.println("WiFi Network Name: " + String(AP_NAME));
  Serial.println("WiFi Password: " + String(AP_PASSWORD));
  Serial.println("Web Interface: http://192.168.4.1 (fixed IP)");
  Serial.println("===============================================");
  Serial.println("Ready! Connect to WiFi and open the web page.");
  Serial.println("===============================================");

  Serial.println();
}

void loop() {
  // CRITICAL: Always call GUI.update() in your main loop
  // This processes web requests and updates control values
  GUI.update();
  
  // Check and update LED states based on toggle switches
  updateLEDStates();
  
  // Print status information periodically
  if (millis() - lastStatusPrint >= STATUS_UPDATE_RATE) {
    printLEDStatus();
    lastStatusPrint = millis();
  }
  
  // Small delay to prevent overwhelming the system
  delay(10);
}

/*
  Function: updateLEDStates()
  
  This function checks each toggle switch individually and updates the corresponding LED.
  It only updates LEDs when their toggle state has changed to minimize
  Serial Monitor output and processing overhead.
*/
void updateLEDStates() {
  // Check each toggle switch individually
  
  // Check Pin 2 LED toggle
  if (ledToggle2.wasToggled()) {
    led2State = ledToggle2.isOn();  // Update global state variable
    digitalWrite(LED_PIN_2, led2State);
    Serial.println("Pin 2 LED: " + String(led2State ? "ON" : "OFF"));
  }
  
  // Check Pin 3 LED toggle
  if (ledToggle3.wasToggled()) {
    led3State = ledToggle3.isOn();  // Update global state variable
    digitalWrite(LED_PIN_3, led3State);
    Serial.println("Pin 3 LED: " + String(led3State ? "ON" : "OFF"));
  }
  
  // Check Pin 4 LED toggle
  if (ledToggle4.wasToggled()) {
    led4State = ledToggle4.isOn();  // Update global state variable
    digitalWrite(LED_PIN_4, led4State);
    Serial.println("Pin 4 LED: " + String(led4State ? "ON" : "OFF"));
  }
  
  // Check Pin 5 LED toggle
  if (ledToggle5.wasToggled()) {
    led5State = ledToggle5.isOn();  // Update global state variable
    digitalWrite(LED_PIN_5, led5State);
    Serial.println("Pin 5 LED: " + String(led5State ? "ON" : "OFF"));
  }
}

/*
  Function: printLEDStatus()
  
  This function prints the current status of all LEDs to the Serial Monitor.
  Useful for debugging and monitoring the system state.
*/
void printLEDStatus() {
  Serial.println("=== LED Status Report ===");
  
  // Print status of each LED
  Serial.println("Pin 2 (LED 1): " + String(led2State ? "ON" : "OFF"));
  Serial.println("Pin 3 (LED 2): " + String(led3State ? "ON" : "OFF"));
  Serial.println("Pin 4 (LED 3): " + String(led4State ? "ON" : "OFF"));
  Serial.println("Pin 5 (LED 4): " + String(led5State ? "ON" : "OFF"));
  
  // Count how many LEDs are currently on
  int ledsOn = 0;
  if (led2State) ledsOn++;
  if (led3State) ledsOn++;
  if (led4State) ledsOn++;
  if (led5State) ledsOn++;
  
  Serial.println("Total LEDs ON: " + String(ledsOn) + "/4");
  Serial.println("Uptime: " + String(millis() / 1000) + " seconds");
  Serial.println("========================");
  Serial.println();
}
