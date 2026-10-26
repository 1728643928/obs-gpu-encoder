# Validation

## Automated

`node scripts/build.mjs --download-obs --tests` builds both native components and the protocol test. CI runs protocol roundtrips, malformed-message bounds and adapter filtering regression cases without requiring a GPU. Adapter tests cover physical/iGPU retention, display-only indirect aliases, software devices, unavailable queries and genuine identical-model dual cards.

On a machine with an AMD encoder, run the hardware harness from a terminal with the matching OBS bin/64bit on PATH:

```powershell
$env:PATH="C:\Program Files\obs-studio\bin\64bit;"+$env:PATH
build\worker-smoke.exe "$PWD\dist\obs-plugins\64bit\obs-gpu-encoder-worker.exe" "C:\Program Files\obs-studio\bin\64bit" 1
```

The final number is the DXGI adapter index. The harness probes real capabilities, encodes 300 1080p60 frames, verifies packet count, monotonic timestamps and keyframe count, then confirms a changed GPU identity is rejected. An optional final path saves a developer-only elementary stream for decoder validation; it is not a product recording feature.

backend-smoke.exe separately validates the in-process backend; worker-smoke.exe validates the actual isolated path.

## OBS acceptance

- Confirm startup log still names the intended rendering GPU.
- Confirm the encoder list contains only successful device/codec combinations.
- Select the iGPU H.264 entry; confirm localized properties, defaults and saved selection.
- Test OBS's actual encoder/output path, check video decode, timing, audio presence and final encoder logs.
- Reopen settings and restart OBS to confirm persistence.
- Verify failed initialization returns an error and does not use another GPU.
- Platform receiving and game performance require separate tests; do not infer them from a local encoder test.

Do not publish private recordings, OBS profiles, stream keys, full system logs or machine-specific dump files with bug reports.

## Installer and release validation

Run **node scripts/check-locales.mjs**, build with **--tests**, then **node scripts/build-installer.mjs** and **node scripts/test-installer.mjs**. The installer test accepts **--obs-root PATH** when using an existing OBS rather than build/_deps/obs-runtime. It installs/uninstalls disposable fixtures in four language modes, retains unrelated files, rejects invalid paths and checks exclusive-file locks. These are installation tests, not OBS runtime/ABI tests. Check the interactive four-language selector and full-scan start/stop separately. Hardware encoding remains outside hosted CI. Run **node scripts/package.mjs** only after all intended source files are tracked in Git.

## Enhanced broadcasting regression and hardware checks

The build now also runs **enhanced-output-test.exe** against the pinned OBS runtime. Its default mode uses real OBS output/encoder objects and graphics-free media fixtures: three tracks, per-track sizes and 60/30/20 fps, 160 Kbps lower rendition, recording isolation, shared enhanced dump, unsupported AV1, missing GPU, 10-bit, invalid settings and backend-initialization failure. Original encoder create callbacks must never run after a forced binding.

The optional hardware mode uses the actual OBS D3D11 renderer and the built plugin, with a packet-capturing output rather than a network stream:

~~~powershell
$env:PATH="E:\\obs-studio\\bin\\64bit;"+$env:PATH
build\\enhanced-output-test.exe --hardware "$PWD\\dist\\obs-plugins\\64bit\\obs-gpu-encoder.dll" "$PWD\\dist\\data\\obs-plugins\\obs-gpu-encoder" gpu_bridge_amf_1002_13c0_
~~~

Replace the prefix with the intended GPU's registered encoder-ID prefix. It starts three simultaneous H.264 renditions for five seconds, checks packet counts and strictly increasing per-track timestamps, checks that independent recording retains its original encoder, and verifies no original encoder was initialized. It does not contact Twitch or start a public broadcast.

For frontend acceptance, use the user's own OBS installation: confirm the Tools dialog, save the selected GPU in the active profile, keep Enhanced Broadcasting enabled, and check the **ENHANCED bound** and **START selected** log entries on a separately authorized live test. Public-platform negotiation and receiving are distinct from the local hardware harness.
