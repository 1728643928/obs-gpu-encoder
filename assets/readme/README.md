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
