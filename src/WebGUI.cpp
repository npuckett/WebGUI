/*
  WebGUI.cpp - Simple Web GUI Library for Arduino
  
  Copyright (c) 2025 WebGUI Library Contributors
*/

#include "WebGUI.h"

// Static member initialization
int GUIElement::nextID = 0;

// Global instance
WebGUI GUI;

// HTML Templates stored in PROGMEM
const char HTML_TEMPLATE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>%TITLE%</title>
    <style>
        %CSS%
    </style>
</head>
<body>
    <div class="container">
        <h1>%HEADING%</h1>
        %ELEMENTS%
    </div>
    <script>
        %JAVASCRIPT%
    </script>
</body>
</html>
)rawliteral";

const char BUTTON_TEMPLATE[] PROGMEM = R"rawliteral(
        <button id="%ID%" class="webgui-button webgui-button-primary" onclick="buttonClick('%ID%')">%LABEL%</button>
)rawliteral";

const char SLIDER_TEMPLATE[] PROGMEM = R"rawliteral(
        <div class="webgui-slider-container">
            <label for="%ID%">%LABEL% <span class="webgui-slider-value" id="%ID%_value">%VALUE%</span></label>
            <input type="range" id="%ID%" class="webgui-slider" min="%MIN%" max="%MAX%" value="%VALUE%">
        </div>
)rawliteral";

const char SENSOR_STATUS_TEMPLATE[] PROGMEM = R"rawliteral(
        <div class="webgui-sensor-container">
            <label class="webgui-sensor-label">%LABEL%</label>
            <span class="webgui-sensor-value" id="%ID%_display">%VALUE%</span>
        </div>
)rawliteral";

// WebGUI Implementation
WebGUI::WebGUI(int port) : serverPort(port), apMode(false), useCustomStyles(false), 
                           pageTitle("Arduino WebGUI"), pageHeading("Control Panel") {
#if defined(ESP32)
    server = new WebServer(port);
#else
    server = new WiFiServer(port);
#endif
}

WebGUI::~WebGUI() {
    if (server) {
#if defined(ESP32)
        server->stop();
#endif
        // Note: WiFiServer on Arduino boards doesn't have stop() method
        delete server;
    }
}

void WebGUI::begin() {
#if defined(ESP32)
    setupRoutes();
#endif
    server->begin();
    Serial.println("WebGUI server started on port " + String(serverPort));
}

void WebGUI::update() {
#if defined(ESP32)
    server->handleClient();
#else
    processClient();
#endif
}

void WebGUI::addElement(GUIElement* element) {
    elements.push_back(element);
}

void WebGUI::startAP(const char* ssid, const char* password) {
    apMode = true;
#if defined(ARDUINO_UNOWIFIR4)
    WiFi.beginAP(ssid, password);
    Serial.println("Access Point started");
    Serial.println("SSID: " + String(ssid));
    Serial.println("IP: " + WiFi.localIP().toString());
#elif defined(ARDUINO_SAMD_NANO_33_IOT)
    WiFi.beginAP(ssid, password);
    Serial.println("Access Point started");
    Serial.println("SSID: " + String(ssid));
    Serial.println("IP: " + WiFi.localIP().toString());
#elif defined(ESP32)
    WiFi.softAP(ssid, password);
    Serial.println("Access Point started");
    Serial.println("SSID: " + String(ssid));
    Serial.println("IP: " + WiFi.softAPIP().toString());
#endif
}

void WebGUI::connectWiFi(const char* ssid, const char* password) {
    apMode = false;
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nWiFi connected");
    Serial.println("IP: " + WiFi.localIP().toString());
}

void WebGUI::setTitle(const char* title) {
    pageTitle = String(title);
}

void WebGUI::setHeading(const char* heading) {
    pageHeading = String(heading);
}

void WebGUI::setCustomCSS(const char* customCSS) {
    this->customCSS = String(customCSS);
    useCustomStyles = true;
}

void WebGUI::setTheme(const WebGUITheme& theme) {
    customCSS = WebGUIStyleManager::getThemedCSS(theme);
    useCustomStyles = true;
}

void WebGUI::useDefaultStyles() {
    useCustomStyles = false;
    customCSS = "";
}

String WebGUI::getIP() {
#if defined(ESP32)
    return apMode ? WiFi.softAPIP().toString() : WiFi.localIP().toString();
#else
    return WiFi.localIP().toString();
#endif
}

