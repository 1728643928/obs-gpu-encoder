# Documentation index

Choose an entry by the task you are trying to complete.

| Document | Purpose / 用途 |
| --- | --- |
| [Main README](../README.md) | English overview, installation, current scope and source map. / 英文概览与项目导航。 |
| [简体中文](README.zh-CN.md) · [繁體中文（台灣）](README.zh-TW.md) · [日本語](README.ja-JP.md) · [한국어](README.ko-KR.md) | Translated getting-started guides. / 中日韩入门指南。 |
| [localization.md](localization.md) | Five-language coverage and Taiwan Mandarin terminology. / 五語範圍與台灣華語翻譯原則。 |
| [enhanced-broadcasting.md](enhanced-broadcasting.md) | Tools GPU selection, multitrack routing, recording independence and limits. / 增强广播选卡、录制独立性与限制。 |
| [architecture.md](architecture.md) | OBS integration, explicit GPU binding and worker/IPC boundaries. / 编码架构与进程边界。 |
| [installer.md](installer.md) | Disk discovery, exact destinations, validation and uninstall design. / 安装器扫描、目标路径及卸载设计。 |
| [testing.md](testing.md) | Commands and expected checks for contributors. / 测试执行方式。 |
| [validation.md](validation.md) | Hardware, OBS and installer results actually observed. / 已完成的验证及范围限制。 |
| [releasing.md](releasing.md) | Build/release automation, forks and maintainer handover. / 构建、发版、Fork 与接手。 |
| [release-notes.md](release-notes.md) | Five-language text published with the current release. / 当前版本的五语发布说明。 |
| [CHANGELOG.md](../CHANGELOG.md) | Versioned change history and unreleased documentation changes. / 版本变化记录。 |
| [CONTRIBUTING.md](../CONTRIBUTING.md) | Contribution process and focused commit messages. / 贡献流程与提交说明要求。 |
| [GOVERNANCE.md](../GOVERNANCE.md) | Who controls upstream access and how independent continuations work. / 原仓库权限和项目延续规则。 |

Implementation entry points are described in [src](../src/README.md), [installer](../installer/README.md), [scripts](../scripts/README.md), [tests](../tests/README.md) and [translations](../data/README.md).

## 简体中文

安装先看对应语言入门指南；改代码先看架构和 src 导航；排错对照 testing 与 validation；接手发版看 releasing。测试说明描述怎么测，验证记录描述实际测过什么，两者用途不同。

## 日本語

導入は言語別ガイド、実装変更は architecture と src、検証は testing と validation、引き継ぎと公開は releasing を参照してください。検証手順と実際の検証結果を区別します。

## 한국어

설치는 언어별 가이드, 구현 수정은 architecture와 src, 검증은 testing과 validation, 인수 및 게시 절차는 releasing을 참고하세요. 테스트 절차와 실제 검증 결과를 구분합니다.

## 繁體中文（台灣）

第一次使用請先看[台灣繁中入門說明](README.zh-TW.md)。要改程式，看 architecture 與 src；要測試或排錯，看 testing 與 validation；要接手發布，看 releasing。翻譯請參考 localization，測試方法與實際驗證結果也請分開記錄。
