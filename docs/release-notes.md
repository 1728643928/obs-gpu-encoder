## English

- New per-profile **Enhanced Broadcasting GPU** setting under **Tools → GPU Encoder Bridge: Enhanced Broadcasting**.
- Keep Enhanced Broadcasting enabled while binding every prepared live video encoder to the selected AMD GPU, even when OBS's usual encoder dropdown is locked.
- Preserve codec, resolution, frame rate, bitrate and keyframe interval; synchronize video tracks and keep audio/network handling in OBS.
- Independent recording choices remain unchanged. OBS's own enhanced-stream dump follows its shared live encoders.
- Reject missing GPUs, unsupported codec/HDR plans and initialization failures without an alternate-GPU or software fallback.
- Four-language plugin/installer/docs, Windows EXE and full source, with upstream references and independent fork Releases.

Preview: Windows x64, OBS 33.x, AMD AMF, SDR/NV12/8-bit. Local regression and three simultaneous 9700X iGPU H.264 streams were validated; public Twitch negotiation/receiving and gaming performance were not. Negotiation still uses OBS's reported hardware capabilities; an unsupported returned plan fails. Dynamic bitrate reconfiguration and zero-copy are not implemented. EXE is unsigned. See docs/enhanced-broadcasting.md and docs/validation.md.

## 简体中文

- 新增按 OBS 配置文件保存的增强广播编码显卡设置，入口：**工具 → GPU Encoder Bridge：增强广播**。
- 保留增强广播开启，在普通编码器下拉框锁定时，也可将全部直播视频轨道绑定到指定 AMD 显卡。
- 保留编码格式、分辨率、帧率、码率和关键帧间隔，多路同步启动；音频与网络仍由 OBS 管理。
- 独立录制的编码器选项和设置保持独立；增强广播自带的直播副本跟随直播编码。
- 显卡缺失、格式/HDR 不支持或初始化失败时明确阻止启动，不自动改用独显或软件编码。
- 保留英中日韩四语、EXE 安装器、完整源码、来源署名与 Fork 独立发版能力。

预览范围：Windows x64、OBS 33.x、AMD AMF、SDR/NV12/8 位。已验证本地回归与 9700X 核显三路 H.264 同时编码，尚未验证公网 Twitch 协商、接收及游戏性能。平台协商仍使用 OBS 上报的硬件能力，不支持的返回方案会报错。暂不支持动态码率重配置和零拷贝。EXE 未签名，详见增强广播说明和验证记录。

## 日本語

- **ツール → GPU Encoder Bridge：拡張配信**に、プロファイルごとのエンコード GPU 設定を追加。
- 拡張配信を有効にしたまま、通常の選択欄が無効でも全配信映像トラックを指定 AMD GPU に固定。
- コーデック、解像度、フレームレート、ビットレート、キーフレーム間隔と同期起動を維持。音声・通信は OBS が担当。
- 独立した録画設定は変更せず、拡張配信のストリーム保存は配信エンコーダーを共有。
- GPU 不在、非対応コーデック/HDR、初期化失敗時は配信を中止し、別 GPU やソフトウェアへ自動変更しない。
- 4 言語、EXE、完全ソース、出典表示と Fork の独立公開を維持。

Windows x64 / OBS 33.x / AMD AMF / SDR・NV12・8 ビットのプレビューです。ローカル回帰と 9700X 内蔵 GPU の H.264 同時 3 系統を検証しました。公開 Twitch の交渉・受信とゲーム性能は未検証です。交渉は OBS のハードウェア情報を使い、非対応の構成は失敗します。動的ビットレート再設定とゼロコピーは未実装、EXE は未署名です。

## 한국어

- **도구 → GPU Encoder Bridge: 향상된 방송**에 프로필별 인코딩 GPU 설정 추가.
- 향상된 방송을 유지하며 일반 선택 상자가 잠겨 있어도 모든 방송 영상 트랙을 선택한 AMD GPU로 고정.
- 코덱, 해상도, 프레임레이트, 비트레이트, 키프레임 간격과 동기 시작 유지. 오디오와 통신은 OBS가 담당.
- 독립 녹화 설정은 변경하지 않고 향상된 방송의 스트림 저장은 방송 인코더를 공유.
- GPU 없음, 지원하지 않는 코덱/HDR 또는 초기화 실패 시 시작을 차단하며 다른 GPU나 소프트웨어로 자동 전환하지 않음.
- 네 언어, EXE, 전체 소스, 출처 표시와 Fork 독립 릴리스 유지.

Windows x64 / OBS 33.x / AMD AMF / SDR·NV12·8비트 시험판입니다. 로컬 회귀 검사와 9700X 내장 GPU의 H.264 동시 3개 스트림을 검증했습니다. 공개 Twitch 협상·수신과 게임 성능은 미검증입니다. 협상은 OBS의 하드웨어 정보를 사용하며 지원하지 않는 구성은 실패합니다. 동적 비트레이트 재설정과 제로 카피는 미구현이며 EXE는 미서명입니다.
