/*
  AP_Basic.ino - WebGUI Library Basic Access Point Example
  
  DESCRIPTION:
  This example demonstrates the basic features of the WebGUI library in Access Point mode.
  Your Arduino creates its own WiFi network that you can connect to directly from any
  device with WiFi capability (phone, tablet, laptop).
  
  FEATURES:
  - LED toggle switch (turns built-in LED on/off)
  - Action button (calls custom function when pressed)
  - PWM brightness slider (controls LED brightness when on)
  - Random sensor display (simulates a sensor reading)
  - System status in Serial Monitor
  
  HARDWARE REQUIRED:
  - Arduino UNO R4 WiFi, Arduino Nano 33 IoT, or ESP32
  - Built-in LED (for toggle control)
  - External LED connected to pin 3 (for brightness control)
  - No additional components needed
  
  NETWORK SETUP:
  1. Upload this sketch to your Arduino
  2. Open Serial Monitor to see WiFi network name and password
  3. Connect your phone/computer to the Arduino's WiFi network
  4. Open a web browser and navigate to http://192.168.4.1 (fixed IP in AP mode)
  
  NETWORK DETAILS:
  - Access Point Mode creates a fixed IP address: 192.168.4.1
  - Default WiFi name: "Arduino-Basic" (customizable in code)
  - Default password: "webgui123" (customizable in code)
  

  
  This example code is in the public domain and can be used as a starting
  point for your own projects.
*/

#include <WebGUI.h>

// Function declarations
void buttonGo();
void updateRandomSensor();
void printSystemStatus();

// Pin definitions - change these if using different pins
int TOGGLE_LED_PIN = LED_BUILTIN;    // Built-in LED for toggle button
int PWM_LED_PIN = 3;                 // External LED for brightness control (PWM capable pin)

// Network configuration - customize these for your project
const char* AP_NAME = "Arduino-Basic";     // WiFi network name
const char* AP_PASSWORD = "webgui123";     // WiFi password (minimum 8 characters)

// Web interface configuration
const char* PAGE_TITLE = "Basic Controls";              // Browser tab title
const char* PAGE_HEADING = "Arduino Basic Control Panel"; // Main page heading

// Control elements - these create the web interface buttons and sliders
Toggle ledToggle("LED Toggle", 20, 50, 200);                   // Toggle: label, x, y, width
Button actionButton("Action Button", 20, 100, 150, 40);        // Button: label, x, y, width, height
Slider pwmSlider("LED Brightness", 20, 160, 0, 255, 128);     // Slider: label, x, y, min, max, default
SensorStatus randomSensor("Random Sensor", 20, 250);           // Status display for sensor data
SensorStatus buttonCounter("Button Presses", 20, 300);         // Display button press count
// SystemStatus systemInfo("System Information", 20, 300);        // System status display - temporarily disabled

// Variables to track system state
int currentBrightness = 128;     // Current PWM brightness value (0-255)
const int UPDATE_RATE = 100;    // How often to update sensor data (milliseconds) - fixed rate
unsigned long lastSensorUpdate = 0; // Track when we last updated the sensor
unsigned long lastStatusPrint = 0;  // Track when we last printed status
int buttonPressCount = 0;        // Track how many times the button was pressed

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  Serial.println("Starting WebGUI AP_Basic Example...");
  
  // Configure hardware pins
  pinMode(TOGGLE_LED_PIN, OUTPUT);
  pinMode(PWM_LED_PIN, OUTPUT);
  digitalWrite(TOGGLE_LED_PIN, LOW);  // Start with toggle LED off
  digitalWrite(PWM_LED_PIN, LOW);     // Start with PWM LED off
  
  // Configure WiFi Access Point
  // This creates a WiFi network that others can connect to
  GUI.startAP(AP_NAME, AP_PASSWORD);
  
  // Configure the web interface appearance
  GUI.setTitle(PAGE_TITLE);              // Browser tab title
  GUI.setHeading(PAGE_HEADING);          // Page heading
  GUI.setTheme(WEBGUI_DEFAULT_THEME);    // Use default light theme
  
  // Add all control elements to the web interface
  // The order you add them determines their order on the page
  GUI.addElement(&ledToggle);     // Add LED toggle switch
  GUI.addElement(&actionButton);  // Add action button
  GUI.addElement(&pwmSlider);     // Add brightness slider
  GUI.addElement(&randomSensor);  // Add sensor status display
  GUI.addElement(&buttonCounter); // Add button press counter
  // GUI.addElement(&systemInfo);    // Add system information display - temporarily disabled
  
  // Start the web server
  GUI.begin();
  
  // Print connection information to Serial Monitor
  Serial.println("===============================================");
  Serial.println("WebGUI Basic Example - Access Point Mode");
  Serial.println("===============================================");
  Serial.println("WiFi Network Name: " + String(AP_NAME));
  Serial.println("WiFi Password: " + String(AP_PASSWORD));
  Serial.println("Web Interface: http://192.168.4.1 (fixed IP)");
  Serial.println("===============================================");
  Serial.println("Connect your device to the WiFi network above,");
  Serial.println("then open the web address in your browser.");
  Serial.println("===============================================");
}

