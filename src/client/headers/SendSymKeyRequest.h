// SendSymKeyRequest.h
#pragma once
#include <boost/asio.hpp>
#include <vector>
#include "Client.h"

class SendSymKeyRequest {
private:
    static const uint8_t VERSION = 1;
    static const uint8_t MESSAGE_TYPE = 1;  // Request for symmetric key
    std::vector<Client>& m_clients;

    void sendSymKeyRequest(boost::asio::ip::tcp::socket& socket,
        const std::array<uint8_t, 16>& myId,
        const std::array<uint8_t, 16>& targetId);
    void handleResponse(boost::asio::ip::tcp::socket& socket);

public:
    SendSymKeyRequest(std::vector<Client>& clients) : m_clients(clients) {}
    void sendRequest(const std::string& address, int port,
        const std::array<uint8_t, 16>& myId);
};