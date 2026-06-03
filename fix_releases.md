# Complete Release Fix - Temperature Monitor Project

## Current Status (as of 2026-06-03)

✅ **What's Working:**
- Git tags exist locally: v1.1.1, v1.2.0
- Tags are pushed to GitHub remote
- v1.2.0 release exists on GitHub with 2 assets
- Bin files built locally in `release/v1.2.0/`

❌ **What's NOT Working:**
- GitHub Actions workflow has **0 runs** (never executed)
- The workflow should auto-build and upload releases when tags are pushed
- Only 2 assets on GitHub release (should have 5: bootloader.bin, partitions.bin, firmware.bin, checksums.txt, FLASH_INSTRUCTIONS.md)

## Root Cause

The GitHub Actions workflow exists at `.github/workflows/build-release.yml` but has never been triggered. This could be due to:

1. **GitHub Actions not enabled** in repository settings
2. **Workflow permissions** not configured correctly
3. **Tags created before the workflow file** was added to the repository
4. **Workflow file not on main branch** when tags were pushed

## The Fix

### Step 1: Verify Workflow is on Main Branch

```bash
# Check if workflow exists on main
git checkout main
git pull origin main
ls -la .github/workflows/build-release.yml
```

If the file doesn't exist on main, that's your problem! The workflow needs to be on the default branch.

### Step 2: Enable GitHub Actions (if disabled)

1. Go to: https://github.com/chanpyaea/temp_monitor_project/settings/actions
2. Under "Actions permissions", select:
   - ✅ "Allow all actions and reusable workflows"
3. Under "Workflow permissions", select:
   - ✅ "Read and write permissions"
   - ✅ "Allow GitHub Actions to create and approve pull requests"
4. Click **Save**

### Step 3: Test the Workflow Manually

```bash
# Trigger the workflow manually to test it
# You'll need to add workflow_dispatch trigger (already present in your workflow)

# Option A: Via GitHub Web UI
# 1. Go to: https://github.com/chanpyaea/temp_monitor_project/actions
# 2. Click "Build and Release Firmware" workflow
# 3. Click "Run workflow" button
# 4. Select main branch and click "Run workflow"

# Option B: Via GitHub CLI (after installing gh)
gh workflow run build-release.yml --ref main
```

### Step 4: Create a Test Tag to Verify Auto-Trigger

```bash
# Create a test tag and push it
git tag v1.2.1-test
git push origin v1.2.1-test

# Then check if workflow runs
# Go to: https://github.com/chanpyaea/temp_monitor_project/actions
# You should see a new workflow run appear
```

### Step 5: Fix Missing Assets in v1.2.0

Your v1.2.0 release only has 2 assets but should have 5. Upload the missing files:

**Option A: Using the Upload Script**
```bash
# First install GitHub CLI
# Download from: https://cli.github.com/
# Or Windows: winget install --id GitHub.cli

# Authenticate
gh auth login

# Upload missing files
bash upload_release.sh v1.2.0
```

**Option B: Manual Upload via Web**
1. Go to: https://github.com/chanpyaea/temp_monitor_project/releases/tag/v1.2.0
2. Click "Edit release" (pencil icon)
3. Drag and drop these files from `release/v1.2.0/`:
   - `temp_monitor_v1.2.0.bin`
   - `bootloader.bin`
   - `partitions.bin`
   - `checksums.txt`
   - `FLASH_INSTRUCTIONS.md`
4. Click "Update release"

## For Future Releases (Automated Way)

Once GitHub Actions is working properly:

```bash
# 1. Make your code changes
git add .
git commit -m "feat: Add new feature"
git push origin main

# 2. Create and push a tag (this triggers the workflow)
git tag v1.3.0
git push origin v1.3.0

# 3. Wait ~2-5 minutes for GitHub Actions to:
#    - Build the firmware
#    - Generate all bin files
#    - Create checksums
#    - Create GitHub release automatically
#    - Upload all 5 files as release assets

# 4. Verify the release
# Go to: https://github.com/chanpyaea/temp_monitor_project/releases
# You should see v1.3.0 with all 5 assets
```

## Troubleshooting

### Problem: Workflow still not running after pushing tag

**Check 1: Is the workflow file on main?**
```bash
git checkout main
git log --oneline --all --graph | grep -i workflow
```

**Check 2: Are Actions enabled?**
- Visit: https://github.com/chanpyaea/temp_monitor_project/settings/actions
- Ensure Actions are enabled

**Check 3: Does the tag match the pattern?**
- Workflow triggers on: `v*.*.*` (e.g., v1.2.0, v2.0.0)
- Won't trigger on: `1.2.0`, `release-1.2.0`, `ver1.2.0`

### Problem: Workflow runs but fails

**View the error logs:**
```bash
# List recent runs
gh run list --workflow=build-release.yml

# View specific run details
gh run view <run-id> --log
```

**Common issues:**
- **Build fails:** Check if PlatformIO can build locally: `pio run`
- **Upload fails:** Check workflow permissions (needs write access)
- **Missing files:** Check if `.pio/build/temp_monitor/` has the bin files after build

### Problem: Local build doesn't produce bin files

```bash
# Clean and rebuild
pio run --target clean
pio run

# Check output
ls -la .pio/build/temp_monitor/
```

Expected files:
- `firmware.bin` (main firmware)
- `bootloader.bin` 
- `partitions.bin`

## Quick Commands Reference

```bash
# View all tags
git tag -l

# View tags on remote
git ls-remote --tags origin

# Push all local tags to remote
git push origin --tags

# Delete a tag locally
git tag -d v1.2.0

# Delete a tag on remote
git push origin --delete v1.2.0

# Create annotated tag (better for releases)
git tag -a v1.3.0 -m "Release v1.3.0: Add new features"
git push origin v1.3.0

# View release on GitHub
# https://github.com/chanpyaea/temp_monitor_project/releases/tag/v1.3.0
```

## Summary

**For v1.2.0 (already released):**
- Upload the missing bin files manually (Option A or B in Step 5)

**For future releases:**
1. Enable GitHub Actions in repository settings
2. Ensure workflow file is on main branch
3. Just push tags: `git tag v1.x.x && git push origin v1.x.x`
4. Let GitHub Actions do everything automatically

**Next Action:** 
Start with Step 2 (Enable GitHub Actions) to ensure the workflow can run in the future.
