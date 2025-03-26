#pragma once
#include <string>
#include <array>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <iomanip>

class MyInfo {
private:
    std::string username;
    std::array<uint8_t, 16> uuid;
    std::string privateKey;
    std::vector<uint8_t> publicKey; // binary
    bool isRegistered;

public:
    MyInfo(const std::string& filePath);

    void readFromFile(const std::string& filePath);
    void writeToFile(const std::string& filePath, const std::string& newUsername,
        const std::array<uint8_t, 16>& newUuid, const std::string& newPrivateKey);
    void reload(const std::string& filePath);
    void print() const;

    // Setters
    void setIsRegistered(bool status);
    void setPrivateKey(const std::string& key);
    void setPublicKey(const std::vector<uint8_t>& key);

    // Getters
    const std::string& getUsername() const;
    const std::array<uint8_t, 16>& getUuid() const;
    const std::string& getPrivateKey() const;
    const std::vector<uint8_t> getPublicKey() const;
    bool getIsRegistered() const;
};