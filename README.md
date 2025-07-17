# WebGUI Library for Arduino

[![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue.svg)](https://www.arduino.cc/)
[![ESP32](https://img.shields.io/badge/ESP32-Compatible-green.svg)](https://www.espressif.com/en/products/socs/esp32)
[![License: LGPL v2.1](https://img.shields.io/badge/License-LGPL%20v2.1-blue.svg)](https://www.gnu.org/licenses/lgpl-2.1)

A simple and powerful web-based GUI library for Arduino that enables you to create beautiful control interfaces accessible through any web browser. Perfect for IoT projects, home automation, and remote device control.

## Table of Contents

- [Features](#features)
- [Compatible Hardware](#compatible-hardware)
- [Installation](#installation)
- [Quick Start](#quick-start)
- [API Reference](#api-reference)
  - [WebGUI Class](#webgui-class)
  - [Button Class](#button-class)
  - [Toggle Class](#toggle-class)
  - [Slider Class](#slider-class)
  - [SensorStatus Class](#sensorstatus-class)
- [Styling and Themes](#styling-and-themes)
- [Layout Guidelines](#layout-guidelines)
- [Network Configuration](#network-configuration)
- [Usage Instructions](#usage-instructions)
- [Advanced Examples](#advanced-examples)
- [Example Projects](#example-projects)
- [Troubleshooting](#troubleshooting)
- [License](#license)
- [Contributing](#contributing)
- [Support](#support)

## Features

- **Web-Based Interface**: Access your Arduino through any web browser - no app installation required
- **Mobile-Friendly**: Responsive design works on phones, tablets, and computers  
- **Interactive Controls**: Buttons, toggles, sliders, and status displays with real-time updates
- **Memory Optimized**: Streaming HTML generation prevents crashes on memory-constrained devices
- **Debounced Input**: Smart client-side debouncing prevents network flooding
- **Status Monitoring**: Read-only displays for sensor data and system status
- **Dual WiFi Modes**: Access Point mode for standalone operation or Station mode for existing networks
- **Customizable Themes**: Built-in themes and custom CSS support
- **Real-Time Updates**: Instant feedback between web interface and Arduino
- **Cross-Platform**: Works on multiple Arduino boards with excellent performance
- **Enhanced Toggle Controls**: New Toggle class for switch-like interactions

## Compatible Hardware

- **Arduino UNO R4 WiFi** *Fully tested and optimized*
- **Arduino Nano 33 IoT** *Fully tested and optimized*
- **ESP32** (all variants) *Excellent performance*

### Memory Requirements
- **Minimum RAM**: 8KB recommended (library uses optimized streaming)
- **Flash**: ~65KB for full feature set including examples
- **Performance**: Handles 8+ GUI elements on Arduino UNO R4 WiFi

## Installation

### Arduino IDE Library Manager (Recommended)
1. Open Arduino IDE
2. Go to **Sketch** → **Include Library** → **Manage Libraries**
3. Search for "WebGUI"
4. Click **Install**

### Manual Installation
1. Download the latest release from [GitHub](https://github.com/npuckett/WebGUI)
2. Extract the ZIP file
3. Copy the `WebGUI` folder to your Arduino `libraries` directory
4. Restart Arduino IDE

## Quick Start

```cpp
#include <WebGUI.h>

// Create a simple LED toggle control
Toggle ledToggle("Built-in LED", 20, 50, 120);

void setup() {
  Serial.begin(115200);
  
  // Configure built-in LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  // Start WiFi Access Point
  GUI.startAP("My-Arduino", "password123");
  
  // Configure interface
  GUI.setTitle("LED Controller");
  
  // Add toggle to GUI
  GUI.addElement(&ledToggle);
  
  // Start web server
  GUI.begin();
  
  Serial.println("Connect to WiFi: My-Arduino");
  Serial.println("Web interface: http://" + GUI.getIP());
}

void loop() {
  GUI.update();  // Always call this in loop()
  
  // Check if toggle was switched
  if (ledToggle.wasToggled()) {
    bool isOn = ledToggle.isOn();
    digitalWrite(LED_BUILTIN, isOn);
    Serial.println("Built-in LED: " + String(isOn ? "ON" : "OFF"));
  }
}
```

## API Reference

### WebGUI Class

The main class that manages the web interface, WiFi connection, and all GUI elements.

#### Setup Methods

**startAP(ssid, password)** - Create WiFi Access Point
```cpp
void setup() {
  // Create a standalone WiFi network
  GUI.startAP("MyDevice", "mypassword");
  // Device will be accessible at 192.168.4.1
}
```

**connectWiFi(ssid, password)** - Connect to existing WiFi
```cpp
void setup() {
  // Connect to your home/office WiFi
  if (GUI.connectWiFi("HomeWiFi", "wifipass")) {
    Serial.println("Connected! IP: " + GUI.getIP());
  } else {
    Serial.println("Connection failed");
  }
}
```

**Static IP Configuration** - Set fixed IP address in Station mode
```cpp
void setup() {
  // Configure static IP before connecting (optional)
  IPAddress staticIP(192, 168, 1, 100);    // Desired static IP
  IPAddress gateway(192, 168, 1, 1);       // Router IP
  IPAddress subnet(255, 255, 255, 0);      // Subnet mask
  IPAddress dns(8, 8, 8, 8);               // DNS server
  
  WiFi.config(staticIP, gateway, subnet, dns);
  
  // Connect with static IP configuration
  GUI.connectWiFi("HomeWiFi", "wifipass");
  Serial.println("Static IP: " + GUI.getIP());
  // Will print: "Static IP: 192.168.1.100"
}
```

**begin()** - Start web server
```cpp
void setup() {
  // Always call after adding all elements
  GUI.addElement(&myButton);
  GUI.addElement(&mySlider);
  GUI.begin();  // Start the web server
}
```

**update()** - Process web requests (call in loop)
```cpp
void loop() {
  GUI.update();  // REQUIRED: Call every loop iteration
  
  // Your code here...
}
```

#### Configuration Methods

**setTitle(title)** - Set browser tab title and page heading
```cpp
void setup() {
  GUI.setTitle("Smart Home Controller");
  // Browser tab will show "Smart Home Controller"
  // Page heading will also be "Smart Home Controller"
}
```

**setCustomCSS(css)** - Add custom styling
```cpp
void setup() {
  GUI.setCustomCSS(
    "body { background: linear-gradient(135deg, #667eea, #764ba2); }"
    ".webgui-button { border-radius: 20px; }"
  );
}
```

**addElement(element)** - Add control to interface
```cpp
Button myBtn("Test", 20, 50);
Slider mySlider("Speed", 20, 100, 0, 100, 50);

void setup() {
  GUI.addElement(&myBtn);     // Add button
  GUI.addElement(&mySlider);  // Add slider
}
```

**getIP()** - Get device IP address
```cpp
void setup() {
  GUI.begin();
  Serial.println("Access at: http://" + GUI.getIP());
}
```

### Button Class

Interactive button control for triggering actions.

#### Constructor
```cpp
Button(label, x, y, width=100, height=40);
```

**Example:**
```cpp
Button powerBtn("Power", 20, 50, 120, 45);    // Custom size
Button resetBtn("Reset", 160, 50);            // Default size
```

#### Methods

**wasPressed()** - Check if button was clicked
```cpp
Button actionBtn("Action", 20, 50);

void loop() {
  GUI.update();
  
  if (actionBtn.wasPressed()) {
    Serial.println("Action button clicked!");
    // Trigger one-time action
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  }
}
```

**isPressed()** - Check current press state
```cpp
Button holdBtn("Hold", 20, 50);

void loop() {
  GUI.update();
  
  if (holdBtn.isPressed()) {
    // Continuous action while held
    analogWrite(6, 255);  // LED at full brightness
  } else {
    analogWrite(6, 0);    // LED off
  }
}
```

**setButtonStyle(style)** - Set button appearance
```cpp
Button primaryBtn("Save", 20, 50);
Button dangerBtn("Delete", 140, 50);

void setup() {
  primaryBtn.setButtonStyle("primary");  // Blue button
  dangerBtn.setButtonStyle("danger");    // Red button
  // Options: "primary", "secondary", "success", "danger", "warning"
}
```

### Toggle Class

Switch-like control for on/off states with visual feedback.

#### Constructor
```cpp
Toggle(label, x, y, width=80);
```

**Example:**
```cpp
Toggle ledToggle("Room Light", 20, 50, 100);   // Custom width
Toggle fanToggle("Ceiling Fan", 20, 120);      // Default width
```

#### Methods

**wasToggled()** - Check if toggle state changed
```cpp
Toggle lightToggle("Light", 20, 50);

void loop() {
  GUI.update();
  
  if (lightToggle.wasToggled()) {
    bool isOn = lightToggle.isOn();
    digitalWrite(2, isOn);  // Control LED on pin 2
    Serial.println("Light: " + String(isOn ? "ON" : "OFF"));
  }
}
```

**isOn()** - Get current toggle state
```cpp
Toggle pumpToggle("Water Pump", 20, 50);

void loop() {
  GUI.update();
  
  // Continuous state checking
  if (pumpToggle.isOn()) {
    digitalWrite(PUMP_PIN, HIGH);
  } else {
    digitalWrite(PUMP_PIN, LOW);
  }
}
```

**setToggleState(state)** - Programmatically set state
```cpp
Toggle autoToggle("Auto Mode", 20, 50);

void setup() {
  autoToggle.setToggleState(true);  // Start in ON position
}

void loop() {
  // Can change state based on conditions
  if (sensorValue > 100) {
    autoToggle.setToggleState(false);  // Force OFF
  }
}
```

### Slider Class

Continuous value control with customizable range.

#### Constructor
```cpp
Slider(label, x, y, minValue, maxValue, defaultValue, width=300);
```

**Example:**
```cpp
Slider brightness("Brightness", 20, 50, 0, 255, 128);      // 8-bit range
Slider temperature("Target °C", 20, 120, 15, 30, 22);      // Temperature range
Slider volume("Volume %", 20, 190, 0, 100, 50, 250);       // Custom width
```

#### Methods

**getIntValue()** - Get current value as integer
```cpp
Slider speedSlider("Motor Speed", 20, 50, 0, 255, 100);

void loop() {
  GUI.update();
  
  int speed = speedSlider.getIntValue();
  analogWrite(MOTOR_PIN, speed);  // PWM control
  
  Serial.println("Speed: " + String(speed));
}
```

**getFloatValue()** - Get current value as float
```cpp
Slider voltageSlider("Voltage", 20, 50, 0, 5, 3.3);

void loop() {
  GUI.update();
  
  float voltage = voltageSlider.getFloatValue();
  // Use for precise analog control
  dacWrite(DAC_PIN, (int)(voltage * 51.2));  // ESP32 DAC
}
```

**setValue(value)** - Set slider value programmatically
```cpp
Slider targetSlider("Target", 20, 50, 0, 100, 50);

void loop() {
  GUI.update();
  
  // Auto-adjust based on sensor
  int sensorReading = analogRead(A0);
  int targetValue = map(sensorReading, 0, 1023, 0, 100);
  targetSlider.setValue(targetValue);
}
```

**setRange(min, max)** - Change min/max values
```cpp
Slider configSlider("Value", 20, 50, 0, 100, 50);

void setup() {
  // Can change range based on mode
  if (advancedMode) {
    configSlider.setRange(0, 1000);  // Extended range
  } else {
    configSlider.setRange(0, 10);    // Simple range
  }
}
```

### SensorStatus Class

Read-only display for sensor data and system status.

#### Constructor
```cpp
SensorStatus(label);
```

**Example:**
```cpp
SensorStatus temperature("Temperature");
SensorStatus humidity("Humidity"); 
SensorStatus uptime("System Uptime");
SensorStatus memoryFree("Free RAM");
```

#### Methods

**setValue(int value)** - Set integer value
```cpp
SensorStatus counter("Button Presses");
int pressCount = 0;

void loop() {
  GUI.update();
  
  if (myButton.wasPressed()) {
    pressCount++;
    counter.setValue(pressCount);  // Shows: "Button Presses: 5"
  }
}
```

**setValue(float value)** - Set float value with precision
```cpp
SensorStatus voltage("Battery Voltage");

void loop() {
  GUI.update();
  
  float volts = analogRead(A0) * (5.0 / 1023.0);
  voltage.setValue(volts);  // Shows: "Battery Voltage: 3.42"
}
```

**setValue(bool value)** - Set boolean value (true/false)
```cpp
SensorStatus doorStatus("Door");
SensorStatus alarmStatus("Alarm Active");

void loop() {
  GUI.update();
  
  bool doorOpen = digitalRead(DOOR_SENSOR_PIN);
  doorStatus.setValue(doorOpen);     // Shows: "Door: true" or "Door: false"
  
  alarmStatus.setValue(!doorOpen);   // Shows: "Alarm Active: true" when door closed
}
```

**setValue(String value)** - Set string value with units
```cpp
SensorStatus temperature("Temperature");
SensorStatus status("System Status");
SensorStatus location("GPS");

void loop() {
  GUI.update();
  
  // With units
  temperature.setValue("23.5°C");
  
  // Status messages
  if (WiFi.status() == WL_CONNECTED) {
    status.setValue("Connected");
  } else {
    status.setValue("Disconnected");
  }
  
  // Complex data
  location.setValue("40.7128°N, 74.0060°W");
}

## Styling and Themes

### Built-in Themes
The WebGUI library uses a single optimized theme designed for readability and performance across all devices.

### Custom CSS
```cpp
GUI.setCustomCSS(
  ".webgui-button { "
  "  background: linear-gradient(145deg, #ff6b6b, #ee5a52); "
  "  border-radius: 20px; "
  "}"
);
```

## Memory Optimization & Performance

### Memory Efficiency Features
The WebGUI library is optimized for memory-constrained devices like Arduino UNO R4 WiFi:

- **Streaming HTML Generation**: Prevents memory crashes by sending HTML in chunks
- **Minimal CSS Footprint**: ~400 bytes vs typical 9KB+ CSS frameworks  
- **Dynamic Memory Management**: Elements only consume memory when actively used
- **Optimized String Handling**: Reduces heap fragmentation

### Performance Benchmarks
**Arduino UNO R4 WiFi (32KB RAM):**
- **4 Elements**: Original stable limit  
- **8 Elements**: Proven with optimization (2x improvement)
- **Memory Usage**: ~16.8% RAM usage with 8 elements
- **Response Time**: <100ms for control updates

### Memory Monitoring
```cpp
// Cross-platform memory monitoring
int getFreeRAM() {
#ifdef ARDUINO_UNOR4_WIFI
  char dummy;
  return (int)&dummy - 0x20000000;
#else
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
#endif
}

SensorStatus memoryStatus("Free RAM");

void loop() {
  GUI.update();
  
  // Monitor memory usage
  memoryStatus.setValue(String(getFreeRAM()) + " bytes");
  
  // Memory warning
  if (getFreeRAM() < 1000) {
    Serial.println("Low memory warning!");
  }
}
```

### Best Practices for Performance
1. **Call GUI.update() once per loop**: Avoid multiple calls
2. **Use appropriate delays**: `delay(10)` recommended for responsive UI
3. **Monitor memory usage**: Especially when adding many elements
4. **Batch status updates**: Update displays every 1-2 seconds, not every loop
5. **Use appropriate data types**: `int` for simple values, `String` only when needed

## Layout Guidelines

### Button Positioning
- Height: 30-40px recommended
- Horizontal spacing: 20px minimum between buttons
- Example: `Button("Btn1", 20, 50)`, `Button("Btn2", 140, 50)`

### Slider Positioning  
- Height: ~60px total (includes label and padding)
- Vertical spacing: 70px recommended between sliders
- Example: `Slider("S1", 20, 100, ...)`, `Slider("S2", 20, 170, ...)`

### SensorStatus Layout
- Automatically positioned in order added
- No manual positioning required
- Displays as label: value pairs
- Perfect for monitoring sensor data without Serial output

## Network Configuration

### Access Point Mode (Standalone Operation)

**When to use:** Portable projects, demonstrations, areas without WiFi, temporary setups

```cpp
void setup() {
  GUI.startAP("MyDevice", "mypassword");
  // Creates network: "MyDevice" with password "mypassword"
  // Device accessible at: 192.168.4.1
}
```

**Benefits:**
- Works anywhere - no existing WiFi required
- Perfect for portable/mobile projects  
- Direct device-to-phone connection
- Consistent IP address (192.168.4.1)
- No network configuration needed

**Limitations:**
- No internet access while connected
- Only one client can connect at a time (typically)
- Must switch WiFi networks on control device

### Station Mode (Connect to Existing WiFi)

**When to use:** Home automation, permanent installations, internet-connected projects

```cpp
void setup() {
  if (GUI.connectWiFi("YourWiFi", "wifipassword")) {
    Serial.println("Connected! IP: " + GUI.getIP());
  } else {
    Serial.println("Failed to connect");
    // Optional: Fallback to AP mode
    GUI.startAP("Backup-Mode", "backup123");
  }
}
```

**Benefits:**
- Internet access maintained
- Multiple devices can access simultaneously
- Integration with home/office network
- Can access from anywhere on network
- Better for permanent installations

**Limitations:**
- Requires existing WiFi network
- IP address may change (use DHCP reservation)
- Network dependency (fails if WiFi down)

### Hybrid Approach (Recommended for Production)

```cpp
void setup() {
  Serial.begin(115200);
  
  // Try to connect to home WiFi first
  Serial.println("Attempting WiFi connection...");
  if (GUI.connectWiFi("HomeWiFi", "password")) {
    Serial.println("Connected to home network");
    Serial.println("Web interface: http://" + GUI.getIP());
  } else {
    // Fallback to Access Point mode
    Serial.println("WiFi failed, starting backup AP...");
    GUI.startAP("Device-Setup", "setup123");
    Serial.println("Setup mode: Connect to 'Device-Setup'");
    Serial.println("Web interface: http://192.168.4.1");
  }
  
  // Continue with setup...
  GUI.begin();
}
```

### Network Troubleshooting

**Getting the IP Address:**
```cpp
void setup() {
  GUI.connectWiFi("YourWiFi", "password");
  GUI.begin();
  
  // Always print IP for easy access
  Serial.println("======================");
  Serial.println("Web Interface Ready!");
  Serial.println("URL: http://" + GUI.getIP());
  Serial.println("======================");
}
```

**Check Connection Status:**
```cpp
void loop() {
  GUI.update();
  
  // Monitor connection status
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost!");
    // Handle reconnection logic
  }
}

## Usage Instructions

1. **Upload your sketch** to the Arduino
2. **Connect to WiFi network** created by Arduino (or your home WiFi if using station mode)
3. **Open web browser** and navigate to the IP address shown in Serial Monitor
4. **Control your project** using the web interface!

## Advanced Examples

## Advanced Examples

### Multi-Control Smart Home System
```cpp
#include <WebGUI.h>

// Control elements  
Button powerBtn("System Power", 20, 50);
Button resetBtn("Reset", 140, 50);
Toggle livingRoomLight("Living Room", 20, 100, 100);
Toggle kitchenLight("Kitchen", 140, 100, 100);
Toggle fanControl("Ceiling Fan", 260, 100, 100);
Slider brightness("Brightness %", 20, 150, 0, 100, 75);
Slider fanSpeed("Fan Speed", 20, 220, 0, 255, 100);

// Status displays
SensorStatus temperature("Temperature");
SensorStatus humidity("Humidity");
SensorStatus systemUptime("Uptime");
SensorStatus memoryFree("Free RAM");
SensorStatus wifiSignal("WiFi Signal");

void setup() {
  Serial.begin(115200);
  
  // Configure pins
  pinMode(2, OUTPUT);  // Living room LED
  pinMode(3, OUTPUT);  // Kitchen LED  
  pinMode(9, OUTPUT);  // Fan control
  pinMode(6, OUTPUT);  // Brightness PWM
  
  // Network setup with fallback
  if (GUI.connectWiFi("HomeWiFi", "password")) {
    Serial.println("Connected to home network");
  } else {
    GUI.startAP("SmartHome-Setup", "setup123");
    Serial.println("Setup mode active");
  }
  
  // Interface configuration
  GUI.setTitle("Smart Home Control");
  
  // Add all elements
  GUI.addElement(&powerBtn);
  GUI.addElement(&resetBtn);
  GUI.addElement(&livingRoomLight);
  GUI.addElement(&kitchenLight);
  GUI.addElement(&fanControl);
  GUI.addElement(&brightness);
  GUI.addElement(&fanSpeed);
  GUI.addElement(&temperature);
  GUI.addElement(&humidity);
  GUI.addElement(&systemUptime);
  GUI.addElement(&memoryFree);
  GUI.addElement(&wifiSignal);
  
  GUI.begin();
  Serial.println("Smart Home System Ready!");
  Serial.println("Access: http://" + GUI.getIP());
}

bool systemPowered = true;

void loop() {
  GUI.update();
  
  // System power control
  if (powerBtn.wasPressed()) {
    systemPowered = !systemPowered;
    if (!systemPowered) {
      // Turn off all devices
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      digitalWrite(9, LOW);
      analogWrite(6, 0);
    }
    Serial.println("System: " + String(systemPowered ? "ON" : "OFF"));
  }
  
  // Reset button
  if (resetBtn.wasPressed()) {
    Serial.println("System reset requested");
    // Reset all controls to default states
    livingRoomLight.setToggleState(false);
    kitchenLight.setToggleState(false);
    fanControl.setToggleState(false);
    brightness.setValue(75);
    fanSpeed.setValue(100);
  }
  
  if (systemPowered) {
    // Light controls
    if (livingRoomLight.wasToggled()) {
      digitalWrite(2, livingRoomLight.isOn());
    }
    
    if (kitchenLight.wasToggled()) {
      digitalWrite(3, kitchenLight.isOn());
    }
    
    // Fan control
    if (fanControl.wasToggled()) {
      if (fanControl.isOn()) {
        analogWrite(9, fanSpeed.getIntValue());
      } else {
        analogWrite(9, 0);
      }
    }
    
    // Brightness control
    int brightnessValue = map(brightness.getIntValue(), 0, 100, 0, 255);
    analogWrite(6, brightnessValue);
    
    // Fan speed update
    if (fanControl.isOn()) {
      analogWrite(9, fanSpeed.getIntValue());
    }
  }
  
  // Update status displays every 2 seconds
  static unsigned long lastUpdate = 0;
  if (millis() - lastUpdate > 2000) {
    // Simulate sensor readings
    temperature.setValue("22.5°C");
    humidity.setValue("45%");
    systemUptime.setValue(String(millis() / 60000) + " min");
    
    // Real system stats
    memoryFree.setValue(String(getFreeRAM()) + " bytes");
    wifiSignal.setValue(String(WiFi.RSSI()) + " dBm");
    
    lastUpdate = millis();
  }
}

// Memory monitoring function
int getFreeRAM() {
#ifdef ARDUINO_UNOR4_WIFI
  char dummy;
  return (int)&dummy - 0x20000000;
#else
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
#endif
}
```

### Servo Control System with Position Feedback
```cpp
#include <WebGUI.h>
#include <Servo.h>

Servo myServo;

// Controls
Slider positionSlider("Target Position", 20, 50, 0, 180, 90);
Button homeBtn("Home Position", 20, 120);
Button sweepBtn("Auto Sweep", 140, 120);
Toggle enableToggle("Servo Enable", 260, 120, 100);

// Status displays
SensorStatus currentPosition("Current Angle");
SensorStatus servoStatus("Servo Status");
SensorStatus targetReached("Target Reached");

int currentPos = 90;
int targetPos = 90;
bool sweepMode = false;
int sweepDirection = 1;

void setup() {
  Serial.begin(115200);
  
  myServo.attach(9);
  myServo.write(90);  // Start at center
  
  GUI.connectWiFi("YourWiFi", "password");
  GUI.setTitle("Servo Controller");
  
  GUI.addElement(&positionSlider);
  GUI.addElement(&homeBtn);
  GUI.addElement(&sweepBtn);
  GUI.addElement(&enableToggle);
  GUI.addElement(&currentPosition);
  GUI.addElement(&servoStatus);
  GUI.addElement(&targetReached);
  
  enableToggle.setToggleState(true);  // Start enabled
  
  GUI.begin();
  Serial.println("Servo Control: http://" + GUI.getIP());
}

void loop() {
  GUI.update();
  
  // Servo enable/disable
  if (enableToggle.wasToggled()) {
    if (enableToggle.isOn()) {
      myServo.attach(9);
      servoStatus.setValue("Enabled");
    } else {
      myServo.detach();
      servoStatus.setValue("Disabled");
      sweepMode = false;
    }
  }
  
  if (enableToggle.isOn()) {
    // Home position
    if (homeBtn.wasPressed()) {
      targetPos = 90;
      positionSlider.setValue(90);
      sweepMode = false;
      Serial.println("Homing servo...");
    }
    
    // Sweep mode toggle
    if (sweepBtn.wasPressed()) {
      sweepMode = !sweepMode;
      Serial.println("Sweep mode: " + String(sweepMode ? "ON" : "OFF"));
    }
    
    // Position control
    if (!sweepMode) {
      targetPos = positionSlider.getIntValue();
    } else {
      // Auto sweep logic
      static unsigned long lastSweepUpdate = 0;
      if (millis() - lastSweepUpdate > 20) {
        targetPos += sweepDirection;
        if (targetPos >= 180 || targetPos <= 0) {
          sweepDirection *= -1;
        }
        positionSlider.setValue(targetPos);
        lastSweepUpdate = millis();
      }
    }
    
    // Smooth servo movement
    if (currentPos != targetPos) {
      if (currentPos < targetPos) {
        currentPos++;
      } else {
        currentPos--;
      }
      myServo.write(currentPos);
      delay(15);  // Smooth movement
    }
  }
  
  // Update status displays
  currentPosition.setValue(String(currentPos) + "°");
  targetReached.setValue(currentPos == targetPos);
}

### Custom Styling
```cpp
void setup() {
  GUI.setCustomCSS(
    "body { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); }"
    ".webgui-button { box-shadow: 0 8px 16px rgba(0,0,0,0.3); }"
    ".webgui-slider { accent-color: #ff6b6b; }"
    ".webgui-status { background: #f8f9fa; border-left: 4px solid #007bff; }"
  );
}
```

### Debouncing Performance
The library automatically implements client-side debouncing for sliders to prevent network flooding:
- Default debounce delay: 100ms
- Prevents rapid network requests during slider dragging
- Maintains responsive UI while protecting Arduino from overload
- No additional configuration required
```

## Troubleshooting

## Troubleshooting

### Common Issues

**Can't connect to WiFi network**
- Check SSID and password in Serial Monitor output
- Verify WiFi credentials are correct (case-sensitive)
- Ensure device is in range of WiFi router
- Try restarting Arduino if network doesn't appear
- For Station mode: Use hybrid approach with AP fallback

**Web page doesn't load**
- Verify IP address in Serial Monitor
- Check that you're connected to Arduino's WiFi network (AP mode)
- Try accessing `192.168.4.1` (default AP IP)
- For Station mode: Ensure you're on the same network
- Clear browser cache and try incognito/private mode

**Controls don't respond**
- Ensure `GUI.update()` is called in `loop()` function
- Check Serial Monitor for error messages
- Verify control positioning doesn't overlap other elements
- Check if `GUI.begin()` was called after adding all elements

**Memory issues / Crashes**
- Monitor memory usage with `getFreeRAM()` function
- Reduce number of GUI elements if memory is low
- Use optimized examples (Station_* versions) for better performance
- Avoid frequent String operations in main loop
- Consider using `SensorStatus` instead of Serial.print for debugging

**Slider performance issues**
- Library includes automatic debouncing (no action needed)
- If experiencing lag, check for Serial.print() in tight loops
- Reduce update frequency for non-critical status displays
- Use `delay(10)` in main loop for responsive UI

**SensorStatus not updating**
- Ensure you're calling `setValue()` with current data
- Check that `GUI.update()` is being called regularly
- Verify the status display was added with `GUI.addElement()`
- Update displays every 1-2 seconds, not every loop iteration

**Toggle switches not working**
- Use `wasToggled()` to detect state changes
- Use `isOn()` to check current state
- Ensure toggle was added to GUI with `GUI.addElement()`
- Check that WiFi connection is stable

### Performance Optimization

**For Arduino UNO R4 WiFi:**
- Maximum recommended elements: 8 (proven stable)
- Update status displays every 2-3 seconds maximum
- Use appropriate variable types (int vs String)
- Monitor free RAM regularly

**Network Optimization:**
- Use Station mode for permanent installations
- Use AP mode for portable/demonstration projects
- Implement connection monitoring for critical applications
- Consider hybrid mode for production systems

### Debug Techniques

**Memory Debugging:**
```cpp
void printMemoryStatus() {
  Serial.println("Free RAM: " + String(getFreeRAM()) + " bytes");
  if (getFreeRAM() < 2000) {
    Serial.println("Memory getting low!");
  }
}
```

**Network Debugging:**
```cpp
void checkWiFiStatus() {
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
  } else {
    Serial.println("WiFi disconnected");
  }
}
```

**Element Debugging:**
```cpp
// Use SensorStatus for real-time debugging without Serial
SensorStatus debugInfo("Debug");

void loop() {
  GUI.update();
  
  debugInfo.setValue("Button presses: " + String(buttonCount));
  // Better than Serial.println for web-accessible debugging
}
```

## Example Projects

The library includes comprehensive examples for both Access Point and Station Mode operation:

### Access Point Mode Examples

#### AP_Basic.ino
**Description**: Introduction to WebGUI with standalone WiFi network creation.
- **Features**: Basic button and slider, LED control, Access Point setup
- **Best for**: Learning the basics, portable projects, demonstrations  
- **Hardware**: LED on pin 2, Arduino UNO R4 WiFi or compatible
- **Network**: Creates "Arduino-WebGUI" network, accessible at 192.168.4.1

#### AP_ToggleAnLED.ino
**Description**: The simplest possible WebGUI example - just toggle the built-in LED on and off.
- **Features**: Single LED toggle control for built-in LED, minimal code example
- **Best for**: Learning the basics, first WebGUI project, no external components
- **Hardware**: Built-in LED only (no additional components needed)
- **Network**: Creates "My-Arduino" network, accessible at 192.168.4.1

#### AP_BlinkAnLED.ino
**Description**: LED blink control with toggle enable/disable and adjustable blink rate.
- **Features**: Toggle to enable/disable blinking, slider for blink rate control, millis-based timing
- **Best for**: Learning timing control, non-blocking LED patterns, rate adjustment
- **Hardware**: Built-in LED only (no additional components needed)
- **Network**: Creates "Arduino-BlinkLED" network for standalone blink control

#### AP_SwitchPanel.ino
**Description**: Multiple LED control using toggle switches in Access Point mode.
- **Features**: 4 LED toggle controls, individual LED management, real-time status
- **Best for**: Multi-device control, switch-like interfaces, panel control systems
- **Hardware**: LEDs on pins 2-5 with 220Ω resistors
- **Network**: Standalone operation, no external WiFi required

#### AP_SensorDebug.ino  
**Description**: Real-time sensor monitoring with web-based debug interface.
- **Features**: Potentiometer reading, temperature simulation, debug status displays
- **Best for**: Sensor development, debugging without Serial Monitor
- **Hardware**: Potentiometer on A0, optional temperature sensor
- **Network**: Access Point mode for field testing

#### AP_ServoTest.ino
**Description**: Servo motor control with angle adjustment and position feedback.
- **Features**: Servo position control, angle slider, position monitoring
- **Best for**: Robotics projects, mechanical control systems
- **Hardware**: Servo motor on pin 9, external power recommended
- **Network**: Portable servo control without WiFi dependency

### Station Mode Examples

#### Station_Basic.ino
**Description**: Connect to existing WiFi network for integrated home automation.
- **Features**: Home network integration, button and slider controls
- **Best for**: Permanent installations, home automation integration
- **Hardware**: LED on pin 2, Arduino UNO R4 WiFi or compatible  
- **Network**: Connects to your home WiFi, accessible from any network device

#### Station_ToggleAnLED.ino
**Description**: The simplest possible WebGUI example integrated with your home network.
- **Features**: Single LED toggle for built-in LED, home network integration
- **Best for**: Learning WiFi integration, first network-connected project
- **Hardware**: Built-in LED only (no additional components needed)
- **Network**: Connects to your home WiFi, accessible from any network device

#### Station_BlinkAnLED.ino
**Description**: Network-integrated LED blink control with rate adjustment.
- **Features**: Toggle enable/disable, slider rate control, home network access
- **Best for**: Remote LED control, learning network timing patterns
- **Hardware**: Built-in LED only (no additional components needed)
- **Network**: Home WiFi integration for remote blink control

#### Station_SwitchPanel.ino
**Description**: Multi-LED control integrated with home network.
- **Features**: 4 LED toggles, network integration, remote access capability
- **Best for**: Room lighting control, permanent LED installations, switch panels
- **Hardware**: LEDs on pins 2-5 with 220Ω resistors
- **Network**: Home WiFi integration, multiple device access

#### Station_SensorDebug.ino
**Description**: Network-integrated sensor monitoring for permanent installations.
- **Features**: Continuous sensor monitoring, network status, remote debugging
- **Best for**: Long-term monitoring, data collection, remote diagnostics
- **Hardware**: Potentiometer on A0, temperature sensor (optional)
- **Network**: Home network integration for remote monitoring

#### Station_ServoTest.ino
**Description**: Network-controlled servo system for automation projects.
- **Features**: Remote servo control, position feedback, network status monitoring
- **Best for**: Home automation, remote mechanical control, security systems
- **Hardware**: Servo motor on pin 9, external power supply recommended
- **Network**: Home WiFi for remote servo control

### Advanced Test Example

#### 8-LED Toggle Test (main.cpp in WebGUI-Dev)
**Description**: Stress test demonstrating optimized library capabilities.
- **Features**: 8 LED toggles, memory optimization validation, performance monitoring
- **Best for**: Testing library limits, demonstrating optimization improvements
- **Hardware**: LEDs on pins 2-9 with 220Ω resistors  
- **Performance**: Proves library can handle 2x original element limit
- **Network**: Station mode with comprehensive status monitoring

### Usage Instructions for Examples

1. **Choose your mode**: Access Point for portable/standalone or Station for home integration
2. **Select example**: Start with ToggleAnLED, then try BlinkAnLED, SwitchPanel, SensorDebug, or ServoTest
3. **Configure network**: Update WiFi credentials in Station mode examples
4. **Upload sketch**: Use Arduino IDE or PlatformIO
5. **Connect**: Access Point (192.168.4.1) or check Serial Monitor for Station IP
6. **Control**: Open web browser and interact with your project

### Progression Path
```
Beginner:     AP_ToggleAnLED → AP_BlinkAnLED → AP_SwitchPanel
Intermediate: Station_ToggleAnLED → Station_BlinkAnLED → Station_SensorDebug  
Advanced:     Station_ServoTest → 8-LED Test
```

Each example includes detailed comments and can serve as a starting point for your own projects. The Access Point examples work immediately without network setup, while Station examples provide better integration for permanent installations.

## License

This library is released under the GNU Lesser General Public License v2.1. See [LICENSE](LICENSE) for details.

## Contributing

Contributions are welcome! Please feel free to submit issues, feature requests, or pull requests.

## Support

- **Documentation**: [GitHub Wiki](https://github.com/npuckett/WebGUI/wiki)
- **Issues**: [GitHub Issues](https://github.com/npuckett/WebGUI/issues)
- **Examples**: See `examples/` folder in library

---

**Made with care for the Arduino community**
