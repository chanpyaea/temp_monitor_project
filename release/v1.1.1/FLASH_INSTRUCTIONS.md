# Flash Instructions for Temperature Monitor v1.1.1

## Quick Flash Guide

### Windows Users

**Using ESP Flash Download Tool (Easiest):**

1. **Download Tool:**
   - Get from: https://www.espressif.com/en/support/download/other-tools
   - Extract and run `flash_download_tool_x.x.x.exe`

2. **Configure:**
   - Select **ESP32** chip
   - Add files with addresses:
     ```
     bootloader.bin      @ 0x1000
     partitions.bin      @ 0x8000
     temp_monitor_v1.1.1.bin @ 0x10000
     ```
   - Check all three checkboxes
   - Select your COM port (e.g., COM3)
   - Baud: 460800
   - Flash mode: DIO
   - Flash frequency: 40MHz

3. **Flash:**
   - Click **START**
   - Wait for "FINISH" message
   - Press reset button on ESP32

**Using esptool (Command Line):**

```cmd
# Install Python and esptool
pip install esptool

# Find your COM port (Device Manager → Ports)
# Replace COM3 with your port

# Erase flash (first time only)
esptool.py --port COM3 erase_flash

# Flash firmware
esptool.py --chip esp32 --port COM3 --baud 460800 ^
  --before default_reset --after hard_reset write_flash -z ^
  --flash_mode dio --flash_freq 40m --flash_size detect ^
  0x1000 bootloader.bin ^
  0x8000 partitions.bin ^
  0x10000 temp_monitor_v1.1.1.bin
```

---

### Linux/Mac Users

```bash
# Install esptool
pip3 install esptool

# Find your port
ls /dev/tty.* # Mac
ls /dev/ttyUSB* # Linux

# Erase flash (first time only)
esptool.py --port /dev/ttyUSB0 erase_flash

# Flash firmware
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 460800 \
  --before default_reset --after hard_reset write_flash -z \
  --flash_mode dio --flash_freq 40m --flash_size detect \
  0x1000 bootloader.bin \
  0x8000 partitions.bin \
  0x10000 temp_monitor_v1.1.1.bin
```

---

## After Flashing

1. **Open Serial Monitor:**
   - Baud rate: 115200
   - You should see boot messages

2. **Connect to WiFi:**
   - Device creates AP: "TempMonitor-Setup"
   - Password: "12345678"

3. **Configure:**
   - Browser opens to http://192.168.4.1
   - Enter WiFi and MQTT settings
   - Save and restart

4. **Verify:**
   - Check serial output for MQTT connection
   - Check Home Assistant for sensors

---

## Troubleshooting

**"Failed to connect" error:**
- Hold BOOT button while connecting USB
- Try different USB cable
- Check drivers installed

**"Invalid head of packet" error:**
- Erase flash first: `esptool.py --port COM3 erase_flash`
- Try lower baud rate: 115200

**No serial output:**
- Check baud rate is 115200
- Press reset button on ESP32
- Try different USB port

---

## File Checksums (SHA256)

```
bootloader.bin: [will be generated]
partitions.bin: [will be generated]
temp_monitor_v1.1.1.bin: [will be generated]
```

---

## Hardware Requirements

- ESP32 development board (any variant)
- AHT10 or DHT22 temperature sensor
- SSD1306 OLED display (128x64, I2C)
- USB cable for programming
- Jumper wires

---

**Need help? Check RELEASE_NOTES.md for detailed setup instructions.**
