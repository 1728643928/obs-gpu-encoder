# GPU Encoder Bridge for OBS

Choose the **encoding GPU by its actual device name** while OBS keeps rendering on another GPU. Keep using OBS's native streaming service, audio, Start Streaming and Stop Streaming controls.

**0.2.0-preview · Windows x64 · OBS Studio 33.x · AMD AMF · SDR / 8-bit**

[English](README.md) · [简体中文](docs/README.zh-CN.md) · [日本語](docs/README.ja-JP.md) · [한국어](docs/README.ko-KR.md)

[Downloads / Releases](https://github.com/1728643928/obs-gpu-encoder/releases) · [Report an issue](https://github.com/1728643928/obs-gpu-encoder/issues) · [Contribute](CONTRIBUTING.md)

## What it does

- Adds native OBS encoder entries such as **AMD Radeon(TM) Graphics — H.264 [GPU 1 / AMF Bridge]**.
- Probes each AMD GPU before offering H.264, HEVC or AV1. Available codecs depend on hardware, drivers and the selected streaming service.
- Excludes software and display-only indirect adapters without merging real identical-model GPUs.
- Offers CBR / VBR, bitrate, keyframe interval and speed / balanced / quality presets.
- Uses an internal worker for explicit GPU binding and AMF isolation. No separate control application or FFmpeg command-line program is needed.
- Provides English, Simplified Chinese, Japanese and Korean plugin controls, installer UI and getting-started documentation.

Windows GPU preferences, display cables and the OBS rendering GPU do not need to change. The plugin does not handle accounts or stream keys. OBS handles publishing and audio.

## Install

1. Close OBS and download the **windows-x64-setup.exe** asset from Releases.
2. Select English, 简体中文, 日本語 or 한국어.
3. Select a detected OBS installation. You can optionally scan **all local fixed drives**, stop the scan at any time, or browse manually.
4. Confirm the OBS root folder. The installer checks for OBS 33.x x64 and the required OBS/FFmpeg DLL layout before copying files.
5. Start OBS, open **Settings → Output → Advanced → Streaming**, and choose the GPU-named **AMF Bridge** encoder.

The scan enumerates directories; it does not read personal document contents or upload files. Inaccessible folders and reparse points are skipped. Network/removable drives and volumes mounted only through reparse points are outside full-scan scope; use manual browsing for those locations. A large disk can take time to scan. The default installer requests administrator access for protected OBS installations; portable installations can use **/CURRENTUSER**.

Manual ZIP installation is also available: merge the ZIP's **obs-plugins** and **data** directories into the OBS root. Do not replace OBS's own DLLs. Keep both plugin DLL and worker EXE together.

## Stream

Choose your platform in OBS as usual. A starting point is **H.264, CBR, 6000 Kbps, 2-second keyframes, Balanced**; adjust bitrate to the platform's requirements. OBS may filter codecs based on the streaming service. If Twitch Enhanced Broadcasting is enabled, disable it when you need your chosen encoder to remain in control.

The GPU number is the current DXGI index, not a permanent physical slot. Selection IDs use hardware identifiers and matching-device order; runtime LUIDs are checked again to avoid selecting the wrong GPU. Recheck your selection after hardware changes. Errors never silently switch to another GPU or software encoding.

## Current limits and evidence

- **AMD only**. Intel QSV and NVIDIA NVENC are extension work, not implemented features.
- SDR / NV12 / 8-bit. HDR / 10-bit are rejected.
- Frames move through system memory to the selected GPU; this is not cross-GPU zero-copy. There is no claimed gaming-performance improvement without measurement.
- Built against OBS 33 / avcodec 62 / avutil 60. Other ABI versions are unsupported by this preview.
- Tested on RX 9060 XT rendering with Ryzen 7 9700X integrated graphics encoding H.264. A previous local OBS RTMP acceptance run received and decoded video/audio for approximately 47 seconds. The 0.2.0 worker passed another 300-frame H.264 test. HEVC/AV1 have capability probes, not the same full streaming acceptance.
- Public-platform receiving, gaming benchmarks, HDR, other vendors and other OBS versions have not been validated. This is a **preview**, not a production-stability claim.

See [architecture](docs/architecture.md), [testing](docs/testing.md), [validation record](docs/validation.md) and [installer design](docs/installer.md).

## Build from complete source

Requirements: Windows x64, Node.js 22+, Git. No Visual Studio or private SDK is required.

~~~powershell
node scripts/check-locales.mjs
node scripts/build.mjs --download-obs --tests
node scripts/build-installer.mjs
node scripts/package.mjs
~~~

For an existing OBS installation, replace **--download-obs** with **--obs-root "C:\Program Files\obs-studio"**. Pinned toolchain/source downloads are SHA-256 verified. The installer compiler is official Inno Setup 6.7.3, also checksum verified. Build outputs go to **dist/** and release assets to **release/**. Build scripts do not install the plugin or change OBS profiles. See [release and maintainer instructions](docs/releasing.md).

## Uninstall

Close OBS. Use the GPU Encoder Bridge entry in Windows Installed Apps, or run **data/obs-plugins/obs-gpu-encoder/uninstall/unins000.exe** under the target OBS root. Only the installer's recorded plugin files are removed. OBS, profiles and other plugins are retained. For manual ZIP installs, remove this plugin's DLL, worker and its data directory after choosing another encoder in OBS.

## Fully open development

All project code, installer source, translations, tests and workflows are available under **GPL-2.0-or-later**, with third-party notices preserved. There is no closed paid component, CLA, private build service, telemetry or project-controlled online activation.

Anyone may fork, create branches, modify and redistribute the project under the license, and publish their own Releases. Fork builds use their own repository's short-lived GitHub Actions token; no maintainer's personal token is needed. Publishing Releases in the upstream repository requires write access granted by its owner. The license cannot grant upstream account permissions.

Read [CONTRIBUTING](CONTRIBUTING.md), [GOVERNANCE](GOVERNANCE.md), [handover and release instructions](docs/releasing.md) and [third-party notices](THIRD-PARTY.md). No stream keys, private profiles, personal logs, driver binaries or test recordings belong in this repository.
