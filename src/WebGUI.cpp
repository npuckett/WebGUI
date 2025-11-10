/*
  WebGUI.cpp - Simple Web GUI Library for Arduino
  
  Copyright (c) 2025 WebGUI Library Contributors
*/

#include "WebGUI.h"

// Platform-specific includes for settings
#if defined(ARDUINO_UNOWIFIR4)
  #include <EEPROM.h>
#elif defined(ARDUINO_SAMD_NANO_33_IOT)
  #include <FlashStorage.h>
  
  // FlashStorage structures for Nano 33 IoT
  // We'll create a simple key-value storage system
  typedef struct {
    bool valid;
    int intValues[10];
    float floatValues[10];
    bool boolValues[10];
    char stringValues[10][16]; // 10 strings, max 15 chars each
    char keys[40][16]; // 40 keys total (10 per type), max 15 chars each
  } FlashSettings;
  
  FlashStorage(flash_settings, FlashSettings);
  
#elif defined(ESP32)
  #include <Preferences.h>
#endif

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

const char TEXTBOX_TEMPLATE[] PROGMEM = R"rawliteral(
        <div class="webgui-textbox-container">
            <label for="%ID%" class="webgui-textbox-label">%LABEL%</label>
            <input type="text" id="%ID%" class="webgui-textbox" value="%VALUE%" placeholder="%PLACEHOLDER%" onchange="textboxChange('%ID%', this.value)">
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
                           pageTitle("Arduino WebGUI"), pageHeading("Control Panel"),
                           settingsInitialized(false) {
#if defined(ESP32)
    server = new WebServer(port);
    preferences = nullptr;
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

GUIElement* WebGUI::findElementByID(const String& id) {
    for (GUIElement* element : elements) {
        if (element->getID() == id) {
            return element;
        }
    }
    return nullptr;
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

bool WebGUI::connectWiFi(const char* ssid, const char* password) {
    apMode = false;
    WiFi.begin(ssid, password);
    
    // Wait up to 30 seconds for connection
    int attempts = 0;
    const int maxAttempts = 30;
    
    while (WiFi.status() != WL_CONNECTED && attempts < maxAttempts) {
        delay(1000);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected");
        Serial.println("IP: " + WiFi.localIP().toString());
        return true;
    } else {
        Serial.println("\nWiFi connection failed");
        return false;
    }
}

bool WebGUI::configureStaticIP(const char* ip, const char* subnet, const char* gateway) {
    IPAddress staticIP, subnetMask, gatewayIP;
    
    if (!staticIP.fromString(ip) || !subnetMask.fromString(subnet) || !gatewayIP.fromString(gateway)) {
        Serial.println("Error: Invalid IP configuration format");
        return false;
    }
    
#if defined(ESP32)
    if (!WiFi.config(staticIP, gatewayIP, subnetMask)) {
        Serial.println("Error: Failed to configure static IP");
        return false;
    }
#else
    // For Arduino boards (UNO R4 WiFi, Nano 33 IoT), WiFi.config() returns void
    WiFi.config(staticIP, gatewayIP, subnetMask);
    Serial.println("Static IP configuration applied (Arduino)");
#endif
    
    Serial.println("Static IP configured successfully");
    Serial.println("IP: " + staticIP.toString());
    Serial.println("Subnet: " + subnetMask.toString());
    Serial.println("Gateway: " + gatewayIP.toString());
    return true;
}

bool WebGUI::connectWiFiWithStaticIP(const char* ssid, const char* password, const char* ip, const char* subnet, const char* gateway) {
    apMode = false;
    
    // Configure static IP first
    if (!configureStaticIP(ip, subnet, gateway)) {
        return false;
    }
    
    // Connect to WiFi
    WiFi.begin(ssid, password);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(1000);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected with static IP");
        Serial.println("IP: " + WiFi.localIP().toString());
        return true;
    } else {
        Serial.println("\nFailed to connect to WiFi with static IP");
        return false;
    }
}

String WebGUI::getCurrentIP() {
    return WiFi.localIP().toString();
}

String WebGUI::getCurrentSubnet() {
    return WiFi.subnetMask().toString();
}

String WebGUI::getCurrentGateway() {
#if defined(ARDUINO_UNOWIFIR4)
    // WORKAROUND: Arduino UNO R4 WiFi has a bug where gatewayIP() returns subnet mask
    // Try to calculate gateway from IP and subnet
    IPAddress ip = WiFi.localIP();
    IPAddress subnet = WiFi.subnetMask();
    
    // Calculate network address and assume gateway is .1
    IPAddress network;
    network[0] = ip[0] & subnet[0];
    network[1] = ip[1] & subnet[1];
    network[2] = ip[2] & subnet[2];
    network[3] = ip[3] & subnet[3];
    
    // Gateway is typically network + 1
    IPAddress gateway = network;
    gateway[3] = gateway[3] + 1;
    
    Serial.println("Gateway calculation workaround:");
    Serial.println("  IP: " + ip.toString());
    Serial.println("  Subnet: " + subnet.toString());
    Serial.println("  Network: " + network.toString());
    Serial.println("  Calculated Gateway: " + gateway.toString());
    Serial.println("  WiFi.gatewayIP(): " + WiFi.gatewayIP().toString());
    
    return gateway.toString();
#else
    return WiFi.gatewayIP().toString();
#endif
}

void WebGUI::restartDevice() {
    Serial.println("🔄 Restarting device...");
    delay(1000);  // Give serial time to print
    
#if defined(ESP32)
    ESP.restart();
#elif defined(ARDUINO_UNOWIFIR4)
    NVIC_SystemReset();  // For Arduino UNO R4 WiFi
#elif defined(ARDUINO_SAMD_NANO_33_IOT)
    NVIC_SystemReset();  // For Arduino Nano 33 IoT
#else
    // Fallback: infinite loop to halt execution
    Serial.println("⚠️ Platform-specific restart not available, halting...");
    while(1) { delay(1000); }
#endif
}

bool WebGUI::autoConfigureNetworkRange(const char* ssid, const char* password, int deviceNumber) {
    Serial.println("🔍 AUTO-DISCOVERY STARTED: Attempting to discover network range...");
    
    // Step 1: Connect via DHCP to discover network
    Serial.println("Step 1: Connecting via DHCP to discover network...");
    WiFi.begin(ssid, password);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(1000);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\n❌ AUTO-DISCOVERY FAILED: Could not connect via DHCP");
        return false;
    }
    
    Serial.println("\n✅ DHCP connection successful!");
    
    // Step 2: Extract network information
    IPAddress dhcpIP = WiFi.localIP();
    IPAddress gateway = WiFi.gatewayIP();
    IPAddress subnet = WiFi.subnetMask();
    
    Serial.println("Step 2: Discovered network configuration:");
    Serial.println("  DHCP IP: " + dhcpIP.toString());
    Serial.println("  Gateway (raw): " + gateway.toString());
    Serial.println("  Subnet: " + subnet.toString());
    
#if defined(ARDUINO_UNOWIFIR4)
    // Apply gateway workaround for UNO R4 WiFi
    IPAddress network;
    network[0] = dhcpIP[0] & subnet[0];
    network[1] = dhcpIP[1] & subnet[1];
    network[2] = dhcpIP[2] & subnet[2];
    network[3] = dhcpIP[3] & subnet[3];
    
    gateway = network;
    gateway[3] = gateway[3] + 1;
    Serial.println("  Gateway (corrected): " + gateway.toString());
#endif
    
    // Step 3: Calculate desired static IP based on network range
    IPAddress staticIP = calculateStaticIP(gateway, subnet, deviceNumber);
    
    Serial.println("Step 3: Calculated Static IP: " + staticIP.toString());
    
    // Step 4: Disconnect and reconnect with static IP
    Serial.println("Step 4: Switching to static IP configuration...");
    WiFi.disconnect();
    delay(1000);
    
#if defined(ESP32)
    if (!WiFi.config(staticIP, gateway, subnet)) {
        Serial.println("Failed to configure static IP");
        return false;
    }
#else
    // For Arduino boards (UNO R4 WiFi, Nano 33 IoT), WiFi.config() returns void
    WiFi.config(staticIP, gateway, subnet);
    Serial.println("Static IP configuration applied (Arduino)");
#endif
    
    WiFi.begin(ssid, password);
    attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        delay(1000);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\n✅ AUTO-DISCOVERY SUCCESSFUL!");
        Serial.println("Final configuration:");
        Serial.println("  IP: " + WiFi.localIP().toString());
        Serial.println("  Subnet: " + WiFi.subnetMask().toString());
        Serial.println("  Gateway: " + getCurrentGateway());
        return true;
    } else {
        Serial.println("\n❌ AUTO-DISCOVERY FAILED: Could not reconnect with static IP");
        return false;
    }
}

