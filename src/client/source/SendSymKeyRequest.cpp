// SendSymKeyRequest.cpp
#include "../headers/SendSymKeyRequest.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>

void SendSymKeyRequest::sendRequest(const std::string& address, int port,
    const std::array<uint8_t, 16>& myId) {

    std::string username;
    std::cout << "Enter username: ";
    std::getline(std::cin >> std::ws, username);

    auto clientIt = std::find_if(m_clients.begin(), m_clients.end(),
        [&username](const Client& client) {
            return client.getUsername() == username;
        });

    if (clientIt == m_clients.end()) {
        throw std::runtime_error("Username not found in clients list");
    }

    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket(io_context);
    boost::asio::ip::tcp::endpoint endpoint(
        boost::asio::ip::address::from_string(address), port);

    socket.connect(endpoint);
    sendSymKeyRequest(socket, myId, clientIt->getId());
    handleResponse(socket);
}

void SendSymKeyRequest::sendSymKeyRequest(boost::asio::ip::tcp::socket& socket,
    const std::array<uint8_t, 16>& myId,
    const std::array<uint8_t, 16>& targetId) {

    std::vector<uint8_t> request;

    // Client ID - 16 bytes
    request.insert(request.end(), myId.begin(), myId.end());
    // Version - 1 byte
    request.push_back(VERSION);
    // Code - 2 bytes (603 in little endian)
    request.push_back(0x5B);
    request.push_back(0x02);
    // Payload size - 4 bytes (21 bytes: 16 for target ID + 1 for type + 4 for content size)
    request.push_back(0x15);
    request.push_back(0x00);
    request.push_back(0x00);
    request.push_back(0x00);
    // Target client ID - 16 bytes
    request.insert(request.end(), targetId.begin(), targetId.end());
    // Message type - 1 byte
    request.push_back(MESSAGE_TYPE);
    // Content size - 4 bytes (0 for this request)
    request.insert(request.end(), 4, 0);

    boost::asio::write(socket, boost::asio::buffer(request));
}

void SendSymKeyRequest::handleResponse(boost::asio::ip::tcp::socket& socket) {
    std::array<uint8_t, 7> responseHeader;
    boost::asio::read(socket, boost::asio::buffer(responseHeader));

    uint16_t responseCode = *reinterpret_cast<uint16_t*>(&responseHeader[1]);
    if (responseCode == 9000) {
        throw std::runtime_error("server responded with an error");
    }
}