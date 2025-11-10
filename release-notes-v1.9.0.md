## 🎉 What's New in v1.9.0

### Arduino Nano 33 IoT Support Fixed! 🔧
This release fixes the **EEPROM.h error** on Arduino Nano 33 IoT and adds proper persistent storage support using FlashStorage.

### Key Changes
- ✅ **FlashStorage Integration**: Replaced EEPROM with FlashStorage for Nano 33 IoT (SAMD21 boards)
- ✅ **Persistent Settings**: Full support for saveSetting() and loadSetting() on Nano 33 IoT
- ✅ **Storage Capacity**: 10 values per type (int, float, bool, string) with 15-character keys
- ✅ **Clear Memory**: Added FlashStorage support in clearMemory() function
- ✅ **Backward Compatible**: Maintains full compatibility with ESP32 (Preferences) and UNO R4 WiFi (EEPROM)

### Installation
The FlashStorage library is now listed as a dependency. Install it via Arduino Library Manager:
1. Go to **Sketch** → **Include Library** → **Manage Libraries**
2. Search for "FlashStorage"
3. Install **FlashStorage** by cmaglie

### Bug Fixes
- Fixed compilation error: `fatal error: EEPROM.h: No such file or directory` on Nano 33 IoT
- Added proper platform-specific includes for SAMD21 architecture

### Documentation
- Updated README with FlashStorage installation instructions
- Added notes about Nano 33 IoT requirements

### Compatibility
- ✅ Arduino UNO R4 WiFi
- ✅ Arduino Nano 33 IoT (now with FlashStorage!)
- ✅ ESP32 (all variants)

### Breaking Changes
None - fully backward compatible with v1.8.x

---

**Full Changelog**: https://github.com/npuckett/WebGUI/compare/v1.8.5...v1.9.0
