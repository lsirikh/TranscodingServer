// UriParts.cpp
#include "UriParts.h"
#include <sstream>
#include <stdexcept>
#include <vector>

void UriParts::parseUri(const std::string& uri) {
    size_t pos = uri.find("//");
    if (pos == std::string::npos) {
        throw std::runtime_error("URI does not contain '//' for username");
    }
    pos += 2; 

    size_t atPos = uri.find('@', pos);
    if (atPos == std::string::npos) {
        atPos = uri.length();
    }
    size_t colonPos = uri.find(':', pos); 
    if (colonPos != std::string::npos && colonPos < atPos) {
        username = uri.substr(pos, colonPos - pos);
        password = uri.substr(colonPos + 1, atPos - colonPos - 1);
    }
    else {
        username = uri.substr(pos, atPos - pos);
    }

    pos = atPos;
    if (pos < uri.length()) {
        std::string rest = uri.substr(pos + 1);
        size_t slashPos = rest.find('/');
        std::string address = (slashPos != std::string::npos) ? rest.substr(0, slashPos) : rest;
        path = (slashPos != std::string::npos) ? rest.substr(slashPos + 1) : "";

        colonPos = address.find(':');
        if (colonPos != std::string::npos) {
            ip = address.substr(0, colonPos);
            port = address.substr(colonPos + 1);
        }
        else {
            ip = address; 
            port = "554";
        }

        std::istringstream ipStream(ip);
        std::vector<std::string> ipParts;
        std::string ipSegment;
        while (std::getline(ipStream, ipSegment, '.')) {
            ipParts.push_back(ipSegment);
        }
        if (!ipParts.empty()) {
            id = ipParts.back();
        }
    }
}
