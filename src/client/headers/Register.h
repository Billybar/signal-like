#pragma once
#include <string>
#include <boost/asio.hpp>
#include <boost/endian/conversion.hpp>
#include <fstream>
#include <iomanip>
#include <sstream>
#include "MyInfo.h"

class Register {
public:
    Register(MyInfo& myInfoRef) : myInfo(myInfoRef) {}
    void registerUser(const std::string& address, int port);

private:
    MyInfo& myInfo;
    static constexpr uint8_t VERSION = 1;
    static constexpr uint16_t REGISTER_CODE = 600;

    struct RequestHeader {
        uint8_t clientId[16] = { 0 };
        uint8_t version;
        uint16_t code;
        uint32_t payloadSize;
    };

    void sendRegistrationRequest(boost::asio::ip::tcp::socket& socket, const std::string& username);
    void handleRegistrationResponse(boost::asio::ip::tcp::socket& socket, const std::string& username);
};
