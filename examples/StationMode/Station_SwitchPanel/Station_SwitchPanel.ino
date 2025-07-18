/*
  Station_SwitchPanel.ino - WebGUI Library Switch Panel Example (Station Mode)
  
  DESCRIPTION:
  This example demonstrates controlling multiple LEDs using toggle switches in Station Mode.
  Your Arduino connects to your existing WiFi network with a web interface featuring 4 toggle switches,
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
  1. Update the WiFi credentials below (WIFI_SSID and WIFI_PASSWORD)
  2. Upload this sketch to your Arduino
  3. Open Serial Monitor to see the assigned IP address
  4. Open a web browser and navigate to the IP address shown in Serial Monitor
  
  NETWORK DETAILS:
  - Station Mode connects to your existing WiFi network
  - Dynamic IP address assigned by your router (shown in Serial Monitor)
  - Access the web interface using the IP address displayed during startup
  
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

// Network configuration - UPDATE THESE WITH YOUR WIFI CREDENTIALS
const char* WIFI_SSID = "YourWiFiName";        // Your WiFi network name
const char* WIFI_PASSWORD = "YourWiFiPassword"; // Your WiFi password

// Web interface configuration
const char* PAGE_TITLE = "Switch Panel Controller";             // Browser tab title and page heading

// LED Toggle switches - 4 LEDs for stable operation
// Toggle constructor: Toggle(label, x, y, width)
Toggle switchToggle2("Switch 2", 20, 50, 80);       // LED on pin 2
Toggle switchToggle3("Switch 3", 120, 50, 80);      // LED on pin 3  
Toggle switchToggle4("Switch 4", 220, 50, 80);      // LED on pin 4
Toggle switchToggle5("Switch 5", 320, 50, 80);      // LED on pin 5

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
  Serial.println("Starting WebGUI Station_SwitchPanel Example...");
  
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
  
  // Connect to existing WiFi network
  // This connects to your router instead of creating a new network
  Serial.println("Connecting to WiFi...");
  if (GUI.connectWiFi(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("Connected to WiFi network: " + String(WIFI_SSID));
    Serial.println("Web interface: http://" + GUI.getIP());
  } else {
    Serial.println("WiFi connection failed - starting backup Access Point");
    GUI.startAP("Switch-Panel", "password123");
    Serial.println("Connect to WiFi: Switch-Panel");
    Serial.println("Web interface: http://192.168.4.1");
  }
  
  // Configure the web interface appearance
  GUI.setTitle(PAGE_TITLE);              // Browser tab title and page heading
  
  // Add LED toggle switches to the web interface
  GUI.addElement(&switchToggle2);
  GUI.addElement(&switchToggle3);
  GUI.addElement(&switchToggle4);
  GUI.addElement(&switchToggle5);
  
  Serial.println("Added 4 switch toggles to web interface");
  
  // Start the web server
  GUI.begin();
  Serial.println("Web server started successfully!");
  
  // Print connection information to Serial Monitor
  Serial.println();
  Serial.println("===============================================");
  Serial.println("WebGUI Switch Panel Example - Station Mode");
  Serial.println("===============================================");
  Serial.println("Connected to WiFi: " + String(WIFI_SSID));
  Serial.println("Arduino IP Address: " + WiFi.localIP().toString());
  Serial.println("Web Interface: http://" + WiFi.localIP().toString());
  Serial.println("===============================================");
  Serial.println("Open the web address above in your browser");
  Serial.println("to access the switch panel interface.");
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
  if (switchToggle2.wasToggled()) {
    led2State = switchToggle2.isOn();  // Update global state variable
    digitalWrite(LED_PIN_2, led2State);
    Serial.println("Pin 2 LED: " + String(led2State ? "ON" : "OFF"));
  }
  
  // Check Pin 3 LED toggle
  if (switchToggle3.wasToggled()) {
    led3State = switchToggle3.isOn();  // Update global state variable
    digitalWrite(LED_PIN_3, led3State);
    Serial.println("Pin 3 LED: " + String(led3State ? "ON" : "OFF"));
  }
  
  // Check Pin 4 LED toggle
  if (switchToggle4.wasToggled()) {
    led4State = switchToggle4.isOn();  // Update global state variable
    digitalWrite(LED_PIN_4, led4State);
    Serial.println("Pin 4 LED: " + String(led4State ? "ON" : "OFF"));
  }
  
  // Check Pin 5 LED toggle
  if (switchToggle5.wasToggled()) {
    led5State = switchToggle5.isOn();  // Update global state variable
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
  
  // Print WiFi connection status
  Serial.println("WiFi Connected: " + String(WiFi.status() == WL_CONNECTED ? "YES" : "NO"));
  Serial.println("IP Address: " + WiFi.localIP().toString());
  
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
