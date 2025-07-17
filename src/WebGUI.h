/*
  WebGUI.h - Simple Web GUI Library for Arduino
  
  Copyright (c) 2025 WebGUI Library Contributors
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
*/

#ifndef WebGUI_h
#define WebGUI_h

#include "Arduino.h"
#include <vector>
#include "WebGUIStyles.h"

// Platform-specific includes
#if defined(ARDUINO_UNOWIFIR4)
  #include <WiFiS3.h>
  #define WEBGUI_WIFI_TYPE WiFiServer
#elif defined(ARDUINO_SAMD_NANO_33_IOT)
  #include <WiFiNINA.h>
  #define WEBGUI_WIFI_TYPE WiFiServer
#elif defined(ESP32)
  #include <WiFi.h>
  #include <WebServer.h>
  #define WEBGUI_WIFI_TYPE WebServer
#else
  #error "Unsupported board! This library supports Arduino UNO R4 WiFi, Arduino Nano 33 IoT, and ESP32"
#endif

// Forward declarations
class GUIElement;
class Button;
class Toggle;
class Slider;
class SensorStatus;
class SystemStatus;

class WebGUI {
  public:
    WebGUI(int port = 80);
    ~WebGUI();
    
    void begin();
    void update();
    void addElement(GUIElement* element);
    void handleRequest();
    
    // Access point configuration
    void startAP(const char* ssid, const char* password = "");
    void connectWiFi(const char* ssid, const char* password);
    
    // Style management
    void setCustomCSS(const char* customCSS);
    void setTheme(const WebGUITheme& theme);
    void useDefaultStyles();
    
    // Page configuration
    void setTitle(const char* title);
    
    String getIP();
    
  private:
    WEBGUI_WIFI_TYPE* server;
    std::vector<GUIElement*> elements;
    int serverPort;
    bool apMode;
    String httpRequest;
    String customCSS;
    bool useCustomStyles;
    String pageTitle;
    String pageHeading;
    
    void setupRoutes();
    void handleRoot();
    void handleSet();
    void handleGet();
    
#if !defined(ESP32)
    void processClient();
    void handleSetRequest(String request);
    String generateGetResponse();
#endif
    
    String generateHTML();
    void streamHTML(WiFiClient& client);  // MEMORY OPTIMIZED: Stream instead of build large strings
    String generateCSS();
    String generateJS();
};

class GUIElement {
  public:
    GUIElement(String label, int x, int y, int width = 200, int height = 30);
    virtual ~GUIElement();
    
    virtual String generateHTML() = 0;
    virtual String generateCSS();
    virtual String generateJS() = 0;
    virtual void handleUpdate(String value) = 0;
    virtual String getValue() = 0;
    
    String getID() { return id; }
    String getLabel() { return label; }
    int getX() { return x; }
    int getY() { return y; }
    int getWidth() { return width; }
    int getHeight() { return height; }
    
    void setPosition(int newX, int newY);
    void setSize(int newWidth, int newHeight);
    
  protected:
    String id;
    String label;
    int x, y, width, height;
    static int nextID;
    
    String generateBaseCSS();
};

class Button : public GUIElement {
  public:
    Button(String label, int x, int y, int width = 100, int height = 40);
    
    String generateHTML() override;
    String generateCSS() override;
    String generateJS() override;
    void handleUpdate(String value) override;
    String getValue() override;
    
    bool wasPressed();
    bool isPressed();
    void setState(bool state);  // Set toggle state for visual feedback
    
    // Style options
    void setButtonStyle(String style = "primary"); // primary, secondary, success, danger, warning
    
  private:
    bool pressed;
    bool pressedFlag;
    unsigned long lastPressTime;
    String buttonStyle;
    
    void resetPress();
};

class Toggle : public GUIElement {
  public:
    Toggle(String label, int x, int y, int width = 200);
    
    String generateHTML() override;
    String generateCSS() override;
    String generateJS() override;
    void handleUpdate(String value) override;
    String getValue() override;
    
    bool isOn();
    bool wasToggled();
    void setState(bool state);
    
    // Calculate proper height for positioning
    static int getRequiredHeight() { return 40; }
    
  private:
    bool state;
    bool stateChanged;
    
    void resetToggle();
};

class Slider : public GUIElement {
  public:
    Slider(String label, int x, int y, int minValue, int maxValue, int defaultValue, int width = 300);
    
    String generateHTML() override;
    String generateCSS() override;
    String generateJS() override;
    void handleUpdate(String value) override;
    String getValue() override;
    
    int getIntValue();
    float getFloatValue();
    int getMinValue() { return minValue; }
    int getMaxValue() { return maxValue; }
    
    void setValue(int value);
    void setRange(int min, int max);
    
    // Debouncing control
    void setDebounceTime(int ms) { debounceMs = ms; }
    int getDebounceTime() { return debounceMs; }
    
    // Calculate proper height for positioning
    static int getRequiredHeight() { return 60; }
    
  private:
    int minValue, maxValue, currentValue;
    bool valueChanged;
    int debounceMs = 100;  // Default 100ms debounce
};

class SensorStatus : public GUIElement {
  public:
    SensorStatus(String label, int x, int y, int width = 200);
    
    String generateHTML() override;
    String generateCSS() override;
    String generateJS() override;
    void handleUpdate(String value) override; // Not used - read-only
    String getValue() override;
    
    // Set values for different data types
    void setValue(int value);
    void setValue(float value, int decimals = 2);
    void setValue(bool value);
    void setValue(String value);
    void setValue(const char* value);
    
    // Get current display value
    String getDisplayValue() { return displayValue; }
    
    // Calculate proper height for positioning
    static int getRequiredHeight() { return 40; }
    
  private:
    String displayValue;
};

class SystemStatus : public GUIElement {
  public:
    SystemStatus(String label, int x, int y, int width = 350);
    
    String generateHTML() override;
    String generateCSS() override;
    String generateJS() override;
    void handleUpdate(String value) override; // Not used - read-only
    String getValue() override;
    
    // Update system information
    void updateMemory(int freeBytes);
    void updateUptime(unsigned long uptimeSeconds);
    void updateSystemInfo(int freeBytes, unsigned long uptimeSeconds);
    
    // Calculate proper height for positioning
    static int getRequiredHeight() { return 80; }
    
  private:
    String systemInfo;
    int freeMemory;
    unsigned long uptime;
    
    String formatUptime(unsigned long seconds);
    String formatMemory(int bytes);
};

// Global instance - can be used directly or create your own
extern WebGUI GUI;

#endif
