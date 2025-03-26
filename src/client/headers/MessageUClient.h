// MessageUClient.h
#pragma once
#include <string>
#include "Client.h"
#include "ServerInfo.h"
#include "MyInfo.h"
#include "Register.h"
#include "RequestClientsList.h"
#include "RequestPublicKey.h"
#include "RequestWaitingMessages.h"
#include "SendTextMessage.h"
#include "SendSymKeyRequest.h"
#include "SendSymKey.h"
#include "../cryptopp_wrapper/RSAWrapper.h"
#include "../cryptopp_wrapper/Base64Wrapper.h"
#include "../cryptopp_wrapper/AESWrapper.h"

class MessageUClient {
private:
    ServerInfo serverInfo;
    MyInfo myInfo;
    std::vector<Client> m_clients;

    // Helper methods declarations
    static std::string getExeDirectory();
    static std::string getServerInfoPath();
    static std::string getMyInfoPath();

public:
    // Default constructor - uses dynamic paths
    MessageUClient();

    // Constructor with custom paths
   // MessageUClient(const std::string& serverInfoPath, const std::string& myInfoPath);

    // Main run loop
    void run();


    // Menu operations
    void registerUser();
    void requestClientsList();
    void requestPublicKey();
    void requestWaitingMessages();
    void sendTextMessage();
    void sendSymKeyRequest();
    void sendSymKey();
};