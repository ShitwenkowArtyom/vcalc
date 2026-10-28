#pragma once

#include <cstddef>
#include <map>
#include <string>

class AuthBase {
public:
    void load(const std::string& fileName);

    bool getPassword(const std::string& login, std::string& password) const;

    std::size_t size() const;

private:
    std::map<std::string, std::string> base;
};