IPAddress WebGUI::calculateStaticIP(IPAddress gateway, IPAddress subnet, int deviceNumber) {
    // For /24 networks (255.255.255.0), use same first 3 octets as gateway
    if (subnet[0] == 255 && subnet[1] == 255 && subnet[2] == 255 && subnet[3] == 0) {
        return IPAddress(gateway[0], gateway[1], gateway[2], deviceNumber);
    }
    
    // For /16 networks (255.255.0.0), use same first 2 octets
    if (subnet[0] == 255 && subnet[1] == 255 && subnet[2] == 0 && subnet[3] == 0) {
        return IPAddress(gateway[0], gateway[1], 0, deviceNumber);
    }
    
    // For /8 networks (255.0.0.0), use same first octet
    if (subnet[0] == 255 && subnet[1] == 0 && subnet[2] == 0 && subnet[3] == 0) {
        return IPAddress(gateway[0], 0, 0, deviceNumber);
    }
    
    // Default: assume /24 and use gateway's network
    return IPAddress(gateway[0], gateway[1], gateway[2], deviceNumber);
}

void WebGUI::setTitle(const char* title) {
    pageTitle = String(title);
    pageHeading = String(title);  // Set both title and heading to the same value
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
    // Reset save status elements when page is refreshed
    // Look for elements with "Save Status" in the label
    for (GUIElement* element : elements) {
        Serial.println("Checking element: " + element->getLabel() + " = " + element->getValue());
        if (element->getLabel().indexOf("Save Status") >= 0) {
            String currentValue = element->getValue();
            Serial.println("Found Save Status element with value: " + currentValue);
            if (currentValue.indexOf("saved") >= 0 || currentValue.indexOf("Saving") >= 0) {
                Serial.println("Resetting save status to 'Ready to save settings'");
                element->handleUpdate("Ready to save settings");
            }
        }
    }
    
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
        
        function updateValue(id, val) {
            fetch('/set?' + id + '=' + val).catch(e => console.log('Error:', e));
        }
        
        function buttonClick(id) {
            fetch('/set?' + id + '=1');
        }
        
        function toggleChange(id, checked) {
            fetch('/set?' + id + '=' + (checked ? 'true' : 'false'));
        }
        
        function textboxChange(id, value) {
            fetch('/set?' + id + '=' + encodeURIComponent(value));
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
        
        // Auto-update function for SensorStatus displays
        function updateSensorDisplays() {
            fetch('/get').then(response => response.json()).then(data => {
                for (let elementId in data) {
                    let displayElement = document.getElementById(elementId + '_display');
                    if (displayElement) {
                        displayElement.textContent = data[elementId];
                    }
                    let toggleElement = document.getElementById(elementId);
                    if (toggleElement && toggleElement.type === 'checkbox') {
                        let shouldBeChecked = (data[elementId] === 'true' || data[elementId] === '1');
                        if (toggleElement.checked !== shouldBeChecked) {
                            toggleElement.checked = shouldBeChecked;
                        }
                    }
                }
            }).catch(error => {
                console.error('Update failed:', error);
            });
        }
        
        // Start auto-updating sensor displays every 500ms
        setInterval(updateSensorDisplays, 500);
        updateSensorDisplays();
    )rawliteral";
    
    for (GUIElement* element : elements) {
        js += element->generateJS();
    }
    
    return js;
}

