# README artwork

The four localized SVG heroes show the plugin's actual division of GPU roles. They are conceptual diagrams, not OBS screenshots or performance comparisons.

| File | Language |
| --- | --- |
| [hero-en.svg](hero-en.svg) | English |
| [hero-zh-cn.svg](hero-zh-cn.svg) | 简体中文 |
| [hero-ja.svg](hero-ja.svg) | 日本語 |
| [hero-ko.svg](hero-ko.svg) | 한국어 |

Edit the text or layout in [generate.mjs](generate.mjs), then regenerate with Node.js:

~~~sh
node assets/readme/generate.mjs
~~~

No package installation, image service or proprietary font is needed. SVGs use system fonts, explicit backgrounds, accessible titles/descriptions and a 1200 × 448 viewBox. Preview them at 900 px and 360 px widths. Essential instructions remain in Markdown when images are unavailable or too small.

The palette uses charcoal `#12191f`, warm white `#f5f3ee`, coral for rendering and mint for encoding. The connecting arrow indicates frame flow; it does not imply zero-copy transfer. Frames currently pass through system memory.

Original artwork and generator are covered by the project's GPL-2.0-or-later license.

中文：修改生成脚本中的四语文本后重新运行；图片是显卡分工示意，不是实际界面截图或性能对比。

日本語：生成スクリプト内の翻訳を編集して再実行してください。図は役割の説明であり、実際の UI や性能比較ではありません。

한국어: 생성 스크립트의 번역을 수정한 뒤 다시 실행하세요. 이미지는 역할 개념도이며 실제 UI 화면이나 성능 비교가 아닙니다.

## Real product screenshots

Captured on 2026-10-26 with Windows 11, OBS Studio 33.0.0 and the published GPU Encoder Bridge 0.2.0-preview binaries. These JPEGs are bounded native UI captures, not generated interface mockups.

| File | What it demonstrates |
| --- | --- |
| [obs-gpu-selection.jpg](obs-gpu-selection.jpg) | The actual encoder popup, with five GPU-named AMF Bridge entries across RX 9060 XT and Ryzen 7 9700X integrated graphics. OBS built-in encoders remain visible. |
| [obs-encoder-settings.jpg](obs-encoder-settings.jpg) | OBS Advanced Output with iGPU H.264 selected, plus rate control, bitrate, keyframe interval and preset. |
| [installer-obs-discovery.jpg](installer-obs-discovery.jpg) | The published EXE detecting an OBS installation and offering an optional fixed-drive scan, before installation. |

The OBS screenshots use an isolated portable profile with no account, stream key or private scene. Custom service was selected to expose all available codec choices; other services can filter the list. The demo rendered on RX 9060 XT and selected the 9700X iGPU for encoding. It did not start a stream or recording, so the screenshots are UI examples rather than new end-to-end validation.

The installer was opened in current-user mode, captured at the discovery step and cancelled. No plugin installation was performed. The full scan button is visible, but the screenshot does not claim a completed scan.

Keep screenshots as real captures when the UI changes. The SVG generator does not create or modify these files. All four README languages share the English screenshots with localized captions and alt text.
