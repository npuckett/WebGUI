/*
  Station_SensorDebug.ino - WebGUI Library Analog Sensor Debug Example (Station Mode)
  
  DESCRIPTION:
  This example demonstrates how to use the WebGUI library to debug analog sensor
  readings with a configurable threshold in Station Mode. Perfect for troubleshooting 
  light sensors, temperature sensors, potentiometers, or any analog input while 
  connected to your existing WiFi network.
  
  FEATURES:
  - Real-time analog sensor value display (A0)
  - Adjustable threshold slider (0-1023)
  - Above/Below threshold status indicator
  - Continuous monitoring with visual feedback
  - Station mode for network-based debugging
  
  HARDWARE REQUIRED:
  - Arduino UNO R4 WiFi, Arduino Nano 33 IoT, or ESP32
  - Light sensor (LDR) connected to A0 with pull-down resistor
  - Or any analog sensor/potentiometer on A0
  
  WIRING EXAMPLE (Light Sensor):
  - A0: Light sensor signal
  - 5V: Light sensor VCC (through 10k resistor)
  - GND: Light sensor GND
  - 10k resistor between A0 and GND (pull-down)
  
  NETWORK SETUP:
  1. Update the WiFi credentials below (WIFI_SSID and WIFI_PASSWORD)
  2. Upload this sketch to your Arduino
  3. Open Serial Monitor to see the assigned IP address
  4. Open a web browser and navigate to the IP address shown in Serial Monitor
  
  NETWORK DETAILS:
  - Station Mode connects to your existing WiFi network
  - Dynamic IP address assigned by your router (shown in Serial Monitor)
  - Access the web interface using the IP address displayed during startup
  
  DEBUGGING WORKFLOW:
  1. Adjust the threshold slider to set your desired trigger point
  2. Cover/uncover the light sensor (or adjust your analog input)
  3. Watch the sensor value and Above/Below status change in real-time
  4. Fine-tune your threshold for optimal sensor triggering
  
  This example code is in the public domain and can be used as a starting
  point for your own sensor debugging projects.
*/

#include <WebGUI.h>

// Function declarations
void updateSensorReading();
void checkThreshold();
void printDebugInfo();

// Pin definitions
const int SENSOR_PIN = A0;               // Analog sensor input pin

// Network configuration - UPDATE THESE WITH YOUR WIFI CREDENTIALS
const char* WIFI_SSID = "YourWiFiName";        // Your WiFi network name
const char* WIFI_PASSWORD = "YourWiFiPassword"; // Your WiFi password

// Web interface configuration
const char* PAGE_TITLE = "Analog Sensor Debug Interface"; // Browser tab title and page heading

// Control elements - these create the web interface
SensorStatus sensorDisplay("Sensor Value (A0)", 20, 50);          // Shows current sensor reading
Slider thresholdSlider("Threshold", 20, 100, 0, 1023, 512);      // Adjustable threshold (0-1023, default 512)
SensorStatus statusDisplay("Threshold Status", 20, 150);          // Shows Above/Below status