// MEMORY OPTIMIZED: Stream HTML directly instead of building large strings in memory
void WebGUI::streamHTML(WiFiClient& client) {
    // Reset save status elements when page is refreshed
    // Look for elements with "Save Status" in the label
    for (GUIElement* element : elements) {
        Serial.println("Checking element: " + element->getLabel() + " = " + element->getValue());
        if (element->getLabel().indexOf("Save Status") >= 0) {
            String currentValue = element->getValue();
            Serial.println("Found Save Status element with value: " + currentValue);
            if (currentValue.indexOf("saved") >= 0 || currentValue.indexOf("Saving") >= 0) {
                Serial.println("Resetting save status to 'Ready to save settings'");
                element->handleUpdate("Ready to save settings");
            }
        }
    }
    
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
    client.print("function textboxChange(id,value){");
    client.print("fetch('/set?'+id+'='+encodeURIComponent(value)).catch(e=>console.log('Error:',e));");
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
    client.print("for(let elementId in data){");
    client.print("let displayElement=document.getElementById(elementId+'_display');");
    client.print("if(displayElement){displayElement.textContent=data[elementId];}");
    client.print("let toggleElement=document.getElementById(elementId);");
    client.print("if(toggleElement&&toggleElement.type==='checkbox'){");
    client.print("let shouldBeChecked=(data[elementId]==='true'||data[elementId]==='1');");
    client.print("if(toggleElement.checked!==shouldBeChecked){toggleElement.checked=shouldBeChecked;}}}");
    client.print("}).catch(error=>{console.error('Update failed:',error);});}");
    
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

// TextBox Implementation
TextBox::TextBox(String label, int x, int y, int width, String placeholder) 
    : GUIElement(label, x, y, width, 30), textValue(""), placeholderText(placeholder), valueChanged(false), lastValue("") {
}

String TextBox::generateHTML() {
    String html = String(TEXTBOX_TEMPLATE);
    html.replace("%ID%", id);
    html.replace("%LABEL%", label);
    html.replace("%VALUE%", textValue);
    html.replace("%PLACEHOLDER%", placeholderText);
    return html;
}

String TextBox::generateCSS() {
    // Memory optimized: return empty string since we're using minimal CSS
    return "";
}

String TextBox::generateJS() {
    // TextBox change handler - use fetch directly like other controls
    return "";  // No individual JS needed, handled by global textboxChange function
}

void TextBox::handleUpdate(String value) {
    lastValue = textValue;
    textValue = value;
    valueChanged = (lastValue != textValue);
}

String TextBox::getValue() {
    return textValue;
}

void TextBox::setValue(String value) {
    textValue = value;
    valueChanged = false;
}

bool TextBox::wasChanged() {
    bool changed = valueChanged;
    valueChanged = false; // Reset flag after reading
    return changed;
}

// IP Address helper methods
bool TextBox::isValidIPAddress() {
    return isValidIPAddress(textValue);
}

bool TextBox::isValidIPAddress(const String& ip) {
    if (ip.length() == 0) return false;
    
    int dotCount = 0;
    int lastDot = -1;
    
    // Check each character and count dots
    for (int i = 0; i < ip.length(); i++) {
        char c = ip.charAt(i);
        if (c == '.') {
            dotCount++;
            if (dotCount > 3) return false; // Too many dots
            if (i == lastDot + 1) return false; // Consecutive dots
            if (i == 0 || i == ip.length() - 1) return false; // Dot at start/end
            lastDot = i;
        } else if (!isDigit(c)) {
            return false; // Invalid character
        }
    }
    
    if (dotCount != 3) return false; // Must have exactly 3 dots
    
    // Check each octet
    int start = 0;
    for (int dot = 0; dot < 4; dot++) {
        int end = (dot == 3) ? ip.length() : ip.indexOf('.', start);
        if (end == -1) return false;
        
        String octet = ip.substring(start, end);
        if (octet.length() == 0 || octet.length() > 3) return false;
        
        int value = octet.toInt();
        if (value < 0 || value > 255) return false;
        
        // Check for leading zeros (except for "0" itself)
        if (octet.length() > 1 && octet.charAt(0) == '0') return false;
        
        start = end + 1;
    }
    
    return true;
}

String TextBox::getIPAddress() {
    if (isValidIPAddress()) {
        return textValue;
    }
    return ""; // Return empty string if invalid
}

void TextBox::setIPAddress(const String& ip) {
    if (isValidIPAddress(ip)) {
        setValue(ip);
    } else {
        Serial.println("Warning: Invalid IP address format: " + ip);
        // Don't set invalid IP, keep current value
    }
}

bool TextBox::isValidSubnetMask(const String& subnet) {
    if (!isValidIPAddress(subnet)) return false;
    
    // Convert to binary and check if it's a valid subnet mask
    // Valid subnet masks have consecutive 1s followed by consecutive 0s
    int parts[4];
    int start = 0;
    for (int i = 0; i < 4; i++) {
        int end = (i == 3) ? subnet.length() : subnet.indexOf('.', start);
        parts[i] = subnet.substring(start, end).toInt();
        start = end + 1;
    }
    
    // Convert to 32-bit number
    uint32_t mask = (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8) | parts[3];
    
    // Check if it's a valid subnet mask (consecutive 1s followed by consecutive 0s)
    uint32_t inverted = ~mask;
    return (inverted & (inverted + 1)) == 0;
}

bool TextBox::validateNetworkConfig(const String& ip, const String& subnet, const String& gateway) {
    // Validate each component
    if (!isValidIPAddress(ip) || !isValidSubnetMask(subnet) || !isValidIPAddress(gateway)) {
        return false;
    }
    
    // Additional validation: Check if IP and gateway are in the same subnet
    // This is a simplified check - could be enhanced for production use
    String ipPrefix = ip.substring(0, ip.lastIndexOf('.'));
    String gatewayPrefix = gateway.substring(0, gateway.lastIndexOf('.'));
    
    // For common /24 networks, check if first 3 octets match
    if (subnet == "255.255.255.0") {
        return ipPrefix == gatewayPrefix;
    }
    
    return true; // For other subnet masks, just validate format
}

String TextBox::formatIPDisplay(const String& ip, const String& subnet, const String& gateway) {
    return "IP: " + ip + " | Subnet: " + subnet + " | Gateway: " + gateway;
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
    // Allow updating the display value (useful for reset operations)
    displayValue = value;
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

// ============================================================================
// Persistent Settings Implementation
// ============================================================================

void WebGUI::initSettings() {
    if (settingsInitialized) return;
    
#if defined(ESP32)
    // Initialize ESP32 Preferences
    if (preferences == nullptr) {
        preferences = new Preferences();
        static_cast<Preferences*>(preferences)->begin("webgui", false);
    }
#elif defined(ARDUINO_SAMD_NANO_33_IOT)
    // FlashStorage doesn't need initialization
    // Data structures are managed through FlashStorage library
#else
    // Initialize EEPROM for Arduino boards
    EEPROM.begin();
#endif
    
    settingsInitialized = true;
}

void WebGUI::saveSetting(const char* key, int value) {
    if (!settingsInitialized) initSettings();
    
#if defined(ESP32)
    static_cast<Preferences*>(preferences)->putInt(key, value);
#elif defined(ARDUINO_SAMD_NANO_33_IOT)
    FlashSettings settings = flash_settings.read();
    if (!settings.valid) {
        memset(&settings, 0, sizeof(settings));
        settings.valid = true;
    }
    
    // Find existing key or empty slot
    int slot = -1;
    for (int i = 0; i < 10; i++) {
        if (strcmp(settings.keys[i], key) == 0) {
            slot = i;
            break;
        }
        if (settings.keys[i][0] == '\0' && slot == -1) {
            slot = i;
        }
    }
    
    if (slot != -1) {
        strncpy(settings.keys[slot], key, 15);
        settings.keys[slot][15] = '\0';
        settings.intValues[slot] = value;
        flash_settings.write(settings);
    }
#else
    // Calculate hash-based address with proper spacing for int (4 bytes)
    uint16_t hash = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hash = hash * 31 + key[i];  // Better hash function
    }
    uint16_t addr = 1616 + (hash % 100) * 16;  // Use different base address from strings
    EEPROM.put(addr, value);
#endif
}

void WebGUI::saveSetting(const char* key, float value) {
    if (!settingsInitialized) initSettings();
    
#if defined(ESP32)
    static_cast<Preferences*>(preferences)->putFloat(key, value);
#elif defined(ARDUINO_SAMD_NANO_33_IOT)
    FlashSettings settings = flash_settings.read();
    if (!settings.valid) {
        memset(&settings, 0, sizeof(settings));
        settings.valid = true;
    }
    
    int slot = -1;
    for (int i = 0; i < 10; i++) {
        if (strcmp(settings.keys[10 + i], key) == 0) {
            slot = i;
            break;
        }
        if (settings.keys[10 + i][0] == '\0' && slot == -1) {
            slot = i;
        }
    }
    
    if (slot != -1) {
        strncpy(settings.keys[10 + slot], key, 15);
        settings.keys[10 + slot][15] = '\0';
        settings.floatValues[slot] = value;
        flash_settings.write(settings);
    }
#else
    uint16_t hash = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hash = hash * 31 + key[i];
    }
    uint16_t addr = 3216 + (hash % 100) * 16;  // Use different base address from strings and ints
    EEPROM.put(addr, value);
#endif
}

