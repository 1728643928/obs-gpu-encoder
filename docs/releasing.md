# Maintainer handover and Releases

## English

Everything required to build and maintain the project is in this repository. The only downloaded items are pinned public sources/toolchains/OBS runtime. No personal access token, original developer's machine, OBS profile, stream key or private build service is needed.

### Upstream release

1. A maintainer with repository write access creates a branch.
2. Change **version.json** to a new version, update **docs/release-notes.md** in all four languages and update CHANGELOG / validation evidence. Keep preview status until broader testing supports a stable claim.
3. Run the documented build, locale, unit, installer and applicable GPU/OBS tests. Submit a PR.
4. Merge to the default branch. A push that changes version.json builds and tests the plugin and installer, uploads build artifacts, then creates a draft Release, uploads all five assets and publishes it. Versions containing a hyphen become prereleases.
5. Inspect the Actions result, release assets and checksums. Existing release assets are never overwritten. A failed upload leaves a draft for inspection; remove the failed draft and its unused tag through GitHub before retrying, or bump to a new version. Do not overwrite a distributed release.

For a first release or a deliberate retry, use **Actions → Build, test and release → Run workflow**, select the default branch and set **publish=true**. A release may only publish from the default branch. Normal branch/PR builds have read-only contents permissions. Only the trusted publish job receives contents:write, using GitHub's short-lived repository-scoped token.

Assets: multilingual EXE installer, manual-install ZIP, complete-source ZIP, SHA256SUMS.txt, build-info.json. The latter records version, commit and asset digests. Builds verify pinned downloads. Byte-for-byte reproducibility across different machines/times has not been established; the Inno PE signing/timestamp environment may differ. Release binaries are currently **unsigned**; no signing key is distributed.

### Fork and independent continuation

Fork, enable GitHub Actions in the fork, create your branches and use the same workflow. No original-owner secret is needed. The release destination and installer project links use **GITHUB_REPOSITORY**, so a fork publishes to itself. Update branding, documentation links and project ownership statements to identify your continuation. Bump the version, merge to your default branch or dispatch the workflow. Preserve license and third-party notices.

The owner may appoint upstream maintainers or transfer the repository using GitHub roles/settings. Fork and release rights do not give write access to the upstream account. Credentials must never be handed over or committed.

## 简体中文

接手者只需公开源码和公开下载的固定版本工具，无需原作者电脑、个人令牌、推流密钥或私有服务。原仓库维护者建立分支，修改 **version.json**、四语 **docs/release-notes.md** 和验证记录，测试后提交 PR。合并到默认分支且版本文件变化时，Actions 自动构建、测试、先创建草稿 Release，上传全部文件后再公开。带连字符的版本标记为预发布。

也可在 Actions 手动运行工作流，选择默认分支并设 **publish=true**。已有发布不会覆盖；上传失败保留草稿，请检查后删除失败草稿及未使用的标签再重试，或升新版本。构建可重现执行，但尚未证明跨机器/时间逐字节一致；EXE 暂未数字签名。

任何人可 Fork、在 Fork 启用 Actions，并使用自己的仓库发布，无需原作者私钥或个人令牌。请改明项目归属、文档链接和发布渠道，并保留许可证与第三方声明。原仓库发版权限仍由所有者授予，交接不要传递账号凭据。

## 日本語

保守者は公開ソースと固定バージョンの公開ツールのみで作業できます。元開発者の PC、個人トークン、配信キーは不要です。ブランチで **version.json**、4 言語の **docs/release-notes.md**、検証記録を更新して PR を提出します。既定ブランチにバージョン変更をマージすると、ビルド・テスト後に草稿 Release を作成し、全ファイルをアップロードしてから公開します。ハイフン付きバージョンはプレリリースです。

Actions の手動実行では既定ブランチと **publish=true** を選択します。公開済みファイルは上書きしません。失敗した草稿と未使用タグを確認して削除した後に再試行するか、バージョンを上げてください。環境をまたぐバイト単位の再現性は未確認で、EXE は未署名です。

Fork で Actions を有効化すれば、元所有者の秘密なしで独自 Releases を公開できます。帰属、リンク、配布先を更新し、ライセンスと第三者表示を保持してください。上流の公開権限は所有者が付与します。アカウント認証情報を引き継がないでください。

## 한국어

유지보수자는 공개 소스와 고정 버전 공개 도구만으로 작업할 수 있습니다. 원 개발자의 PC, 개인 토큰, 방송 키는 필요 없습니다. 브랜치에서 **version.json**, 네 언어 **docs/release-notes.md**, 검증 기록을 갱신하고 PR을 제출하세요. 기본 브랜치에 버전 변경을 병합하면 빌드·테스트 후 초안 Release를 만들고 모든 파일을 업로드한 뒤 게시합니다. 하이픈이 포함된 버전은 시험판으로 표시합니다.

Actions 수동 실행 시 기본 브랜치와 **publish=true**를 선택하세요. 기존 릴리스 파일은 덮어쓰지 않습니다. 실패한 초안과 사용하지 않는 태그를 확인·삭제한 뒤 재시도하거나 버전을 올리세요. 환경 간 바이트 단위 재현성은 확인하지 않았으며 EXE에는 아직 디지털 서명이 없습니다.

Fork에서 Actions를 활성화하면 원 소유자의 비밀 없이 자체 Releases를 게시할 수 있습니다. 귀속, 문서 링크, 배포 채널을 수정하고 라이선스 및 제삼자 고지를 유지하세요. 원본 게시 권한은 소유자가 부여합니다. 계정 인증 정보를 인계하지 마세요.
