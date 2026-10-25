# GPU Encoder Bridge for OBS

按显卡真实名称选择编码设备，让 OBS 继续在另一张显卡上渲染。沿用 OBS 原生的直播平台、音频及开始/停止直播按钮。

**0.2.0-preview · Windows x64 · OBS Studio 33.x · AMD AMF · SDR / 8 位**

[English](../README.md) · [简体中文](README.zh-CN.md) · [日本語](README.ja-JP.md) · [한국어](README.ko-KR.md)

[下载与 Releases](https://github.com/1728643928/obs-gpu-encoder/releases) · [问题反馈](https://github.com/1728643928/obs-gpu-encoder/issues)

## 功能

- 在 OBS 原生编码器列表中显示显卡名与格式，如 **AMD Radeon(TM) Graphics — H.264 [GPU 1 / AMF Bridge]**。
- 实际探测 AMD 显卡编码能力，成功后才提供 H.264、HEVC 或 AV1。直播平台也会限制可选格式。
- 排除软件显卡及仅用于间接显示的虚拟设备；真实同型号双卡仍分别保留。
- 支持 CBR / VBR、码率、关键帧间隔和速度/均衡/质量预设。
- 内部编码进程隔离 AMF 初始化，无需额外控制软件或 FFmpeg 命令行程序。
- 插件、安装器和入门说明提供英、中、日、韩四种语言。

无需改 Windows 节能显卡选项、显示器插线或 OBS 渲染显卡。插件不读取平台账号或推流密钥，音频与网络推流由 OBS 处理。

## 安装与使用

1. 关闭 OBS，在 Releases 下载 **windows-x64-setup.exe**。
2. 选择安装器语言，选择发现的 OBS 位置。可选全盘扫描，随时停止；也可手动浏览。
3. 选择 OBS 根目录。安装器检查 OBS 33.x x64 版本及必需 DLL 布局。
4. 打开 OBS → 设置 → 输出 → 高级 → 直播，选择显卡名称对应的 **AMF Bridge** 编码器。
5. 在 OBS 原有直播页面选择平台，照常点击开始直播。

全盘扫描查找本地固定磁盘目录，不读取个人文档内容、不上传文件。无权限目录及重解析点/链接会跳过，网络盘、可移动盘和仅通过挂载链接访问的卷请手动浏览。磁盘较大时扫描可能较慢。默认安装器为受保护目录申请管理员权限，便携版可使用 **/CURRENTUSER**。

也可下载手动安装 ZIP，将 **obs-plugins** 与 **data** 合并到 OBS 根目录。DLL 与 worker EXE 必须一起安装，不要覆盖 OBS 自带 DLL。

直播起点：**H.264、CBR、6000 Kbps、关键帧 2 秒、均衡**，实际码率请按平台要求调整。Twitch 的 Enhanced Broadcasting 可能覆盖编码器选择，需要固定显卡时请关闭该功能。GPU 编号是当前 DXGI 枚举编号，硬件变更后应重新检查；失败不会悄悄改用其他显卡或软件编码。

## 当前范围与验证

目前仅实现 AMD AMF；Intel QSV、NVIDIA NVENC、HDR 和 10 位尚未实现。只支持 SDR / NV12 / 8 位，依赖 OBS 33 / avcodec 62 / avutil 60。原始画面通过系统内存传输，不是跨卡零拷贝，也没有未经实测的游戏性能保证。

已在 RX 9060 XT 渲染、9700X 核显 H.264 编码环境验证。此前原生 OBS 本地 RTMP 测试约 47 秒，视频与音频接收解码成功；0.2.0 再次通过 300 帧编码测试。HEVC/AV1 仅有能力探测，未完成同等级直播验收；公网平台接收、游戏性能和其他显卡/OBS 版本尚未验证。此版本仍为预览版。

## 目录与文件职责

GitHub 文件列表中间一列显示的是**最近一次提交说明**，不是每个文件的自定义介绍；同一次提交涉及多个文件时，说明会相同。实际职责请对照此表或进入目录查看 README。

| 目录 | 内容与入口 |
| --- | --- |
| [.github/](../.github/README.md) | 云端构建、测试、发版工作流和问题模板。 |
| [data/](../data/README.md) | OBS 插件的英中日韩四语文本。 |
| [docs/](README.md) | 入门、架构、测试、验证记录及维护交接。 |
| [installer/](../installer/README.md) | EXE 安装向导、OBS 扫描、路径校验和四语提示。 |
| [scripts/](../scripts/README.md) | 构建、翻译校验、安装测试、打包与发布脚本。 |
| [src/](../src/README.md) | OBS 接入、AMD 编码后端、设备筛选及独立进程通信。 |
| [tests/](../tests/README.md) | 协议/筛选回归、显卡编码测试和文件锁辅助程序。 |
| [third-party/](../third-party/README.md) | 第三方运行时、工具链和安装器声明。 |

根目录的 [CHANGELOG.md](../CHANGELOG.md) 记录版本变化，[CONTRIBUTING.md](../CONTRIBUTING.md) 说明贡献流程，[GOVERNANCE.md](../GOVERNANCE.md) 说明维护权限与接手，[LICENSE](../LICENSE) 保存许可证，[THIRD-PARTY.md](../THIRD-PARTY.md) 说明依赖，[version.json](../version.json) 保存构建使用的版本号。[.gitattributes](../.gitattributes) 管理文本换行，[.gitignore](../.gitignore) 排除编译缓存及测试产物。

## 构建、接手与分支

Windows x64、Node.js 22+、Git：

~~~powershell
node scripts/check-locales.mjs
node scripts/build.mjs --download-obs --tests
node scripts/build-installer.mjs
node scripts/package.mjs
~~~

可用 **--obs-root "C:\Program Files\obs-studio"** 代替下载 OBS。官方工具链和源码按固定 SHA-256 校验。产物位于 **dist/** 和 **release/**。构建过程不安装插件、不修改 OBS 配置。

完整源码、安装器源码、翻译、测试和 CI 均公开，采用 **GPL-2.0-or-later**，保留第三方声明。没有闭源收费组件、CLA、遥测或在线激活。任何人可 Fork、建分支、修改并按许可证再发布；Fork 可使用自己的 GitHub Actions 临时令牌发布自己的 Releases。向原仓库发版仍需仓库所有者授予写权限，开源许可证不能代替账号权限。

详见[贡献指南](../CONTRIBUTING.md)、[治理与接手](../GOVERNANCE.md)、[发布流程](releasing.md)、[安装器设计](installer.md)、[验证记录](validation.md)和[第三方声明](../THIRD-PARTY.md)。

## 卸载

关闭 OBS，在 Windows 已安装的应用中卸载 GPU Encoder Bridge，或运行目标 OBS 下 **data/obs-plugins/obs-gpu-encoder/uninstall/unins000.exe**。卸载仅移除安装器记录的本插件文件，保留 OBS、配置和其他插件。手动 ZIP 安装请先改选其他编码器，再删除本插件 DLL、worker 和对应数据目录。
