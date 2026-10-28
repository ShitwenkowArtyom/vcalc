#include "ClientHandler.h"
#include "CryptoAuth.h"
#include "Network.h"
#include "errors.h"

#include <cstdint>
#include <string>

#include <sys/socket.h>
#include <sys/time.h>

ClientHandler::ClientHandler(
    int socket,
    const std::string& peer,
    Logger& logger,
    const AuthBase& auth
)
    : clientSocket(socket)
    , peer(peer)
    , logger(logger)
    , auth(auth)
{
}

void ClientHandler::handle() {
    try {
        setSocketTimeouts(clientSocket, 180);

        if (!authenticate()) {
            logger.logWarning("Authentication failed from " + peer);
            sendAll(clientSocket, "ERR");
            return;
        }

        sendAll(clientSocket, "OK");

        logger.logInfo(
            "Successful authentication from " + peer +
            " user " + login
        );

        processVectors();
    } catch (const NetworkError& e) {
        logger.logWarning(
            std::string("Network error with ") + peer + ": " + e.what()
        );
    } catch (const ServerError& e) {
        logger.logError(
            std::string("Server error with ") + peer + ": " + e.what()
        );
    } catch (...) {
        logger.logError("Unknown server error");
    }
}

bool ClientHandler::authenticate() {
    std::string msg = recvMessage(clientSocket);

    if (msg.size() <= CryptoAuth::SALT_HEX_LEN + CryptoAuth::HASH_HEX_LEN) {
        return false;
    }

    std::string hash = msg.substr(msg.size() - CryptoAuth::HASH_HEX_LEN);

    std::string salt = msg.substr(
        msg.size() - CryptoAuth::HASH_HEX_LEN - CryptoAuth::SALT_HEX_LEN,
        CryptoAuth::SALT_HEX_LEN
    );

    std::string tryLogin = msg.substr(
        0,
        msg.size() - CryptoAuth::HASH_HEX_LEN - CryptoAuth::SALT_HEX_LEN
    );

    if (!CryptoAuth::isHexUpper(salt) || !CryptoAuth::isHexUpper(hash)) {
        return false;
    }

    std::string password;

    if (!auth.getPassword(tryLogin, password)) {
        return false;
    }

    if (!CryptoAuth::verifyHashSha1(hash, salt, password)) {
        return false;
    }

    login = tryLogin;

    return true;
}

void ClientHandler::processVectors() {
    std::uint32_t numVectors = 0;

    recvExact(clientSocket, &numVectors, sizeof(numVectors));

    logger.logInfo(
        "Client " + peer + " will send " +
        std::to_string(numVectors) + " vectors"
    );

    for (std::uint32_t i = 0; i < numVectors; i++) {
        std::uint32_t len = 0;

        recvExact(clientSocket, &len, sizeof(len));

        if (len == 0 || len > 1000000) {
            throw ServerError("Bad vector size");
        }

        Vector v(len);

        recvExact(clientSocket, v.data(), len * sizeof(v[0]));

        std::int32_t result = calculator.average(v);

        std::uint32_t resultsCount = 1;

        sendAll(
            clientSocket,
            std::string(
                reinterpret_cast<char*>(&resultsCount),
                sizeof(resultsCount)
            )
        );

        sendAll(
            clientSocket,
            std::string(reinterpret_cast<char*>(&result), sizeof(result))
        );

        logger.logInfo(
            "Calculated result for " + peer +
            ": " + std::to_string(result)
        );
    }
}