# Native encoder modules

The plugin and worker are separate Windows x64 binaries. Start with this file when changing encoder behavior.

| File | Responsibility / 职责 |
| --- | --- |
| [plugin.cpp](plugin.cpp) | OBS module entry point; registers GPU/codec encoder choices, properties, defaults and frame callbacks. / OBS 接口、编码器列表、参数和画面回调。 |
| [enhanced-frontend.cpp](enhanced-frontend.cpp) | Four-language Tools dialog, per-profile enhanced GPU selection and pre-start lifecycle integration. / 增强广播四语选卡设置及启动前回调。 |
| [enhanced-output.cpp](enhanced-output.cpp) | Validate and replace every live video encoder, preserve rendition metadata, synchronize groups and block fallback. / 全轨道替换、参数保留、同步与失败阻断。 |
| [enhanced-output.hpp](enhanced-output.hpp) | GPU/codec bindings and routing/frontend interfaces. / 增强广播绑定和接管接口。 |
| [backend.hpp](backend.hpp) | Shared adapter, codec, settings and packet types; the backend interface. / 显卡、格式、参数和数据包接口。 |
| [backend.cpp](backend.cpp) | DXGI discovery, device identity, D3D11 binding, AMF capability probes and encoding through OBS-provided FFmpeg DLLs. / 显卡枚举、硬件绑定、能力探测及实际编码。 |
| [adapter-policy.hpp](adapter-policy.hpp) | Filters software and display-only indirect adapters while retaining real same-model cards. / 过滤软件及间接显示设备，保留同型号实体显卡。 |
| [worker.cpp](worker.cpp) | Worker executable entry point; receives requests and operates its encoder instance. / 独立编码进程入口和请求处理。 |
| [worker-client.hpp](worker-client.hpp) | Interface used by the plugin to communicate with the worker. / 插件调用编码进程的接口。 |
| [worker-client.cpp](worker-client.cpp) | Starts and owns the worker, connects its local pipe, exchanges frames/packets, enforces timeouts and cleanup. / 进程管理、本地管道、数据交换、超时和清理。 |
| [protocol.hpp](protocol.hpp) | Versioned wire messages, serialization and bounds checks shared by both processes. / 两端共用的协议、序列化与边界检查。 |

## Frame path

OBS supplies NV12 frames → worker client sends them over a local pipe → worker binds the selected GPU → AMF returns encoded packets → OBS publishes the stream and handles audio.

The current backend is AMD AMF, SDR/NV12/8-bit. Intel/NVIDIA/HDR and cross-GPU zero-copy are not implemented. Preserve device-identity checks and explicit errors when extending it.

Read [architecture](../docs/architecture.md) before changing process boundaries and [tests](../tests/README.md) before changing the protocol or adapter policy.

## 简体中文

上表按实际职责区分文件。OBS 渲染与编码选卡相互独立，编码进程只返回编码数据包，音频和网络直播由 OBS 处理。修改协议需同时检查两端及协议测试；修改选卡需验证实际设备身份，避免初始化失败后切换到其他显卡。

## 日本語

表は OBS 接続、AMD バックエンド、ワーカー、通信プロトコルの役割を示します。音声と配信通信は OBS が担当します。変更時は明示的 GPU 指定、デバイス識別、通信の境界検証と終了処理を維持してください。

## 한국어

표는 OBS 연결, AMD 백엔드, 작업 프로세스와 통신 프로토콜의 역할을 구분합니다. 오디오와 방송 통신은 OBS가 담당합니다. 수정 시 명시적 GPU 지정, 장치 식별, 통신 경계 검사와 종료 처리를 유지하세요.
