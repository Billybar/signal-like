// SendTextMessage.h
#pragma once
#include <boost/asio.hpp>
#include <vector>
#include "Client.h"
#include "../cryptopp_wrapper/AESWrapper.h"

class SendTextMessage {
private:
    static const uint8_t VERSION = 1;
    static const uint8_t MESSAGE_TYPE = 3;  // Text message type
    std::vector<Client>& m_clients;

    void sendTextMessageRequest(boost::asio::ip::tcp::socket& socket,
        const std::array<uint8_t, 16>& myId,
        const std::array<uint8_t, 16>& targetId,
        const std::string& message);

    void handleSendResponse(boost::asio::ip::tcp::socket& socket);

public:
    SendTextMessage(std::vector<Client>& clients) : m_clients(clients) {}
    void sendMessage(const std::string& address, int port,
        const std::array<uint8_t, 16>& myId);
};