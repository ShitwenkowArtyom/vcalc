#include "UserInterface.h"

#include <cstdlib>
#include <iostream>
#include <string>

#include "AuthBase.h"
#include "Logger.h"
#include "Server.h"
#include "errors.h"

static std::string expandHome(const std::string& path) {
    if (path.size() >= 2 && path[0] == '~' && path[1] == '/') {
        const char* home = std::getenv("HOME");

        if (home != nullptr) {
            return std::string(home) + path.substr(1);
        }
    }

    return path;
}

UserInterface::UserInterface() {
    params.authFile = "~/base/vcalc.conf";
    params.logFile = "~/log/vcalc.log";
    params.port = 33333;
}

bool UserInterface::parse(int argc, const char** argv) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            return false;
        }

        if (arg == "-b" || arg == "--base") {
            if (i + 1 >= argc) {
                throw ServerError("No value for option " + arg);
            }

            params.authFile = argv[++i];
        } else if (arg == "-l" || arg == "--log") {
            if (i + 1 >= argc) {
                throw ServerError("No value for option " + arg);
            }

            params.logFile = argv[++i];
        } else if (arg == "-p" || arg == "--port") {
            if (i + 1 >= argc) {
                throw ServerError("No value for option " + arg);
            }

            try {
                params.port = std::stoi(argv[++i]);
            } catch (...) {
                throw ServerError("Bad port value");
            }
        } else {
            throw ServerError("Unknown option: " + arg);
        }
    }

    if (params.port < 1 || params.port > 65535) {
        throw ServerError("Bad port");
    }

    return true;
}

std::string UserInterface::getDescription() const {
    return "Usage: server [options]\n"
           "  -h, --help          Show this help\n"
           "  -b, --base <file>   Auth base file (default: ~/base/vcalc.conf)\n"
           "  -l, --log <file>    Log file (default: ~/log/vcalc.log)\n"
           "  -p, --port <port>   Server port (default: 33333)\n";
}

void UserInterface::run(int argc, const char** argv) {
    if (!parse(argc, argv)) {
        std::cout << getDescription();
        return;
    }

    params.authFile = expandHome(params.authFile);
    params.logFile = expandHome(params.logFile);

    Logger logger(params.logFile);

    logger.logInfo("Server is starting");

    AuthBase base;

    try {
        base.load(params.authFile);
    } catch (const std::exception& e) {
        logger.logError(std::string("Auth base load error: ") + e.what());
        throw;
    }

    logger.logInfo("Auth base loaded: " + std::to_string(base.size()) + " users");

    Server server(logger, base);

    server.run(params);
}