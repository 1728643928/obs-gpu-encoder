# Native test harnesses

| File | What it verifies / 验证内容 | GPU required |
| --- | --- | --- |
| [protocol-test.cpp](protocol-test.cpp) | Settings/UTF-8 roundtrips and malformed/oversized/truncated message rejection. / 协议往返和异常消息边界。 | No |
| [adapter-policy-test.cpp](adapter-policy-test.cpp) | Physical/iGPU retention, indirect/software rejection, unavailable-query behavior and identical-model real cards. / 显卡筛选策略。 | No |
| [backend-smoke.cpp](backend-smoke.cpp) | Direct backend capabilities and encoding behavior. / 直接调用后端的能力及编码测试。 | Yes, AMD |
| [worker-smoke.cpp](worker-smoke.cpp) | Actual worker path: 300 frames, packet/timestamp/keyframe checks and changed-LUID rejection. / 独立进程完整编码链路与设备身份拒绝。 | Yes, AMD |
| [lockfile.cpp](lockfile.cpp) | Holds a fixture file with an exclusive Win32 handle so installer refusal can be tested. / 为安装器占用保护提供文件锁。 | No |

**node scripts/build.mjs --download-obs --tests** builds the harnesses and runs CPU-only protocol/adapter tests. Hardware smoke tests are run explicitly on an appropriate machine; hosted CI success alone does not prove hardware encoding or game performance.

The JavaScript [installer test](../scripts/test-installer.mjs) uses the lock helper and disposable OBS-layout fixtures. Fixture DLL placeholders are never loaded. Installation tests do not replace an OBS runtime test.

Follow [testing instructions](../docs/testing.md) for commands and [validation records](../docs/validation.md) for the checks actually completed.

## 简体中文

协议和筛选策略可在无显卡的 CI 中验证；两个 smoke 程序需要实际 AMD 编码设备。lockfile 只是安装测试辅助程序。请区分“构建通过”“安装通过”和“OBS 实际编码通过”，并按验证记录陈述结果。

## 日本語

プロトコルとアダプター方針は GPU なしで検証できます。smoke テストは AMD 実機が必要です。lockfile はインストール検証用です。ビルド成功、配置成功、OBS でのエンコード成功を区別してください。

## 한국어

프로토콜과 어댑터 정책은 GPU 없이 검사할 수 있습니다. smoke 테스트에는 실제 AMD 장치가 필요하며 lockfile은 설치 검증용입니다. 빌드 성공, 설치 성공과 실제 OBS 인코딩 성공을 구분하세요.
