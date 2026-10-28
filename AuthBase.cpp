#include "AuthBase.h"
#include "errors.h"

#include <fstream>
#include <string>

void AuthBase::load(const std::string& fileName) {
    std::ifstream file(fileName);

    if (!file.is_open()) {
        throw FileError("Cannot open auth base file: " + fileName);
    }

    base.clear();

    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        std::size_t pos = line.find(':');

        if (pos == std::string::npos) {
            throw AuthError("Bad auth base line: " + line);
        }

        base[line.substr(0, pos)] = line.substr(pos + 1);
    }
}

bool AuthBase::getPassword(const std::string& login, std::string& password) const {
    auto it = base.find(login);

    if (it == base.end()) {
        return false;
    }

    password = it->second;

    return true;
}

std::size_t AuthBase::size() const {
    return base.size();
}