// Variables to track sensor state
int currentSensorValue = 0;      // Current analog reading from A0
int thresholdValue = 512;        // Current threshold setting
String thresholdStatus = "Below"; // Current status: "Above" or "Below"
const int UPDATE_RATE = 200;     // How often to read sensor (milliseconds)
unsigned long lastSensorRead = 0;    // Track when we last read the sensor
unsigned long lastDebugPrint = 0;    // Track when we last printed debug info

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  Serial.println("Starting WebGUI Station_SensorDebug Example...");
  
  // Configure analog input pin
  pinMode(SENSOR_PIN, INPUT);
  
  // Connect to existing WiFi network
  // This connects to your router instead of creating a new network
  Serial.println("Connecting to WiFi...");
  if (GUI.connectWiFi(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("Connected to WiFi network: " + String(WIFI_SSID));
    Serial.println("Web interface: http://" + GUI.getIP());
  } else {
    Serial.println("WiFi connection failed - starting backup Access Point");
    GUI.startAP("Sensor-Debug", "password123");
    Serial.println("Connect to WiFi: Sensor-Debug");
    Serial.println("Web interface: http://192.168.4.1");
  }
  
  // Configure the web interface appearance
  GUI.setTitle(PAGE_TITLE);              // Browser tab title and page heading
  
  // Add all control elements to the web interface
  // The order you add them determines their order on the page
  GUI.addElement(&sensorDisplay);     // Add sensor value display
  GUI.addElement(&thresholdSlider);   // Add threshold adjustment slider
  GUI.addElement(&statusDisplay);     // Add above/below status display
  
  // Start the web server
  GUI.begin();
  
  // Initialize sensor displays with starting values
  sensorDisplay.setValue(String(currentSensorValue));
  statusDisplay.setValue(thresholdStatus);
  
  // Print connection information to Serial Monitor
  Serial.println("===============================================");
  Serial.println("WebGUI Sensor Debug Example - Station Mode");
  Serial.println("===============================================");
  Serial.println("Connected to WiFi: " + String(WIFI_SSID));
  Serial.println("Arduino IP Address: " + WiFi.localIP().toString());
  Serial.println("Web Interface: http://" + WiFi.localIP().toString());
  Serial.println("===============================================");
  Serial.println("Open the web address above in your browser");
  Serial.println("to access the sensor debug interface.");
  Serial.println("===============================================");
  Serial.println("Sensor Pin: A" + String(SENSOR_PIN - A0));
  Serial.println("Update Rate: " + String(UPDATE_RATE) + " ms");
  Serial.println("Initial Threshold: " + String(thresholdValue));
  Serial.println("===============================================");
}

/*
  Function: updateSensorReading()
  
  Reads the analog sensor value from A0 and updates the web display.
  This function is called at the rate specified by UPDATE_RATE.
*/
void updateSensorReading() {
  // Check if enough time has passed since the last reading
  if (millis() - lastSensorRead >= UPDATE_RATE) {
    
    // Read the analog sensor value (0-1023 for 10-bit ADC)
    currentSensorValue = analogRead(SENSOR_PIN);
    
    // Update the sensor display on the web page
    sensorDisplay.setValue(String(currentSensorValue));
    
    // Remember when we last read the sensor
    lastSensorRead = millis();
  }
}

/*
  Function: checkThreshold()
  
  Compares the current sensor value against the threshold and updates
  the status display. This function implements the core logic that
  you would use in your actual projects.
*/
void checkThreshold() {
  // Get the current threshold value from the slider
  thresholdValue = thresholdSlider.getIntValue();
  
  // This is the key comparison that you would use in your projects
  if (currentSensorValue > thresholdValue) {
    thresholdStatus = "Above";
  } else {
    thresholdStatus = "Below";
  }
  
  // Update the status display on the web page
  statusDisplay.setValue(thresholdStatus);
}

/*
  Function: printDebugInfo()
  
  Prints detailed sensor information to the Serial Monitor.
  Useful for debugging and data logging.
*/
void printDebugInfo() {
  Serial.println("--- Sensor Debug Info ---");
  Serial.println("WiFi Connected: " + String(WiFi.status() == WL_CONNECTED ? "YES" : "NO"));
  Serial.println("IP Address: " + WiFi.localIP().toString());
  Serial.println("Sensor Value (A0): " + String(currentSensorValue) + " / 1023");
  Serial.println("Threshold Setting: " + String(thresholdValue) + " / 1023");
  Serial.println("Status: " + thresholdStatus + " threshold");
  Serial.println("Difference: " + String(currentSensorValue - thresholdValue));
  Serial.println("Percentage: " + String((currentSensorValue * 100) / 1023) + "%");
  Serial.println("Uptime: " + String(millis() / 1000) + " seconds");
  Serial.println("------------------------");
}

void loop() {
  // CRITICAL: Always call GUI.update() in your main loop
  // This processes web requests and updates control values
  GUI.update();
  
  // Read the sensor value at the specified rate
  updateSensorReading();
  
  // Check the sensor value against the threshold
  checkThreshold();
  
  // Print debug information every 2 seconds
  if (millis() - lastDebugPrint >= 2000) {
    printDebugInfo();
    lastDebugPrint = millis();
  }
  
  // Small delay to prevent overwhelming the system
  delay(10);
}
