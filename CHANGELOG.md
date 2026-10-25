# Changelog

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