void WebGUI::setupRoutes() {
#if defined(ESP32)
    server->on("/", [this]() { handleRoot(); });
    server->on("/set", [this]() { handleSet(); });
    server->on("/get", [this]() { handleGet(); });
#endif
    // For Arduino boards, routes are handled in processClient()
}

#if !defined(ESP32)
void WebGUI::processClient() {
    WiFiClient client = server->available();
    if (client) {
        String currentLine = "";
        httpRequest = "";
        
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                httpRequest += c;
                
                if (c == '\n') {
                    if (currentLine.length() == 0) {
                        // End of HTTP request, process it
                        if (httpRequest.indexOf("GET /set?") >= 0) {
                            handleSetRequest(httpRequest);
                            client.println("HTTP/1.1 200 OK");
                            client.println("Content-Type: text/plain");
                            client.println("Connection: close");
                            client.println();
                            client.println("OK");
                        } else if (httpRequest.indexOf("GET /get") >= 0) {
                            String response = generateGetResponse();
                            client.println("HTTP/1.1 200 OK");
                            client.println("Content-Type: application/json");
                            client.println("Connection: close");
                            client.println();
                            client.println(response);
                        } else {
                            // Default route - send main page
                            String html = generateHTML();
                            client.println("HTTP/1.1 200 OK");
                            client.println("Content-Type: text/html");
                            client.println("Connection: close");
                            client.println();
                            client.println(html);
                        }
                        break;
                    } else {
                        currentLine = "";
                    }
                } else if (c != '\r') {
                    currentLine += c;
                }
            }
        }
        client.stop();
    }
}

void WebGUI::handleSetRequest(String request) {
    // Parse parameters from GET request
    int paramStart = request.indexOf("?") + 1;
    int paramEnd = request.indexOf(" ", paramStart);
    String params = request.substring(paramStart, paramEnd);
    
    // Split multiple parameters
    int start = 0;
    int end = params.indexOf("&");
    
    while (start < params.length()) {
        String param;
        if (end == -1) {
            param = params.substring(start);
            start = params.length();
        } else {
            param = params.substring(start, end);
            start = end + 1;
            end = params.indexOf("&", start);
        }
        
        // Parse parameter name and value
        int eqPos = param.indexOf("=");
        if (eqPos > 0) {
            String paramName = param.substring(0, eqPos);
            String paramValue = param.substring(eqPos + 1);
            
            // Find matching element
            for (GUIElement* element : elements) {
                if (element->getID() == paramName) {
                    element->handleUpdate(paramValue);
                    break;
                }
            }
        }
    }
}

String WebGUI::generateGetResponse() {
    String response = "{";
    for (size_t i = 0; i < elements.size(); i++) {
        if (i > 0) response += ",";
        response += "\"" + elements[i]->getID() + "\":\"" + elements[i]->getValue() + "\"";
    }
    response += "}";
    return response;
}
#endif

void WebGUI::handleRoot() {
#if defined(ESP32)
    String html = generateHTML();
    server->send(200, "text/html", html);
#endif
}

void WebGUI::handleSet() {
#if defined(ESP32)
    // Process all arguments
    for (int i = 0; i < server->args(); i++) {
        String paramName = server->argName(i);
        String paramValue = server->arg(i);
        
        // Find the element with matching ID
        for (GUIElement* element : elements) {
            if (element->getID() == paramName) {
                element->handleUpdate(paramValue);
                break;
            }
        }
    }
    
    server->send(200, "text/plain", "OK");
#endif
}

void WebGUI::handleGet() {
#if defined(ESP32)
    String response = "{";
    for (size_t i = 0; i < elements.size(); i++) {
        if (i > 0) response += ",";
        response += "\"" + elements[i]->getID() + "\":\"" + elements[i]->getValue() + "\"";
    }
    response += "}";
    
    server->send(200, "application/json", response);
#endif
}

String WebGUI::generateHTML() {
    String html = String(HTML_TEMPLATE);
    
    // Set title and heading
    html.replace("%TITLE%", pageTitle);
    html.replace("%HEADING%", pageHeading);
    
    // Generate CSS
    String css = generateCSS();
    html.replace("%CSS%", css);
    
    // Generate HTML elements
    String elementsHTML = "";
    for (GUIElement* element : elements) {
        elementsHTML += element->generateHTML();
    }
    html.replace("%ELEMENTS%", elementsHTML);
    
    // Generate JavaScript
    String js = generateJS();
    html.replace("%JAVASCRIPT%", js);
    
    return html;
}

