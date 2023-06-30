// #include <iostream>
// #include <cstring>
// #include <thread>

// #include <winsock2.h>
// #include <windows.h>
// #include <stdlib.h>
// #include <stdio.h>

// #ifdef _WIN32
// #include <winsock2.h>
// #include <ws2tcpip.h>
// #pragma comment(lib, "ws2_32.lib")
// #endif

// using namespace std;

// const int PORT = 5000;

// int main()
// {
//     int localSocket;
//     // sockaddr_in server_addr;

//     // Initialize Winsock
//     WSADATA wsaData;
//     int iResult1 = WSAStartup(MAKEWORD(2, 2), &wsaData);
//     if (iResult1 != 0)
//     {
//         cout << "WSAStartup failed with error: " << iResult1 << endl;
//         return 1;
//     }

//     // Create a socket
//     localSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//     if (localSocket == INVALID_SOCKET)
//     {
//         cout << "socket() failed with error: " << WSAGetLastError() << endl;
//         WSACleanup();
//         return 1;
//     }

//     // Set the SO_REUSEADDR socket option
//     int optval = 1;
//     int iResult2 = setsockopt(localSocket, SOL_SOCKET, SO_REUSEADDR, (char *)&optval, sizeof(optval));
//     if (iResult2 != 0)
//     {
//         cout << "setsockopt() failed with error: " << WSAGetLastError() << endl;
//         closesocket(localSocket);
//         WSACleanup();
//         return 1;
//     }

//     // Bind the socket to the port
//     sockaddr_in server_addr;
//     server_addr.sin_family = AF_INET;
//     server_addr.sin_port = htons(PORT);
//     server_addr.sin_addr.s_addr = INADDR_ANY;
//     int iResult3 = bind(localSocket, (sockaddr *)&server_addr, sizeof(server_addr));
//     if (iResult3 != 0)
//     {
//         if (WSAGetLastError() == 10013)
//         {
//             cout << "The port is in use. Please try running the application as an administrator." << endl;
//         }
//         else
//         {
//             cout << "bind() failed with error: " << WSAGetLastError() << endl;
//         }
//         closesocket(localSocket);
//         WSACleanup();
//         return 1;
//     }

//     // Listen for connectios
//     listen(localSocket, 1);

//     // Accept a connection
//     SOCKET client_socket = accept(localSocket, NULL, NULL);
//     if (client_socket == INVALID_SOCKET)
//     {
//         cout << "accept() failed with error: " << WSAGetLastError() << endl;
//         closesocket(client_socket);
//         WSACleanup();

//         return 1;
//     }

//     // Start a thread to handle the client
//     thread client_thread([&]()
//                          {
//        // Receive messages from the client
//        char message[1024];
//        int bytes_received = recv(client_socket, message, sizeof(message), 0);
//        while (bytes_received > 0) {
//            cout << "Client: " << message << endl;
//            bytes_received = recv(client_socket, message, sizeof(message), 0);
//        }

//        // Close the socket
//        closesocket(client_socket); });
//     client_thread.detach();

//     // Send messages to the client
//     string message;
//     while (true)
//     {
//         cout << "Enter a message: ";
//         cin >> message;
//         send(client_socket, message.c_str(), message.length(), 0);
//     }

//     // Close the socket
//     closesocket(localSocket);
//     WSACleanup();

//     return 0;
// }

// ---------------------------------------------------------------------------------------------------------------------------------

// Second Attempt
#include <iostream>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

const int PORT = 5000;

int main()
{
    // Initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0)
    {
        std::cerr << "WSAStartup failed with error: " << iResult << std::endl;
        return 1;
    }

    // Create a socket
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "socket() failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Connect to the server
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server's IP address
    iResult = connect(clientSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
    if (iResult == SOCKET_ERROR)
    {
        std::cerr << "connect() failed with error: " << WSAGetLastError() << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to the server" << std::endl;

    // Send and receive data with the server
    while (true)
    {
        std::string message;
        message = "";
        std::cout << "Enter a message: ";
        std::getline(std::cin, message);

        if (message.empty())
        {
            continue;
        }

        iResult = send(clientSocket, message.c_str(), message.length(), 0);
        if (iResult == SOCKET_ERROR)
        {
            std::cerr << "send() failed with error: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        // Receive response from the server
        char buffer[1024];
        iResult = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (iResult > 0)
        {
            std::cout << "Server response: " << buffer << std::endl;
        }
        else if (iResult == 0)
        {
            std::cout << "Server closed the connection" << std::endl;
            break;
        }
        else
        {
            std::cerr << "recv() failed with error: " << WSAGetLastError() << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }
    }

    // Close the socket
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
