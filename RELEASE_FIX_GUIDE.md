# GitHub Release Fix Guide

## Problem
You're creating git tags and building releases locally, but they're not appearing on GitHub releases.

## Root Cause
The GitHub Actions workflow (`.github/workflows/build-release.yml`) is configured to automatically build and upload releases when you push tags, but:
1. Either the workflow isn't running
2. Or you're building locally but not uploading to GitHub

## Solution Options

### Option 1: Use GitHub Actions (Recommended)

This is the cleanest approach - let GitHub Actions build and release automatically:

1. **Make your changes and commit them:**
   ```bash
   git add .
   git commit -m "Your changes"
   git push
   ```

2. **Create and push a tag:**
   ```bash
   git tag v1.3.0
   git push origin v1.3.0
   ```

3. **Wait for GitHub Actions to complete:**
   - Go to: https://github.com/chanpyaea/temp_monitor_project/actions
   - The workflow "Build and Release Firmware" should start automatically
   - It will build the firmware and create the GitHub release with all bin files

### Option 2: Upload Existing Local Files

If you already have the bin files built locally in `release/v1.2.0/`, use the upload script:

1. **Install GitHub CLI if not installed:**
   ```bash
   # Windows (using winget)
   winget install --id GitHub.cli

   # Or download from https://cli.github.com/
   ```

2. **Authenticate with GitHub:**
   ```bash
   gh auth login
   ```

3. **Upload your release:**
   ```bash
   bash upload_release.sh v1.2.0
   ```

### Option 3: Manual Upload via GitHub Web UI

1. Go to: https://github.com/chanpyaea/temp_monitor_project/releases/new
2. Select tag: `v1.2.0`
3. Drag and drop these files from `release/v1.2.0/`:
   - `temp_monitor_v1.2.0.bin`
   - `bootloader.bin`
   - `partitions.bin`
   - `checksums.txt`
   - `FLASH_INSTRUCTIONS.md`
4. Click "Publish release"

## Verify GitHub Actions is Working

Check if your workflows are running:

```bash
# View recent workflow runs
gh run list --workflow=build-release.yml

# View a specific run
gh run view <run-id>
```

## Common Issues

### Issue 1: Tags not pushed to remote
**Symptom:** Local tags exist but GitHub Actions doesn't run

**Fix:**
```bash
# Push all tags
git push origin --tags

# Or push a specific tag
git push origin v1.2.0
```

### Issue 2: Workflow didn't run
**Check:**
- Go to https://github.com/chanpyaea/temp_monitor_project/actions
- Look for workflow runs matching your tag
- If no runs, the tag push might have failed

### Issue 3: Workflow failed
**Check the logs:**
```bash
gh run list --workflow=build-release.yml
gh run view <failed-run-id> --log
```

Common failures:
- Missing `GITHUB_TOKEN` permission (should be automatic)
- Build errors in firmware
- Missing dependencies

## Next Steps

For your v1.2.0 release, I recommend:

1. **Upload the existing files:**
   ```bash
   bash upload_release.sh v1.2.0
   ```

2. **For future releases, use the automated workflow:**
   ```bash
   git tag v1.3.0
   git push origin v1.3.0
   # Wait for GitHub Actions to complete
   ```

## Checking Your Releases

View all releases:
```bash
gh release list
```

View a specific release:
```bash
gh release view v1.2.0
```

Open release in browser:
```bash
gh release view v1.2.0 --web
```
