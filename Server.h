#pragma once

#include "AuthBase.h"
#include "Logger.h"
#include "Params.h"

class Server {
public:
    Server(Logger& logger, const AuthBase& auth);

    void run(const Params& params);

private:
    Logger& logger;
    const AuthBase& auth;
};