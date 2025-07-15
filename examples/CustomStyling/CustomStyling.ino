#include <WebGUI.h>

/*
  Custom Styling Example - WebGUI Library
  
  This example demonstrates how to customize the appearance of your
  WebGUI interface using custom CSS and themes.
  
  Features shown:
  - Custom CSS styling
  - Color themes
  - Gradient backgrounds
  - Button styling variations
  
  Created by WebGUI Library Contributors
  This example code is in the public domain.
*/

// Create styled controls
Button redButton("Red Action", 20, 50, 120, 45);
Button blueButton("Blue Action", 160, 50, 120, 45);
Button greenButton("Green Action", 300, 50, 120, 45);

Slider temperatureSlider("Temperature", 20, 120, 0, 100, 25, 400);
Slider humiditySlider("Humidity", 20, 200, 0, 100, 60, 400);

void setup() {
  Serial.begin(115200);
  
  // Start WiFi Access Point
  GUI.startAP("Styled-GUI", "design123");
  
  // Configure page
  GUI.setTitle("Custom Styled Interface");
  GUI.setHeading("🎨 Beautiful Arduino Control Panel");
  
  // Apply dark theme
  GUI.setTheme(WEBGUI_DARK_THEME);
  
  // Add custom CSS for enhanced styling
  GUI.setCustomCSS(
    "body { "
    "  background: linear-gradient(135deg, #1e3c72 0%, #2a5298 100%); "
    "  font-family: 'Segoe UI', Arial, sans-serif; "
    "} "
    
    ".container { "
    "  background: rgba(255, 255, 255, 0.95); "
    "  backdrop-filter: blur(10px); "
    "  border-radius: 20px; "
    "  box-shadow: 0 20px 40px rgba(0,0,0,0.3); "
    "} "
    
    "h1 { "
    "  background: linear-gradient(45deg, #ff6b6b, #4ecdc4); "
    "  -webkit-background-clip: text; "
    "  -webkit-text-fill-color: transparent; "
    "  text-align: center; "
    "  font-size: 32px; "
    "} "
    
    "#element_0 { /* Red Button */ "
    "  background: linear-gradient(145deg, #ff6b6b, #ff5252); "
    "  border: none; "
    "  border-radius: 15px; "
    "  color: white; "
    "  font-weight: bold; "
    "  transition: all 0.3s ease; "
    "  box-shadow: 0 8px 16px rgba(255, 107, 107, 0.4); "
    "} "
    
    "#element_0:hover { "
    "  transform: translateY(-3px); "
    "  box-shadow: 0 12px 20px rgba(255, 107, 107, 0.6); "
    "} "
    
    "#element_1 { /* Blue Button */ "
    "  background: linear-gradient(145deg, #4ecdc4, #44a08d); "
    "  border: none; "
    "  border-radius: 15px; "
    "  color: white; "
    "  font-weight: bold; "
    "  transition: all 0.3s ease; "
    "  box-shadow: 0 8px 16px rgba(78, 205, 196, 0.4); "
    "} "
    
    "#element_1:hover { "
    "  transform: translateY(-3px); "
    "  box-shadow: 0 12px 20px rgba(78, 205, 196, 0.6); "
    "} "
    
    "#element_2 { /* Green Button */ "
    "  background: linear-gradient(145deg, #a8e6cf, #88d8a3); "
    "  border: none; "
    "  border-radius: 15px; "
    "  color: #2d5a87; "
    "  font-weight: bold; "
    "  transition: all 0.3s ease; "
    "  box-shadow: 0 8px 16px rgba(168, 230, 207, 0.4); "
    "} "
    
    "#element_2:hover { "
    "  transform: translateY(-3px); "
    "  box-shadow: 0 12px 20px rgba(168, 230, 207, 0.6); "
    "} "
    
    ".webgui-slider-container { "
    "  background: linear-gradient(145deg, #f0f0f0, #ffffff); "
    "  border-radius: 20px; "
    "  border: 2px solid #e0e0e0; "
    "  box-shadow: inset 0 2px 4px rgba(0,0,0,0.1); "
    "} "
    
    ".webgui-slider { "
    "  accent-color: #4ecdc4; "
    "  height: 12px; "
    "  border-radius: 6px; "
    "} "
    
    ".webgui-slider-value { "
    "  background: linear-gradient(145deg, #4ecdc4, #44a08d); "
    "  color: white; "
    "  border-radius: 20px; "
    "  padding: 8px 15px; "
    "  font-weight: bold; "
    "  box-shadow: 0 4px 8px rgba(0,0,0,0.2); "
    "} "
  );
  
  // Add controls to GUI
  GUI.addElement(&redButton);
  GUI.addElement(&blueButton);
  GUI.addElement(&greenButton);
  GUI.addElement(&temperatureSlider);
  GUI.addElement(&humiditySlider);
  
  // Start the web server
  GUI.begin();
  
  Serial.println("=== Custom Styled WebGUI ===");
  Serial.println("WiFi Access Point started");
  Serial.println("SSID: Styled-GUI");
  Serial.println("Password: design123");
  Serial.println("GUI available at: http://" + GUI.getIP());
  Serial.println("=============================");
}

void loop() {
  GUI.update();
  
  // Handle button presses with styled feedback
  if (redButton.wasPressed()) {
    Serial.println("🔴 Red action triggered!");
  }
  
  if (blueButton.wasPressed()) {
    Serial.println("🔵 Blue action triggered!");
  }
  
  if (greenButton.wasPressed()) {
    Serial.println("🟢 Green action triggered!");
  }
  
  // Read slider values
  int temperature = temperatureSlider.getIntValue();
  int humidity = humiditySlider.getIntValue();
  
  // Print values occasionally
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint > 2000) {
    Serial.println("🌡️ Temperature: " + String(temperature) + "°C");
    Serial.println("💧 Humidity: " + String(humidity) + "%");
    lastPrint = millis();
  }
  
  delay(10);
}
