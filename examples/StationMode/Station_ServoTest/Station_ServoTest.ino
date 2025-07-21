/*
  Station_ServoTest.ino - WebGUI Library 3-Servo Control with Master Toggle (Stati// Web interface configuration
const char* PAGE_TITLE = "3-Servo Test with Master Toggle";  // Browser tab title and page headingMode)
  
  DESCRIPTION:
  This example demonstrates controlling 3 servo motors with an Attach/Detach toggle
  in Station Mode. Features a memory-optimized WebGUI interface with real-time
  servo position control and servo attachment control while connected to your
  existing WiFi network.
  
  FEATURES:
  - Servo attach/detach toggle (controls all servos)
  - 3 servo position sliders (Pin 3, Pin 5, Pin 6) with real-time value display
  - Memory-optimized WebGUI implementation (4-element limit respected)
  - Real-time servo position control (0-180 degrees)
  - Serial Monitor status reporting with memory monitoring
  - Clean, responsive web interface
  
  HARDWARE REQUIRED:
  - Arduino UNO R4 WiFi (tested platform)
  - 3 servo motors connected to pins 3, 5, 6
  - External 5V power supply for servos (recommended for multiple servos)
  - Breadboard and jumper wires
  
  WIRING:
  Pin 3 -> Servo 1 Signal Wire (Orange/Yellow)
  Pin 5 -> Servo 2 Signal Wire (Orange/Yellow)
  Pin 6 -> Servo 3 Signal Wire (Orange/Yellow)
  
  Servo Power (External 5V Supply Recommended):
  5V+ -> All Servo Red Wires
  GND -> All Servo Brown/Black Wires
  Arduino GND -> External Supply GND (common ground)
  
  NETWORK SETUP:
  1. Update the WiFi credentials below (WIFI_SSID and WIFI_PASSWORD)
  2. Upload this sketch to your Arduino UNO R4 WiFi
  3. Open Serial Monitor (115200 baud) to see connection info and IP address
  4. Open browser to the IP address shown in Serial Monitor
  
  NETWORK DETAILS:
  - Station Mode connects to your existing WiFi network
  - Dynamic IP address assigned by your router (shown in Serial Monitor)
  - Access the web interface using the IP address displayed during startup
  
  MEMORY OPTIMIZATION:
  - Uses streaming HTML generation (no large string buffers)
  - Minimal CSS (~400 bytes vs 9KB original)
  - 4-element GUI limit respected (discovered Arduino UNO R4 WiFi limit)
  - Real-time memory monitoring in Serial output
  
  This example code is in the public domain and can be used as a starting
  point for your own servo control projects.
*/

#include <WebGUI.h>
#include <Servo.h>

// getFreeRAM() is now available from WebGUI library

// Pin definitions for servos
const int SERVO_PIN_3 = 3;
const int SERVO_PIN_5 = 5;
const int SERVO_PIN_6 = 6;

// Network configuration - UPDATE THESE WITH YOUR WIFI CREDENTIALS
const char* WIFI_SSID = "YourWiFiName";        // Your WiFi network name
const char* WIFI_PASSWORD = "YourWiFiPassword"; // Your WiFi password

// Web interface configuration
const char* PAGE_TITLE = "3-Servo Test with Attach/Detach Toggle";  // Browser tab title and page heading

// GUI Elements: Attach/Detach toggle + 3 sliders = 4 total (Arduino UNO R4 WiFi limit)
// Toggle constructor: Toggle(label, x, y, width)
Toggle ADswitch("Attach/Detach Servos", 20, 20, 200);          // Servo attach/detach control

// Slider constructor: Slider(label, x, y, min, max, default, width)
Slider servoSlider3("Pin 3 Position", 20, 80, 0, 180, 90);       // Servo 1 position control
Slider servoSlider5("Pin 5 Position", 20, 150, 0, 180, 90);      // Servo 2 position control
Slider servoSlider6("Pin 6 Position", 20, 220, 0, 180, 90);      // Servo 3 position control

// Servo objects
Servo servo3;   // Servo on pin 3
Servo servo5;   // Servo on pin 5
Servo servo6;   // Servo on pin 6

// Global variables to track servo states
int servo3Position = 90;     // Current position of servo on pin 3
int servo5Position = 90;     // Current position of servo on pin 5
int servo6Position = 90;     // Current position of servo on pin 6

// Servo attachment control state
bool servosEnabled = true;   // Attach/detach toggle state (servos start attached)

