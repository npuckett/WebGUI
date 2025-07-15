/*
  WebGUIStyles.h - Shared CSS Style System for WebGUI Library
  
  This file contains the default CSS styles used by all WebGUI elements.
  Users can modify these styles to customize the appearance of their web interface.
  
  Copyright (c) 2025 WebGUI Library Contributors
*/

#ifndef WebGUIStyles_h
#define WebGUIStyles_h

#include "Arduino.h"

// Default CSS styles stored in PROGMEM to save RAM
const char WEBGUI_DEFAULT_CSS[] PROGMEM = R"rawliteral(
/* Base styles */
body {
    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
    margin: 0;
    padding: 20px;
    background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
    min-height: 100vh;
    color: #333;
}

.container {
    position: relative;
    background-color: white;
    border-radius: 12px;
    padding: 30px;
    box-shadow: 0 8px 32px rgba(0,0,0,0.1);
    min-height: 500px;
    max-width: 800px;
    margin: 0 auto;
}

h1 {
    color: #2c3e50;
    margin: 0 0 30px 0;
    font-size: 28px;
    font-weight: 300;
    text-align: center;
    border-bottom: 2px solid #ecf0f1;
    padding-bottom: 15px;
}

/* Button Styles */
.webgui-button {
    background: linear-gradient(145deg, #007bff, #0056b3);
    color: white;
    border: none;
    border-radius: 8px;
    cursor: pointer;
    font-size: 14px;
    font-weight: 600;
    transition: all 0.3s ease;
    box-shadow: 0 4px 12px rgba(0,123,255,0.3);
    text-transform: uppercase;
    letter-spacing: 0.5px;
}

.webgui-button:hover {
    background: linear-gradient(145deg, #0056b3, #004085);
    transform: translateY(-2px);
    box-shadow: 0 6px 20px rgba(0,123,255,0.4);
}

.webgui-button:active {
    transform: translateY(0);
    box-shadow: 0 2px 8px rgba(0,123,255,0.3);
}

/* Button Variants */
.webgui-button.primary {
    background: linear-gradient(145deg, #007bff, #0056b3);
}

.webgui-button.secondary {
    background: linear-gradient(145deg, #6c757d, #545b62);
}

.webgui-button.success {
    background: linear-gradient(145deg, #28a745, #1e7e34);
}

.webgui-button.danger {
    background: linear-gradient(145deg, #dc3545, #bd2130);
}

.webgui-button.warning {
    background: linear-gradient(145deg, #ffc107, #d39e00);
    color: #212529;
}

/* Slider Container Styles */
.webgui-slider-container {
    background: #f8f9fa;
    border-radius: 10px;
    padding: 15px;
    margin-bottom: 10px;
    border: 1px solid #e9ecef;
    transition: all 0.2s ease;
}

.webgui-slider-container:hover {
    background: #e9ecef;
    border-color: #007bff;
}

.webgui-slider-container label {
    display: block;
    margin-bottom: 8px;
    font-weight: 600;
    color: #495057;
    font-size: 14px;
}

.webgui-slider-value {
    float: right;
    background: #007bff;
    color: white;
    padding: 4px 10px;
    border-radius: 15px;
    font-size: 13px;
    font-weight: bold;
    min-width: 35px;
    text-align: center;
    margin-left: 10px;
}

/* Slider Input Styles */
.webgui-slider {
    width: 100%;
    height: 8px;
    border-radius: 4px;
    background: #dee2e6;
    outline: none;
    -webkit-appearance: none;
    appearance: none;
    transition: all 0.2s ease;
}

.webgui-slider::-webkit-slider-thumb {
    -webkit-appearance: none;
    appearance: none;
    width: 20px;
    height: 20px;
    border-radius: 50%;
    background: #007bff;
    cursor: pointer;
    border: 3px solid white;
    box-shadow: 0 2px 6px rgba(0,0,0,0.2);
    transition: all 0.2s ease;
}

.webgui-slider::-webkit-slider-thumb:hover {
    background: #0056b3;
    transform: scale(1.1);
    box-shadow: 0 3px 10px rgba(0,0,0,0.3);
}

.webgui-slider::-moz-range-thumb {
    width: 20px;
    height: 20px;
    border-radius: 50%;
    background: #007bff;
    cursor: pointer;
    border: 3px solid white;
    box-shadow: 0 2px 6px rgba(0,0,0,0.2);
}

/* Responsive Design */
@media (max-width: 600px) {
    body {
        padding: 10px;
    }
    
    .container {
        padding: 20px;
        border-radius: 8px;
    }
    
    h1 {
        font-size: 24px;
    }
    
    .webgui-button {
        font-size: 12px;
        min-height: 40px;
    }
    
    .webgui-slider-container {
        padding: 12px;
    }
}

/* Animation Classes */
.webgui-fadeIn {
    animation: fadeIn 0.5s ease-in;
}

@keyframes fadeIn {
    from { opacity: 0; transform: translateY(10px); }
    to { opacity: 1; transform: translateY(0); }
}

/* Grid Layout Helper */
.webgui-grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(250px, 1fr));
    gap: 20px;
    margin-top: 20px;
}

.webgui-card {
    background: white;
    border-radius: 8px;
    padding: 20px;
    box-shadow: 0 2px 8px rgba(0,0,0,0.1);
    border: 1px solid #e9ecef;
}
)rawliteral";

// Color themes that users can easily modify
struct WebGUITheme {
    const char* primary;
    const char* secondary;
    const char* success;
    const char* danger;
    const char* warning;
    const char* background;
    const char* surface;
    const char* text;
};

// Default theme
const WebGUITheme WEBGUI_DEFAULT_THEME = {
    "#007bff",  // primary
    "#6c757d",  // secondary  
    "#28a745",  // success
    "#dc3545",  // danger
    "#ffc107",  // warning
    "#f8f9fa",  // background
    "#ffffff",  // surface
    "#212529"   // text
};

// Dark theme option
const WebGUITheme WEBGUI_DARK_THEME = {
    "#0d6efd",  // primary
    "#6c757d",  // secondary
    "#198754",  // success
    "#dc3545",  // danger
    "#ffc107",  // warning
    "#212529",  // background
    "#343a40",  // surface
    "#ffffff"   // text
};

class WebGUIStyleManager {
  public:
    static String getDefaultCSS();
    static String getThemedCSS(const WebGUITheme& theme);
    static String generateCustomCSS(const char* customCSS);
    
  private:
    static String replaceThemeVariables(String css, const WebGUITheme& theme);
};

#endif