/*
  WebGUIStyles.h - MEMORY OPTIMIZED - Minimal CSS for WebGUI Library
  
  This is a memory-optimized version that uses browser defaults 
  to save ~9KB of RAM that was previously used for fancy CSS.
  
  Copyright (c) 2025 WebGUI Library Contributors
*/

#ifndef WebGUIStyles_h
#define WebGUIStyles_h

#include "Arduino.h"

// MEMORY-OPTIMIZED: Minimal CSS - uses browser defaults to save ~9KB RAM
const char WEBGUI_DEFAULT_CSS[] PROGMEM = R"rawliteral(
body { margin: 20px; font-family: Arial, sans-serif; }
h1 { margin-bottom: 20px; }
input[type="range"] { width: 300px; margin: 10px; }
input[type="text"] { width: 300px; padding: 8px; margin: 5px 0; border: 1px solid #ccc; border-radius: 4px; font-size: 14px; }
input[type="text"]:focus { border-color: #007bff; outline: none; box-shadow: 0 0 5px rgba(0,123,255,0.5); }
button { padding: 10px; margin: 5px; border: 1px solid #ccc; background: #f8f9fa; cursor: pointer; }
button:hover { background: #e9ecef; }
.webgui-button-active { background: #007bff; color: white; }
.webgui-button-inactive { background: #f8f9fa; color: #333; }
label { display: block; margin: 10px 0 5px 0; font-weight: bold; }
.webgui-slider-value { color: #007bff; font-weight: normal; }
.webgui-textbox-container { margin: 15px 0; }
.webgui-textbox-label { display: block; margin: 10px 0 5px 0; font-weight: bold; }
.webgui-textbox { width: 100%; padding: 8px; border: 1px solid #ccc; border-radius: 4px; font-size: 14px; }
.webgui-textbox:focus { border-color: #007bff; outline: none; box-shadow: 0 0 5px rgba(0,123,255,0.5); }
.webgui-sensor-container { margin: 15px 0; }
.webgui-sensor-label { display: block; margin: 10px 0 5px 0; font-weight: bold; }
.webgui-sensor-value { color: #007bff; font-weight: bold; font-size: 1.1em; }
.webgui-toggle-container { margin: 15px 0; }
.webgui-toggle-switch { position: relative; display: inline-block; width: 60px; height: 34px; }
.webgui-toggle-input { opacity: 0; width: 0; height: 0; }
.webgui-toggle-slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background: #ccc; transition: 0.4s; border-radius: 34px; }
.webgui-toggle-slider:before { position: absolute; content: ""; height: 26px; width: 26px; left: 4px; bottom: 4px; background: white; transition: 0.4s; border-radius: 50%; }
.webgui-toggle-input:checked + .webgui-toggle-slider { background: #2196F3; }
.webgui-toggle-input:checked + .webgui-toggle-slider:before { transform: translateX(26px); }
)rawliteral";

// Theme structures (kept for compatibility but not used)
struct WebGUITheme {
    const char* primaryColor;
    const char* secondaryColor;
    const char* backgroundColor;
    const char* textColor;
    const char* fontFamily;
    
    WebGUITheme(const char* primary = "#007bff", 
                const char* secondary = "#6c757d", 
                const char* background = "#ffffff", 
                const char* text = "#333333", 
                const char* font = "Arial, sans-serif") 
        : primaryColor(primary), secondaryColor(secondary), 
          backgroundColor(background), textColor(text), fontFamily(font) {}
};

const WebGUITheme WEBGUI_DEFAULT_THEME;

// Simplified style manager class
class WebGUIStyleManager {
  public:
    static String getDefaultCSS() {
        return String(WEBGUI_DEFAULT_CSS);
    }
    
    static String getThemedCSS(const WebGUITheme& theme) {
        // Memory optimized: just return default CSS regardless of theme
        return getDefaultCSS();
    }
    
    static String generateCustomCSS(const char* customCSS) {
        // Memory optimized: combine minimal default with custom
        return getDefaultCSS() + String(customCSS);
    }
    
  private:
    // No complex theme replacement needed
};

#endif