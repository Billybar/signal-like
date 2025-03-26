#include "../headers/ServerInfo.h"
#include <stdexcept>
#include <fstream>
#include <iostream>

ServerInfo::ServerInfo(const std::string& filePath) {
    readFromFile(filePath);
}

void ServerInfo::readFromFile(const std::string& filePath) {
    std::ifstream serverInfoFile(filePath);
    if (!serverInfoFile.is_open()) {
        throw std::runtime_error("Could not open server.info file");
    }

    std::string serverLine;
    if (!std::getline(serverInfoFile, serverLine)) {
        throw std::runtime_error("Failed to read server info");
    }

    // Find the colon separator
    size_t colonPos = serverLine.find(':');
    if (colonPos == std::string::npos) {
        throw std::runtime_error("Invalid server.info format");
    }

    // Extract IP and port
    serverAddress = serverLine.substr(0, colonPos);
    try {
        serverPort = std::stoi(serverLine.substr(colonPos + 1));
    }
    catch (const std::exception&) {
        throw std::runtime_error("Invalid port number");
    }
}

const std::string& ServerInfo::getAddress() const {
    return serverAddress;
}

int ServerInfo::getPort() const {
    return serverPort;
}

void ServerInfo::print() const {
    std::cout << "Server Address: " << serverAddress << std::endl;
    std::cout << "Server Port: " << serverPort << std::endl;
}