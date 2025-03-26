# Signal-like - Secure Messaging Application

## Overview
Signal-like is a secure, end-to-end encrypted messaging system consisting of:
1. A client application (C++)
2. A server application (Python)

The system allows users to register, exchange messages securely, and manage encryption keys for private communications.

## Security Features
- **RSA Encryption**: Used for key exchange
- **AES Encryption**: Used for encrypting message content
- **End-to-End Encryption**: Messages are encrypted on the sender's device and can only be decrypted on the recipient's device
- **Secure Key Exchange**: Symmetric keys are exchanged securely using RSA public/private key pairs

## Project Structure

### Client (C++)
- **Crypto Wrappers**: Interfaces to crypto libraries (AES, RSA, Base64)
- **Network Communication**: Handles client-server protocol using Boost.Asio
- **User Management**: Manages user registration and client information

### Server (Python)
- **Message Routing**: Routes messages between clients
- **User Database**: Manages registered users
- **Message Queue**: Stores messages for offline users

## Setup

### Prerequisites
- C++ compiler with C++11 support
- Python 3.6+
- Boost libraries
- Crypto++ library

### Building the Client
```bash
# Navigate to the client directory
cd src/client

# Build using your preferred build system
# For example, with CMake:
mkdir build && cd build
cmake ..
make
```

### Running the Server
```bash
# Navigate to the server directory
cd src/server

# Run the server
python server.py
```

## Usage

### First-time Setup
1. Start the server
2. Launch the client
3. Register a new user (option 110)

### Basic Commands
- **110**: Register a new user
- **120**: Request client list
- **130**: Request a user's public key
- **140**: Check for waiting messages
- **150**: Send a text message
- **151**: Send a request for a symmetric key
- **152**: Send your symmetric key
- **0**: Exit

### Secure Messaging Flow
1. Request the clients list (120)
2. Request a target user's public key (130)
3. Send your symmetric key encrypted with their public key (152)
4. The recipient must request waiting messages (140) to receive your key
5. Now both parties can exchange encrypted messages (150)

## Configuration Files

### Client
- **server.info**: Contains server IP and port (e.g., "127.0.0.1:5000")
- **my.info**: Created during registration, contains:
  - Username
  - Client UUID
  - Private key (Base64 encoded)

### Server
- **myport.info**: Contains the port number for the server

## Protocol Design

Signal-like uses a custom binary protocol:

### Request Header Format
```
[Client ID (16 bytes)][Version (1 byte)][Code (2 bytes)][Payload Size (4 bytes)]
```

### Response Header Format
```
[Version (1 byte)][Code (2 bytes)][Payload Size (4 bytes)]
```

### Request Codes
- **600**: Registration
- **601**: Client List
- **602**: Public Key
- **603**: Send Message
- **604**: Waiting Messages

### Response Codes
- **2100**: Registration Success
- **2101**: Client List
- **2102**: Public Key
- **2103**: Message Sent
- **2104**: Waiting Messages
- **9000**: Error

### Message Types
- **1**: Request for symmetric key
- **2**: Symmetric key
- **3**: Text message

## Security Considerations
- The system uses a zero-initialization vector (IV) for AES encryption. In a production environment, a secure random IV should be generated for each message.
- The client stores the private key locally in Base64 encoding. Consider adding additional protection for the private key file.
- Consider implementing certificate validation to prevent man-in-the-middle attacks.

## Future Improvements
- Add file sharing capabilities
- Implement message delivery confirmations
- Add support for group messaging
- Implement proper session management
- Add support for message expiration

## License
This project is for educational purposes only.
