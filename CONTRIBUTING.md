# Contributing / 贡献 / 貢献 / 기여

## English

Fork the repository, create a feature branch, make focused changes, and open a pull request. No CLA or contributor fee is required; contributions are under GPL-2.0-or-later with existing notices retained.

- Read the README, architecture and testing documents.
- Keep explicit GPU binding, capability probes, IPC bounds and worker cleanup. Do not add silent GPU/software fallbacks.
- Keep platform credentials and network publishing outside the encoder.
- Keep all four plugin and installer locales in sync; run **node scripts/check-locales.mjs**.
- Run the build/unit tests. For installer changes, run **node scripts/test-installer.mjs**. For encoding changes, include hardware and OBS streaming evidence; CI does not have an AMD GPU.
- Do not commit profiles, keys, recordings, personal logs, driver binaries, caches or build outputs.
- Keep source and installer builds available to forks without personal maintainer secrets. Bump version.json for a release and update four-language release notes.

Use issues for reproducible bugs and feature proposals. Include OS, OBS/plugin/driver versions, GPU names, selected codec and reproduction steps. Review logs and remove stream keys and private paths before posting. See GOVERNANCE.md and docs/releasing.md for handover and release rights.

## 简体中文

Fork 后建立功能分支，提交范围清晰的修改并发起 PR。无需 CLA 或费用，贡献按 GPL-2.0-or-later 公开并保留原声明。请保持显卡显式绑定、能力检测、IPC 边界和进程清理，不添加静默回退。四种语言须同步，通过语言检查及适合修改范围的测试；编码修改还需实际显卡与 OBS 证据。请勿提交推流密钥、个人配置、日志、录制、驱动或构建缓存。发版须更新 version.json 和四语发布说明。问题反馈请写明环境与复现步骤，并先清除日志中的隐私信息。

## 日本語

Fork して機能ブランチを作成し、変更を絞って PR を送ってください。CLA や料金は不要です。貢献は既存表示を保持した GPL-2.0-or-later で公開します。明示的 GPU 選択、能力検証、IPC 境界、ワーカー終了処理を維持し、自動フォールバックを追加しないでください。4 言語と適切なテストを更新し、エンコード変更には実機と OBS の検証を含めてください。配信キー、個人設定、ログ、録画、ドライバー、ビルドキャッシュはコミットしないでください。公開前に version.json と 4 言語のリリースノートを更新します。不具合報告には環境と再現手順を記載し、個人情報を除去してください。

## 한국어

Fork 후 기능 브랜치를 만들고 범위를 명확히 한 변경을 PR로 제출하세요. CLA나 비용은 필요하지 않으며 기존 고지를 유지한 GPL-2.0-or-later로 기여합니다. 명시적 GPU 지정, 기능 검사, IPC 제한 및 작업 프로세스 정리를 유지하고 자동 대체 동작을 추가하지 마세요. 네 언어와 관련 테스트를 갱신하고 인코딩 변경에는 실제 GPU 및 OBS 검증을 포함하세요. 방송 키, 개인 설정, 로그, 녹화, 드라이버, 빌드 캐시는 커밋하지 마세요. 릴리스 전 version.json과 네 언어 릴리스 노트를 갱신하세요. 문제 보고에는 환경과 재현 절차를 적고 개인 정보를 제거하세요.
