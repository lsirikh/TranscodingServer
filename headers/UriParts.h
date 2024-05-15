#pragma once
// UriParts.h
#ifndef URIPARTS_H
#define URIPARTS_H

#include <string>

struct UriParts {
    std::string username;
    std::string password;
    std::string ip;
    std::string port;
    std::string path;
    std::string id;

    void parseUri(const std::string& uri);
};

#endif // URIPARTS_H
