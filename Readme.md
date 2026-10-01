# RTSP Transcoding Server

RTSP 카메라 영상을 받아 중계하거나 해상도·프레임레이트를 변환해 다시 RTSP로 제공하는 C++ 서버입니다. GStreamer의 RTSP media factory로 출력 경로를 만들고, Crow HTTP API로 입력 채널을 추가·삭제합니다.

## 영상 처리 흐름

```text
RTSP 입력 → H.264 RTP depayload → 원본 중계 또는 디코딩·크기 변경·인코딩 → RTSP 출력
                                  ↑
                           Crow HTTP 채널 관리 API
```

## 출력 프로파일

| 경로 | 처리 |
|---|---|
| `/{id}/first` | H.264 depayload / payload를 통한 중계 |
| `/{id}/second` | 1280×960, 15 fps, H.264 인코딩 |
| `/{id}/third` | 640×480, 10 fps, H.264 인코딩 |
| `/{id}/fourth` | 320×240, 10 fps, H.264 인코딩 |

변환 파이프라인은 `nvh264dec`로 디코딩하고 `x264enc`로 인코딩합니다. 따라서 NVIDIA 디코더 플러그인과 해당 실행 환경이 필요합니다. 표의 값은 코드에 설정된 출력 조건이며 측정 성능이 아닙니다.

## HTTP API

| 메서드 | 경로 | 기능 |
|---|---|---|
| GET | `/get-config` | 서버 설정 조회 |
| POST | `/add` | 입력 채널 등록 |
| POST | `/remove` | 입력 채널 삭제 |
| GET | `/get-sessions` | 세션 조회 |
| GET | `/get-medias` | 미디어 조회 |

`/add` 요청 형식:

```json
{"rtsp_uris":[{"id":"camera01","url":"rtsp://camera.example/stream"}]}
```

`/remove` 요청 형식:

```json
{"rtsp_ids":[{"id":"camera01"}]}
```

현재 소스의 HTTP 포트는 `8080`, RTSP 포트는 `8555`입니다. 서버 주소와 입력 URL은 실행 환경에 맞게 설정해야 합니다.

## 코드와 빌드 조건

- [main.cpp](main.cpp): HTTP 라우트와 서버 시작
- [TranscodingService.cpp](TranscodingService.cpp): RTSP 서버, media factory, 변환 파이프라인
- [UriParts.cpp](UriParts.cpp): 입력 URL과 출력 경로 처리
- [CMakeLists.txt](CMakeLists.txt): C++17 / CMake 3.10 이상, GStreamer 개발 패키지 구성

GStreamer core/base/video/RTSP server 개발 패키지, Crow 헤더 및 관련 의존성이 필요합니다. 현재 CMake는 `GST` 이름으로 패키지를 검색하지만 링크 단계에는 `GSTREAMER_*` 변수도 사용합니다. 빌드 전에 변수와 RTSP server 링크 설정을 맞춰야 하며, 저장된 빌드 결과물을 새 환경에서 그대로 사용하는 방식은 권장하지 않습니다.

## 사용 범위

현재 API에는 접근 인증을 추가해야 합니다. 입력 URL의 인증 정보가 응답이나 로그에 포함될 수 있으므로 실제 카메라 계정을 넣기 전에 해당 처리를 수정해야 합니다. 세션 수 제한 해제 설정은 무제한 처리 성능을 의미하지 않습니다. 장시간 운전·동시 접속 부하에 대한 보장 수치는 없습니다.
