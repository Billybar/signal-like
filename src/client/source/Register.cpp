// Register.cpp
#include "../headers/Register.h"
#include <array>
#include <vector>
#include <stdexcept>

void Register::registerUser(const std::string& address, int port) {

    // Get user name from User
    std::string username;
    std::cout << "Enter username: ";
    std::getline(std::cin >> std::ws, username);
    username.push_back('\0');

    
    boost::asio::io_context io_context;                 // create communication object
    boost::asio::ip::tcp::socket socket(io_context);    // create TCP communication
    boost::asio::ip::tcp::endpoint endpoint(            // Set endpoint (server)
        boost::asio::ip::address::from_string(address), port);
    socket.connect(endpoint);                           // Connect to server

    sendRegistrationRequest(socket, username);
    handleRegistrationResponse(socket, username);
}

void Register::sendRegistrationRequest(boost::asio::ip::tcp::socket& socket, const std::string& username) {
    std::vector<uint8_t> request;
    std::vector<uint8_t> publicKey = myInfo.getPublicKey();

    // Client ID - 16 bytes of zeros
    request.insert(request.end(), 16, 0);

    // Version - 1 byte
    request.push_back(VERSION);

    // Code - 2 bytes (600 = 0x258 in little endian: 0x58 0x02)
    request.push_back(0x58);
    request.push_back(0x02);

    // Payload size - 4 bytes (415 = 0x19F in little endian)
    request.push_back(0x9F);
    request.push_back(0x01);
    request.push_back(0x00);
    request.push_back(0x00);

    // Username (null padded to 255 bytes)
    request.insert(request.end(), username.begin(), username.end());
    request.insert(request.end(), 255 - username.length(), 0);

    // Public key (160 bytes)
    request.insert(request.end(), publicKey.begin(), publicKey.end());

    boost::asio::write(socket, boost::asio::buffer(request));
}

void Register::handleRegistrationResponse(boost::asio::ip::tcp::socket& socket, const std::string& username) {
    // Read response header (7 bytes)
    std::array<uint8_t, 7> responseHeader;
    boost::asio::read(socket, boost::asio::buffer(responseHeader));

    uint16_t responseCode = *reinterpret_cast<uint16_t*>(&responseHeader[1]);
    uint32_t payloadSize = *reinterpret_cast<uint32_t*>(&responseHeader[3]);

    if (responseCode == 9000) {
        throw std::runtime_error("server responded with an error");
    }

    if (responseCode == 2100 && payloadSize == 16) {
        // Read client ID
        std::array<uint8_t, 16> clientId;
        boost::asio::read(socket, boost::asio::buffer(clientId));

        // --- Save client ID to my.info file
        // Convert username and clientId to hex string
        std::stringstream hexClientId;
        for (const auto& byte : clientId) {
            hexClientId << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }


        // Get executable directory path
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        std::string exePath(buffer);
        std::string exeDir = exePath.substr(0, exePath.find_last_of("\\/"));

        // Create my.info in executable directory
        std::string filePath = exeDir + "\\my.info";
        std::ofstream file(filePath);
        if (!file) {
            throw std::runtime_error("Unable to create my.info file");
        }

        // Extract actual username by removing NULL terminator from buffer
        std::string usernameStr(reinterpret_cast<const char*>(username.data()));
        usernameStr = usernameStr.substr(0, usernameStr.find('\0'));

        // Write username, client ID and private key
        file << usernameStr << std::endl;
        file << hexClientId.str() << std::endl;
        file << myInfo.getPrivateKey() << std::endl;

        file.close();

        // update register status
        myInfo.setIsRegistered(true);
        myInfo.reload(filePath);
    }
}