# GPU Encoder Bridge for OBS

실제 GPU 이름으로 인코딩 장치를 선택하고 OBS는 다른 GPU에서 렌더링하도록 유지합니다. OBS의 기본 방송 서비스, 오디오 및 방송 시작/중지 버튼을 그대로 사용합니다.

**0.2.0-preview · Windows x64 · OBS Studio 33.x · AMD AMF · SDR / 8비트**

[English](../README.md) · [简体中文](README.zh-CN.md) · [日本語](README.ja-JP.md) · [한국어](README.ko-KR.md)

[다운로드 / Releases](https://github.com/1728643928/obs-gpu-encoder/releases) · [문제 신고](https://github.com/1728643928/obs-gpu-encoder/issues)

## 기능

- OBS 기본 인코더 목록에 **AMD Radeon(TM) Graphics — H.264 [GPU 1 / AMF Bridge]**처럼 GPU 이름과 형식을 표시합니다.
- 실제 인코딩 기능을 확인한 AMD GPU에만 H.264, HEVC, AV1을 제공합니다. 방송 서비스도 형식을 제한합니다.
- 소프트웨어 및 표시 전용 간접 어댑터를 제외하며 같은 모델의 실제 GPU는 각각 유지합니다.
- CBR / VBR, 비트레이트, 키프레임 간격, 속도/균형/품질 설정을 제공합니다.
- 내부 작업 프로세스로 AMF를 격리합니다. 별도 제어 앱이나 FFmpeg 명령줄 프로그램이 필요 없습니다.
- 플러그인, 설치 프로그램, 시작 문서를 영어·중국어·일본어·한국어로 제공합니다.

Windows GPU 설정, 모니터 케이블, OBS 렌더링 GPU를 바꾸지 않아도 됩니다. 플러그인은 계정이나 방송 키를 처리하지 않습니다. 오디오와 네트워크 송출은 OBS가 담당합니다.

## 설치 및 방송

1. OBS를 종료하고 Releases에서 **windows-x64-setup.exe**를 다운로드합니다.
2. 언어와 검색된 OBS 설치 위치를 선택합니다. 모든 로컬 고정 드라이브를 검색하고 중지하거나 직접 경로를 지정할 수 있습니다.
3. OBS 루트 폴더를 확인합니다. OBS 33.x x64 버전과 필수 DLL 구성을 검사합니다.
4. OBS 설정 → 출력 → 고급 → 방송에서 원하는 GPU 이름의 **AMF Bridge** 인코더를 선택합니다.
5. OBS 기본 서비스 설정과 방송 시작 버튼을 사용합니다.

전체 검색은 로컬 고정 드라이브의 디렉터리를 확인하며 개인 문서 내용을 읽거나 업로드하지 않습니다. 접근할 수 없는 폴더와 재분석 지점/링크는 건너뜁니다. 네트워크·이동식 드라이브와 링크로만 마운트된 볼륨은 직접 경로를 지정하세요. 큰 드라이브는 검색 시간이 걸릴 수 있습니다. 기본 설치는 관리자 권한을 요청합니다. 포터블 설치에는 **/CURRENTUSER**를 사용할 수 있습니다.

수동 ZIP의 **obs-plugins**, **data**를 OBS 루트에 합칠 수도 있습니다. DLL과 worker EXE를 함께 설치하고 OBS 기본 DLL을 덮어쓰지 마세요.

시작 설정 예시는 **H.264, CBR, 6000 Kbps, 키프레임 2초, 균형**입니다. 비트레이트는 서비스 조건에 맞추세요. Twitch Enhanced Broadcasting은 인코더 선택을 바꿀 수 있으므로 GPU를 고정하려면 끄세요. GPU 번호는 현재 DXGI 순서이며 하드웨어 변경 후 다시 확인해야 합니다. 오류가 발생해도 다른 GPU나 소프트웨어로 자동 전환하지 않습니다.

## 지원 범위와 검증

현재 AMD AMF만 구현되어 있습니다. Intel QSV, NVIDIA NVENC, HDR, 10비트는 미구현입니다. SDR / NV12 / 8비트와 OBS 33 / avcodec 62 / avutil 60을 지원합니다. 프레임은 시스템 메모리를 거쳐 전달되며 GPU 간 제로 카피가 아닙니다. 게임 성능 향상은 보장하지 않습니다.

RX 9060 XT 렌더링과 9700X 내장 GPU H.264 인코딩 환경에서 확인했습니다. 이전 OBS 로컬 RTMP 테스트에서 약 47초의 영상과 오디오를 수신·디코딩했습니다. 0.2.0에서도 300프레임 테스트를 통과했습니다. HEVC/AV1은 기능 확인만 수행했습니다. 공개 방송 플랫폼 수신, 게임 성능, 다른 GPU와 OBS 버전은 검증하지 않았습니다. 미리 보기 버전입니다.

## 디렉터리와 파일의 역할

GitHub 파일 목록 가운데 열은 **마지막 커밋 메시지**입니다. 한 커밋에서 여러 경로를 변경하면 같은 메시지가 표시됩니다. 디렉터리별 README에 자세한 역할이 있습니다.

| 디렉터리 | 내용 |
| --- | --- |
| [.github/](../.github/README.md) | CI, 검증, 게시와 버그 템플릿. |
| [data/](../data/README.md) | OBS 플러그인의 네 언어 리소스. |
| [docs/](README.md) | 시작, 구조, 검증과 인수 가이드. |
| [installer/](../installer/README.md) | EXE, OBS 검색, 경로 검사와 번역. |
| [scripts/](../scripts/README.md) | 빌드, 번역 검사, 설치 테스트, 패키징과 게시. |
| [src/](../src/README.md) | OBS 연결, AMD 백엔드, GPU 정책과 IPC. |
| [tests/](../tests/README.md) | CPU 회귀, 실제 GPU 테스트와 파일 잠금 도구. |
| [third-party/](../third-party/README.md) | 런타임, 도구체인과 설치 프로그램의 제삼자 고지. |

루트의 [CHANGELOG.md](../CHANGELOG.md)는 변경 이력, [CONTRIBUTING.md](../CONTRIBUTING.md)는 기여 절차, [GOVERNANCE.md](../GOVERNANCE.md)는 권한 및 후속 유지보수, [LICENSE](../LICENSE)는 라이선스, [THIRD-PARTY.md](../THIRD-PARTY.md)는 의존성, [version.json](../version.json)은 빌드 버전을 담습니다. [.gitattributes](../.gitattributes)는 줄바꿈, [.gitignore](../.gitignore)는 생성물 제외를 관리합니다.

## 빌드 및 인수

Windows x64, Node.js 22+, Git이 필요합니다.

~~~powershell
node scripts/check-locales.mjs
node scripts/build.mjs --download-obs --tests
node scripts/build-installer.mjs
node scripts/package.mjs
~~~

**--download-obs** 대신 **--obs-root "C:\Program Files\obs-studio"**를 사용할 수 있습니다. 공식 도구와 소스는 고정 SHA-256으로 확인합니다. 결과는 **dist/**와 **release/**에 생성됩니다. 빌드는 OBS 설정이나 설치를 변경하지 않습니다.

전체 소스, 설치 프로그램 소스, 번역, 테스트, 워크플로를 **GPL-2.0-or-later**로 공개하며 제삼자 고지를 유지합니다. 비공개 유료 구성 요소, CLA, 원격 측정, 온라인 활성화가 없습니다. 누구나 Fork, 분기, 수정 및 재배포할 수 있습니다. Fork는 자체 GitHub Actions의 임시 토큰으로 Releases를 게시할 수 있습니다. 원본 저장소에 게시하려면 소유자가 부여한 쓰기 권한이 필요합니다.

[기여](../CONTRIBUTING.md), [운영 및 인수](../GOVERNANCE.md), [릴리스](releasing.md), [설치 프로그램](installer.md), [검증](validation.md), [제삼자 고지](../THIRD-PARTY.md)를 참고하세요.

## 제거

OBS를 종료한 후 Windows 설치된 앱 또는 OBS 아래 **data/obs-plugins/obs-gpu-encoder/uninstall/unins000.exe**를 실행하세요. 설치 기록에 있는 플러그인 파일만 제거하며 OBS, 설정, 다른 플러그인은 유지합니다. 수동 ZIP 설치는 다른 인코더를 선택한 후 이 플러그인의 DLL, worker 및 데이터 폴더를 삭제하세요.
