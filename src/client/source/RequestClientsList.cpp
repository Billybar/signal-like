// RequestClientsList.cpp
#include "../headers/RequestClientsList.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>


void RequestClientsList::getClientsList(const std::string& address, int port, const std::array<uint8_t, 16>& myId) {
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket(io_context);
    boost::asio::ip::tcp::endpoint endpoint( boost::asio::ip::address::from_string(address), port );

    socket.connect(endpoint);
    sendListRequest(socket, myId);
    handleListResponse(socket);
}

void RequestClientsList::sendListRequest(boost::asio::ip::tcp::socket& socket, const std::array<uint8_t, 16>& myId) {
    std::vector<uint8_t> request;

    // Client ID - 16 bytes
    request.insert(request.end(), myId.begin(), myId.end());

    // Version - 1 byte
    request.push_back(VERSION);

    // Code - 2 bytes (601 in little endian)
    request.push_back(0x59);
    request.push_back(0x02);

    // Payload size - 4 bytes (0 for this request)
    request.insert(request.end(), 4, 0);

    boost::asio::write(socket, boost::asio::buffer(request));
}

void RequestClientsList::handleListResponse(boost::asio::ip::tcp::socket& socket) {
    std::array<uint8_t, 7> responseHeader;
    boost::asio::read(socket, boost::asio::buffer(responseHeader));

    uint16_t responseCode = *reinterpret_cast<uint16_t*>(&responseHeader[1]);
    uint32_t payloadSize = *reinterpret_cast<uint32_t*>(&responseHeader[3]);

    if (responseCode == 9000) {
        throw std::runtime_error("server responded with an error");
    }

    if (responseCode == 2101 && payloadSize > 0) {
        m_clients.clear();
        size_t numClients = payloadSize / 271;

        for (size_t i = 0; i < numClients; i++) {
            std::array<uint8_t, 16> clientId;
            boost::asio::read(socket, boost::asio::buffer(clientId));

            std::array<char, 255> usernameBuffer;
            boost::asio::read(socket, boost::asio::buffer(usernameBuffer));

            std::string username(usernameBuffer.data());
            username = username.substr(0, username.find('\0'));

            m_clients.emplace_back(clientId, username);
            std::cout << username << std::endl;
        }
    }
}