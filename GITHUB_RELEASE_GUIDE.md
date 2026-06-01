# GitHub Release Instructions

## ✅ Git Push Complete!

Your code and tag have been pushed to GitHub:
- Repository: https://github.com/chanpyaea/temp_monitor_project
- Tag: v1.1.1
- Binaries: In `release/v1.1.1/` folder

---

## 📦 Create GitHub Release (Manual Steps)

### Option 1: Using GitHub Web Interface (Recommended)

1. **Go to your repository:**
   ```
   https://github.com/chanpyaea/temp_monitor_project
   ```

2. **Navigate to Releases:**
   - Click **"Releases"** on the right sidebar
   - Or go to: https://github.com/chanpyaea/temp_monitor_project/releases

3. **Create New Release:**
   - Click **"Draft a new release"**
   - Or click **"Create a new release"**

4. **Fill in Release Details:**

   **Choose a tag:** `v1.1.1` (should appear in dropdown)

   **Release title:** `v1.1.1 - Home Assistant Discovery Fix`

   **Description:** (Copy this)
   ```markdown
   ## 🎉 Home Assistant Integration Now Working!

   Critical fix for MQTT auto-discovery in Home Assistant. All sensors now appear automatically!

   ### 🔧 What's Fixed
   - **MQTT Buffer Size:** Increased to 1024 bytes (was 256) - fixes discovery message truncation
   - **Discovery Timing:** Sent immediately after connection (was 30s delay)
   - **Compilation Errors:** All resolved (name conflicts, wrong libraries, linker errors)

   ### ✅ Verified Working
   - ✅ All 4 sensors appear in Home Assistant automatically
   - ✅ Temperature: 31.2°C
   - ✅ Humidity: 78.7%
   - ✅ WiFi Signal: -34 dBm
   - ✅ Uptime tracking

   ### 📦 Installation

   **Quick Flash (Pre-compiled Binary):**
   1. Download all 3 `.bin` files below
   2. Use [esptool](https://github.com/espressif/esptool) or [ESP Flash Tool](https://www.espressif.com/en/support/download/other-tools)
   3. Flash at addresses:
      - `bootloader.bin` @ 0x1000
      - `partitions.bin` @ 0x8000
      - `temp_monitor_v1.1.1.bin` @ 0x10000
   4. See `FLASH_INSTRUCTIONS.md` for detailed steps

   **Build from Source:**
   ```bash
   git clone https://github.com/chanpyaea/temp_monitor_project.git
   cd temp_monitor_project
   platformio run -t upload
   ```

   ### 🔧 First-Time Setup
   1. Power on ESP32
   2. Connect to WiFi: **"TempMonitor-Setup"** (password: **12345678**)
   3. Browser opens to http://192.168.4.1
   4. Enter WiFi and MQTT settings
   5. Save and restart
   6. Check Home Assistant → MQTT → Devices → "TempMonitor"

   ### 📊 Features
   - Dual sensor support (AHT10/DHT22 auto-detect)
   - OLED display (SSD1306)
   - Web configuration interface
   - Serial configuration menu (press 'c')
   - Home Assistant MQTT auto-discovery
   - Safe boot with crash recovery
   - Persistent configuration storage

   ### 📚 Documentation
   - [RELEASE_NOTES.md](release/v1.1.1/RELEASE_NOTES.md) - Complete release notes
   - [FLASH_INSTRUCTIONS.md](release/v1.1.1/FLASH_INSTRUCTIONS.md) - Flashing guide
   - [CONFIGURATION_GUIDE.md](CONFIGURATION_GUIDE.md) - Setup instructions
   - [HOMEASSISTANT_FIX.md](HOMEASSISTANT_FIX.md) - Technical details

   ### 🔐 Checksums (SHA256)
   ```
   3d234a7471f67b013686dabd4dee7c1fa915c9928463616a94bc9297acf1abf8  bootloader.bin
   148b959cbff1c38aa8e1d5c0ba9d612c54997b945e56a63f41223eef650653a1  partitions.bin
   76f395412d6282c5fca84f27aada72211a51e7dd763a2aaf0434f292d2fb449a  temp_monitor_v1.1.1.bin
   ```

   ### 🐛 Bug Reports
   Open an issue on GitHub with:
   - Serial monitor output (115200 baud)
   - Hardware configuration
   - Home Assistant version

   ---

   **Full Changelog:** [CHANGELOG.md](CHANGELOG.md)
   ```

5. **Attach Binary Files:**
   - Click **"Attach binaries by dropping them here or selecting them"**
   - Upload these files from `release/v1.1.1/`:
     - ✅ `temp_monitor_v1.1.1.bin` (main firmware - 907 KB)
     - ✅ `bootloader.bin` (18 KB)
     - ✅ `partitions.bin` (3 KB)
     - ✅ `checksums.txt` (checksums)
     - ✅ `RELEASE_NOTES.md` (documentation)
     - ✅ `FLASH_INSTRUCTIONS.md` (flashing guide)

6. **Set as Latest Release:**
   - ✅ Check **"Set as the latest release"**
   - ✅ Check **"Create a discussion for this release"** (optional)

7. **Publish:**
   - Click **"Publish release"**

---

### Option 2: Using GitHub CLI (gh)

```bash
cd "C:\Users\ACER\Documents\PlatformIO\Projects\temp_monitor_project"

# Install GitHub CLI if not installed
# Download from: https://cli.github.com/

# Login to GitHub
gh auth login

# Create release with files
gh release create v1.1.1 \
  release/v1.1.1/temp_monitor_v1.1.1.bin \
  release/v1.1.1/bootloader.bin \
  release/v1.1.1/partitions.bin \
  release/v1.1.1/checksums.txt \
  release/v1.1.1/RELEASE_NOTES.md \
  release/v1.1.1/FLASH_INSTRUCTIONS.md \
  --title "v1.1.1 - Home Assistant Discovery Fix" \
  --notes-file release/v1.1.1/RELEASE_NOTES.md
```

---

## ✅ After Publishing

Your release will be available at:
```
https://github.com/chanpyaea/temp_monitor_project/releases/tag/v1.1.1
```

Users can download:
- Pre-compiled binaries (no build tools needed)
- Complete documentation
- Checksums for verification

---

## 📢 Share Your Release

After publishing, you can share:
- Direct link: `https://github.com/chanpyaea/temp_monitor_project/releases/latest`
- On Home Assistant forums
- On Reddit r/homeassistant
- On ESP32 communities

---

## 🎉 Release Complete!

Your Temperature Monitor v1.1.1 is now available for everyone to use!

**Repository:** https://github.com/chanpyaea/temp_monitor_project  
**Release:** v1.1.1  
**Status:** Production Ready  
**Date:** June 1, 2026
