// RequestWaitingMessages.cpp
#include "../headers/RequestWaitingMessages.h"
#include <iostream>
#include <algorithm>
#include <stdexcept>



void RequestWaitingMessages::getWaitingMessages(
    const std::string& address, int port, const std::array<uint8_t, 16>& myId) {

    boost::asio::io_context io_context;
    boost::asio::ip::tcp::socket socket(io_context);
    boost::asio::ip::tcp::endpoint endpoint(
        boost::asio::ip::address::from_string(address), static_cast<unsigned short>(port));

    socket.connect(endpoint);
    sendWaitingMessagesRequest(socket, myId);
    handleWaitingMessagesResponse(socket);
}

void RequestWaitingMessages::sendWaitingMessagesRequest(
    boost::asio::ip::tcp::socket& socket, const std::array<uint8_t, 16>& myId) {

    std::vector<uint8_t> request;

    // Client ID - 16 bytes
    request.insert(request.end(), myId.begin(), myId.end());
    // Version - 1 byte
    request.push_back(VERSION);
    // Code - 2 bytes (604 in little endian)
    request.push_back(0x5C);
    request.push_back(0x02);
    // Payload size - 4 bytes (0 for this request)
    request.insert(request.end(), 4, 0);

    boost::asio::write(socket, boost::asio::buffer(request));
}

std::string RequestWaitingMessages::getClientUsername(const std::array<uint8_t, 16>& clientId) {
    auto it = std::find_if(m_clients.begin(), m_clients.end(),
        [&clientId](const Client& client) {
            return client.getId() == clientId;
        });

    return (it != m_clients.end()) ? it->getUsername() : "Unknown User";
}

void RequestWaitingMessages::handleWaitingMessagesResponse(boost::asio::ip::tcp::socket& socket) {
    std::array<uint8_t, 7> responseHeader;
    boost::asio::read(socket, boost::asio::buffer(responseHeader));

    uint16_t responseCode = *reinterpret_cast<uint16_t*>(&responseHeader[1]);
    uint32_t payloadSize = *reinterpret_cast<uint32_t*>(&responseHeader[3]);

    if (responseCode == 9000) {
        throw std::runtime_error("server responded with an error");
    }

    if (responseCode == 2104 && payloadSize > 0) {
        while (payloadSize > 0) {
            // Read sender ID - 16 bytes
            std::array<uint8_t, 16> fromClient;
            boost::asio::read(socket, boost::asio::buffer(fromClient));
            payloadSize -= 16;

            // Read message ID - 4 bytes
            uint32_t messageId;
            boost::asio::read(socket, boost::asio::buffer(&messageId, 4));
            payloadSize -= 4;

            // Read message type - 1 byte
            uint8_t messageType;
            boost::asio::read(socket, boost::asio::buffer(&messageType, 1));
            payloadSize -= 1;

            // Read message size - 4 bytes
            uint32_t messageSize;
            boost::asio::read(socket, boost::asio::buffer(&messageSize, 4));
            payloadSize -= 4;

            // Print sender
            std::string username = getClientUsername(fromClient);
            std::cout << "From: " << username << std::endl;
            std::cout << "Content:" << std::endl;

            // Handle different message types
            switch (messageType) {
            case 1: // Request for symmetric key
                std::cout << "Request for symmetric key" << std::endl;
                break;
            case 2: // Send symmetric key
                if (messageSize > 0) {
                    std::vector<uint8_t> content(messageSize);
                    boost::asio::read(socket, boost::asio::buffer(content));
                    payloadSize -= messageSize;
                    std::cout << "symmetric key received" << std::endl;

                    // Find client and update their symmetric key
                    auto clientIt = std::find_if(m_clients.begin(), m_clients.end(),
                        [&fromClient](const Client& client) {
                            return client.getId() == fromClient;
                        });
                    if (clientIt != m_clients.end()) {
                        try {
                            // Get and decode the private key from Base64
                            std::string privKeyStr = m_myInfo.getPrivateKey();

                            std::string decodedKey = Base64Wrapper::decode(privKeyStr);

                            // Create RSA wrapper with decoded key
                            RSAPrivateWrapper rsaPrivate(decodedKey);

                            // Decrypt the symmetric key
                            std::string decryptedKey = rsaPrivate.decrypt(
                                reinterpret_cast<const char*>(content.data()),
                                static_cast<unsigned int>(content.size())
                            );

                            // Store decrypted symmetric key
                            std::vector<uint8_t> keyBytes(decryptedKey.begin(), decryptedKey.end());
                            clientIt->setSymmetricKey(keyBytes);

                            // Print the decrypted symmetric key
                            std::cout << "Decrypted symmetric key: ";
                            for (char byte : keyBytes) {
                                printf("%02x", static_cast<uint8_t>(byte));
                            }
                            std::cout << std::endl;

                        }
                        catch (const std::exception& e) {
                            std::cerr << "Error decrypting symmetric key: " << e.what() << std::endl;
                        }
                    }
                }
                break;
            case 3: // Text message
                if (messageSize > 0) {
                    std::vector<char> content(messageSize);
                    boost::asio::read(socket, boost::asio::buffer(content));
                    payloadSize -= messageSize;

                    auto clientIt = std::find_if(m_clients.begin(), m_clients.end(),
                        [&fromClient](const Client& client) {
                            return client.getId() == fromClient;
                        });

                    if (clientIt != m_clients.end() && clientIt->hasSymmetricKey()) {
                        // Create AES wrapper with the client's symmetric key
                        const std::vector<uint8_t>& symKey = clientIt->getSymmetricKey();
                        AESWrapper aesWrapper(symKey.data(), AESWrapper::DEFAULT_KEYLENGTH);

                        // Decrypt the message
                        std::string decryptedMsg = aesWrapper.decrypt(
                            content.data(),
                            messageSize
                        );

                        // Print decrypted message
                        std::cout << decryptedMsg << std::endl;
                    }
                    else {
                        std::cout << "can't decrypt message" << std::endl;
                    }
                }
                break;
            default:
                if (messageSize > 0) {
                    std::vector<char> content(messageSize);
                    boost::asio::read(socket, boost::asio::buffer(content));
                    payloadSize -= messageSize;
                }
                break;
            }
            std::cout << "-----<EOM>-----\n" << std::endl;
        }
    }
}