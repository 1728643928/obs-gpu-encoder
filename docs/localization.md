# Language coverage and Taiwan Mandarin

The plugin, installer, homepage, release notes and example UI captures have five language options. English remains the primary project README. OBS selects plugin resources from its UI language; the installer has its own language selector.

| UI option | OBS locale / homepage | Installer switch |
| --- | --- | --- |
| English | en-US / ../README.md | /LANG=en |
| 简体中文 | zh-CN / README.zh-CN.md | /LANG=zh |
| 繁體中文（台灣） | zh-TW / README.zh-TW.md | /LANG=tw |
| 日本語 | ja-JP / README.ja-JP.md | /LANG=ja |
| 한국어 | ko-KR / README.ko-KR.md | /LANG=ko |

## Taiwan Mandarin is a separate locale

The target is **Taiwan Mandarin written in Traditional Chinese**, not Cantonese. Character conversion alone does not satisfy this locale. Rewrite sentence order, explanations and calls to action as well as vocabulary. Use approachable software language, without forcing slang, phonetic spellings or Cantonese particles. Preserve technical meaning, error conditions, numbers, units and validation limits.

| Meaning | Taiwan wording used here |
| --- | --- |
| Integrated / discrete GPU | 內顯／獨顯 |
| Plugin | 外掛 |
| Bitrate | 位元率 |
| Keyframe | 關鍵影格 |
| Profile | 設定檔 |
| Preset | 預設組態 |
| File / folder | 檔案／資料夾 |
| Software / driver | 軟體／驅動程式 |
| System memory | 系統記憶體 |
| Save / uninstall | 儲存／解除安裝 |
| Log file | 記錄檔 |
| Start Streaming (OBS control) | 開始串流 |

Use **直播** for the creator's activity and **串流** for OBS controls and the transport operation when that reads naturally. Keep OBS navigation labels consistent with the actual upstream zh-TW UI. Do not relabel OBS's own controls or edit screenshots to make them match our preferred wording. GPU model names, codec identifiers, AMF Bridge, persisted IDs, commands and source filenames retain their spelling.

Project-owned labels and guides are authored here. Inno Setup's standard Traditional Chinese resource is adapted from its pinned upstream translation: preserve the author notices; our change sets the language-selector name to 繁體中文（台灣）. See [THIRD-PARTY.md](../THIRD-PARTY.md).

## Review and validation

1. Review the whole Taiwan Mandarin sentence for meaning and natural phrasing; do not approve based only on character conversion.
2. Keep plugin keys and installer messages synchronized. Preserve every installer placeholder such as %1 and %2.
3. Run **node scripts/check-locales.mjs**. Build output and installer tests must include zh-TW.
4. Inspect the actual OBS property labels, Tools dialog, installer selector and discovery page for clipping or incorrect glyphs. Capture genuine UI in the matching language.
5. Keep historical test records tied to their original versions. A new locale does not broaden GPU, codec or streaming-platform validation.

## 繁體中文（台灣）

這個語系要寫的是台灣人平常會用的華語，不是把簡體字換成繁體字，也不是改寫成粵語。請先讀懂原句，再用自然的句子重寫；可以用「你可以……」「選好後按儲存」「不用另外開程式」這類直接的說法，不必刻意加流行語或語助詞。

專案用詞以內顯、位元率、關鍵影格、設定檔、檔案、資料夾、記憶體、儲存與解除安裝為主。引用 OBS 選單時，請照實際介面標示；顯示卡型號、編碼格式、程式指令與檔名保留原名。請特別確認錯誤條件、數字、單位與 %1／%2 等替代欄位都沒有變動。

翻譯完成後，要在 OBS 與安裝程式裡實際看過文字是否完整，再更新繁中範例圖片。五語檢查會確認鍵名與必要欄位，語氣和語意仍需要人工檢閱。
