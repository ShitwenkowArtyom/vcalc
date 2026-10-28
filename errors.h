#pragma once

#include <stdexcept>
#include <string>

class ServerError : public std::runtime_error {
public:
    explicit ServerError(const std::string& msg) : std::runtime_error(msg) {}
};

class FileError : public ServerError {
public:
    explicit FileError(const std::string& msg) : ServerError(msg) {}
};

class AuthError : public ServerError {
public:
    explicit AuthError(const std::string& msg) : ServerError(msg) {}
};

class NetworkError : public ServerError {
public:
    explicit NetworkError(const std::string& msg) : ServerError(msg) {}
};