// Variables to track system state
unsigned long lastStatusPrint = 0;        // Track when we last printed status
const int STATUS_UPDATE_RATE = 3000;     // Print status every 3 seconds
unsigned long lastMemoryCheck = 0;       // Track memory usage
const int MEMORY_CHECK_INTERVAL = 5000;  // Check memory every 5 seconds

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  Serial.println("Starting WebGUI Station_ServoTest Example...");
  
  // Print initial memory usage
  Serial.println("Free RAM at startup: " + String(getFreeRAM()) + " bytes");
  
  // Initialize servos and attach to pins (enabled by default)
  servo3.attach(SERVO_PIN_3);
  servo5.attach(SERVO_PIN_5);
  servo6.attach(SERVO_PIN_6);
  
  // Set initial servo positions to center (90 degrees)
  servo3.write(servo3Position);
  servo5.write(servo5Position);
  servo6.write(servo6Position);
  
  Serial.println("Servos attached and positioned at 90 degrees");
  
  // Connect to existing WiFi network
  // This connects to your router instead of creating a new network
  Serial.println("Connecting to WiFi...");
  if (GUI.connectWiFi(WIFI_SSID, WIFI_PASSWORD)) {
    Serial.println("Connected to WiFi network: " + String(WIFI_SSID));
    Serial.println("Web interface: http://" + GUI.getIP());
  } else {
    Serial.println("WiFi connection failed - starting backup Access Point");
    GUI.startAP("Servo-Controller", "password123");
    Serial.println("Connect to WiFi: Servo-Controller");
    Serial.println("Web interface: http://192.168.4.1");
  }
  
  // Small delay to let WiFi stabilize
  delay(2000);
  
  // Configure the web interface appearance
  GUI.setTitle(PAGE_TITLE);              // Browser tab title and page heading
  
  // Add GUI elements (4 total - respects Arduino UNO R4 WiFi limit)
  GUI.addElement(&ADswitch);         // Attach/detach toggle (element 1)
  GUI.addElement(&servoSlider3);      // Servo 3 slider (element 2)
  GUI.addElement(&servoSlider5);      // Servo 5 slider (element 3)
  GUI.addElement(&servoSlider6);      // Servo 6 slider (element 4)
  
  // Set initial toggle state to match servo state
  ADswitch.setState(servosEnabled);
  
  Serial.println("Added 4 GUI elements: 1 toggle + 3 sliders");
  
  // Print memory usage before starting server
  Serial.println("Free RAM before server start: " + String(getFreeRAM()) + " bytes");
  
  // Start the web server
  GUI.begin();
  Serial.println("Web server started successfully!");
  
  // Print final memory usage
  Serial.println("Free RAM after server start: " + String(getFreeRAM()) + " bytes");
  
  // Print connection information
  Serial.println();
  Serial.println("===============================================");
  Serial.println("WebGUI Servo Test - Station Mode");
  Serial.println("===============================================");
  Serial.println("Connected to WiFi: " + String(WIFI_SSID));
  Serial.println("Arduino IP Address: " + WiFi.localIP().toString());
  Serial.println("Web Interface: http://" + WiFi.localIP().toString());
  Serial.println("===============================================");
  Serial.println("Open the web address above in your browser");
  Serial.println("to access the servo control interface.");
  Serial.println("===============================================");
  Serial.println("Controls Available:");
  Serial.println("  Servo Attach/Detach Toggle (controls all)");
  Serial.println("  Pin 3 Position Slider (0-180°)");
  Serial.println("  Pin 5 Position Slider (0-180°)");
  Serial.println("  Pin 6 Position Slider (0-180°)");
  Serial.println("===============================================");
  Serial.println("Hardware Setup:");
  Serial.println("  Pin 3 -> Servo 1 Signal Wire");
  Serial.println("  Pin 5 -> Servo 2 Signal Wire");
  Serial.println("  Pin 6 -> Servo 3 Signal Wire");
  Serial.println("  Use external 5V supply for servos");
  Serial.println("===============================================");
  Serial.println("Ready! Open web interface for servo control.");
  Serial.println("===============================================");
  Serial.println();
}

