// Original, dependency-free README artwork. Run: node assets/readme/generate.mjs
import { writeFile } from 'node:fs/promises';

const copy = {
  en: {
    subtitle: 'Choose the encoding GPU. Keep OBS on your gaming GPU.',
    render: '01 / RENDER', encode: '02 / ENCODE',
    renderTitle: 'Game + OBS preview', renderDetail: 'On your gaming GPU',
    encodeTitle: 'Your chosen AMD GPU', encodeDetail: 'An iGPU or another AMD card',
    footer: 'Streaming controls and audio stay in OBS.',
    description: 'Conceptual GPU roles: the game and OBS preview stay on the rendering GPU; an independently chosen AMD GPU encodes. Frames pass through system memory. OBS handles streaming and audio.'
  },
  'zh-cn': {
    subtitle: '独显继续负责游戏与 OBS 渲染，编码显卡由你选择。',
    render: '01 / 渲染', encode: '02 / 编码',
    renderTitle: '游戏与 OBS 预览', renderDetail: '继续使用游戏显卡',
    encodeTitle: '你指定的 AMD 显卡', encodeDetail: '核显，或另一张 AMD 显卡',
    footer: '直播控制与音频仍由 OBS 处理。',
    description: '显卡分工示意：游戏与 OBS 预览继续使用渲染显卡，另行指定 AMD 显卡编码。画面经系统内存传输，OBS 负责网络推流与音频。'
  },
  'zh-tw': {
    subtitle: '遊戲和 OBS 繼續用獨顯，編碼用哪張卡由你決定。',
    render: '01 / 渲染', encode: '02 / 編碼',
    renderTitle: '遊戲與 OBS 預覽', renderDetail: '維持使用遊戲用顯示卡',
    encodeTitle: '你指定的 AMD 顯示卡', encodeDetail: '內顯，或另一張 AMD 顯示卡',
    footer: '串流控制與音訊，照常交給 OBS。',
    description: '顯示卡分工示意：遊戲與 OBS 預覽維持使用渲染顯示卡，編碼則交給指定的 AMD 顯示卡。影格經過系統記憶體傳輸，串流與音訊由 OBS 處理。'
  },
  ja: {
    subtitle: 'ゲーム用 GPU はそのまま。エンコード用 GPU を選べます。',
    render: '01 / 描画', encode: '02 / エンコード',
    renderTitle: 'ゲームと OBS プレビュー', renderDetail: 'ゲーム用 GPU で描画',
    encodeTitle: '指定した AMD GPU', encodeDetail: '内蔵 GPU または別の AMD GPU',
    footer: '配信操作と音声処理は引き続き OBS が担当。',
    description: 'GPU の役割を示す概念図。ゲームと OBS プレビューは描画 GPU に残し、指定した AMD GPU でエンコードします。フレームはシステムメモリを経由し、OBS が配信と音声を担当します。'
  },
  ko: {
    subtitle: '게임 GPU는 그대로. 인코딩할 GPU는 직접 선택하세요.',
    render: '01 / 렌더링', encode: '02 / 인코딩',
    renderTitle: '게임과 OBS 미리 보기', renderDetail: '게임용 GPU에서 렌더링',
    encodeTitle: '직접 선택한 AMD GPU', encodeDetail: '내장 GPU 또는 다른 AMD 그래픽 카드',
    footer: '방송 제어와 오디오는 계속 OBS가 담당합니다.',
    description: 'GPU 역할 개념도. 게임과 OBS 미리 보기는 렌더링 GPU에서 실행하고 선택한 AMD GPU가 인코딩합니다. 프레임은 시스템 메모리를 거치며 OBS가 방송과 오디오를 처리합니다.'
  }
};

const escapeXml = s => s.replaceAll('&', '&amp;').replaceAll('<', '&lt;').replaceAll('>', '&gt;').replaceAll('"', '&quot;');
const text = (x, y, size, fill, content, extra = '') =>
  '  <text x="' + x + '" y="' + y + '" font-size="' + size + '" fill="' + fill + '" ' + extra + '>' + escapeXml(content) + '</text>';

for (const [language, c] of Object.entries(copy)) {
  const svg = [
    '<svg xmlns="http://www.w3.org/2000/svg" width="1200" height="448" viewBox="0 0 1200 448" role="img" aria-labelledby="title desc">',
    '  <title id="title">GPU Encoder Bridge for OBS</title>',
    '  <desc id="desc">' + escapeXml(c.description) + '</desc>',
    '  <rect width="1200" height="448" rx="20" fill="#12191f"/>',
    '  <g font-family="-apple-system, BlinkMacSystemFont, &apos;Segoe UI&apos;, &apos;Microsoft YaHei&apos;, &apos;Yu Gothic&apos;, &apos;Malgun Gothic&apos;, sans-serif">',
    text(56, 53, 20, '#a8b4be', 'OBS STUDIO / WINDOWS x64', 'letter-spacing="1.4"'),
    text(1144, 53, 20, '#a8b4be', 'AMD AMF', 'text-anchor="end" letter-spacing="1.4"'),
    text(56, 137, 66, '#f5f3ee', 'GPU Encoder Bridge', 'font-weight="700" letter-spacing="-1.7"'),
    text(56, 185, 26, '#c2ccd3', c.subtitle),
    '  <path d="M56 222H1144" stroke="#35424d"/>',
    '  <g id="render-role">',
    text(56, 264, 20, '#ff9d83', c.render, 'font-weight="600" letter-spacing="1"'),
    text(56, 313, language === 'ja' ? 31 : 34, '#f5f3ee', c.renderTitle, 'font-weight="600"'),
    text(56, 352, 23, '#a8b4be', c.renderDetail),
    '  </g>',
    '  <path d="M558 305H622M610 293L622 305L610 317" fill="none" stroke="#91d6ba" stroke-width="3" stroke-linecap="round" stroke-linejoin="round"/>',
    '  <g id="encoding-role">',
    text(660, 264, 20, '#91d6ba', c.encode, 'font-weight="600" letter-spacing="1"'),
    text(660, 313, 34, '#f5f3ee', c.encodeTitle, 'font-weight="600"'),
    text(660, 352, language === 'ko' ? 22 : 23, '#a8b4be', c.encodeDetail),
    '  </g>',
    '  <path d="M56 383H1144" stroke="#35424d"/>',
    text(56, 418, 21, '#c2ccd3', c.footer),
    '  </g>',
    '</svg>',
    ''
  ].join('\n');
  await writeFile(new URL('hero-' + language + '.svg', import.meta.url), svg);
}
