# Release Automation Setup

## Overview

Automated GitHub Actions workflow that builds ESP32 firmware binaries and publishes them to GitHub Releases whenever a new version tag is pushed.

## What Was Added

### 1. GitHub Actions Workflow
**File:** `.github/workflows/build-release.yml`

**Triggers:**
- When a tag matching `v*.*.*` is pushed (e.g., `v1.2.0`, `v2.0.1`)
- Manual workflow dispatch from GitHub Actions tab

**What It Does:**
1. Sets up Ubuntu build environment
2. Installs Python and PlatformIO
3. Caches PlatformIO dependencies for faster builds
4. Builds the firmware for ESP32
5. Extracts compiled binaries:
   - `firmware.bin` → `temp_monitor_vX.X.X.bin`
   - `bootloader.bin`
   - `partitions.bin`
6. Generates SHA256 checksums
7. Creates flashing instructions
8. Uploads everything to GitHub Release

### 2. Updated README
Added comprehensive sections:
- **Download Pre-built Binaries** - Instructions for end users
- **Flash using esptool.py** - Command-line flashing (Linux/Mac/Windows)
- **Flash using ESP Flash Download Tool** - GUI flashing for Windows
- **Release Process** - Guide for maintainers

## How to Create a Release

### Step 1: Update Version
Edit `platformio.ini`:
```ini
build_flags =
    -DAPP_VERSION=\"1.3.0\"  # Change this
```

### Step 2: Commit and Tag
```bash
git add platformio.ini
git commit -m "Bump version to v1.3.0"
git push

git tag -a v1.3.0 -m "Release v1.3.0 - Description of changes"
git push origin v1.3.0
```

### Step 3: Wait for Build
- GitHub Actions automatically starts building
- Takes ~3-5 minutes
- Check progress at: `https://github.com/chanpyaea/temp_monitor_project/actions`

### Step 4: Verify Release
- Go to: `https://github.com/chanpyaea/temp_monitor_project/releases`
- New release should contain:
  - `temp_monitor_v1.3.0.bin`
  - `bootloader.bin`
  - `partitions.bin`
  - `checksums.txt`
  - `FLASH_INSTRUCTIONS.md`

## Binary Locations After Build

When PlatformIO builds locally, binaries are in:
```
.pio/build/temp_monitor/
├── firmware.bin          # Main application
├── bootloader.bin        # ESP32 bootloader
└── partitions.bin        # Partition table
```

The workflow copies these to `release_files/` and renames the main firmware to include the version tag.

## Flashing Addresses

All ESP32 binaries must be flashed to specific addresses:
- `bootloader.bin` @ `0x1000`
- `partitions.bin` @ `0x8000`
- `temp_monitor_vX.X.X.bin` @ `0x10000`

## Checksum Verification

Each release includes `checksums.txt` with SHA256 hashes. Verify after download:

**Linux/Mac:**
```bash
sha256sum -c checksums.txt
```

**Windows (PowerShell):**
```powershell
Get-FileHash temp_monitor_v1.3.0.bin -Algorithm SHA256
```

## Troubleshooting

### Build Fails
- Check GitHub Actions logs
- Verify `platformio.ini` is valid
- Ensure all dependencies are listed in `lib_deps`

### Binaries Not Attached to Release
- Verify tag format is `vX.X.X` (e.g., `v1.2.0`)
- Check that workflow has `GITHUB_TOKEN` permissions (auto-granted)
- Look for errors in the "Upload Release Assets" step

### Wrong Binary Names
- Version in filename comes from git tag, not `APP_VERSION`
- Tag `v1.2.0` creates `temp_monitor_v1.2.0.bin`

## Manual Build Without Release

To test the build process locally:
```bash
pio run
ls -lh .pio/build/temp_monitor/*.bin
```

To trigger GitHub build without creating a release:
1. Go to Actions tab
2. Select "Build and Release Firmware"
3. Click "Run workflow"
4. Select branch and click "Run"

## Benefits

✅ **Consistent builds** - Same environment every time  
✅ **No local setup needed** - Users don't need PlatformIO  
✅ **Automatic checksums** - Verify file integrity  
✅ **Version tracking** - Clear mapping between code and binaries  
✅ **Easy rollback** - All versions available in Releases  
✅ **Professional appearance** - Proper release notes and instructions

## Next Steps (Optional Enhancements)

1. **Add changelog generation** - Auto-generate from commit messages
2. **Build multiple variants** - Debug/release builds, different board configs
3. **OTA binary packaging** - Generate OTA-compatible bins
4. **Automated testing** - Run unit tests before release
5. **Draft releases** - Review before publishing
6. **Release notifications** - Post to Discord/Slack when released

## Current Status

✅ Workflow created in correct directory  
✅ README updated with download instructions  
✅ Release process documented  
⏳ Ready for next tag push to trigger first automated build  

The v1.2.0 release already exists with binaries, so the automation is ready for the next version (v1.3.0 or later).
