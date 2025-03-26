// RequestWaitingMessages.h
#pragma once
#include <boost/asio.hpp>
#include <vector>
#include "MyInfo.h"
#include "Client.h"
#include "../cryptopp_wrapper/RSAWrapper.h"
#include "../cryptopp_wrapper/Base64Wrapper.h"
#include "../cryptopp_wrapper/AESWrapper.h"

class RequestWaitingMessages {
private:
    static const uint8_t VERSION = 1;
    std::vector<Client>& m_clients;
    MyInfo& m_myInfo; // 

    void sendWaitingMessagesRequest(boost::asio::ip::tcp::socket& socket,
        const std::array<uint8_t, 16>& myId);
    void handleWaitingMessagesResponse(boost::asio::ip::tcp::socket& socket);
    std::string getClientUsername(const std::array<uint8_t, 16>& clientId);

public:
    RequestWaitingMessages(std::vector<Client>& clients, MyInfo& myInfo) : m_clients(clients), m_myInfo(myInfo) {}
    void getWaitingMessages(const std::string& address, int port,
        const std::array<uint8_t, 16>& myId);
};