void WebGUI::saveSetting(const char* key, bool value) {
    if (!settingsInitialized) initSettings();
    
#if defined(ESP32)
    static_cast<Preferences*>(preferences)->putBool(key, value);
#elif defined(ARDUINO_SAMD_NANO_33_IOT)
    FlashSettings settings = flash_settings.read();
    if (!settings.valid) {
        memset(&settings, 0, sizeof(settings));
        settings.valid = true;
    }
    
    int slot = -1;
    for (int i = 0; i < 10; i++) {
        if (strcmp(settings.keys[20 + i], key) == 0) {
            slot = i;
            break;
        }
        if (settings.keys[20 + i][0] == '\0' && slot == -1) {
            slot = i;
        }
    }
    
    if (slot != -1) {
        strncpy(settings.keys[20 + slot], key, 15);
        settings.keys[20 + slot][15] = '\0';
        settings.boolValues[slot] = value;
        flash_settings.write(settings);
    }
#else
    uint16_t addr = 16;
    for (int i = 0; key[i] != '\0'; i++) {
        addr += key[i];
    }
    addr = (addr % 1000) + 16;
    EEPROM.put(addr, value);
#endif
}

void WebGUI::saveSetting(const char* key, const char* value) {
    if (!settingsInitialized) initSettings();
    
#if defined(ESP32)
    static_cast<Preferences*>(preferences)->putString(key, value);
#elif defined(ARDUINO_SAMD_NANO_33_IOT)
    FlashSettings settings = flash_settings.read();
    if (!settings.valid) {
        memset(&settings, 0, sizeof(settings));
        settings.valid = true;
    }
    
    int slot = -1;
    for (int i = 0; i < 10; i++) {
        if (strcmp(settings.keys[30 + i], key) == 0) {
            slot = i;
            break;
        }
        if (settings.keys[30 + i][0] == '\0' && slot == -1) {
            slot = i;
        }
    }
    
    if (slot != -1) {
        strncpy(settings.keys[30 + slot], key, 15);
        settings.keys[30 + slot][15] = '\0';
        strncpy(settings.stringValues[slot], value, 15);
        settings.stringValues[slot][15] = '\0';
        flash_settings.write(settings);
    }
#else
    uint16_t hash = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hash = hash * 31 + key[i];
    }
    uint16_t addr = 16 + (hash % 100) * 16;  // 16-byte slots for longer strings
    // For EEPROM, store string length first, then string data
    uint8_t len = strlen(value);
    EEPROM.put(addr, len);
    for (uint8_t i = 0; i < len && i < 15; i++) { // Allow up to 15 characters in 16-byte slot
        EEPROM.put(addr + 1 + i, value[i]);
    }
