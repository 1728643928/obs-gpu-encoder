# Installer design / 安装器 / インストーラー / 설치 프로그램

## English

The complete installer is in **installer/setup.iss** and **installer/messages.iss**, built with official Inno Setup 6.7.3. The compiler and Simplified Chinese translation are pinned to the upstream release; translations/third-party notices are retained. Five languages are offered on every interactive launch. Local builds may pass **--compiler PATH/ISCC.exe** to the build-installer script.

Discovery checks public OBS installation registry keys and common folders first. Full scan is opt-in and uses Win32 fixed-drive detection. It walks directory entries in bounded timer batches (at most 200 entries per tick), maintains its own directory stack and closes search handles on stop/exit. It never recursively calls the scanner or pumps messages inside a file operation. The UI stays available for stopping. Access-denied locations and reparse-point children are skipped. Discovery checks for **bin/64bit/obs64.exe** and **obs.dll**; installation additionally requires executable version 33.x and **avcodec-62.dll / avutil-60.dll**. No personal document contents or network endpoints are accessed by the scanner.

The user selects one installation. Explicit **/DIR=PATH** is preserved and is not replaced by an automatically discovered location. The installer checks target DLL/worker availability before copying and refuses occupied/unwritable targets. It does not kill OBS or request automatic restarts. Each target has a path-derived uninstall identity. Uninstall removes only recorded project files; unknown files under the plugin directory remain. No wildcard recursive deletion is used. Profiles, GPU preferences and other plugins are not modified.

The published setup requests administrator access by default to support Program Files. **/CURRENTUSER** is available for writable portable installations. For visual testing only, compile **/DRequiredPrivileges=lowest /O<test-output> /FInstaller-UI-Test**; this changes only the test executable's privilege requirement. Do not distribute a test variant under the release asset name. Automated installation tests use the actual release installer with **/CURRENTUSER**, a real OBS version resource and placeholder DLLs in disposable fixtures; no fixture DLL is loaded. Hardware tests are separate.

Limitations: local fixed drive letters only; no traversal into mount points/junctions/symlinks, inaccessible directories or network/removable drives. Manual browsing can target an accessible installation outside scan scope. Huge directory trees can take time. No claim of finding every protected or unmounted installation is made. The build is unsigned; code signing is optional future work requiring the distributor's own certificate.

## 简体中文

安装器全部源码位于 **installer/**，由固定版本的官方 Inno Setup 编译。启动时可选五种语言。先查常见位置与 OBS 安装注册表，再由用户决定是否扫描所有本地固定磁盘。扫描按定时器分批处理目录，随时可停止；不读取个人文档、不上传数据。无权限目录和重解析点/链接会跳过，网络盘和可移动盘请手动浏览。

安装前验证 OBS 33.x 与 DLL 布局，并检查目标文件占用/写权限。显式 **/DIR** 不会被自动发现结果覆盖。不结束 OBS、不改显卡设置、不改配置。每个安装位置有独立的卸载身份，卸载只移除记录中的本插件文件，保留未知文件及其他插件。受保护目录默认申请管理员权限，便携安装可用 **/CURRENTUSER**。

## 日本語

全インストーラーソースは **installer/** にあり、固定版の公式 Inno Setup でビルドします。起動時に 5 言語を選択できます。一般的な場所と OBS レジストリ情報を確認し、必要ならローカル固定ドライブ全体を検索します。タイマーでディレクトリを分割処理し、停止可能です。個人文書は読まず、送信しません。アクセス不可やリンクは除外し、ネットワークやリムーバブルドライブは手動指定します。

OBS 33.x、DLL 構成、対象ファイルの占有/書き込み可否を検証します。明示した **/DIR** を自動検出で上書きしません。OBS を強制終了せず、GPU 設定やプロファイルを変更しません。配置先ごとにアンインストール情報を持ち、記録した本プラグインのファイルのみ除去します。保護フォルダーでは通常管理者権限を要求し、ポータブル版は **/CURRENTUSER** を使用できます。

## 한국어

설치 프로그램 전체 소스는 **installer/**에 있으며 고정 버전 공식 Inno Setup으로 빌드합니다. 시작할 때 다섯 언어를 선택할 수 있습니다. 일반 위치와 OBS 레지스트리를 확인하고 필요하면 로컬 고정 드라이브 전체를 검색합니다. 타이머로 디렉터리를 나누어 처리하며 중지할 수 있습니다. 개인 문서를 읽거나 업로드하지 않습니다. 접근 불가 폴더와 링크는 제외하며 네트워크·이동식 드라이브는 직접 지정합니다.

OBS 33.x, DLL 구성, 대상 파일 점유/쓰기 가능 여부를 확인합니다. 명시적 **/DIR**은 자동 감지 결과로 바뀌지 않습니다. OBS를 강제 종료하거나 GPU 설정·프로필을 변경하지 않습니다. 경로별 제거 정보를 사용하며 기록된 플러그인 파일만 지웁니다. 보호된 폴더에는 기본적으로 관리자 권한을 요청하며 포터블 설치는 **/CURRENTUSER**를 사용할 수 있습니다.

## 繁體中文（台灣）

安裝程式提供五種語言，台灣繁中的命令列選項是 **/LANG=tw**。請選 OBS 安裝根目錄，外掛會自動放入對應的 **obs-plugins/64bit/**。完整掃描只搜尋本機固定磁碟的資料夾，可以隨時停止，不會上傳檔案。安裝或解除安裝前請先關閉 OBS；無效路徑、缺少必要 DLL，以及使用中的目標檔案都會被拒絕。
