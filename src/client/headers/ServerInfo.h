#pragma once
#include <string>

class ServerInfo {
private:
    std::string serverAddress;
    int serverPort;

public:
    // Constructor loads server info from file
    ServerInfo(const std::string& filePath);

    void readFromFile(const std::string& filePath);

    // Getters
    const std::string& getAddress() const;
    int getPort() const;

    void print() const;
};