#endif
}

int WebGUI::loadIntSetting(const char* key) {
    if (!settingsInitialized) initSettings();
    
#if defined(ESP32)
    return static_cast<Preferences*>(preferences)->getInt(key, 0);
#elif defined(ARDUINO_SAMD_NANO_33_IOT)
    FlashSettings settings = flash_settings.read();
    if (!settings.valid) return 0;
    
    for (int i = 0; i < 10; i++) {
        if (strcmp(settings.keys[i], key) == 0) {
            return settings.intValues[i];
        }
    }
    return 0;
#else
    uint16_t hash = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hash = hash * 31 + key[i];
    }
    uint16_t addr = 1616 + (hash % 100) * 16;  // Match saveSetting address
    int value;
    EEPROM.get(addr, value);
    return value;
#endif
}

float WebGUI::loadFloatSetting(const char* key) {
    if (!settingsInitialized) initSettings();
    
#if defined(ESP32)
    return static_cast<Preferences*>(preferences)->getFloat(key, 0.0);
#elif defined(ARDUINO_SAMD_NANO_33_IOT)
    FlashSettings settings = flash_settings.read();
    if (!settings.valid) return 0.0;
    
    for (int i = 0; i < 10; i++) {
        if (strcmp(settings.keys[10 + i], key) == 0) {
            return settings.floatValues[i];
        }
    }
    return 0.0;
#else
    uint16_t hash = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hash = hash * 31 + key[i];
    }
    uint16_t addr = 3216 + (hash % 100) * 16;  // Match saveSetting address
    float value;
    EEPROM.get(addr, value);
    return value;