String WebGUI::generateCSS() {
    if (useCustomStyles) {
        return customCSS;
    } else {
        return WebGUIStyleManager::getDefaultCSS();
    }
}

String WebGUI::generateJS() {
    String js = R"rawliteral(
        function buttonClick(id) {
            fetch('/set?' + id + '=1');
        }
        
        // Original immediate slider function (for backward compatibility)
        function sliderChange(id, value) {
            document.getElementById(id + '_value').textContent = value;
            fetch('/set?' + id + '=' + value);
        }
        
        // New debounced slider function
        function debouncedSliderChange(id, value, debounceMs) {
            // Update display immediately for responsiveness
            document.getElementById(id + '_value').textContent = value;
            
            // Clear existing timeout for this slider
            if (window['timeout_' + id]) {
                clearTimeout(window['timeout_' + id]);
            }
            
            // Set new timeout for network request
            window['timeout_' + id] = setTimeout(() => {
                fetch('/set?' + id + '=' + value);
            }, debounceMs);
        }
    )rawliteral";
    
    for (GUIElement* element : elements) {
        js += element->generateJS();
    }
    
    return js;
}

// GUIElement Implementation
GUIElement::GUIElement(String label, int x, int y, int width, int height) 
    : label(label), x(x), y(y), width(width), height(height) {
    id = "element" + String(nextID++);
}

GUIElement::~GUIElement() {
}

String GUIElement::generateCSS() {
    return generateBaseCSS();
}

String GUIElement::generateBaseCSS() {
    return "#" + id + " {\n" +
           "    position: absolute;\n" +
           "    left: " + String(x) + "px;\n" +
           "    top: " + String(y) + "px;\n" +
           "    width: " + String(width) + "px;\n" +
           "    height: " + String(height) + "px;\n" +
           "}\n";
}

void GUIElement::setPosition(int newX, int newY) {
    x = newX;
    y = newY;
}

void GUIElement::setSize(int newWidth, int newHeight) {
    width = newWidth;
    height = newHeight;
}

// Button Implementation
Button::Button(String label, int x, int y, int width, int height) 
    : GUIElement(label, x, y, width, height), pressed(false), pressedFlag(false), lastPressTime(0), buttonStyle("primary") {
}

String Button::generateHTML() {
    String html = String(BUTTON_TEMPLATE);
    html.replace("%ID%", id);
    html.replace("%LABEL%", label);
    return html;
}

String Button::generateCSS() {
    return String("#") + id + " { position: absolute; left: " + String(x) + 
           "px; top: " + String(y) + "px; width: " + String(width) + 
           "px; height: " + String(height) + "px; }\n";
}

String Button::generateJS() {
    return "";
}

void Button::handleUpdate(String value) {
    if (value == "1") {
        pressed = true;
        pressedFlag = true;
        lastPressTime = millis();
    }
}

String Button::getValue() {
    return pressed ? "1" : "0";
}

bool Button::wasPressed() {
    if (pressedFlag) {
        pressedFlag = false;
        return true;
    }
    return false;
}

bool Button::isPressed() {
    return pressed;
}

void Button::resetPress() {
    pressed = false;
    pressedFlag = false;
}

// Slider Implementation
Slider::Slider(String label, int x, int y, int minValue, int maxValue, int defaultValue, int width) 
    : GUIElement(label, x, y, width, 60), minValue(minValue), maxValue(maxValue), currentValue(defaultValue), valueChanged(false) {
}

String Slider::generateHTML() {
    String html = String(SLIDER_TEMPLATE);
    html.replace("%ID%", id);
    html.replace("%LABEL%", label);
    html.replace("%MIN%", String(minValue));
    html.replace("%MAX%", String(maxValue));
    html.replace("%VALUE%", String(currentValue));
    return html;
}

String Slider::generateCSS() {
    return String("#") + id + "_container { position: absolute; left: " + String(x) + 
           "px; top: " + String(y) + "px; width: " + String(width) + "px; }\n";
}

String Slider::generateJS() {
    // Generate JavaScript that configures this specific slider to use debouncing
    return "document.getElementById('" + id + "').oninput = function() { debouncedSliderChange('" + 
           id + "', this.value, " + String(debounceMs) + "); };\n";
}

