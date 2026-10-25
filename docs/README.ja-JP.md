# GPU Encoder Bridge for OBS

実際の GPU 名でエンコード用デバイスを選択し、OBS の描画は別の GPU で続けられます。OBS 標準の配信サービス、音声、配信開始/停止ボタンをそのまま使用します。

**0.2.0-preview · Windows x64 · OBS Studio 33.x · AMD AMF · SDR / 8 ビット**

[English](../README.md) · [简体中文](README.zh-CN.md) · [日本語](README.ja-JP.md) · [한국어](README.ko-KR.md)

[ダウンロード / Releases](https://github.com/1728643928/obs-gpu-encoder/releases) · [問題を報告](https://github.com/1728643928/obs-gpu-encoder/issues)

## 機能

- OBS 標準の一覧に **AMD Radeon(TM) Graphics — H.264 [GPU 1 / AMF Bridge]** のように GPU 名と形式を表示します。
- 実際のエンコード能力を調べ、対応する AMD GPU にのみ H.264、HEVC、AV1 を表示します。配信サービスも形式を制限します。
- ソフトウェア GPU と表示専用の間接アダプターを除外します。同じ型番の実 GPU は個別に保持します。
- CBR / VBR、ビットレート、キーフレーム間隔、速度/バランス/画質のプリセットに対応します。
- 内部ワーカーで AMF を分離します。別の制御アプリや FFmpeg コマンドは不要です。
- プラグイン、インストーラー、入門文書は英語・中国語・日本語・韓国語に対応します。

Windows の GPU 設定やモニター接続、OBS の描画 GPU を変える必要はありません。プラグインはアカウントや配信キーを扱いません。音声と配信通信は OBS が担当します。

## インストールと配信

1. OBS を終了し、Releases から **windows-x64-setup.exe** をダウンロードします。
2. 言語と検出された OBS を選択します。固定ドライブ全体を検索して停止することも、手動で指定することもできます。
3. OBS のルートフォルダーを確認します。OBS 33.x x64 と必要な DLL 構成を検証します。
4. OBS の設定 → 出力 → 詳細 → 配信で、目的の GPU 名が付いた **AMF Bridge** エンコーダーを選択します。
5. OBS 標準のサービス設定と配信開始ボタンを使用します。

全体検索はローカル固定ドライブのディレクトリを調べ、個人文書の内容を読んだり送信したりしません。アクセス不可の場所と再解析ポイント/リンクは除外します。ネットワーク、リムーバブルドライブ、リンクのみでマウントされたボリュームは手動指定してください。大きなドライブでは時間がかかります。通常は管理者権限を要求します。ポータブル版は **/CURRENTUSER** を指定できます。

手動 ZIP の **obs-plugins** と **data** を OBS ルートに統合する方法もあります。DLL と worker EXE を両方配置し、OBS 自体の DLL は上書きしないでください。

初期設定の例は **H.264、CBR、6000 Kbps、キーフレーム 2 秒、バランス** です。ビットレートは配信先の条件に合わせてください。Twitch Enhanced Broadcasting は選択を上書きする場合があるため、GPU を固定したい場合は無効にしてください。GPU 番号は現在の DXGI 順序です。ハードウェア変更後は再確認してください。失敗時に別 GPU やソフトウェアへ自動切り替えしません。

## 対応範囲と検証

現在は AMD AMF のみです。Intel QSV、NVIDIA NVENC、HDR、10 ビットは未実装です。SDR / NV12 / 8 ビット、OBS 33 / avcodec 62 / avutil 60 に対応します。システムメモリ経由でフレームを転送し、GPU 間ゼロコピーではありません。ゲーム性能の向上は保証しません。

RX 9060 XT で描画し、9700X 内蔵 GPU で H.264 をエンコードする環境で検証しました。以前の OBS ローカル RTMP テストでは約 47 秒の音声・映像を受信してデコードしました。0.2.0 でも 300 フレームのテストが成功しました。HEVC/AV1 は能力確認のみです。公開配信先、ゲーム性能、他の GPU や OBS バージョンは未検証です。本版はプレビューです。

## ディレクトリとファイルの役割

GitHub のファイル一覧の中央列は**最後のコミットメッセージ**です。1 回のコミットで複数パスを変更すると、同じメッセージが表示されます。各ディレクトリの README に詳細があります。

| ディレクトリ | 内容 |
| --- | --- |
| [.github/](../.github/README.md) | CI、検証、公開と不具合テンプレート。 |
| [data/](../data/README.md) | OBS プラグインの 4 言語リソース。 |
| [docs/](README.md) | 導入、構成、検証と引き継ぎガイド。 |
| [installer/](../installer/README.md) | EXE、OBS 検索、配置先検証と翻訳。 |
| [scripts/](../scripts/README.md) | ビルド、翻訳検証、配置テスト、パッケージと公開。 |
| [src/](../src/README.md) | OBS 接続、AMD バックエンド、GPU 方針と IPC。 |
| [tests/](../tests/README.md) | CPU 回帰、GPU 実機テストとファイルロック補助。 |
| [third-party/](../third-party/README.md) | ランタイム、ツール、インストーラーの第三者表示。 |

ルートの [CHANGELOG.md](../CHANGELOG.md) は変更履歴、[CONTRIBUTING.md](../CONTRIBUTING.md) は貢献手順、[GOVERNANCE.md](../GOVERNANCE.md) は権限と継続、[LICENSE](../LICENSE) はライセンス、[THIRD-PARTY.md](../THIRD-PARTY.md) は依存関係、[version.json](../version.json) はビルド用バージョンです。[.gitattributes](../.gitattributes) は改行、[.gitignore](../.gitignore) は生成物の除外を管理します。

## ビルドと引き継ぎ

Windows x64、Node.js 22+、Git が必要です。

~~~powershell
node scripts/check-locales.mjs
node scripts/build.mjs --download-obs --tests
node scripts/build-installer.mjs
node scripts/package.mjs
~~~

**--download-obs** の代わりに **--obs-root "C:\Program Files\obs-studio"** を指定できます。公式ツールとソースを固定 SHA-256 で検証します。出力は **dist/** と **release/** です。ビルドは OBS の設定やインストールを変更しません。

全ソース、インストーラー、翻訳、テスト、ワークフローを **GPL-2.0-or-later** で公開し、第三者の表示を保持します。非公開の有料部分、CLA、テレメトリ、オンライン認証はありません。誰でも Fork、分岐、変更、再配布が可能です。Fork は自身の GitHub Actions の一時トークンで独自 Releases を公開できます。上流リポジトリへの公開には所有者からの書き込み権限が必要です。

[貢献](../CONTRIBUTING.md)、[運営と引き継ぎ](../GOVERNANCE.md)、[リリース](releasing.md)、[インストーラー設計](installer.md)、[検証](validation.md)、[第三者の表示](../THIRD-PARTY.md)を参照してください。

## アンインストール

OBS を終了し、Windows のインストール済みアプリ、または OBS 内の **data/obs-plugins/obs-gpu-encoder/uninstall/unins000.exe** を使用します。記録された本プラグインのファイルのみ削除し、OBS、設定、他のプラグインは保持します。手動 ZIP では別のエンコーダーを選択してから本プラグインの DLL、worker、データフォルダーを削除します。
