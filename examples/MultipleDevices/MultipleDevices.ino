#include <WebGUI.h>

/*
  Multiple Devices Example - WebGUI Library
  
  This example shows how to control multiple devices/sensors
  from a single WebGUI interface. Perfect for home automation
  or multi-device projects.
  
  Features:
  - Multiple device control
  - Real-time sensor monitoring
  - Device status indicators
  - Grouped controls
  
  Hardware connections:
  - LED on pin 2 (built-in LED)
  - Servo on pin 9
  - Fan/Motor on pin 10
  - Temperature sensor on A0 (optional)
  
  Created by WebGUI Library Contributors
  This example code is in the public domain.
*/

#include <Servo.h>

// Hardware pins
const int LED_PIN = 2;
const int SERVO_PIN = 9;
const int FAN_PIN = 10;
const int TEMP_SENSOR_PIN = A0;

// Servo object
Servo myServo;

// Device Control Buttons (Row 1)
Button ledToggle("LED", 20, 50, 90, 40);
Button servoHome("Servo Home", 120, 50, 110, 40);
Button fanToggle("Fan", 240, 50, 90, 40);
Button emergencyStop("STOP ALL", 340, 50, 110, 40);

// Control Sliders (Column 1)
Slider ledBrightness("LED Brightness", 20, 110, 0, 255, 128, 180);
Slider servoAngle("Servo Angle", 20, 190, 0, 180, 90, 180);
Slider fanSpeed("Fan Speed", 20, 270, 0, 255, 100, 180);

// Monitoring Displays (Column 2) - Read-only status
SensorStatus temperatureDisplay("Temperature");
SensorStatus systemLoad("System Load");
SensorStatus uptime("Uptime");

// Device states
bool ledState = false;
bool fanState = false;
int currentBrightness = 128;
int currentServoAngle = 90;
int currentFanSpeed = 100;
unsigned long startTime;

