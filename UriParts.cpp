// UriParts.cpp
#include "UriParts.h"
#include <sstream>
#include <stdexcept>
#include <vector>
#include <random>

std::string UriParts::generate_uuid() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::uniform_int_distribution<> dis2(8, 11);

    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 8; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 4; i++) ss << dis(gen);
    ss << "-4";
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    ss << dis2(gen);
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 12; i++) ss << dis(gen);
    return ss.str();
}

// std::string UriParts::generate_simple_id() {
//     using namespace std::chrono;
//     auto now = system_clock::now();
//     auto duration = now.time_since_epoch();
//     auto millis = duration_cast<milliseconds>(duration).count();
//     return std::to_string(millis);
// }

void UriParts::parseUri(const std::string& iid, const std::string& uri) {
    full_path = uri; // Save the full URI
    size_t pos = uri.find("//");
    if (pos == std::string::npos) {
        throw std::runtime_error("URI does not contain '//' for username");
    }
    pos += 2;

    size_t atPos = uri.find('@', pos);
    if (atPos != std::string::npos) {
        size_t colonPos = uri.find(':', pos);
        if (colonPos != std::string::npos && colonPos < atPos) {
            username = uri.substr(pos, colonPos - pos);
            password = uri.substr(colonPos + 1, atPos - colonPos - 1);
        } else {
            username = uri.substr(pos, atPos - pos);
            password = "";
        }
        pos = atPos + 1;
    } else {
        username = "";
        password = "";
    }

    std::string rest = uri.substr(pos);
    size_t slashPos = rest.find('/');
    std::string address = (slashPos != std::string::npos) ? rest.substr(0, slashPos) : rest;
    path = (slashPos != std::string::npos) ? rest.substr(slashPos + 1) : "";

    size_t colonPos = address.find(':');
    if (colonPos != std::string::npos) {
        ip = address.substr(0, colonPos);
        port = address.substr(colonPos + 1);
    } else {
        ip = address;
        port = "554";
    }

    id = iid;

    mount_points.uri_first = "/" + id + "/first";
    mount_points.uri_second = "/" + id + "/second";
    mount_points.uri_third = "/" + id + "/third";
    mount_points.uri_fourth = "/" + id + "/fourth";
}

