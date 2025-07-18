/*
  Station_SetIPAddress - WebGUI Library (Station Mode)
  
  DESCRIPTION:
  This example demonstrates static IP configuration with the WebGUI library.
  Configure your network settings through a clean web interface with persistent storage.
  
  FEATURES:
  - Static IP configuration via web interface
  - TextBox controls for IP address, subnet mask, and gateway
  - Persistent storage of network configuration
  - Apply, Reset, and Restart buttons
  - Real-time network status display
  - Input validation and error handling
  - Cross-platform device restart functionality
  
  HARDWARE REQUIRED:
  - Arduino UNO R4 WiFi, Arduino Nano 33 IoT, or ESP32
  - No external components needed
  
  NETWORK SETUP:
  1. Update WiFi credentials below (WIFI_SSID and WIFI_PASSWORD)
  2. Update DEFAULT network settings for your network range
  3. Upload this sketch to your Arduino
  4. Check Serial Monitor for the IP address
  5. Open web browser and navigate to the IP
  6. Customize IP settings and click Apply for permanent configuration
  7. Click Restart to apply new network settings
  
  USAGE:
  - Access web interface at the displayed IP address
  - Modify network settings using the text boxes
  - Click "Apply Settings" to save configuration to persistent storage
  - Click "Reset to Default" to restore factory defaults
  - Click "Restart Device" to apply new network configuration
  - Settings persist across power cycles and restarts
  
  This example code is in the public domain and can be used as a starting
  point for your own projects.
*/

#include <WebGUI.h>

// Network configuration - UPDATE THESE WITH YOUR WIFI CREDENTIALS
const char* WIFI_SSID = "YourNetworkName";        // Your WiFi network name
const char* WIFI_PASSWORD = "YourPassword";       // Your WiFi password

// Default network configuration - UPDATE THESE FOR YOUR NETWORK RANGE
const char* DEFAULT_IP = "192.168.1.100";         // Default static IP address
const char* DEFAULT_SUBNET = "255.255.255.0";     // Default subnet mask
const char* DEFAULT_GATEWAY = "192.168.1.1";      // Default gateway IP address

// Web interface configuration
const char* PAGE_TITLE = "Network Configuration";

// Current network settings (loaded from persistent storage)
String current_ip;
String current_subnet;
String current_gateway;

// Control elements for network configuration interface
TextBox ipAddressBox("IP Address", 20, 50, 300, "192.168.1.100");
TextBox subnetMaskBox("Subnet Mask", 20, 100, 300, "255.255.255.0");
TextBox gatewayBox("Gateway", 20, 150, 300, "192.168.1.1");
Button applyButton("Apply Settings", 20, 200, 120, 40);
Button resetButton("Reset to Default", 150, 200, 140, 40);
Button restartButton("Restart Device", 300, 200, 120, 40);
SensorStatus networkStatus("Network Status", 20, 250, 400);
SensorStatus currentIPStatus("Current Network", 20, 300, 400);

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);
  Serial.println("Starting WebGUI Network Configuration Example...");
  
  // Initialize the settings system
  GUI.initSettings();
  
  // Load saved network configuration
  current_ip = GUI.loadStringSetting("custom_ip");
  current_subnet = GUI.loadStringSetting("custom_subnet");
  current_gateway = GUI.loadStringSetting("custom_gateway");
  
  // Use saved settings if available, otherwise use defaults
  if (current_ip.length() == 0) {
    current_ip = DEFAULT_IP;
    current_subnet = DEFAULT_SUBNET;
    current_gateway = DEFAULT_GATEWAY;
    Serial.println("Using default network configuration");
  } else {
    Serial.println("Using saved network configuration");
  }
  
  Serial.println("Network Configuration:");
  Serial.println("  IP: " + current_ip);
  Serial.println("  Subnet: " + current_subnet);
  Serial.println("  Gateway: " + current_gateway);
  
  // Set TextBox values to current configuration
  ipAddressBox.setIPAddress(current_ip);
  subnetMaskBox.setValue(current_subnet);
  gatewayBox.setIPAddress(current_gateway);
  
  // Connect to WiFi with static IP
  Serial.println("Connecting to WiFi with static IP...");
  bool connected = GUI.connectWiFiWithStaticIP(
    WIFI_SSID, 
    WIFI_PASSWORD, 
    current_ip.c_str(), 
    current_subnet.c_str(), 
    current_gateway.c_str()
  );
  
  if (connected) {
    Serial.println("WiFi connected successfully!");
    networkStatus.setValue("Connected - Static IP Configuration");
  } else {
    Serial.println("WiFi connection failed!");
    networkStatus.setValue("Connection Failed - Check WiFi credentials and network settings");
  }
  
  // Display current network information
  String networkInfo = TextBox::formatIPDisplay(current_ip, current_subnet, current_gateway);
  currentIPStatus.setValue(networkInfo);
  
  // Display network information to serial
  Serial.println("=== NETWORK INFORMATION ===");
  Serial.println("Current IP: " + GUI.getCurrentIP());
  Serial.println("Current Subnet: " + GUI.getCurrentSubnet());
  Serial.println("Current Gateway: " + GUI.getCurrentGateway());
  Serial.println("WiFi Status: " + String(WiFi.status()));
  Serial.print("SSID: "); Serial.println(WiFi.SSID());
  Serial.println("Signal Strength: " + String(WiFi.RSSI()) + " dBm");
  Serial.println("Web interface: http://" + GUI.getCurrentIP());
  Serial.println("========================");
  
  // Configure the web interface
  GUI.setTitle(PAGE_TITLE);
  
  // Add control elements to the web interface
  GUI.addElement(&ipAddressBox);
  GUI.addElement(&subnetMaskBox);
  GUI.addElement(&gatewayBox);
  GUI.addElement(&applyButton);
  GUI.addElement(&resetButton);
  GUI.addElement(&restartButton);
  GUI.addElement(&networkStatus);
  GUI.addElement(&currentIPStatus);
  
  // Start the web server
  Serial.println("Starting web server...");
  GUI.begin();
  Serial.println("✅ Web server started successfully!");
  
  Serial.println("");
  Serial.println("🌐 Network Configuration ready!");
  Serial.println("📱 Access the web interface at: http://" + GUI.getCurrentIP());
  Serial.println("");
}

