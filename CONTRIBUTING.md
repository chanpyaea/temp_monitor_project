# Contributing to Temperature Monitor

Thank you for your interest in contributing to the Temperature Monitor project! 🎉

---

## 🚀 Quick Start

1. **Fork the repository**
2. **Clone your fork:**
   ```bash
   git clone https://github.com/YOUR_USERNAME/temp_monitor_project.git
   cd temp_monitor_project
   ```
3. **Create a branch:**
   ```bash
   git checkout -b feature/your-feature-name
   ```
4. **Make your changes**
5. **Test thoroughly**
6. **Submit a pull request**

---

## 📋 Ways to Contribute

### 🐛 Bug Reports
- Check existing issues first
- Include serial monitor output (115200 baud)
- Describe hardware configuration
- List steps to reproduce
- Include expected vs actual behavior

### ✨ Feature Requests
- Check the [ROADMAP.md](ROADMAP.md) first
- Describe the use case
- Explain expected behavior
- Add mockups if UI-related

### 💻 Code Contributions
- Pick an issue from [GitHub Issues](https://github.com/chanpyaea/temp_monitor_project/issues)
- Or implement features from [ROADMAP.md](ROADMAP.md)
- Follow coding standards below
- Add tests if applicable
- Update documentation

### 📚 Documentation
- Fix typos
- Improve clarity
- Add examples
- Translate to other languages

### 🧪 Testing
- Test on different ESP32 boards
- Test with different sensors
- Report compatibility issues
- Share test results

---

## 🏗️ Development Setup

### Prerequisites
- [PlatformIO](https://platformio.org/)
- [Git](https://git-scm.com/)
- ESP32 development board
- USB cable
- Sensors (AHT10 or DHT22)

### Build & Upload
```bash
# Build
platformio run

# Upload
platformio run -t upload

# Monitor
platformio device monitor -b 115200
```

---

## 📝 Coding Standards

### Code Style
- **Indentation:** 4 spaces (no tabs)
- **Line length:** Max 100 characters
- **Naming:**
  - Classes: `PascalCase`
  - Functions: `camelCase`
  - Variables: `snake_case`
  - Constants: `UPPER_SNAKE_CASE`
  - Private members: `trailing_underscore_`

### Example:
```cpp
class SensorManager {
public:
    void begin();
    bool readSensor();
    
private:
    void updateData();
    float temperature_;
    static constexpr int MAX_RETRIES = 3;
};
```

### Comments
- Use `//` for single-line comments
- Use `/* */` for multi-line comments
- Document public APIs
- Explain "why", not "what"
- No commented-out code in commits

### Logging
```cpp
LOGI("Tag", "Info message");      // Info
LOGW("Tag", "Warning message");   // Warning
LOGE("Tag", "Error message");     // Error
LOGD("Tag", "Debug message");     // Debug
```

---

## 🧪 Testing Guidelines

### Before Submitting PR:
- [ ] Code compiles without warnings
- [ ] Tested on real hardware
- [ ] Memory usage checked (no leaks)
- [ ] Serial output reviewed
- [ ] Config persists across reboots
- [ ] Home Assistant integration works
- [ ] Web interface tested
- [ ] Serial menu tested

### Test Checklist:
```
[ ] Fresh install (erase flash first)
[ ] Config via web interface
[ ] Config via serial menu
[ ] WiFi reconnection after router reboot
[ ] MQTT reconnection after broker restart
[ ] Sensor hot-plug (if applicable)
[ ] Safe boot after 3 crashes
[ ] OTA update (if implemented)
[ ] 24-hour stability test
```

---

## 📦 Pull Request Process

### 1. Before Creating PR:
- [ ] Update from main branch
- [ ] Resolve conflicts
- [ ] Test thoroughly
- [ ] Update documentation
- [ ] Update CHANGELOG.md
- [ ] Add yourself to contributors

### 2. PR Title Format:
```
feat: Add OTA update support
fix: Resolve MQTT buffer overflow
docs: Update configuration guide
refactor: Improve sensor manager
test: Add unit tests for storage
```

**Prefixes:**
- `feat:` - New feature
- `fix:` - Bug fix
- `docs:` - Documentation only
- `style:` - Code style (formatting)
- `refactor:` - Code refactoring
- `test:` - Adding tests
- `chore:` - Maintenance tasks

### 3. PR Description Template:
```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing
- [ ] Tested on ESP32
- [ ] Tested with AHT10
- [ ] Tested with DHT22
- [ ] Home Assistant integration verified
- [ ] 24-hour stability test passed

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Documentation updated
- [ ] No new warnings
- [ ] CHANGELOG.md updated

## Screenshots (if applicable)
Add screenshots here

## Related Issues
Closes #123
```

### 4. Review Process:
1. Automated checks run
2. Maintainer reviews code
3. Feedback addressed
4. Approved and merged
5. Included in next release

---

## 🏛️ Architecture Guidelines

### Manager Pattern
Each subsystem has a manager class:
- `StorageManager` - Config persistence
- `SensorManager` - Sensor polling
- `NetworkManager` - WiFi management
- `MqttManager` - MQTT client
- `DisplayManager` - OLED display

### Adding New Manager:
1. Create `src/modules/YourManager.cpp/h`
2. Follow existing manager structure
3. Add to `main.cpp`
4. Document in `ARCHITECTURE.md`

### File Organization:
```
src/
├── config/         # Configuration and constants
├── core/           # Core systems (logger, boot, watchdog)
├── modules/        # Feature managers
├── drivers/        # Hardware drivers
├── ui/             # User interfaces
└── main.cpp        # Entry point
```

---

## 🐛 Debugging Tips

### Serial Monitor:
```cpp
// Add debug logging
LOGD("YourTag", "Variable value: %d", value);

// Check memory
LOGI("Memory", "Free heap: %u", ESP.getFreeHeap());

// Trace execution
LOGD("YourTag", "Entering function");
```

### Common Issues:
1. **Memory leaks:** Check `min_free_heap` in logs
2. **MQTT issues:** Enable debug logging
3. **WiFi problems:** Check RSSI and signal
4. **Sensor errors:** Verify I2C with scanner

---

## 📚 Documentation Standards

### Code Documentation:
```cpp
/**
 * @brief Reads temperature and humidity from sensor
 * 
 * @param temperature Output parameter for temperature in Celsius
 * @param humidity Output parameter for humidity in percent
 * @return true if read successful, false otherwise
 */
bool readSensor(float &temperature, float &humidity);
```

### Markdown Files:
- Use clear headings
- Add code examples
- Include screenshots
- Keep it concise
- Update table of contents

---

## 🎯 Priority Features (Good First Issues)

### Easy (Good for beginners):
- [ ] Add Fahrenheit display option
- [ ] Add LED status indicator
- [ ] Add buzzer for alerts
- [ ] Improve OLED display layout
- [ ] Add more sensor types

### Medium:
- [ ] Implement data logging
- [ ] Add alert system
- [ ] Enhance web interface
- [ ] Add OTA updates

### Hard:
- [ ] Multi-device support
- [ ] Cloud integration
- [ ] Advanced automation

---

## 🏆 Recognition

### Contributors will be:
- Listed in README.md
- Mentioned in release notes
- Credited in commit messages
- Invited to maintainer team (active contributors)

### Hall of Fame:
Top contributors get special recognition!

---

## 📞 Getting Help

### Questions?
- Open a [GitHub Discussion](https://github.com/chanpyaea/temp_monitor_project/discussions)
- Check existing issues
- Read documentation

### Stuck?
- Ask in pull request comments
- Tag maintainers
- Join community chat (if available)

---

## 📜 Code of Conduct

### Our Standards:
- Be respectful and inclusive
- Welcome newcomers
- Accept constructive criticism
- Focus on what's best for the project
- Show empathy

### Unacceptable:
- Harassment or discrimination
- Trolling or insulting comments
- Personal attacks
- Publishing private information
- Unprofessional conduct

---

## 📄 License

By contributing, you agree that your contributions will be licensed under the MIT License.

---

## 🙏 Thank You!

Every contribution, no matter how small, makes this project better!

**Happy coding!** 🚀

---

**Questions?** Open an issue or discussion on GitHub.

**Maintainer:** @chanpyaea  
**Last Updated:** June 1, 2026
