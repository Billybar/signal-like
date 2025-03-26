// SendSymKey.h
#pragma once
#include <boost/asio.hpp>
#include <vector>
#include "Client.h"
#include "../cryptopp_wrapper/AESWrapper.h"
#include "../cryptopp_wrapper/RSAWrapper.h"

class SendSymKey {
private:
    static const uint8_t VERSION = 1;
    static const uint8_t MESSAGE_TYPE = 2;  // Symmetric key message type
    std::vector<Client>& m_clients;

    void sendSymKeyRequest(boost::asio::ip::tcp::socket& socket,
        const std::array<uint8_t, 16>& myId,
        const std::array<uint8_t, 16>& targetId,
        const std::vector<uint8_t>& encryptedKey);

    void handleResponse(boost::asio::ip::tcp::socket& socket);

public:
    SendSymKey(std::vector<Client>& clients) : m_clients(clients) {}
    void sendKey(const std::string& address, int port,
        const std::array<uint8_t, 16>& myId);
};