/*
  AP_SaveSettings.ino - WebGUI Library Persistent Settings Example (Access Point Mode)
  
  DESCRIPTION:
  This example demonstrates the simplified persistent settings API in the WebGUI library.
  Two sliders control configurable values, and a save button stores them permanently using
  the built-in settings management system. Values are automatically restored on startup.
  
  FEATURES:
  - Two configurable sliders with persistent storage
  - Save button to store current values to memory
  - Automatic restoration of saved values on startup
  - Visual feedback showing save status
  - Access Point Mode (creates its own WiFi network)
  - Simplified API - no manual EEPROM/Preferences handling needed
  
  HARDWARE REQUIRED:
  - Arduino UNO R4 WiFi, Arduino Nano 33 IoT, or ESP32
  - No external components needed
  
  NETWORK SETUP:
  1. Upload this sketch to your Arduino
  2. Open Serial Monitor to see the Access Point details
  3. Connect your device to the "Settings-Controller" WiFi network (password: "settings123")
  4. Open a web browser and navigate to http://192.168.4.1
  5. Adjust sliders and click "Save Settings" to store values permanently
  6. Reset the board - values will be restored automatically
  
  This example code is in the public domain and can be used as a starting
  point for your own projects.
*/

#include <WebGUI.h>

// Access Point configuration
const char* AP_SSID = "Settings-Controller";   // Access Point name
const char* AP_PASSWORD = "settings123";       // Access Point password (min 8 characters)

// Web interface configuration
const char* PAGE_TITLE = "Persistent Settings Controller";

// Configurable values - loaded from persistent storage
int setting1_value;
int setting2_value;

// Control elements
Slider setting1Slider("Motor Speed", 20, 50, 0, 100, 50);      // Range: 0-100, Default: 50
Slider setting2Slider("Sensor Threshold", 20, 120, 0, 50, 25); // Range: 0-50, Default: 25
Button saveButton("Save Settings", 20, 190, 150, 40);
SensorStatus saveStatus("Save Status", 20, 240, 300);           // Status display for feedback

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  Serial.println("Starting WebGUI Persistent Settings Example (Access Point Mode)...");
  
  // Initialize the settings system
  GUI.initSettings();
  
  // Load saved values from persistent storage
  setting1_value = GUI.loadIntSetting("setting1");  
  setting2_value = GUI.loadIntSetting("setting2");  
  
  Serial.println("Loaded persistent values:");
  Serial.println("  Motor Speed: " + String(setting1_value));
  Serial.println("  Sensor Threshold: " + String(setting2_value));
  
  // Update sliders with loaded values
  setting1Slider.setValue(setting1_value);
  setting2Slider.setValue(setting2_value);
  
  // Start Access Point
  Serial.println("Starting Access Point: " + String(AP_SSID));
  GUI.startAP(AP_SSID, AP_PASSWORD);
  
  Serial.println("Access Point started successfully!");
  Serial.println("Network Name (SSID): " + String(AP_SSID));
  Serial.println("Password: " + String(AP_PASSWORD));
  Serial.println("Web interface available at: http://192.168.4.1");
  
  // Configure the web interface appearance
  GUI.setTitle(PAGE_TITLE);
  
  // Add control elements to the web interface
  GUI.addElement(&setting1Slider);
  GUI.addElement(&setting2Slider);
  GUI.addElement(&saveButton);
  GUI.addElement(&saveStatus);
  
  // Start the web server
  GUI.begin();
  
  // Initialize save status display
  saveStatus.setValue("Ready to save settings");
  GUI.update();
  
  Serial.println("Persistent Settings Controller ready!");
  Serial.println("Connect to WiFi '" + String(AP_SSID) + "' and visit http://192.168.4.1");
  Serial.println("Adjust sliders and click 'Save Settings' to store values permanently.");
}

void loop() {
  // CRITICAL: Always call GUI.update() in your main loop
  // This processes web requests and updates control values
  GUI.update();
  
  // Read current values from sliders
  setting1_value = setting1Slider.getIntValue();
  setting2_value = setting2Slider.getIntValue();
  
  // Check if save button was pressed
  if (saveButton.wasPressed()) {
    Serial.println("Save button pressed - storing values to persistent memory...");
    
    // Show saving status
    saveStatus.setValue("Saving settings...");
    
    // Save current values using the simplified API
    GUI.saveSetting("setting1", setting1_value);
    GUI.saveSetting("setting2", setting2_value);
    
    // Show success status with saved values
    saveStatus.setValue("Settings saved! Motor:" + String(setting1_value) + " Sensor:" + String(setting2_value));
    
    Serial.println("Values saved to persistent storage:");
    Serial.println("  Motor Speed: " + String(setting1_value) + " (range: 0-100)");
    Serial.println("  Sensor Threshold: " + String(setting2_value) + " (range: 0-50)");
    Serial.println("Save operation completed successfully!");
    Serial.println("Values will be restored automatically after reset.");
  }
  
  // Small delay to prevent overwhelming the system
  delay(10);
}
