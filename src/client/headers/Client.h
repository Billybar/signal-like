// Client.h
#pragma once
#include <string>
#include <array>
#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>

class Client {
private:
    std::array<uint8_t, 16> m_id;
    std::string m_username;
    std::vector<uint8_t> m_publicKey;
    std::vector<uint8_t> m_symmetricKey;

public:
    Client(const std::array<uint8_t, 16>& clientId, const std::string& name);

    // Getters
    const std::array<uint8_t, 16>& getId() const;
    const std::string& getUsername() const;
    const std::vector<uint8_t>& getPublicKey() const;
    const std::vector<uint8_t>& getSymmetricKey() const;

    // Setters
    void setPublicKey(const std::vector<uint8_t>& key);
    void setSymmetricKey(const std::vector<uint8_t>& key);

    // Check if client has keys
    bool hasPublicKey() const;
    bool hasSymmetricKey() const;

    // Print client information
    void print() const;
};