#endif
}

bool WebGUI::loadBoolSetting(const char* key) {
    if (!settingsInitialized) initSettings();
    
#if defined(ESP32)
    return static_cast<Preferences*>(preferences)->getBool(key, false);
#elif defined(ARDUINO_SAMD_NANO_33_IOT)
    FlashSettings settings = flash_settings.read();
    if (!settings.valid) return false;
    
    for (int i = 0; i < 10; i++) {
        if (strcmp(settings.keys[20 + i], key) == 0) {
            return settings.boolValues[i];
        }
    }
    return false;
#else
    uint16_t hash = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hash = hash * 31 + key[i];
    }
    uint16_t addr = 16 + (hash % 200) * 8;
    bool value;
    EEPROM.get(addr, value);
    return value;
#endif
}

String WebGUI::loadStringSetting(const char* key) {
    if (!settingsInitialized) initSettings();
    
#if defined(ESP32)
    return static_cast<Preferences*>(preferences)->getString(key, "");
#elif defined(ARDUINO_SAMD_NANO_33_IOT)
    FlashSettings settings = flash_settings.read();
    if (!settings.valid) return "";
    
    for (int i = 0; i < 10; i++) {
        if (strcmp(settings.keys[30 + i], key) == 0) {
            return String(settings.stringValues[i]);
        }
    }
    return "";
#else
    uint16_t hash = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        hash = hash * 31 + key[i];
    }
    uint16_t addr = 16 + (hash % 100) * 16;  // 16-byte slots for longer strings
    uint8_t len;
    EEPROM.get(addr, len);
    if (len > 15) return ""; // Safety check for 16-byte slot
    
    char buffer[16] = {0};
    for (uint8_t i = 0; i < len; i++) {
        EEPROM.get(addr + 1 + i, buffer[i]);
    }
    return String(buffer);
