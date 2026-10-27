# README artwork

The five localized SVG heroes show the plugin's actual division of GPU roles. They are conceptual diagrams, not OBS screenshots or performance comparisons.

| File | Language |
| --- | --- |
| [hero-en.svg](hero-en.svg) | English |
| [hero-zh-cn.svg](hero-zh-cn.svg) | 简体中文 |
| [hero-zh-tw.svg](hero-zh-tw.svg) | 繁體中文（台灣） |
| [hero-ja.svg](hero-ja.svg) | 日本語 |
| [hero-ko.svg](hero-ko.svg) | 한국어 |

Edit the text or layout in [generate.mjs](generate.mjs), then regenerate with Node.js:

~~~sh
node assets/readme/generate.mjs
~~~

No package installation, image service or proprietary font is needed. SVGs use system fonts, explicit backgrounds, accessible titles/descriptions and a 1200 × 448 viewBox. Preview them at 900 px and 360 px widths. Essential instructions remain in Markdown when images are unavailable or too small.

The palette uses charcoal `#12191f`, warm white `#f5f3ee`, coral for rendering and mint for encoding. The connecting arrow indicates frame flow; it does not imply zero-copy transfer. Frames currently pass through system memory.

Original artwork and generator are covered by the project's GPL-2.0-or-later license.

中文：修改生成脚本中的五语文本后重新运行；图片是显卡分工示意，不是实际界面截图或性能对比。

日本語：生成スクリプト内の翻訳を編集して再実行してください。図は役割の説明であり、実際の UI や性能比較ではありません。

한국어: 생성 스크립트의 번역을 수정한 뒤 다시 실행하세요. 이미지는 역할 개념도이며 실제 UI 화면이나 성능 비교가 아닙니다.

## Design reference

The README redesign used [oil-oil/beautify-github-readme](https://github.com/oil-oil/beautify-github-readme) as guidance for content hierarchy, a project-specific visual identity, accessible SVGs and desktop/mobile review. The GPU-role artwork and generator were created for this repository. [THIRD-PARTY.md](../../THIRD-PARTY.md#readme-design-reference) records the reference snapshot and scope.

## Localized product screenshots

The original four-language set was captured on 2026-10-27 with Windows 11, OBS Studio 33.0.0 and GPU Encoder Bridge 0.3.0-preview. The 0.3.1 documentation/release update does not change the encoder implementation or these controls. These are native, bounded UI captures; no translated text is painted over an image.

| Page language | Capture folder |
| --- | --- |
| English | [screenshots/en/](screenshots/en/) |
| 简体中文 | [screenshots/zh-cn/](screenshots/zh-cn/) |
| 繁體中文（台灣） | [screenshots/zh-tw/](screenshots/zh-tw/) |
| 日本語 | [screenshots/ja/](screenshots/ja/) |
| 한국어 | [screenshots/ko/](screenshots/ko/) |

Each folder contains the same four views in that language:

- **obs-gpu-selection.jpg** — Advanced Output with the video-encoder popup open. The full settings window provides localized context around GPU/codec names.
- **obs-encoder-settings.jpg** — iGPU H.264 selected with CBR, 6000 Kbps, 2-second keyframes and Balanced.
- **enhanced-gpu-settings.jpg** — the profile-specific Tools dialog with forced iGPU encoding enabled.
- **installer-obs-discovery.jpg** — the localized EXE discovery step, detecting the actual OBS installation and offering the optional local fixed-drive scan.

All OBS images use the user's existing OBS executable and installed plugin, with a disposable, account-free portable configuration. No other OBS installation was downloaded. A blank custom service exposes all probed formats; no stream or recording was started. The original daily-use profiles, scenes, language and enhanced-broadcast settings were not changed. The temporary configuration was removed after capture.

Each installer language was opened in current-user mode, captured before installation and cancelled. No installation or full-disk scan was performed for these pictures.

Keep the screenshots and all five README image paths synchronized. GPU model names, codec names and the product name intentionally retain their original spelling. The SVG generator does not create or modify screenshots. These images demonstrate configuration, not public-platform receiving or performance.

## Taiwan Mandarin additions in 0.3.2

The four zh-tw captures were added on 2026-10-27. The OBS views use the same user-owned OBS 33.0.0 installation with the new zh-TW module resource and a disposable account-free portable profile. Standard OBS controls use OBS's upstream zh-TW resource; project-owned text is authored for Taiwan Mandarin. The daily-use configuration was unchanged.

The installer view comes from the actual [published 0.3.2 EXE](https://github.com/1728643928/obs-gpu-encoder/releases/tag/v0.3.2), built by GitHub Actions from commit f0537d55a3b737e4e30cd803e9eae3c374c62884. Its SHA-256 is **3e4a69c7f790dcc236312adeae259fe6668c82218c63ee19d9565bcce57207c2**. The five-option selector and Taiwan discovery page were checked in current-user mode, then cancelled before installation; no full scan was started. These native captures bring the total to 20, with four matching views for each of the five README languages.
