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
        <button id="%ID%" class="webgui-button" onclick="buttonClick('%ID%')">%LABEL%</button>
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

const char TOGGLE_TEMPLATE[] PROGMEM = R"rawliteral(
        <div class="webgui-toggle-container">
            <label class="webgui-toggle-label">%LABEL%</label>
            <label class="webgui-toggle-switch">
                <input type="checkbox" id="%ID%" class="webgui-toggle-input" onchange="toggleChange('%ID%', this.checked)">
                <span class="webgui-toggle-slider"></span>
            </label>
        </div>
)rawliteral";

const char SYSTEM_STATUS_TEMPLATE[] PROGMEM = R"rawliteral(
        <div class="webgui-system-container">
            <label class="webgui-system-label">%LABEL%</label>
            <div class="webgui-system-content" id="%ID%_display">%VALUE%</div>
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
                            // MEMORY OPTIMIZED: Stream HTML directly instead of building large strings
                            client.println("HTTP/1.1 200 OK");
                            client.println("Content-Type: text/html");
                            client.println("Connection: close");
                            client.println();
                            streamHTML(client);
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
        // Button state tracking
        var buttonStates = {};
        
        function buttonClick(id) {
            fetch('/set?' + id + '=1');
        }
        
        function toggleChange(id, checked) {
            fetch('/set?' + id + '=' + (checked ? 'true' : 'false'));
        }
        
        // Initialize button states on page load
        function initializeButtonStates() {
            // Set all buttons to inactive state initially
            var buttons = document.querySelectorAll('.webgui-button');
            buttons.forEach(function(button) {
                buttonStates[button.id] = false;
                button.classList.add('webgui-button-inactive');
            });
        }
        
        // Call initialization when page loads
        document.addEventListener('DOMContentLoaded', initializeButtonStates);
        
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

