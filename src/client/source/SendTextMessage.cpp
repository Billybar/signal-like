// SendTextMessage.cpp
#include "../headers/SendTextMessage.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>


void SendTextMessage::sendMessage(const std::string& address, int port,
    const std::array<uint8_t, 16>& myId) {

    // get peer user name
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

    // check hasSymmetricKey
    if (!clientIt->hasSymmetricKey()) {
        throw std::runtime_error("Must exchange symmetric key first");
    }

    // get msg to send to peer
    std::string message;
    std::cout << "Enter message content: ";
    std::getline(std::cin >> std::ws, message);

    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket(io_context);
    boost::asio::ip::tcp::endpoint endpoint(
        boost::asio::ip::address::from_string(address), port);

    socket.connect(endpoint);
    sendTextMessageRequest(socket, myId, clientIt->getId(), message);
    handleSendResponse(socket);
}

void SendTextMessage::sendTextMessageRequest(boost::asio::ip::tcp::socket& socket,
    const std::array<uint8_t, 16>& myId,
    const std::array<uint8_t, 16>& targetId,
    const std::string& message) {

    auto clientIt = std::find_if(m_clients.begin(), m_clients.end(),
        [&targetId](const Client& client) {
            return client.getId() == targetId;
        });

    // -------------- START WITH CRYPTO -----------------------
     
    // Encrypt message using recipient's symmetric key
    AESWrapper aes(&clientIt->getSymmetricKey()[0], AESWrapper::DEFAULT_KEYLENGTH);
    std::string encryptedMsg = aes.encrypt(message.c_str(), static_cast<unsigned int>(message.length()));
    
    // -------------- END WITH CRYPTO -----------------------

    std::vector<uint8_t> request;

    // Client ID - 16 bytes
    request.insert(request.end(), myId.begin(), myId.end());
    // Version - 1 byte
    request.push_back(VERSION);
    // Code - 2 bytes (603 in little endian)
    request.push_back(0x5B);
    request.push_back(0x02);
    // Payload size - 4 bytes (16 + 1 + 4 + message size)
    uint32_t payloadSize = 16 + 1 + 4 + static_cast<uint32_t>(encryptedMsg.length());
    request.push_back(payloadSize & 0xFF);
    request.push_back((payloadSize >> 8) & 0xFF);
    request.push_back((payloadSize >> 16) & 0xFF);
    request.push_back((payloadSize >> 24) & 0xFF);
    // Target client ID - 16 bytes
    request.insert(request.end(), targetId.begin(), targetId.end());
    // Message type - 1 byte
    request.push_back(MESSAGE_TYPE);
    // Content size - 4 bytes
    uint32_t contentSize = static_cast<uint32_t>(encryptedMsg.length());
    request.push_back(contentSize & 0xFF);
    request.push_back((contentSize >> 8) & 0xFF);
    request.push_back((contentSize >> 16) & 0xFF);
    request.push_back((contentSize >> 24) & 0xFF);
    // Encrypted message content
    request.insert(request.end(), encryptedMsg.begin(), encryptedMsg.end());

    boost::asio::write(socket, boost::asio::buffer(request));
}

void SendTextMessage::handleSendResponse(boost::asio::ip::tcp::socket& socket) {
    std::array<uint8_t, 7> responseHeader;
    boost::asio::read(socket, boost::asio::buffer(responseHeader));

    uint16_t responseCode = *reinterpret_cast<uint16_t*>(&responseHeader[1]);
    if (responseCode == 9000) {
        throw std::runtime_error("server responded with an error");
    }
}