void setup() {
  Serial.begin(115200);
  
  // Initialize hardware
  pinMode(LED_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  myServo.attach(SERVO_PIN);
  myServo.write(90); // Center position
  
  startTime = millis();
  
  // Start WiFi Access Point
  GUI.startAP("MultiDevice-Hub", "devices123");
  
  // Configure interface
  GUI.setTitle("Multi-Device Controller");
  GUI.setHeading("Smart Home Control Center");
  
  // Add custom styling for device groups
  GUI.setCustomCSS(
    "body { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); } "
    ".container { background: rgba(255,255,255,0.95); } "
    "h1 { color: #2c3e50; text-align: center; } "
    
    // LED Controls (Green theme)
    "#element_0 { background: linear-gradient(145deg, #27ae60, #2ecc71); border-radius: 10px; } "
    "#element_4_container { border-left: 4px solid #27ae60; } "
    
    // Servo Controls (Blue theme)  
    "#element_1 { background: linear-gradient(145deg, #3498db, #5dade2); border-radius: 10px; } "
    "#element_5_container { border-left: 4px solid #3498db; } "
    
    // Fan Controls (Orange theme)
    "#element_2 { background: linear-gradient(145deg, #e67e22, #f39c12); border-radius: 10px; } "
    "#element_6_container { border-left: 4px solid #e67e22; } "
    
    // Emergency Stop (Red theme)
    "#element_3 { background: linear-gradient(145deg, #e74c3c, #c0392b); border-radius: 10px; color: white; font-weight: bold; } "
    
    // Status displays (Gray theme)
    "#element_7_container, #element_8_container, #element_9_container { "
    "  border-left: 4px solid #95a5a6; background: #ecf0f1; border-radius: 8px; "
    "} "
  );
  
  // Add all controls
  GUI.addElement(&ledToggle);
  GUI.addElement(&servoHome);
  GUI.addElement(&fanToggle);
  GUI.addElement(&emergencyStop);
  GUI.addElement(&ledBrightness);
  GUI.addElement(&servoAngle);
  GUI.addElement(&fanSpeed);
  GUI.addElement(&temperatureDisplay);
  GUI.addElement(&systemLoad);
  GUI.addElement(&uptime);
  
  // Start web server
  GUI.begin();
  
  Serial.println("=== Multi-Device Control Hub ===");
  Serial.println("WiFi Access Point: MultiDevice-Hub");
  Serial.println("Password: devices123");
  Serial.println("Web Interface: http://" + GUI.getIP());
  Serial.println("Device Status: All systems ready");
  Serial.println("================================");
}

void loop() {
  GUI.update();
  
  // Handle button controls
  if (ledToggle.wasPressed()) {
    ledState = !ledState;
    Serial.println("LED " + String(ledState ? "ON" : "OFF"));
  }
  
  if (servoHome.wasPressed()) {
    currentServoAngle = 90;
    servoAngle.setValue(90);
    myServo.write(90);
    Serial.println("Servo moved to home position");
  }
  
  if (fanToggle.wasPressed()) {
    fanState = !fanState;
    if (!fanState) {
      analogWrite(FAN_PIN, 0);
      currentFanSpeed = 0;
      fanSpeed.setValue(0);
    }
    Serial.println("Fan " + String(fanState ? "ON" : "OFF"));
  }
  
  if (emergencyStop.wasPressed()) {
    // Emergency stop all devices
    ledState = false;
    fanState = false;
    currentBrightness = 0;
    currentFanSpeed = 0;
    currentServoAngle = 90;
    
    digitalWrite(LED_PIN, LOW);
    analogWrite(FAN_PIN, 0);
    myServo.write(90);
    
    // Reset sliders
    ledBrightness.setValue(0);
    fanSpeed.setValue(0);
    servoAngle.setValue(90);
    
    Serial.println("EMERGENCY STOP - All devices stopped!");
  }
  
  // Read control slider values
  currentBrightness = ledBrightness.getIntValue();
  currentServoAngle = servoAngle.getIntValue();
  currentFanSpeed = fanSpeed.getIntValue();
  
  // Apply device controls
  if (ledState) {
    analogWrite(LED_PIN, currentBrightness);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  
  myServo.write(currentServoAngle);
  
  if (fanState) {
    analogWrite(FAN_PIN, currentFanSpeed);
  } else {
    analogWrite(FAN_PIN, 0);
  }
  
  // Update monitoring displays
  updateMonitoringDisplays();
  
  // Status reporting
  static unsigned long lastStatus = 0;
  if (millis() - lastStatus > 5000) {
    printSystemStatus();
    lastStatus = millis();
  }
  
  delay(10);
}

void updateMonitoringDisplays() {
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate < 1000) return; // Update every second
  
  // Simulate temperature reading (replace with real sensor)
  int tempReading = analogRead(TEMP_SENSOR_PIN);
  int temperature = map(tempReading, 0, 1023, 15, 35); // Simulate 15-35°C range
  temperatureDisplay.setValue(String(temperature) + " C");
  
  // Calculate system load based on active devices
  int load = 0;
  if (ledState) load += 20;
  if (fanState) load += 40;
  if (currentServoAngle != 90) load += 15; // Servo active if not at home
  load += random(5, 15); // Add some variation
  systemLoad.setValue(String(constrain(load, 0, 100)) + "%");
  
  // Update uptime in minutes
  unsigned long uptimeMinutes = (millis() - startTime) / 60000;
  uptime.setValue(String(min((int)uptimeMinutes, 1440)) + " min");
  
  lastUpdate = millis();
}

void printSystemStatus() {
  Serial.println("--- System Status ---");
  Serial.println("LED: " + String(ledState ? "ON" : "OFF") + " (Brightness: " + String(currentBrightness) + ")");
  Serial.println("Servo: " + String(currentServoAngle) + " degrees");
  Serial.println("Fan: " + String(fanState ? "ON" : "OFF") + " (Speed: " + String(currentFanSpeed) + ")");
  Serial.println("Temperature: " + String(map(analogRead(TEMP_SENSOR_PIN), 0, 1023, 15, 35)) + " C");
  Serial.println("System Load: " + String(constrain((ledState ? 20 : 0) + (fanState ? 40 : 0) + (currentServoAngle != 90 ? 15 : 0) + random(5, 15), 0, 100)) + "%");
  Serial.println("Uptime: " + String((millis() - startTime) / 60000) + " minutes");
  Serial.println("--------------------");
}
