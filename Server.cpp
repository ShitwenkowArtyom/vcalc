#include "Server.h"
#include "ClientHandler.h"
#include "Network.h"
#include "errors.h"

#include <string>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

Server::Server(Logger& logger, const AuthBase& auth)
    : logger(logger)
    , auth(auth)
{
}

void Server::run(const Params& params) {
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (listenSocket == -1) {
        throw NetworkError("socket error");
    }

    sockaddr_in* addr = new sockaddr_in;
    addr->sin_family = AF_INET;
    addr->sin_port = htons(static_cast<unsigned short>(params.port));
    addr->sin_addr.s_addr = htonl(INADDR_ANY);

    int reuse = 1;
    setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    if (bind(listenSocket, reinterpret_cast<sockaddr*>(addr), sizeof(sockaddr_in)) == -1) {
        delete addr;
        throw NetworkError("bind error");
    }

    delete addr;

    if (listen(listenSocket, 16) == -1) {
        throw NetworkError("listen error");
    }

    logger.logInfo("Server started on port " + std::to_string(params.port));

    while (true) {
        sockaddr_in* peerAddr = new sockaddr_in;
        socklen_t peerLen = sizeof(sockaddr_in);

        int clientSocket = accept(
            listenSocket,
            reinterpret_cast<sockaddr*>(peerAddr),
            &peerLen
        );

        if (clientSocket == -1) {
            delete peerAddr;
            throw NetworkError("accept error");
        }

        std::string peer =
            std::string(inet_ntoa(peerAddr->sin_addr)) +
            ":" +
            std::to_string(ntohs(peerAddr->sin_port));

        delete peerAddr;

        logger.logInfo("New connection from " + peer);

        ClientHandler handler(clientSocket, peer, logger, auth);

        handler.handle();

        close(clientSocket);

        logger.logInfo("Session finished for " + peer);
    }
}