void loop() {
  // CRITICAL: Always call GUI.update() in your main loop
  // This processes web requests and updates control values
  GUI.update();
  
  // Check if Apply button was pressed
  if (applyButton.wasPressed()) {
    Serial.println("Apply button pressed - saving new network configuration...");
    
    // Get new values from TextBoxes using the new helper methods
    String new_ip = ipAddressBox.getIPAddress();
    String new_subnet = subnetMaskBox.getValue();
    String new_gateway = gatewayBox.getIPAddress();
    
    // Validate the complete network configuration
    if (new_ip.length() == 0 || new_gateway.length() == 0) {
        networkStatus.setValue("Error: Invalid IP address format. Please check your entries.");
        Serial.println("❌ Validation failed - invalid IP address format");
        return;
    }
    
    if (!TextBox::validateNetworkConfig(new_ip, new_subnet, new_gateway)) {
        networkStatus.setValue("Error: Invalid network configuration. IP and Gateway must be in same subnet.");
        Serial.println("❌ Validation failed - IP and Gateway not in same subnet");
        Serial.println("   IP: " + new_ip + ", Subnet: " + new_subnet + ", Gateway: " + new_gateway);
        return;
    }
    
    // Additional validation: Check if IP is in a reasonable range
    if (new_ip.endsWith(".0") || new_ip.endsWith(".255")) {
        networkStatus.setValue("Warning: IP ending in .0 or .255 may not work properly.");
        Serial.println("⚠️ Warning - IP ends in .0 or .255, this may cause issues");
    }
    
    Serial.println("New Network Configuration:");
    Serial.println("  IP: " + new_ip);
    Serial.println("  Subnet: " + new_subnet);
    Serial.println("  Gateway: " + new_gateway);
    
    // Update current network display using the helper method
    String newNetworkInfo = "SAVED: " + TextBox::formatIPDisplay(new_ip, new_subnet, new_gateway);
    currentIPStatus.setValue(newNetworkInfo);
    
    // Save new configuration to persistent storage
    GUI.saveSetting("custom_ip", new_ip.c_str());
    GUI.saveSetting("custom_subnet", new_subnet.c_str());
    GUI.saveSetting("custom_gateway", new_gateway.c_str());
    
    // Show success status
    networkStatus.setValue("Settings saved! Restart device to apply new IP configuration.");
    
    Serial.println("Network configuration saved to persistent storage.");
    Serial.println("Device must be restarted to apply new network settings.");
  }
  
  // Check if Reset button was pressed
  if (resetButton.wasPressed()) {
    Serial.println("Reset button pressed - restoring default configuration...");
    
    // Update current network display using the helper method
    String defaultNetworkInfo = "RESET: " + TextBox::formatIPDisplay(DEFAULT_IP, DEFAULT_SUBNET, DEFAULT_GATEWAY);
    currentIPStatus.setValue(defaultNetworkInfo);
    
    // Clear saved settings (this will cause defaults to be used on next boot)
    GUI.saveSetting("custom_ip", "");
    GUI.saveSetting("custom_subnet", "");
    GUI.saveSetting("custom_gateway", "");
    
    // Reset TextBoxes to default values using the helper method
    ipAddressBox.setIPAddress(DEFAULT_IP);
    subnetMaskBox.setValue(DEFAULT_SUBNET);
    gatewayBox.setIPAddress(DEFAULT_GATEWAY);
    
    // Show success status
    networkStatus.setValue("Default settings restored! Restart device to apply default IP configuration.");
    
    Serial.println("Default configuration restored.");
    Serial.println("Device must be restarted to apply default network settings.");
  }
  
  // Check if Restart button was pressed
  if (restartButton.wasPressed()) {
    Serial.println("Restart button pressed - restarting device...");
    
    // Show restart message
    networkStatus.setValue("Restarting device to apply network configuration...");
    currentIPStatus.setValue("Restarting...");
    delay(2000);  // Give user time to see the message
    
    // Restart the device using library function
    GUI.restartDevice();
  }
  
  // Check if any TextBox values were changed (for real-time feedback)
  if (ipAddressBox.wasChanged() || subnetMaskBox.wasChanged() || gatewayBox.wasChanged()) {
    networkStatus.setValue("Configuration modified - Click Apply to save changes");
  }
  
  // Small delay to prevent overwhelming the system
  delay(10);
}
