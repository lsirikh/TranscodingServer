cmake_minimum_required(VERSION 3.10)

# 프로젝트 이름과 버전
project(RTSPTranscodingServer VERSION 1.0)

# C++ 표준 설정
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# GStreamer 패키지 찾기
find_package(PkgConfig REQUIRED)
pkg_check_modules(GSTREAMER REQUIRED gstreamer-1.0>=1.4)
pkg_check_modules(GSTREAMER_RTSP_SERVER REQUIRED gstreamer-rtsp-server-1.0>=1.4)

# 헤더 파일 포함 디렉토리 설정
include_directories(${GSTREAMER_INCLUDE_DIRS})
include_directories(${GSTREAMER_RTSP_SERVER_INCLUDE_DIRS})
include_directories(headers)

# 링크 라이브러리 설정
link_directories(${GSTREAMER_LIBRARY_DIRS})
link_directories(${GSTREAMER_RTSP_SERVER_LIBRARY_DIRS})

# 컴파일 대상 설정
add_executable(RTSPTranscodingServer main.cpp TranscodingService.cpp UriParts.cpp datetime.cpp)

# 링크 설정
target_link_libraries(RTSPTranscodingServer ${GSTREAMER_LIBRARIES} ${GSTREAMER_RTSP_SERVER_LIBRARIES} gstreamer-1.0 gstrtsp-1.0 pthread)
