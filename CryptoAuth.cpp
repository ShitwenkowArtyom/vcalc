#include "CryptoAuth.h"

#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>

#include <string>

bool CryptoAuth::isHexUpper(const std::string& s) {
    if (s.empty()) {
        return false;
    }

    for (char c : s) {
        bool digit = (c >= '0' && c <= '9');
        bool letter = (c >= 'A' && c <= 'F');

        if (!digit && !letter) {
            return false;
        }
    }

    return true;
}

bool CryptoAuth::verifyHashSha1(
    const std::string& hashHex,
    const std::string& saltHex,
    const std::string& password
) {
    try {
        std::string binaryHash;

        CryptoPP::StringSource(
            hashHex,
            true,
            new CryptoPP::HexDecoder(
                new CryptoPP::StringSink(binaryHash)
            )
        );

        std::string data = binaryHash;
        data.append(saltHex);
        data.append(password);

        bool verified = false;

        CryptoPP::SHA1 sha1;

        CryptoPP::StringSource(
            data,
            true,
            new CryptoPP::HashVerificationFilter(
                sha1,
                new CryptoPP::ArraySink(
                    reinterpret_cast<CryptoPP::byte*>(&verified),
                    sizeof(verified)
                )
            )
        );

        return verified;
    } catch (const CryptoPP::Exception&) {
        return false;
    }
}