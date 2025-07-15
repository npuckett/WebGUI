# WebGUI Library for Arduino

[![Arduino](https://img.shields.io/badge/Arduino-Compatible-blue.svg)](https://www.arduino.cc/)
[![ESP32](https://img.shields.io/badge/ESP32-Compatible-green.svg)](https://www.espressif.com/en/products/socs/esp32)
[![License: LGPL v2.1](https://img.shields.io/badge/License-LGPL%20v2.1-blue.svg)](https://www.gnu.org/licenses/lgpl-2.1)

A simple and powerful web-based GUI library for Arduino that enables you to create beautiful control interfaces accessible through any web browser. Perfect for IoT projects, home automation, and remote device control.

## Features

- **Web-Based Interface**: Access your Arduino through any web browser - no app installation required
- **Mobile-Friendly**: Responsive design works on phones, tablets, and computers
- **Easy Controls**: Simple buttons and sliders with real-time updates
- **WiFi Access Point**: Creates its own WiFi network for direct connection
- **Customizable Themes**: Built-in themes and custom CSS support
- **Real-Time Updates**: Instant feedback between web interface and Arduino
- **Cross-Platform**: Works on multiple Arduino boards

## Compatible Hardware

- **Arduino UNO R4 WiFi**
- **Arduino Nano 33 IoT**
- **ESP32** (all variants)

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

// Create controls
Button myButton("Click Me!", 20, 50);
Slider mySlider("Brightness", 20, 100, 0, 255, 128);

void setup() {
  Serial.begin(115200);
  
  // Start WiFi Access Point
  GUI.startAP("My-Arduino", "password123");
  
  // Add controls to GUI
  GUI.addElement(&myButton);
  GUI.addElement(&mySlider);
  
  // Start web server
  GUI.begin();
  
  Serial.println("Connect to WiFi: My-Arduino");
  Serial.println("Visit: http://" + GUI.getIP());
}

void loop() {
  GUI.update();
  
  if (myButton.wasPressed()) {
    Serial.println("Button pressed!");
  }
  
  int brightness = mySlider.getIntValue();
  // Use brightness value...
}
```

## API Reference

### WebGUI Class

#### Setup Methods
```cpp
GUI.startAP(ssid, password);           // Create WiFi access point
GUI.connectWiFi(ssid, password);       // Connect to existing WiFi
GUI.begin();                           // Start web server
GUI.update();                          // Process web requests (call in loop)
```

#### Configuration Methods
```cpp
GUI.setTitle("My Project");            // Set browser tab title
GUI.setHeading("Control Panel");       // Set page heading
GUI.setTheme(WEBGUI_DARK_THEME);      // Apply color theme
GUI.setCustomCSS("button { ... }");   // Add custom styling
GUI.addElement(&myControl);           // Add control to interface
```

### Button Class

#### Constructor
```cpp
Button(label, x, y, width=100, height=40);
```

#### Methods
```cpp
bool wasPressed();                     // Check if button was clicked
bool isPressed();                      // Check current press state
void setButtonStyle("primary");        // Set button style
```

### Slider Class

#### Constructor
```cpp
Slider(label, x, y, minValue, maxValue, defaultValue, width=300);
```

#### Methods
```cpp
int getIntValue();                     // Get current value as integer
float getFloatValue();                 // Get current value as float
void setValue(value);                  // Set slider value
void setRange(min, max);              // Change min/max values
```

## Styling and Themes

### Built-in Themes
```cpp
GUI.setTheme(WEBGUI_DEFAULT_THEME);   // Light theme
GUI.setTheme(WEBGUI_DARK_THEME);      // Dark theme
```

### Custom CSS
```cpp
GUI.setCustomCSS(
  ".webgui-button { "
  "  background: linear-gradient(145deg, #ff6b6b, #ee5a52); "
  "  border-radius: 20px; "
  "}"
);
```

## Layout Guidelines

### Button Positioning
- Height: 30-40px recommended
- Horizontal spacing: 20px minimum between buttons
- Example: `Button("Btn1", 20, 50)`, `Button("Btn2", 140, 50)`

### Slider Positioning  
- Height: ~60px total (includes label and padding)
- Vertical spacing: 70px recommended between sliders
- Example: `Slider("S1", 20, 100, ...)`, `Slider("S2", 20, 170, ...)`

## Network Configuration

### Access Point Mode (Default)
```cpp
GUI.startAP("MyDevice", "mypassword");
```
- Creates its own WiFi network
- No internet router required
- Perfect for portable projects

### Station Mode (Connect to existing WiFi)
```cpp
GUI.connectWiFi("YourWiFi", "wifipassword");
```
- Connects to your home/office WiFi
- Allows internet access alongside device control

## Usage Instructions

1. **Upload your sketch** to the Arduino
2. **Connect to WiFi network** created by Arduino (or your home WiFi if using station mode)
3. **Open web browser** and navigate to the IP address shown in Serial Monitor
4. **Control your project** using the web interface!

## Advanced Examples

### Multiple Controls
```cpp
Button power("Power", 20, 50);
Button reset("Reset", 140, 50);
Slider speed("Speed", 20, 100, 0, 100, 50);
Slider angle("Angle", 20, 170, 0, 180, 90);

void setup() {
  GUI.startAP("Robot-Control", "robot123");
  GUI.setTitle("Robot Controller");
  GUI.setHeading("My Robot Dashboard");
  
  GUI.addElement(&power);
  GUI.addElement(&reset);
  GUI.addElement(&speed);
  GUI.addElement(&angle);
  
  GUI.begin();
}
```

### Custom Styling
```cpp
void setup() {
  GUI.setCustomCSS(
    "body { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); }"
    ".webgui-button { box-shadow: 0 8px 16px rgba(0,0,0,0.3); }"
    ".webgui-slider { accent-color: #ff6b6b; }"
  );
}
```

## Troubleshooting

### Common Issues

**Can't connect to WiFi network**
- Check SSID and password in Serial Monitor output
- Ensure device WiFi is enabled
- Try restarting Arduino if network doesn't appear

**Web page doesn't load**
- Verify IP address in Serial Monitor
- Check that you're connected to Arduino's WiFi network
- Try accessing `192.168.4.1` (default AP IP)

**Controls don't respond**
- Ensure `GUI.update()` is called in `loop()`
- Check Serial Monitor for error messages
- Verify control positioning doesn't overlap

**Compilation errors**
- Install required WiFi libraries (WiFiS3, WiFiNINA)
- Select correct board in Arduino IDE
- Update to latest Arduino IDE version

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
