#pragma once

#include <cstddef>
#include <string>

class CryptoAuth {
public:
    static constexpr std::size_t SALT_HEX_LEN = 16;
    static constexpr std::size_t HASH_HEX_LEN = 40;

    static bool verifyHashSha1(
        const std::string& hashHex,
        const std::string& saltHex,
        const std::string& password
    );

    static bool isHexUpper(const std::string& s);
};