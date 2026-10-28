#pragma once

#include <cstddef>
#include <string>

void setSocketTimeouts(int socket, int seconds);

void sendAll(int socket, const std::string& data);

std::string recvMessage(int socket);

void recvExact(int socket, void* buffer, std::size_t size);