# Flash Instructions for v1.2.0

## Prerequisites

### Hardware
- ESP32 development board
- USB cable (data-capable, not charge-only)
- Computer (Windows/Mac/Linux)

### Software
Choose one:
- **Option A:** [esptool.py](https://github.com/espressif/esptool) (recommended, cross-platform)
- **Option B:** [ESP Flash Download Tool](https://www.espressif.com/en/support/download/other-tools) (Windows GUI)
- **Option C:** Arduino IDE or PlatformIO

---

## Method 1: Using esptool.py (Recommended)

### Step 1: Install esptool
```bash
pip install esptool
```

### Step 2: Download Files
Download these 3 files from the release:
- `bootloader.bin`
- `partitions.bin`
- `temp_monitor_v1.2.0.bin`

### Step 3: Connect ESP32
1. Connect ESP32 to computer via USB
2. Find COM port:
   - **Windows:** Check Device Manager → Ports (COM3, COM4, etc.)
   - **Mac/Linux:** Run `ls /dev/tty.*` or `ls /dev/ttyUSB*`

### Step 4: Erase Flash (First-time only)
```bash
esptool.py --port COM3 erase_flash
```
Replace `COM3` with your port.

### Step 5: Flash Firmware
```bash
esptool.py --chip esp32 --port COM3 --baud 921600 \
  --before default_reset --after hard_reset write_flash -z \
  --flash_mode dio --flash_freq 40m --flash_size detect \
  0x1000 bootloader.bin \
  0x8000 partitions.bin \
  0x10000 temp_monitor_v1.2.0.bin
```

**Adjust:**
- Replace `COM3` with your port
- Mac/Linux: use `/dev/ttyUSB0` or `/dev/tty.usbserial-*`
- Lower baud if errors: `--baud 115200`

### Step 6: Verify
After flashing completes:
```
Hash of data verified.

Leaving...
Hard resetting via RTS pin...
```

---

## Method 2: ESP Flash Download Tool (Windows GUI)

### Step 1: Download Tool
Get from: https://www.espressif.com/en/support/download/other-tools

### Step 2: Configure
1. Run `flash_download_tool_x.x.x.exe`
2. Select **ESP32** → **Develop**
3. Add files with addresses:

   | File | Address | Checkbox |
   |------|---------|----------|
   | `bootloader.bin` | `0x1000` | ✅ |
   | `partitions.bin` | `0x8000` | ✅ |
   | `temp_monitor_v1.2.0.bin` | `0x10000` | ✅ |

4. Settings:
   - **SPI Speed:** 40MHz
   - **SPI Mode:** DIO
   - **COM Port:** Your ESP32 port
   - **Baud:** 921600

### Step 3: Flash
1. Click **START**
2. Wait for "FINISH" message

---

## Method 3: PlatformIO (Build from Source)

### Step 1: Install PlatformIO
```bash
pip install platformio
```

### Step 2: Clone Repository
```bash
git clone https://github.com/chanpyaea/temp_monitor_project.git
cd temp_monitor_project
git checkout v1.2.0
```

### Step 3: Build and Upload
```bash
platformio run -t upload
```

---

## Troubleshooting

### Error: "Failed to connect"
**Solutions:**
1. Hold **BOOT** button while connecting USB
2. Try lower baud rate: `--baud 115200`
3. Check USB cable (must support data)
4. Install CH340/CP2102 drivers

### Error: "Invalid header"
**Solutions:**
1. Re-download binary files (may be corrupted)
2. Verify checksums with `checksums.txt`
3. Use correct flash addresses

### Error: "Timed out waiting for packet header"
**Solutions:**
1. Press **RST** button on ESP32
2. Try different USB port
3. Reduce baud rate to 115200

### ESP32 won't boot after flashing
**Solutions:**
1. Erase flash completely: `esptool.py --port COM3 erase_flash`
2. Re-flash all three files
3. Press **RST** button

---

## Post-Flash Configuration

### Step 1: Serial Monitor (Optional)
Connect at **115200 baud** to see boot logs:
```
=== Temperature Monitor v1.2.0 ===
[INFO] Main: Reset reason: 1
[INFO] Display: Display initialized successfully
[INFO] Network: WiFi not configured
```

### Step 2: Connect to AP
1. Look for WiFi network: **"TempMonitor-Setup"**
2. Password: **12345678**
3. Browser should auto-open to http://192.168.4.1

### Step 3: Configure
1. Enter your WiFi credentials
2. Enter MQTT broker details (optional)
3. Click **Save**
4. Device will restart and connect

### Step 4: Verify
Check serial monitor for:
```
[INFO] WiFi: Connected to YourNetwork
[INFO] WiFi: IP address: 192.168.1.xxx
[INFO] MQTT: Connected to broker
[INFO] DataLogger: Initialized - capacity: 288 entries
```

---

## Verification Checksums (SHA256)

Compare with `checksums.txt`:
```
3d234a7471f67b013686dabd4dee7c1fa915c9928463616a94bc9297acf1abf8  bootloader.bin
148b959cbff1c38aa8e1d5c0ba9d612c54997b945e56a63f41223eef650653a1  partitions.bin
24bb8a3cc9f66fc29f509f3e80e67319c3ab331b2838188812f44c502f4c5d45  temp_monitor_v1.2.0.bin
```

**Verify on Linux/Mac:**
```bash
sha256sum -c checksums.txt
```

**Verify on Windows (PowerShell):**
```powershell
Get-FileHash temp_monitor_v1.2.0.bin -Algorithm SHA256
```

---

## Upgrade from v1.1.1

### Quick Upgrade
If you're already running v1.1.1, just flash the new firmware:
```bash
esptool.py --port COM3 --baud 921600 write_flash 0x10000 temp_monitor_v1.2.0.bin
```

**Configuration is preserved!** No need to re-enter WiFi/MQTT settings.

---

## Factory Reset

To completely erase and start fresh:
```bash
# Erase everything
esptool.py --port COM3 erase_flash

# Flash firmware
esptool.py --chip esp32 --port COM3 --baud 921600 \
  write_flash -z \
  0x1000 bootloader.bin \
  0x8000 partitions.bin \
  0x10000 temp_monitor_v1.2.0.bin
```

---

## Support

**Issues?** Open a ticket: https://github.com/chanpyaea/temp_monitor_project/issues

Include:
- Your OS and esptool version
- Full error message
- ESP32 board type
- Steps you've tried
