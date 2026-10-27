# Validation record

## 0.3.2 Taiwan Mandarin and five-language distribution

2026-10-27, Windows 11 and the user's existing OBS Studio 33.0.0 x64 installation.

- The [release build](https://github.com/1728643928/obs-gpu-encoder/actions/runs/40425593678) passed compilation, protocol, adapter-policy, enhanced-binding and installer checks.
- Five plugin locale key sets and five installer message sets match. Placeholder parity and all five installer language declarations passed.
- The released installer passed install/uninstall fixtures in English, Simplified Chinese, Traditional Chinese (Taiwan), Japanese and Korean. All five locale files were installed; unrelated files survived uninstall. Invalid OBS roots and an exclusively locked target were rejected.
- The actual OBS zh-TW encoder settings, GPU popup and enhanced-broadcast dialog were inspected for correct glyphs and clipping. Three bounded native screenshots use the new plugin resource and OBS's upstream zh-TW controls. Daily-use profiles, scenes and language were unchanged; the account-free capture configuration was removed.
- The [published 0.3.2 installer](https://github.com/1728643928/obs-gpu-encoder/releases/tag/v0.3.2), verified against its SHA-256 manifest, showed all five language choices including 繁體中文（台灣）. Its Taiwan discovery page detected the real OBS root and displayed the optional scan controls. The installer was cancelled before copying files; this UI check did not start a full-drive scan.
- Four matching Taiwan UI captures are stored alongside the original four-language set. Their provenance is recorded in [assets/readme/README.md](../assets/readme/README.md#taiwan-mandarin-additions-in-032).

This release adds localization and documentation, not broader GPU, codec or public-platform validation. Historical records below retain their original version and language counts.

## 0.1.0-preview

2026-10-25, Windows 11, OBS Studio 33.0.0 x64.

- OBS rendering adapter: AMD Radeon RX 9060 XT (DXGI 0).
- Encoding adapter: AMD Radeon(TM) Graphics (DXGI 1, PCI 1002:13c0).
- AMD iGPU capability probes: H.264 and HEVC passed; AV1 rejected. RX 9060 XT probes: H.264, HEVC and AV1 passed.
- Direct and isolated-worker H.264 harnesses: 300 input frames / 300 output packets at 1920×1080, 60 fps, with 3 keyframes over 5 seconds. Elementary streams decoded successfully.
- Changed GPU LUID was rejected by the worker rather than selecting another adapter.
- Protocol tests passed UTF-8 and settings roundtrip, truncated input, oversized packet count and trailing-data rejection.
- OBS native Start Streaming → local RTMP receiver: 2842 H.264 frames received over approximately 47.35 seconds at 1080p60. AAC packets were present. Video and audio timestamps were strictly increasing. All 23 observed keyframe intervals were 2000 ms. Receiver and full audio/video decoder exited with code 0 and no decoder errors.
- OBS startup log retained the RX 9060 XT rendering device; encoder log selected the iGPU explicitly.
- OBS output settings displayed the native iGPU entry and localized controls. A separate preview profile preserves the original platform configuration. Twitch Enhanced Broadcasting was disabled only in the preview so it cannot replace the selected encoder.

Public-platform receiving, gaming performance, HDR, 10-bit, Intel, NVIDIA and other OBS versions were not validated. HEVC/AV1 received capability probes, not the same full OBS streaming acceptance as iGPU H.264. Audio was decoded but not auditioned. The RTMP test recording and private OBS profiles are excluded from distributed artifacts.

## 0.1.1 adapter filtering regression

The duplicate RX 9060 XT entry at DXGI index 2 had D3DKMT type 0x342 (IndirectDisplayDevice=1, RenderSupported=0) and no valid PCI address. It is a display-only indirect adapter rather than a second physical rendering GPU. The physical RX 9060 XT at DXGI 0 and iGPU at DXGI 1 were retained.

Adapter-policy regression tests passed, including preserving two real identical-model cards and retaining unknown-query adapters. The fixed worker enumerated exactly the two actual rendering GPUs and again encoded 300/300 iGPU H.264 frames with 3 keyframes.

## 0.2.0 four-language installer and public release build

- Four OBS locale key sets and installer message key sets match; referenced messages have translations.
- Plugin/worker compiled; protocol and adapter-policy tests passed.
- 9700X iGPU H.264: 300 submitted frames, 300 packets, 3 keyframes; altered LUID rejected. Exactly one physical RX 9060 XT and one iGPU were enumerated.
- Release installer ran in four language modes against disposable Unicode/space path fixtures. Plugin DLL, worker and all four locale files installed; uninstall preserved an unrelated plugin and an unrecorded user file. A real OBS executable supplied the version resource; fixture DLL placeholders were never loaded.
- Invalid root and a target file held with an exclusive Win32 handle were rejected before plugin files were copied.
- The /DIR command-line regression preserves the explicitly selected target rather than replacing it with a detected installation.
- Interactive installer showed all four language options. Japanese discovery UI detected the real OBS installation, scanned more than 24,000 directories across local fixed drives and stopped on request with navigation re-enabled. This checked start/cancel behavior; it was not an exhaustive completion of every drive. The UI test used the identical wizard script with the privilege requirement lowered for the test copy only.

## 0.3.0 enhanced broadcasting GPU binding

2026-10-27, Windows 11 and the user's OBS Studio 33.0.0 runtime.

- Build, protocol and adapter-policy checks passed. The new graphics-free OBS regression verified three-track replacement, frame-rate preservation without double division, low-bitrate rendition settings, independent recording isolation and shared enhanced-stream dump.
- Invalid plans are blocked before stream start: unsupported AV1 on an H.264-only test binding, missing saved GPU, 10-bit input, invalid bitrate and simulated backend initialization failure. Original encoder callbacks did not run as a fallback.
- A separate local hardware harness used the actual OBS D3D11 renderer on RX 9060 XT and three real AMF Bridge encoders on the Ryzen 7 9700X iGPU: 1920×1080 at 60 fps / 6000 Kbps, 1280×720 at 30 fps / 2500 Kbps, and 640×360 at 20 fps / 160 Kbps.
- In a five-second run, the capturing output received 298, 149 and 99 packets respectively, with strictly increasing timestamps per track. Start logs bound all three workers to PCI 1002:13c0; the original encoder was never initialized. The process exited successfully after cleanup. Packet totals reflect group startup and output shutdown, not a fixed input-frame benchmark.
- Four-language installer fixture checks passed, including required frontend API dependency, invalid paths, occupied files and retention of unrelated files.

These checks demonstrate local multi-stream routing and encoding. They do not establish successful public Twitch negotiation/receiving, a gaming-performance improvement, or every iGPU's simultaneous-encoding capacity. Account eligibility and platform-returned codecs still apply.

The 0.3.0 plugin was then installed into the user's existing OBS installation. First-use settings with absent profile keys, selecting the iGPU, saving and reopening were verified through the actual UI. The current profile was saved with forced iGPU binding and Twitch Enhanced Broadcasting enabled. No public stream was started for this check.
