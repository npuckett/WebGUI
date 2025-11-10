## 🐛 Bug Fix Release - v1.9.1

### ESP32 SensorStatus Auto-Update Fixed! 🔧

This release fixes a critical bug where **SensorStatus displays were not automatically updating on ESP32** boards. The auto-update functionality was working correctly on Arduino Nano 33 IoT and UNO R4 WiFi, but was missing from the ESP32 code path.

### What Was Fixed
- ✅ **ESP32 Auto-Update**: Added missing auto-update JavaScript to `generateJS()` function
- ✅ **Real-time Updates**: SensorStatus displays now update every 500ms on ESP32
- ✅ **Code Parity**: ESP32 and Arduino boards now have identical auto-update behavior
- ✅ **No Breaking Changes**: Fully backward compatible with v1.9.0

### Technical Details
The issue was in the `generateJS()` function which is used by ESP32 to build the HTML page. The auto-update JavaScript code was only present in the `streamHTML()` function used by Arduino boards. This release adds the missing code:

```javascript
// Auto-update function for SensorStatus displays
function updateSensorDisplays() {
    fetch('/get').then(response => response.json()).then(data => {
        // Updates all sensor displays automatically
    });
}
setInterval(updateSensorDisplays, 500);
```

### Compatibility
- ✅ Arduino UNO R4 WiFi
- ✅ Arduino Nano 33 IoT (with FlashStorage)
- ✅ ESP32 (all variants) - **NOW FIXED!**

### Upgrading from v1.9.0
Simply update the library - no code changes required. Your SensorStatus displays will start auto-updating immediately on ESP32.

---

**Full Changelog**: https://github.com/npuckett/WebGUI/compare/v1.9.0...v1.9.1