void loop() {
  // Periodic memory monitoring
  if (millis() - lastMemoryCheck >= MEMORY_CHECK_INTERVAL) {
    int freeRAM = getFreeRAM();
    Serial.println("Free RAM: " + String(freeRAM) + " bytes");
    
    // Memory warning threshold
    if (freeRAM < 1000) {
      Serial.println("WARNING: Low memory! (" + String(freeRAM) + " bytes free)");
    }
    
    lastMemoryCheck = millis();
  }
  
  // Process web requests (CRITICAL - must be called in loop)
  GUI.update();
  
  // Update servo states based on GUI controls
  updateServoStates();
  
  // Periodic status reporting
  if (millis() - lastStatusPrint >= STATUS_UPDATE_RATE) {
    printServoStatus();
    lastStatusPrint = millis();
  }
  
  // Small delay for system stability
  delay(50);
}

/*
  Function: updateServoStates()
  
  Monitors the Attach/Detach toggle and position sliders, updating servo states accordingly.
  - Attach/Detach toggle controls servo attachment (power saving when detached)
  - Position sliders control servo angles when attached
  - Provides serial feedback for all state changes
*/
void updateServoStates() {
  // Check if Attach/Detach toggle state changed
  if (ADswitch.wasToggled()) {
    servosEnabled = ADswitch.isOn();
    
    if (servosEnabled) {
      // Attach all servos and position them
      servo3.attach(SERVO_PIN_3);
      servo5.attach(SERVO_PIN_5);
      servo6.attach(SERVO_PIN_6);
      
      // Update positions to current slider values
      servo3Position = servoSlider3.getIntValue();
      servo5Position = servoSlider5.getIntValue();
      servo6Position = servoSlider6.getIntValue();
      
      servo3.write(servo3Position);
      servo5.write(servo5Position);
      servo6.write(servo6Position);
      
      Serial.println("ATTACH/DETACH TOGGLE: All servos ATTACHED and positioned");
    } else {
      // Detach all servos (power saving)
      servo3.detach();
      servo5.detach();
      servo6.detach();
      
      Serial.println("ATTACH/DETACH TOGGLE: All servos DETACHED (power saving)");
    }
  }
  
  // Update individual servo positions (only when attached)
  if (servosEnabled) {
    // Pin 3 Servo Position Control
    int newPosition3 = servoSlider3.getIntValue();
    if (newPosition3 != servo3Position) {
      servo3Position = newPosition3;
      servo3.write(servo3Position);
      Serial.println("Pin 3 Servo: " + String(servo3Position) + "°");
    }
    
    // Pin 5 Servo Position Control
    int newPosition5 = servoSlider5.getIntValue();
    if (newPosition5 != servo5Position) {
      servo5Position = newPosition5;
      servo5.write(servo5Position);
      Serial.println("Pin 5 Servo: " + String(servo5Position) + "°");
    }
    
    // Pin 6 Servo Position Control
    int newPosition6 = servoSlider6.getIntValue();
    if (newPosition6 != servo6Position) {
      servo6Position = newPosition6;
      servo6.write(servo6Position);
      Serial.println("Pin 6 Servo: " + String(servo6Position) + "°");
    }
  }
}

/*
  Function: printServoStatus()
  
  Prints comprehensive status report to Serial Monitor including:
  - Attach/detach toggle state
  - Individual servo positions and attachment states
  - System uptime and memory usage
  - WiFi connection status
*/
void printServoStatus() {
  Serial.println("=== Servo Status Report ===");
  
  // WiFi connection status
  Serial.println("WiFi Connected: " + String(WiFi.status() == WL_CONNECTED ? "YES" : "NO"));
  Serial.println("IP Address: " + WiFi.localIP().toString());
  
  // Attach/detach toggle status
  Serial.println("Attach/Detach Toggle: " + String(servosEnabled ? "ATTACHED" : "DETACHED"));
  
  // Individual servo status
  String attachState = servosEnabled ? "ATTACHED" : "DETACHED";
  Serial.println("Pin 3 Servo: " + String(servo3Position) + "° (" + attachState + ")");
  Serial.println("Pin 5 Servo: " + String(servo5Position) + "° (" + attachState + ")");
  Serial.println("Pin 6 Servo: " + String(servo6Position) + "° (" + attachState + ")");
  
  // System status summary
  Serial.println("System Status: " + String(servosEnabled ? "3/3 ACTIVE" : "0/3 ACTIVE"));
  Serial.println("Uptime: " + String(millis() / 1000) + " seconds");
  Serial.println("Free RAM: " + String(getFreeRAM()) + " bytes");
  
  Serial.println("===========================");
  Serial.println();
}
