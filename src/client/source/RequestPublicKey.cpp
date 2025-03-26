// RequestPublicKey.cpp
#include "../headers/RequestPublicKey.h"
#include <stdexcept>
#include <algorithm>
#include <iostream>

void RequestPublicKey::getPublicKey(const std::string& address, int port,
    const std::array<uint8_t, 16>& myId) {

    // Get target username
    std::string username;
    std::cout << "Enter username: ";
    std::getline(std::cin >> std::ws, username);

    // Find client ID by username
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
    sendPublicKeyRequest(socket, myId, clientIt->getId());
    handlePublicKeyResponse(socket, clientIt->getId());
}

void RequestPublicKey::sendPublicKeyRequest(boost::asio::ip::tcp::socket& socket,
    const std::array<uint8_t, 16>& myId,
    const std::array<uint8_t, 16>& targetId) {
    std::vector<uint8_t> request;

    // Client ID - 16 bytes
    request.insert(request.end(), myId.begin(), myId.end());
    // Version - 1 byte
    request.push_back(VERSION);
    // Code - 2 bytes (602 in little endian)
    request.push_back(0x5A);
    request.push_back(0x02);
    // Payload size - 4 bytes (16 for client ID)
    request.push_back(0x10);
    request.push_back(0x00);
    request.push_back(0x00);
    request.push_back(0x00);
    // Target client ID - 16 bytes
    request.insert(request.end(), targetId.begin(), targetId.end());

    boost::asio::write(socket, boost::asio::buffer(request));
}

void RequestPublicKey::handlePublicKeyResponse(boost::asio::ip::tcp::socket& socket,
    const std::array<uint8_t, 16>& targetId) {
    std::array<uint8_t, 7> responseHeader;
    boost::asio::read(socket, boost::asio::buffer(responseHeader));

    uint16_t responseCode = *reinterpret_cast<uint16_t*>(&responseHeader[1]);
    uint32_t payloadSize = *reinterpret_cast<uint32_t*>(&responseHeader[3]);

    if (responseCode == 9000) {
        throw std::runtime_error("server responded with an error");
    }

    if (responseCode == 2102 && payloadSize == 176) {  // 16 bytes ID + 160 bytes public key
        std::array<uint8_t, 16> clientId;
        boost::asio::read(socket, boost::asio::buffer(clientId));

        std::vector<uint8_t> publicKey(160);
        boost::asio::read(socket, boost::asio::buffer(publicKey));

        // Find client and update their public key
        auto clientIt = std::find_if(m_clients.begin(), m_clients.end(),
            [&targetId](const Client& client) {
                return client.getId() == targetId;
            });

        if (clientIt != m_clients.end()) {
            clientIt->setPublicKey(publicKey);
            clientIt->print();
        }
    }
}