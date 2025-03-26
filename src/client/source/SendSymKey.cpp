// SendSymKey.cpp
#include "../headers/SendSymKey.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>

void SendSymKey::sendKey(const std::string& address, int port,
    const std::array<uint8_t, 16>& myId) {

    // Get target username
    std::string username;
    std::cout << "Enter username: ";
    std::getline(std::cin >> std::ws, username);

    // Find client by username
    auto clientIt = std::find_if(m_clients.begin(), m_clients.end(),
        [&username](const Client& client) {
            return client.getUsername() == username;
        });

    if (clientIt == m_clients.end()) {
        throw std::runtime_error("Username not found in clients list");
    }

    if (!clientIt->hasPublicKey()) {
        throw std::runtime_error("Must request public key first");
    }


    // -------------- START CRYPTO -----------------------

    // Generate symmetric key    -- TO DO --
    unsigned char symKey[AESWrapper::DEFAULT_KEYLENGTH];
    AESWrapper::GenerateKey(symKey, AESWrapper::DEFAULT_KEYLENGTH);

    // Create RSA wrapper with client's public key  -- TO DO --
    RSAPublicWrapper rsaPublic(std::string(clientIt->getPublicKey().begin(),
        clientIt->getPublicKey().end()));

    // Encrypt symmetric key with client's public key  -- TO DO --
    std::string encryptedKey = rsaPublic.encrypt(
        reinterpret_cast<const char*>(symKey),
        AESWrapper::DEFAULT_KEYLENGTH
    );


    // Store symmetric key for this client
    std::vector<uint8_t> symKeyVec(symKey, symKey + AESWrapper::DEFAULT_KEYLENGTH);
    clientIt->setSymmetricKey(symKeyVec);

    // 
    std::vector<uint8_t> encryptedKeyVec(encryptedKey.begin(), encryptedKey.end());

    // Print the sym Key Vec
    std::cout << "sym Key Vec (store): ";
    for (uint8_t byte : symKeyVec) {
        printf("%02x", byte);
    }
    std::cout << std::endl;

    // Print the encrypted key
    std::cout << "Encrypted key (Send): ";
    for (uint8_t byte : encryptedKeyVec) {
        printf("%02x", byte);
    }
    std::cout << std::endl;

    // Send encrypted key
    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket(io_context);
    boost::asio::ip::tcp::endpoint endpoint(
        boost::asio::ip::address::from_string(address), static_cast<unsigned short>(port));

    socket.connect(endpoint);
    sendSymKeyRequest(socket, myId, clientIt->getId(), encryptedKeyVec);
    handleResponse(socket);

    // -------------- END CRYPTO -----------------------

}

void SendSymKey::sendSymKeyRequest(boost::asio::ip::tcp::socket& socket,
    const std::array<uint8_t, 16>& myId,
    const std::array<uint8_t, 16>& targetId,
    const std::vector<uint8_t>& encryptedKey) {

    std::vector<uint8_t> request;

    // Client ID - 16 bytes
    request.insert(request.end(), myId.begin(), myId.end());
    // Version - 1 byte
    request.push_back(VERSION);
    // Code - 2 bytes (603 in little endian)
    request.push_back(0x5B);
    request.push_back(0x02);
    // Payload size - 4 bytes (16 + 1 + 4 + content_size)
    uint32_t payloadSize = 21 + static_cast<uint32_t>(encryptedKey.size());
    request.insert(request.end(),
        reinterpret_cast<uint8_t*>(&payloadSize),
        reinterpret_cast<uint8_t*>(&payloadSize) + 4);

    // Target client ID - 16 bytes
    request.insert(request.end(), targetId.begin(), targetId.end());
    // Message type - 1 byte
    request.push_back(MESSAGE_TYPE);
    // Content size - 4 bytes
    uint32_t contentSize = static_cast<uint32_t>(encryptedKey.size());
    request.insert(request.end(),
        reinterpret_cast<uint8_t*>(&contentSize),
        reinterpret_cast<uint8_t*>(&contentSize) + 4);
    // Encrypted symmetric key
    request.insert(request.end(), encryptedKey.begin(), encryptedKey.end());

    boost::asio::write(socket, boost::asio::buffer(request));
}

void SendSymKey::handleResponse(boost::asio::ip::tcp::socket& socket) {
    std::array<uint8_t, 7> responseHeader;
    boost::asio::read(socket, boost::asio::buffer(responseHeader));

    uint16_t responseCode = *reinterpret_cast<uint16_t*>(&responseHeader[1]);

    if (responseCode == 9000) {
        throw std::runtime_error("server responded with an error");
    }
}