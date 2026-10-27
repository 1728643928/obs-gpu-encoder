# Windows installer

This directory defines the five-language EXE installer; it does not contain the encoder implementation.

| File | Responsibility / 职责 |
| --- | --- |
| [setup.iss](setup.iss) | Wizard, OBS discovery, cancellable fixed-drive scan, destination checks, file installation and uninstall. / 安装向导、OBS 扫描、目标校验、安装与卸载。 |
| [messages.iss](messages.iss) | Project-specific English, Simplified Chinese, Traditional Chinese (Taiwan), Japanese and Korean wizard messages. / 本项目五语安装提示。 |
| [languages/ChineseTraditional.isl](languages/ChineseTraditional.isl) | Upstream Traditional Chinese standard messages, with the selector name set to 繁體中文（台灣）; Enfeng Tsao and Samuel Lee notices retained. |
| [languages/ChineseSimplified.isl](languages/ChineseSimplified.isl) | Upstream Simplified Chinese messages for Inno Setup's standard controls. / Inno Setup 标准界面的上游简体中文资源。 |

## Destination contract

Choose the **OBS root folder**, for example **E:\\obs-studio**. The installer appends these paths:

| Installed item | Destination relative to the selected OBS root |
| --- | --- |
| Plugin DLL and worker EXE | **obs-plugins/64bit/** |
| Five OBS locale files | **data/obs-plugins/obs-gpu-encoder/locale/** |
| Project/third-party notices | **data/obs-plugins/obs-gpu-encoder/** |
| Uninstaller | **data/obs-plugins/obs-gpu-encoder/uninstall/** |

Discovery recognizes a root by **bin/64bit/obs64.exe** and **obs.dll**. Installation additionally checks the executable's OBS 33.x version and the expected FFmpeg DLL filenames. An explicit **/DIR** is preserved. Occupied or unwritable target files are rejected before copying.

The optional full scan visits local fixed-drive directories in timer batches, skips inaccessible locations and reparse points, and can be stopped. Other locations can be selected manually.

Build with **node scripts/build-installer.mjs** after building the plugin. See [installer design](../docs/installer.md) and [installation tests](../scripts/test-installer.mjs).

## 简体中文

选择 OBS 根目录后，安装器自动将 DLL 和 worker 放入该 OBS 的 **obs-plugins/64bit/**，五语资源放入对应 **data/obs-plugins/obs-gpu-encoder/locale/**。无需自行选择插件子目录。扫描、复制、卸载逻辑在 setup.iss，五语提示在 messages.iss。

## 日本語

OBS のルートを選ぶと、DLL と worker は **obs-plugins/64bit/**、翻訳は **data/obs-plugins/obs-gpu-encoder/locale/** に配置します。検索・検証・配置・削除は setup.iss、5 言語の追加メッセージは messages.iss が担当します。

## 한국어

OBS 루트를 선택하면 DLL과 worker는 **obs-plugins/64bit/**, 번역은 **data/obs-plugins/obs-gpu-encoder/locale/**에 설치됩니다. 검색·검사·설치·제거는 setup.iss, 다섯 언어 사용자 지정 문구는 messages.iss가 담당합니다.

## 繁體中文（台灣）

選擇 OBS 安裝根目錄後，安裝程式會自動把外掛放進這份 OBS 的 **obs-plugins/64bit/**，五語資源則放進 **data/obs-plugins/obs-gpu-encoder/locale/**。台灣繁中選項的命令列代碼是 **/LANG=tw**。標準介面沿用上游繁中翻譯並保留署名，專案的搜尋與錯誤提示另外以台灣華語撰寫。