// MEMORY OPTIMIZED: Stream HTML directly instead of building large strings in memory
void WebGUI::streamHTML(WiFiClient& client) {
    // Send HTML template start - broken into small chunks
    client.print("<!DOCTYPE html><html><head><meta charset=\"UTF-8\">");
    client.print("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
    client.print("<title>");
    client.print(pageTitle);
    client.print("</title><style>");
    
    // Stream minimal CSS directly 
    client.print(WEBGUI_DEFAULT_CSS);
    
    client.print("</style></head><body><h1>");
    client.print(pageHeading);
    client.print("</h1>");
    
    // Stream each element's HTML directly
    for (GUIElement* element : elements) {
        String elementHTML = element->generateHTML();
        client.print(elementHTML);
    }
    
    // Stream JavaScript - minimal version
    client.print("<script>");
    client.print("function updateValue(id,val){");
    client.print("fetch('/set?'+id+'='+val).catch(e=>console.log('Error:',e));");
    client.print("}");
    client.print("function buttonClick(id){");
    client.print("fetch('/set?'+id+'=1').catch(e=>console.log('Error:',e));");
    client.print("}");
    client.print("function toggleChange(id,checked){");
    client.print("fetch('/set?'+id+'='+(checked?'true':'false')).catch(e=>console.log('Error:',e));");
    client.print("}");
    client.print("function toggleButton(id){");
    client.print("const btn=document.getElementById(id);");
    client.print("const newState=btn.textContent==='ON'?'OFF':'ON';");
    client.print("btn.textContent=newState;");
    client.print("updateValue(id,newState==='ON'?'1':'0');");
    client.print("}");
    
    // Auto-update function for SensorStatus displays
    client.print("function updateSensorDisplays(){");
    client.print("fetch('/get').then(response=>response.json()).then(data=>{");
    client.print("let debugArea=document.getElementById('debug_area');");
    client.print("if(!debugArea){debugArea=document.createElement('div');debugArea.id='debug_area';");
    client.print("debugArea.style.cssText='position:fixed;top:10px;right:10px;background:rgba(0,0,0,0.8);color:white;padding:10px;font-size:12px;max-width:300px;';");
    client.print("document.body.appendChild(debugArea);}");
    client.print("debugArea.innerHTML='Last update: '+new Date().toLocaleTimeString()+'<br>Data: '+JSON.stringify(data);");
    client.print("for(let elementId in data){");
    client.print("let displayElement=document.getElementById(elementId+'_display');");
    client.print("if(displayElement){displayElement.textContent=data[elementId];");
    client.print("debugArea.innerHTML+='<br>Updated: '+elementId+'_display = '+data[elementId];}");
    client.print("else{debugArea.innerHTML+='<br>NOT FOUND: '+elementId+'_display';}");
    client.print("let toggleElement=document.getElementById(elementId);");
    client.print("if(toggleElement&&toggleElement.type==='checkbox'){");
    client.print("let shouldBeChecked=(data[elementId]==='true'||data[elementId]==='1');");
    client.print("if(toggleElement.checked!==shouldBeChecked){toggleElement.checked=shouldBeChecked;}}}");
    client.print("}).catch(error=>{");
    client.print("let debugArea=document.getElementById('debug_area');");
    client.print("if(!debugArea){debugArea=document.createElement('div');debugArea.id='debug_area';");
    client.print("debugArea.style.cssText='position:fixed;top:10px;right:10px;background:rgba(255,0,0,0.8);color:white;padding:10px;font-size:12px;max-width:300px;';");
    client.print("document.body.appendChild(debugArea);}");
    client.print("debugArea.innerHTML='ERROR: '+error.toString();});}");
    
    // Start auto-updating sensor displays every 500ms
    client.print("setInterval(updateSensorDisplays,500);");
    client.print("updateSensorDisplays();");
    
    // Stream each element's JavaScript for event handlers
    for (GUIElement* element : elements) {
        String elementJS = element->generateJS();
        client.print(elementJS);
    }
    
    client.print("</script></body></html>");
}

// =====================================================
// GUIElement Base Class Implementation  
// =====================================================

GUIElement::GUIElement(String label, int x, int y, int width, int height) 
    : label(label), x(x), y(y), width(width), height(height) {
    id = "element" + String(nextID++);
}

GUIElement::~GUIElement() {
    // Base destructor
}

String GUIElement::generateCSS() {
    // Base implementation - memory optimized: return empty string
    return "";
}

String GUIElement::generateJS() {
    // Base implementation - memory optimized: return empty string
    return "";
}

void GUIElement::handleUpdate(String value) {
    // Base implementation - does nothing by default
}

String GUIElement::getValue() {
    // Base implementation - returns empty string
    return "";
}

// =====================================================
// Slider Implementation
// =====================================================

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

String Slider::generateCSS() {
    // Memory optimized: return empty string since we're using minimal CSS
    return "";
}

String Slider::generateJS() {
    // Memory optimized: return minimal JavaScript for slider updates with value display
    return "document.getElementById('" + id + "').oninput = function() { "
           "document.getElementById('" + id + "_value').textContent = this.value; "
           "updateValue('" + id + "', this.value); };\n";
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
    // Memory optimized: return empty string since we're using minimal CSS
    return "";
}

String Button::generateJS() {
    // JavaScript for button functionality is in the main JS template (buttonClick function)
    return "";
}

void Button::handleUpdate(String value) {
    if (value == "1") {
        pressed = !pressed;  // Toggle state on each click
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

void Button::setState(bool state) {
    pressed = state;
}

void Button::setButtonStyle(String style) {
    buttonStyle = style;
}

// Toggle Implementation
Toggle::Toggle(String label, int x, int y, int width) 
    : GUIElement(label, x, y, width, 40), state(false), stateChanged(false) {
}

String Toggle::generateHTML() {
    String html = String(TOGGLE_TEMPLATE);
    html.replace("%ID%", id);
    html.replace("%LABEL%", label);
    
    // Set initial checkbox state based on current toggle state
    if (state) {
        html.replace("type=\"checkbox\"", "type=\"checkbox\" checked");
    }
    
    return html;
}

String Toggle::generateCSS() {
    // Memory optimized: return empty string since we're using minimal CSS
    return "";
}

String Toggle::generateJS() {
    // JavaScript for toggle functionality is in the main JS template (toggleChange function)
    return "";
}

void Toggle::handleUpdate(String value) {
    bool newState = (value == "1" || value == "true");
    if (newState != state) {
        state = newState;
        stateChanged = true;
    }
}

String Toggle::getValue() {
    return state ? "1" : "0";
}

bool Toggle::isOn() {
    return state;
}

bool Toggle::wasToggled() {
    if (stateChanged) {
        stateChanged = false;
        return true;
    }
    return false;
}

void Toggle::setState(bool newState) {
    state = newState;
}

void Toggle::resetToggle() {
    state = false;
    stateChanged = false;
}

// SensorStatus Implementation
SensorStatus::SensorStatus(String label, int x, int y, int width) 
    : GUIElement(label, x, y, width, 40), displayValue("0") {
}

String SensorStatus::generateHTML() {
    String html = String(SENSOR_STATUS_TEMPLATE);
    html.replace("%ID%", id);
    html.replace("%LABEL%", label);
    html.replace("%VALUE%", displayValue);
    return html;
}

String SensorStatus::generateCSS() {
    // Memory optimized: return empty string since we're using minimal CSS
    return "";
}

String SensorStatus::generateJS() {
    // SensorStatus is read-only, no JavaScript needed
    return "";
}

void SensorStatus::handleUpdate(String value) {
    // SensorStatus is read-only, this method is not used
    // Values are updated via setValue() methods from Arduino code
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
    displayValue = value ? "true" : "false";
}

void SensorStatus::setValue(String value) {
    displayValue = value;
}

void SensorStatus::setValue(const char* value) {
    displayValue = String(value);
}