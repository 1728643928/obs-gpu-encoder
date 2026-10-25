# OBS plugin resources

The four files under **locale/** translate OBS encoder property labels, presets and project messages.

| File | Language |
| --- | --- |
| [locale/en-US.ini](locale/en-US.ini) | English / 英语 |
| [locale/zh-CN.ini](locale/zh-CN.ini) | 简体中文 |
| [locale/ja-JP.ini](locale/ja-JP.ini) | 日本語 |
| [locale/ko-KR.ini](locale/ko-KR.ini) | 한국어 |

OBS's selected language chooses the locale; the module's default locale is **en-US**. Build output places these files under **dist/data/obs-plugins/obs-gpu-encoder/locale/**.

Keep keys synchronized across all four files and translate values only. Do not translate codec identifiers, persisted setting values or GPU identity strings. Run **node scripts/check-locales.mjs** after edits.

Installer-specific text lives in [installer/messages.iss](../installer/messages.iss), separately from these OBS resources. Low-level driver/library diagnostic details may remain English.

## 简体中文

这四个文件控制 OBS 插件内部的参数名称、预设和提示，跟随 OBS 的界面语言。修改时保持四语键名一致，只翻译文字值；安装器文本位于 installer/messages.iss。底层驱动与库的诊断详情可能保留英文。

## 日本語

OBS の言語設定に応じてプロパティ、プリセット、メッセージを表示します。4 言語のキーを揃え、値のみ翻訳してください。インストーラー文言は installer/messages.iss にあります。低水準診断は英語の場合があります。

## 한국어

OBS 언어 설정에 따라 속성, 사전 설정과 문구를 표시합니다. 네 언어의 키를 일치시키고 값만 번역하세요. 설치 프로그램 문구는 installer/messages.iss에 있으며 저수준 진단은 영어일 수 있습니다.