void Slider::handleUpdate(String value) {
    int newValue = value.toInt();
    if (newValue != currentValue) {
        currentValue = constrain(newValue, minValue, maxValue);
        valueChanged = true;
    }
}

String Slider::getValue() {
    return String(currentValue);
}

int Slider::getIntValue() {
    return currentValue;
}

float Slider::getFloatValue() {
    return (float)currentValue;
}

void Slider::setValue(int value) {
    currentValue = constrain(value, minValue, maxValue);
}

void Slider::setRange(int min, int max) {
    minValue = min;
    maxValue = max;
    currentValue = constrain(currentValue, minValue, maxValue);
}

// SensorStatus Implementation
SensorStatus::SensorStatus(String label, int x, int y, int width) 
    : GUIElement(label, x, y, width, 40), displayValue("--") {
}

String SensorStatus::generateHTML() {
    String html = String(SENSOR_STATUS_TEMPLATE);
    html.replace("%ID%", id);
    html.replace("%LABEL%", label);
    html.replace("%VALUE%", displayValue);
    return html;
}

String SensorStatus::generateCSS() {
    return String("#") + id + "_container { position: absolute; left: " + String(x) + 
           "px; top: " + String(y) + "px; width: " + String(width) + "px; }\n";
}

String SensorStatus::generateJS() {
    return "";
}

void SensorStatus::handleUpdate(String value) {
    // Read-only element - updates come from setValue() calls in code
}

String SensorStatus::getValue() {
    return displayValue;
}

void SensorStatus::setValue(int value) {
    displayValue = String(value);
}

void SensorStatus::setValue(float value, int decimals) {
    displayValue = String(value, decimals);
}

void SensorStatus::setValue(bool value) {
    displayValue = value ? "TRUE" : "FALSE";
}

void SensorStatus::setValue(String value) {
    displayValue = value;
}

void SensorStatus::setValue(const char* value) {
    displayValue = String(value);
}

// WebGUIStyleManager Implementation
String WebGUIStyleManager::getDefaultCSS() {
    return String(
        "body { font-family: Arial, sans-serif; margin: 20px; background: #f0f0f0; }\n"
        ".container { background: white; padding: 30px; border-radius: 10px; max-width: 800px; margin: 0 auto; }\n"
        "h1 { color: #333; text-align: center; border-bottom: 2px solid #ddd; padding-bottom: 15px; }\n"
        ".webgui-button { background: #007bff; color: white; border: none; padding: 10px 20px; border-radius: 5px; cursor: pointer; margin: 5px; }\n"
        ".webgui-button:hover { background: #0056b3; }\n"
        ".webgui-slider-container { background: #f8f9fa; border-radius: 10px; padding: 15px; margin: 10px 0; border: 1px solid #ddd; }\n"
        ".webgui-slider-container label { display: block; margin-bottom: 8px; font-weight: bold; }\n"
        ".webgui-slider-value { float: right; background: #007bff; color: white; padding: 4px 10px; border-radius: 15px; font-size: 13px; }\n"
        ".webgui-slider { width: 100%; height: 8px; border-radius: 4px; background: #ddd; outline: none; }\n"
        ".webgui-sensor-container { background: #f8f9fa; border-radius: 8px; padding: 12px; margin: 8px 0; border-left: 4px solid #6c757d; display: flex; align-items: center; }\n"
        ".webgui-sensor-label { font-weight: bold; color: #495057; margin-right: auto; }\n"
        ".webgui-sensor-value { font-family: monospace; font-size: 16px; font-weight: bold; color: #495057; }\n"
    );
}

String WebGUIStyleManager::getThemedCSS(const WebGUITheme& theme) {
    String css = getDefaultCSS();
    return replaceThemeVariables(css, theme);
}

String WebGUIStyleManager::generateCustomCSS(const char* customCSS) {
    return String(customCSS);
}

String WebGUIStyleManager::replaceThemeVariables(String css, const WebGUITheme& theme) {
    // Replace theme variables in CSS
    css.replace("%PRIMARY%", theme.primary);
    css.replace("%SECONDARY%", theme.secondary);
    css.replace("%SUCCESS%", theme.success);
    css.replace("%DANGER%", theme.danger);
    css.replace("%WARNING%", theme.warning);
    css.replace("%BACKGROUND%", theme.background);
    css.replace("%SURFACE%", theme.surface);
    css.replace("%TEXT%", theme.text);
    return css;
}