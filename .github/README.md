# GitHub automation and issue intake

| Path | Responsibility / 职责 |
| --- | --- |
| [workflows/build.yml](workflows/build.yml) | Builds/tests the Windows plugin and installer, packages artifacts, and conditionally publishes a release. / 构建、测试、打包及条件发版。 |
| [ISSUE_TEMPLATE/bug_report.md](ISSUE_TEMPLATE/bug_report.md) | Requests environment details and reproducible steps for bug reports. / 引导反馈环境与复现步骤。 |

The build job checks translations, runs CPU regression tests, builds the EXE, tests fixture installation/uninstall and packages assets. Hardware encoding remains a separate real-machine check.

The release-intent script requests publication only from the repository's default branch: a push changing **version.json**, or a manual workflow dispatch with **publish=true**. Other branch/PR builds do not request publication through this script.

The publish job uses a short-lived repository token with **contents: write** to create a draft, upload all release assets and publish afterward. Forks derive the repository destination from Actions context. No personal maintainer token is required.

See [release instructions](../docs/releasing.md) for execution and recovery, and [script roles](../scripts/README.md) for individual commands.

## 简体中文

工作流负责构建和安装验证，发布条件由默认分支事件决定。实际 AMD 显卡编码验收需另行实测。问题模板要求环境和复现步骤，请先移除日志中的密钥及个人信息。

## 日本語

ワークフローはビルドとインストール検証を担当し、既定ブランチのイベントに応じて公開します。AMD 実機のエンコード検証は別途必要です。不具合ログから認証情報や個人情報を除去してください。

## 한국어

워크플로는 빌드와 설치 검증을 담당하고 기본 브랜치 이벤트에 따라 게시합니다. 실제 AMD 인코딩 검증은 별도로 필요합니다. 문제 보고 로그에서 인증 정보와 개인 정보를 제거하세요.
