// RequestPublicKey.h
#pragma once
#include <boost/asio.hpp>
#include <vector>
#include <string>
#include <array>
#include <iomanip>
#include "Client.h"

class RequestPublicKey {
private:
    static const uint8_t VERSION = 1;
    std::vector<Client>& m_clients; // Reference to clients vector from MessageUClient - updates affect the original vector

    void sendPublicKeyRequest(boost::asio::ip::tcp::socket& socket,
        const std::array<uint8_t, 16>& myId,
        const std::array<uint8_t, 16>& targetId);
    void handlePublicKeyResponse(boost::asio::ip::tcp::socket& socket,
        const std::array<uint8_t, 16>& targetId);

public:
    RequestPublicKey(std::vector<Client>& clients) : m_clients(clients) {}
    void getPublicKey(const std::string& address, int port,
        const std::array<uint8_t, 16>& myId);
};