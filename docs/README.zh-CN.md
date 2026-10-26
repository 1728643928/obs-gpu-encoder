<p align="right">
  <a href="../README.md">English</a> · <strong>简体中文</strong> · <a href="README.ja-JP.md">日本語</a> · <a href="README.ko-KR.md">한국어</a>
</p>

<p align="center">
  <img src="../assets/readme/hero-zh-cn.svg" width="100%" alt="GPU Encoder Bridge：游戏与 OBS 渲染继续使用独显，另外指定 AMD 核显或其他 AMD 显卡编码。">
</p>

**[下载 Windows 安装器 →](https://github.com/1728643928/obs-gpu-encoder/releases/download/v0.2.0-preview/GPU-Encoder-Bridge-0.2.0-preview-windows-x64-setup.exe)** · [全部下载](https://github.com/1728643928/obs-gpu-encoder/releases) · [文档索引](README.md) · [问题反馈](https://github.com/1728643928/obs-gpu-encoder/issues)

**0.2.0-preview** · Windows x64 · OBS Studio 33.x · AMD AMF · GPL-2.0-or-later

**按真实显卡名选择编码设备，OBS 继续在游戏显卡上运行。**

GPU Encoder Bridge 在 OBS 原生编码器列表中加入显卡名称，让 AMD 核显或另一张 AMD 显卡负责编码，游戏、显示器与 OBS 渲染仍使用游戏显卡。无需更改 Windows 显卡偏好设置或显示器插线。

直播平台、音频、网络连接与开始/停止直播仍由 OBS 负责，无需另开控制软件。

## 已在真实双显卡环境验证

**渲染：Radeon RX 9060 XT · 编码：Ryzen 7 9700X 核显，H.264**

| 检查 | 实际结果 |
| --- | --- |
| 0.2.0 独立编码进程测试 | 1080p60 输入 300 帧，输出 300 个数据包、3 个关键帧。 |
| 此前 OBS 本地 RTMP 测试 | 约 47 秒视频和音频成功接收、解码；OBS 仍在 RX 9060 XT 上渲染。 |
| 四语安装器检查 | 四种语言安装、卸载通过；无效路径与被占用目标遭拒绝。 |

这些是有明确范围的验证记录，不是游戏性能跑分。HEVC/AV1 仅有能力探测，未完成同等级 OBS 直播验收；公网直播平台接收尚未验证。[查看完整验证记录 →](validation.md)

## 可以选择什么

- **显卡与编码格式。** 每个条目同时显示真实设备及 H.264、HEVC 或 AV1，实际探测成功才提供；直播平台可能进一步限制格式。
- **熟悉的 OBS 参数。** 支持 CBR/VBR、码率、关键帧间隔，以及速度、均衡、质量预设。
- **明确绑定设备。** 过滤仅用于间接显示的虚拟别名，真实同型号双卡分别保留；出错不会悄悄换显卡或改为软件编码。
- **四种语言。** 插件、EXE 安装器与入门文档均提供英文、简体中文、日文和韩文。

## 安装并开始直播

1. **关闭 OBS**，下载上方安装器。
2. 选择语言与 **OBS 根目录**。向导自动查找常见位置，也可扫描全部本地固定磁盘、随时停止，或手动浏览；复制前检查 OBS 33.x x64 及必需 DLL。
3. 启动 OBS → **设置 → 输出 → 高级 → 直播**，选择目标显卡名称对应的 **AMF Bridge** 编码器。
4. 在 OBS 中配置平台与音频，照常点击 **开始直播**。

条目示例：**AMD Radeon(TM) Graphics — H.264 [GPU 1 / AMF Bridge]**。GPU 编号是当前 DXGI 枚举顺序，硬件变更后请重新检查。

可从 H.264、CBR、关键帧 2 秒、均衡开始设置，码率按平台要求填写。需要固定编码器时，请关闭可能覆盖选择的 Twitch Enhanced Broadcasting。

<details>
<summary>具体安装位置与全盘扫描范围</summary>

选择 OBS 根目录，安装器会自动放入对应插件目录：

| OBS 根目录下 | 文件 |
| --- | --- |
| `obs-plugins/64bit/` | `obs-gpu-encoder.dll` 与 `obs-gpu-encoder-worker.exe` |
| `data/obs-plugins/obs-gpu-encoder/` | 四语资源、声明与卸载器。 |

扫描只枚举本地固定磁盘目录，不读取个人文档内容、不上传文件。无权限目录与重解析点会跳过；网络盘、可移动盘及仅通过链接挂载的卷请手动浏览。大容量磁盘可能耗时，可随时停止。

默认安装器为受保护目录申请管理员权限；可写的便携安装可使用 **/CURRENTUSER**。目前 EXE 未数字签名。[安装器详细说明](installer.md)

</details>

<details>
<summary>手动 ZIP 安装与卸载</summary>

将 ZIP 中的 **obs-plugins** 和 **data** 合并到 OBS 根目录。插件 DLL 与 worker EXE 必须一起安装，不要覆盖 OBS 自带 DLL。

卸载前关闭 OBS，使用 Windows 已安装的应用中的 GPU Encoder Bridge，或运行目标 OBS 下 **data/obs-plugins/obs-gpu-encoder/uninstall/unins000.exe**。安装器只移除其记录的本插件文件，保留 OBS、配置与其他插件。手动安装请先改选其他编码器，再删除本插件 DLL、worker 和对应数据目录。

</details>

## 支持范围与取舍

| 项目 | 当前预览版 |
| --- | --- |
| 平台 | Windows x64；OBS 33 / avcodec 62 / avutil 60 ABI。 |
| 编码 | AMD AMF；设备探测成功的 H.264、HEVC 与 AV1。 |
| 视频 | SDR / NV12 / 8 位，拒绝 HDR 与 10 位。 |
| 其他厂商 | 尚未实现 Intel QSV 与 NVIDIA NVENC。 |
| 画面传输 | 系统内存 → 本地编码进程 → 指定 AMD 显卡，尚未实现跨卡零拷贝。 |

内部隐藏进程隔离 AMF 初始化、明确绑定编码显卡，再将编码数据返回 OBS。跨卡传输有成本，不保证游戏性能提升。其他显卡、其他 OBS ABI 和公网平台仍需更多验证。[架构](architecture.md) · [测试](testing.md)

## 完整开源，可以继续维护

插件、编码进程、安装器、翻译、测试与发版工作流均按 **GPL-2.0-or-later** 公开，保留第三方声明。没有闭源收费组件、CLA、遥测或在线激活。

任何人可 Fork、创建分支、修改并按许可证再发布。Fork 可用自己仓库的 GitHub Actions 临时令牌发布自己的 Releases，无需原作者私钥或电脑。向原仓库发版仍需所有者授予权限。

[贡献指南](../CONTRIBUTING.md) · [治理与接手](../GOVERNANCE.md) · [发布自己的 Fork](releasing.md) · [许可证](../LICENSE) · [第三方声明](../THIRD-PARTY.md)

<details>
<summary>从源码构建</summary>

需要 Windows x64、Node.js 22+ 与 Git，无需 Visual Studio 或私有 SDK。

~~~powershell
node scripts/check-locales.mjs
node scripts/build.mjs --download-obs --tests
node scripts/build-installer.mjs
node scripts/package.mjs
~~~

已有 OBS 时可将 **--download-obs** 换成 **--obs-root "C:\Program Files\obs-studio"**。公开工具链与源码按固定 SHA-256 校验。产物位于 **dist/** 和 **release/**；构建不安装插件、不修改 OBS 配置。

</details>

<details>
<summary>项目目录导航</summary>

| 目录 | 职责与入口 |
| --- | --- |
| [src/](../src/README.md) | OBS 原生接入、AMD 后端、显卡筛选与进程通信。 |
| [installer/](../installer/README.md) | EXE 向导、OBS 查找、路径校验与四语提示。 |
| [scripts/](../scripts/README.md) | 构建、翻译检查、安装测试、打包与发版。 |
| [tests/](../tests/README.md) | CPU 回归、实际显卡测试与文件锁辅助工具。 |
| [data/](../data/README.md) | OBS 四语资源与翻译规则。 |
| [docs/](README.md) | 入门、架构、测试、验证与发布交接。 |
| [.github/](../.github/AUTOMATION.md) | CI、发版权限与问题模板。 |
| [assets/readme/](../assets/readme/README.md) | 可编辑四语视觉与无依赖生成脚本。 |
| [third-party/](../third-party/README.md) | 运行时、工具链与安装器的第三方许可原文。 |

[CHANGELOG.md](../CHANGELOG.md) 记录版本变化，[version.json](../version.json) 控制构建和发版版本；[.gitattributes](../.gitattributes) 管理换行，[.gitignore](../.gitignore) 排除生成文件。贡献、权限、许可证与依赖说明见上方链接。

</details>
