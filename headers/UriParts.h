#pragma once
// UriParts.h
#ifndef URIPARTS_H
#define URIPARTS_H

#include <string>
#include <chrono>

struct MountPoints {
    std::string uri_first;
    std::string uri_second;
    std::string uri_third;
    std::string uri_fourth;
};

struct UriParts {
    std::string username;
    std::string password;
    std::string ip;
    std::string port;
    std::string path;
    std::string id;
    std::string full_path;
    MountPoints mount_points;

    void parseUri(const std::string& uri);
private:
    std::string generate_uuid();
};

#endif // URIPARTS_H