#endif
}

void WebGUI::clearMemory() {
#if defined(ESP32) || defined(ESP8266)
    // For ESP32/ESP8266 - Clear all Preferences
    if (preferences) {
        static_cast<Preferences*>(preferences)->clear();
        Serial.println("✅ ESP32 Preferences cleared");
    }
#elif defined(ARDUINO_SAMD_NANO_33_IOT)
    // For Nano 33 IoT - Clear FlashStorage
    FlashSettings settings;
    memset(&settings, 0, sizeof(settings));
    flash_settings.write(settings);
    Serial.println("✅ Nano 33 IoT Flash Storage cleared");
#else
    // For Arduino UNO R4 WiFi and other EEPROM-based systems
    // Clear first 1024 bytes of EEPROM (more than enough for most applications)
    for (int i = 0; i < 1024; i++) {
        EEPROM.write(i, 0xFF); // 0xFF is the erased state for EEPROM
    }
    #if defined(ARDUINO_UNOR4_WIFI)
        // Arduino UNO R4 WiFi doesn't require EEPROM.commit()
        Serial.println("✅ Arduino UNO R4 WiFi EEPROM cleared (1024 bytes)");
    #else
        // Other Arduino platforms may need commit
        EEPROM.commit();
        Serial.println("✅ Arduino EEPROM cleared (1024 bytes)");
    #endif
#endif
}

// Utility Functions Implementation

// Cross-platform function to get available RAM
int getFreeRAM() {
#ifdef ARDUINO_UNOR4_WIFI
    // For Arduino UNO R4 WiFi (Renesas RA platform)
    // Simple stack-based approximation
    char dummy;
    return (int)&dummy - 0x20000000; // Approximate available stack space
#else
    // For AVR-based Arduinos (UNO, Nano, etc.)
    extern int __heap_start, *__brkval;
    int v;
    return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
#endif
}

// Cross-platform function to clear all stored memory/settings
void clearMemory() {
    GUI.clearMemory();
}