/*
  Function: updateRandomSensor()
  
  This function simulates reading data from a sensor and updates the web display.
  In a real project, you would replace the random number generation with
  actual sensor readings (temperature, humidity, distance, etc.).
*/
void updateRandomSensor() {
  // Check if enough time has passed since the last update
  if (millis() - lastSensorUpdate >= UPDATE_RATE) {
    
    // Generate a random sensor value (simulating real sensor data)
    // In a real project, this would be something like:
    // int sensorValue = analogRead(A0);
    // float temperature = readTemperatureSensor();
    int randomValue = random(0, 1024);  // Random number between 0-1023
    
    // Update the sensor status display on the web page
    // The setValue() function accepts different data types:
    randomSensor.setValue(String(randomValue) );  
    // You could also use: randomSensor.setValue(randomValue);  // Just the number
    
    // Print to Serial Monitor for debugging
    Serial.println("Sensor updated: " + String(randomValue));
    
    // Remember when we last updated
    lastSensorUpdate = millis();
  }
}

/*
  Function: printSystemStatus()
  
  This function prints the current system status to the Serial Monitor.
  Useful for debugging and monitoring your system.
*/
void printSystemStatus() {
  Serial.println("--- System Status ---");
  Serial.println("Toggle LED (Built-in): " + String(ledToggle.isOn() ? "ON" : "OFF"));
  Serial.println("PWM LED Brightness (Pin 3): " + String(currentBrightness) + "/255");
  Serial.println("Button Press Count: " + String(buttonPressCount));
  Serial.println("Update Rate: " + String(UPDATE_RATE) + " ms (fixed)");
  Serial.println("Uptime: " + String(millis() / 1000) + " seconds");
  Serial.println("--------------------");
}

void loop() {
  // CRITICAL: Always call GUI.update() in your main loop
  // This processes web requests and updates control values
  GUI.update();
  
  // Check if the LED toggle switch was changed
  if (ledToggle.wasToggled()) 
  {
    bool ledState = ledToggle.isOn();  // Get the current toggle state
    Serial.println("Toggle LED: " + String(ledState ? "ON" : "OFF"));
    
    // Control the built-in LED with simple on/off
    digitalWrite(TOGGLE_LED_PIN, ledState);
  }
  
  // Check if the action button was pressed
  if (actionButton.wasPressed()) 
  {
    buttonGo();  // Call your custom function
  }
  
  // Read the current values from the sliders
  currentBrightness = pwmSlider.getIntValue();    // Get brightness (0-255)
  
  // Control the PWM LED with brightness slider (always active)
  analogWrite(PWM_LED_PIN, currentBrightness);
  
 
  
  // Update the random sensor reading at the specified rate
  updateRandomSensor();
  
  // Update system information at the same rate (without frequent memory calls)
  // systemInfo.updateSystemInfo(getFreeMemory(), millis() / 1000);
  
  // Print status information at the update rate
  if (millis() - lastStatusPrint >= UPDATE_RATE) {
    printSystemStatus();
    
    lastStatusPrint = millis();
  }
  
  // Small delay to prevent overwhelming the system
  delay(10);
}

/*
  Function: buttonGo()
  
  This function is called when the "Print Message" button is pressed.
  Replace this empty function with your own custom action:
  - Send data to a server
  - Trigger a motor or actuator
  - Start/stop a process
  - Save settings to EEPROM
  - Take a sensor reading
  - Send an email or notification
  - Reset a counter or timer
  - Toggle a relay or output pin
  - etc.
*/
void buttonGo() {
  // Increment the button press counter
  buttonPressCount++;
  
  // Update the button counter display on the web page
  buttonCounter.setValue(buttonPressCount);
  
  // Add your custom button action here
  Serial.println("Button pressed #" + String(buttonPressCount) + " - add your custom code here!");
  
  // Examples of what you might do:
  // sendDataToServer();
  // triggerMotor();
  // saveSettings();
  // takePhotoWithCamera();
  // resetCounters();
  // toggleRelay(RELAY_PIN);
}




