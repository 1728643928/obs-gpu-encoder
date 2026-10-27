# Build and release scripts

Run commands from the repository root using Node.js 22+ on Windows x64. See the [main README](../README.md) for the complete build sequence.

| Script | Responsibility / 职责 |
| --- | --- |
| [build.mjs](build.mjs) | Downloads pinned inputs, builds the plugin/worker, copies resources and optionally builds/runs unit tests. / 主构建、依赖校验、资源复制和单元测试。 |
| [build-installer.mjs](build-installer.mjs) | Obtains the pinned Inno Setup compiler and builds the five-language setup EXE. / 构建五语 EXE 安装器。 |
| [check-locales.mjs](check-locales.mjs) | Checks five-language key parity and translation keys referenced by the plugin/installer. / 检查五语资源键及引用是否完整。 |
| [test-installer.mjs](test-installer.mjs) | Installs/uninstalls disposable fixtures in five languages; tests retained files, invalid targets and file locks. / 五语安装卸载、文件保留、错误路径及占用保护测试。 |
| [package.mjs](package.mjs) | Creates manual-install/source ZIPs, SHA-256 checksums and build provenance from tracked source. / 打包插件与源码、生成校验值和构建信息。 |
| [ci-release-intent.mjs](ci-release-intent.mjs) | Determines whether the Actions event requests publishing from the default branch. / 判断默认分支事件是否需要发版。 |
| [publish-release.mjs](publish-release.mjs) | Creates a draft Release, uploads the complete asset set and publishes it after all uploads succeed. / 创建发布草稿、上传全部产物后公开。 |

**build.mjs** and **build-installer.mjs** create build outputs; installing the plugin is a separate step. **test-installer.mjs** intentionally installs only into disposable fixtures under **build/**.

**package.mjs** uses Git's tracked-file list. Track intended source before packaging; keep private profiles, keys, recordings and build caches out of Git.

Release scripts consume GitHub Actions context and the publish job's short-lived token. They do not need a maintainer's personal access token. See [release instructions](../docs/releasing.md).

## 简体中文

上表按构建、翻译校验、安装验证、打包和发版区分脚本。普通构建不会把插件安装进你的 OBS；安装测试使用临时目录。发版前请更新版本与五语发布说明，并按发布文档执行。

## 日本語

ビルド、翻訳確認、インストール検証、パッケージ作成、公開を別のスクリプトで担当します。通常ビルドは OBS を変更せず、インストールテストは一時配置先を使用します。公開手順は releasing.md を参照してください。

## 한국어

빌드, 번역 검사, 설치 검증, 패키징과 게시를 각각의 스크립트가 담당합니다. 일반 빌드는 OBS를 변경하지 않으며 설치 테스트는 임시 경로를 사용합니다. 릴리스 절차는 releasing.md를 참고하세요.
