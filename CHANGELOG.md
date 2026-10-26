# Changelog

## Unreleased

## 0.3.1

- Replace shared screenshots with 16 native captures matching all four README languages.
- Document screenshot provenance and preserve the actual installation's daily-use configuration.
- Publish a regular release and explicitly mark regular versions as Latest only after every asset uploads; keep prereleases separate.
- Keep the encoder implementation and Enhanced Broadcasting experimental validation scope unchanged from 0.3.0-preview.

## 0.3.0-preview

- Add per-profile Enhanced Broadcasting GPU selection through the Tools menu.
- Bind every prepared video track before initialization; preserve rendition settings, synchronization and the shared enhanced-stream dump while keeping independent recording unchanged.
- Block missing/unsupported GPUs and initialization failures without alternate-GPU fallback.
- Add real-OBS routing regressions and a simultaneous three-track iGPU hardware harness.
- Validate the frontend API dependency during installation and fix Windows source-header extraction.

- Credit implementation references, dependencies, reused installer translation and README design guidance on all four homepages; document exact sources and future attribution rules.

- Add real OBS GPU/codec selection, iGPU settings and installer-discovery screenshots with four-language captions.

- Redesign the four-language README with original GPU-role artwork, direct installer links and clearly scoped validation results.

- Add file-by-file guides for native source, installer, scripts, tests, locale resources, CI and third-party notices.
- Add repository maps to the English, Chinese, Japanese and Korean getting-started guides.
- Add module navigation and contribution/handover entry points.
- Earlier documentation-only commits left the 0.2.0 release assets unchanged.

## 0.2.0-preview

- Four plugin and installer languages, English-first homepage and translated getting-started docs.
- EXE installer: common-path discovery, cancellable fixed-disk scan, manual target, version/occupancy checks and scoped uninstall.
- Public complete-source packaging and fork-capable build/release workflow.
- Four-language install/uninstall, invalid target and occupied file regression checks.

## 0.1.1-preview

- Filter display-only indirect adapters using D3DKMT device flags, fixing duplicate RX 9060 XT entries caused by a virtual display alias.
- Preserve separate physical cards even when their model names match.
- Add regression tests for adapter filtering and unavailable kernel queries.

## 0.1.0-preview

- Native OBS encoder types named by GPU and codec.
- AMD AMF H.264, HEVC and AV1 capability probing.
- Explicit independent encoding GPU selection.
- Local worker isolation, bounded IPC and owned worker cleanup.
- CBR / VBR, bitrate, keyframe interval and quality controls.
- English / Simplified Chinese properties.
- Fixed-version portable build, protocol tests and hardware smoke harnesses.
