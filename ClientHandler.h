#pragma once

#include <string>

#include "AuthBase.h"
#include "Calculator.h"
#include "Logger.h"

class ClientHandler {
public:
    ClientHandler(
        int socket,
        const std::string& peer,
        Logger& logger,
        const AuthBase& auth
    );

    void handle();

private:
    int clientSocket;
    std::string peer;
    Logger& logger;
    const AuthBase& auth;
    Calculator calculator;
    std::string login;

    bool authenticate();
    void processVectors();
};