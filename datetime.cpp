#include "datetime.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <sstream>

std::ostringstream getCurrentTime() {
    // 현재 시간 가져오기
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    // 현재 시간을 포맷팅하여 출력
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S")
        << '.' << std::setw(3) << std::setfill('0') << now_ms.count();
    
    return oss;
}
