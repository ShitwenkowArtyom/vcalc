#include "Network.h"
#include "errors.h"

#include <cstring>
#include <string>

#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#ifndef MSG_NOSIGNAL
#define MSG_NOSIGNAL 0
#endif

static void setTimeoutOption(int socket, int option, int seconds) {
    struct timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;

    if (setsockopt(socket, SOL_SOCKET, option, &timeout, sizeof(timeout)) == -1) {
        throw NetworkError("setsockopt error");
    }
}

void setSocketTimeouts(int socket, int seconds) {
    setTimeoutOption(socket, SO_RCVTIMEO, seconds);
    setTimeoutOption(socket, SO_SNDTIMEO, seconds);
}

void sendAll(int socket, const std::string& data) {
    std::size_t sent = 0;

    while (sent < data.size()) {
        ssize_t result = send(
            socket,
            data.data() + sent,
            data.size() - sent,
            MSG_NOSIGNAL
        );

        if (result < 0) {
            throw NetworkError("send error");
        }

        if (result == 0) {
            throw NetworkError("send error");
        }

        sent += static_cast<std::size_t>(result);
    }
}

std::string recvMessage(int socket) {
    char buffer[4096];

    ssize_t result = recv(socket, buffer, sizeof(buffer), 0);

    if (result < 0) {
        throw NetworkError("recv error");
    }

    return std::string(buffer, static_cast<std::size_t>(result));
}

void recvExact(int socket, void* buffer, std::size_t size) {
    std::size_t got = 0;
    char* ptr = static_cast<char*>(buffer);

    while (got < size) {
        ssize_t result = recv(socket, ptr + got, size - got, 0);

        if (result < 0) {
            throw NetworkError("recv error");
        }

        if (result == 0) {
            throw NetworkError("connection closed");
        }

        got += static_cast<std::size_t>(result);
    }
}