# Architecture

OBS adapter selection and encoder device selection are independent.

1. The OBS plugin starts a local capability worker. The worker enumerates DXGI adapters, filters software and display-only indirect adapters through D3DKMT_ADAPTERTYPE, and attempts short real encoding sessions for each supported AMD codec. Unknown kernel query results are retained; identical model names are never merged.
2. Successful pairs become native OBS encoder types with a readable device name. Stable type IDs use PCI identifiers and the ordinal of matching hardware; fresh runtime LUIDs validate the device again.
3. OBS supplies NV12 frames through its raw-frame encoder API. The plugin forwards them over a bounded, versioned, local named pipe to a hidden worker.
4. The worker creates a D3D11 device on the explicitly selected adapter and attaches that device to FFmpeg's AMF encoder. Encoded packets and codec headers return to OBS.
5. OBS handles AAC, timestamps, service compatibility, RTMP(S), streaming controls and reconnects.

The worker isolates AMF initialization and driver state from OBS and other plugins. Direct in-process AMF initialization failed to offer the iGPU in the development environment although identical standalone encoding succeeded; the worker path is used consistently rather than silently changing backends.

## Boundaries

- No platform endpoints, passwords, stream keys or OBS profile files are read by the plugin.
- No network listener or network publishing code exists in the worker.
- The named pipe rejects remote clients. A preconnected client handle is inherited through an explicit handle allowlist.
- IPC validates magic, lengths, operation kind, packet counts, frame sizes and device identity.
- Frames are synchronous and ordered. I/O has timeouts; excessive packet queues fail explicitly. This favors correctness over pipelined maximum throughput.
- A kill-on-close job (where available), owned process handle and pipe EOF clean up workers.
- There is no software encoder or alternate-GPU fallback.
- HDR is rejected, preventing accidental unsupported transfer/color handling.
- The preview does not implement dynamic bitrate updates, ROI, multitrack optimization or zero-copy textures.

## Extension points

backend.hpp separates adapter discovery, codec descriptions, settings and encoded packets. Additional vendor backends should keep explicit device binding and capability tests. Avoid presenting all codecs solely from vendor/model names. New settings require matching worker protocol validation and OBS properties/defaults.

protocol.hpp is a private versioned Windows-x64 wire format. Change its magic/version when changing incompatible layouts. Worker and plugin must come from the same release.
