# Enhanced Broadcasting GPU binding

Available in **0.3.0-preview**, on Windows x64 with OBS Studio 33.x.

## Use your selected GPU

1. Stop streaming and open **Tools → GPU Encoder Bridge: Enhanced Broadcasting**.
2. Enable **Force enhanced-broadcast video encoding onto the selected GPU**, choose the actual GPU name, and save.
3. Keep **Enhanced Broadcasting** enabled in OBS's Stream settings. Use OBS's normal Start Streaming button.

The setting is saved per OBS profile and is off by default. It does not change Windows GPU preferences or the OBS rendering adapter. The ordinary Streaming encoder dropdown may remain locked while Enhanced Broadcasting is on; the Tools setting controls the actual video encoders created for that mode.

Select another GPU explicitly after hardware changes. A missing saved adapter is not replaced automatically. Stop streaming before changing the setting.

## What happens at stream start

OBS first negotiates the platform's enhanced-broadcast plan. Its public **STREAMING_STARTING** event then runs after preparation and before output start. The plugin obtains the prepared streaming output, creates GPU-bound replacements for every video track, and attaches them before OBS initializes the encoders.

Each replacement retains the codec, dimensions, frame-rate divisor, bitrate, keyframe interval, scale type and SDR color/range selection. Rate-control names are translated to the backend's CBR/VBR values. Vendor-specific preset names are not portable; the bridge uses its Balanced preset when no compatible preset is present.

All replacements share an OBS encoder group for synchronized startup. Parent video sources are used so a 30 fps rendition from a 60 fps canvas is not divided a second time.

Audio, platform authentication, negotiation, network publishing and reconnects remain with OBS. No proxy, new network listener, platform endpoint override, injected hook or modified OBS executable is used.

## Recording

Independent recording encoder choices and settings are untouched. The Recording encoder list continues to contain all registered GPU/codec combinations, without the streaming platform's codec filter.

OBS's optional enhanced-stream dump deliberately shares the live encoders. Only the internally named MP4/FLV multitrack dump outputs that share the original video encoders follow the replacements. An independently configured recording output is never rewritten.

Independent outputs created by a third-party multistream plugin are outside this binding. Outputs that explicitly reuse the main stream follow that plugin's own sharing behavior.

## Failure and current limits

If the selected GPU is missing, any negotiated codec is unsupported, the plan requests HDR/10-bit, or a replacement cannot be attached, the plugin blocks video initialization and reports an error. It does not keep an original encoder on another GPU or silently fall back to software. Worker initialization/encoding failures also fail explicitly.

Negotiation still uses OBS's reported system capabilities. This feature binds the plan returned by the platform; it does not rewrite the request or renegotiate codecs. An AV1 plan cannot be forced onto an H.264/HEVC-only iGPU. The number and workload of simultaneous tracks must also fit the chosen hardware.

The current backend remains SDR/NV12/8-bit AMD AMF with system-memory frame transfer. Dynamic bitrate reconfiguration and cross-GPU zero-copy are not implemented. Availability on other OBS versions and successful receiving by a public platform require separate validation.

## Implementation and references

- [enhanced-frontend.cpp](../src/enhanced-frontend.cpp): four-language Tools dialog, per-profile preferences and lifecycle callback.
- [enhanced-output.cpp](../src/enhanced-output.cpp): atomic plan validation, replacement encoders, companion stream dump and failure guard.
- [Regression and hardware harness](../tests/enhanced-output-test.cpp).
- OBS 33 sources: [pre-start event timing](https://github.com/obsproject/obs-studio/blob/33.0.0/frontend/widgets/OBSBasic_Streaming.cpp), [prepared streaming-output API](https://github.com/obsproject/obs-studio/blob/33.0.0/frontend/OBSStudioAPI.cpp), [multitrack output](https://github.com/obsproject/obs-studio/blob/33.0.0/frontend/utility/MultitrackVideoOutput.cpp) and [public encoder/output APIs](https://github.com/obsproject/obs-studio/blob/33.0.0/libobs/obs.h).

These OBS interfaces and implementation references are credited in [THIRD-PARTY.md](../THIRD-PARTY.md). See [validation](validation.md) for completed checks and their limits.
