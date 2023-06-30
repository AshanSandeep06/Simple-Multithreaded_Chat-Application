#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#include <thread>
#include <mutex>
#include <unistd.h>

#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#endif

using namespace std;

constexpr int PORT = 5000;

// Server running command = g++ -std=c++11 ServerSide\Server.cpp -o server -lws2_32

class ClientHandler
{
private:
    std::vector<ClientHandler *> &clientHandlersList;
    std::mutex &mtx;

public:
    int clientSocket;
    ClientHandler(int socket, std::vector<ClientHandler *> &clients, std::mutex &lock)
        : clientSocket(socket), clientHandlersList(clients), mtx(lock) {}

    void operate()
    {
        try
        {
            int bytesRead;

            while (true)
            {
                bytesRead = 0;

                char buffer[1024];
                bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

                cout << clientSocket << std::endl
                     << buffer << std::endl
                     << sizeof(buffer) << std::endl
                     << bytesRead << std::endl;

                // Sending again to the Client

                std::string message;
                message = "";
                cout << "Enter a message to Client : ";
                std::getline(std::cin, message);

                send(clientSocket, message.c_str(), message.length(), 0);
                // send(clientSocket, buffer, bytesRead, 0);
            }

            if (bytesRead == 0)
            {
                // Client disconnected
                std::cout << "Client disconnected." << std::endl;
            }
            else
            {
                // Error in receiving data
                std::cerr << "Error in receiving data from client." << std::endl;
            }

            close(clientSocket);

            // Remove the client from the list
            // std::lock_guard<std::mutex> lock(mtx);
            // clientHandlersList.erase(std::remove(clientHandlersList.begin(), clientHandlersList.end(), this),
            // clientHandlersList.end());
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }
};

int main()
{
//

// Initialize Winsock on Windows
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Failed to initialize Winsock." << std::endl;
        return 1;
    }
#endif

    int serverSocket;
    struct sockaddr_in serverAddress
    {
    };

    std::vector<ClientHandler *> clientHandlersList;
    std::mutex mtx;

    // Create the Server Socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1)
    {
        std::cerr << "Failed to create socket" << std::endl;
        return 1;
    }

    // Bind the Socket to specific address and port
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);

    if (bind(serverSocket, reinterpret_cast<struct sockaddr *>(&serverAddress), sizeof(serverAddress)) == -1)
    {
        std::cerr << "Failed to bind socket." << std::endl;
        close(serverSocket);
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, SOMAXCONN) == -1)
    {
        std::cerr << "Failed to listen on socket." << std::endl;
        close(serverSocket);
        return 1;
    }

    std::cout << "Server has Started in Port: " << PORT << std::endl;

    while (true)
    {
        struct sockaddr_in clientAddress
        {
        };

        int clientAddressLength = sizeof(clientAddress);

        // Accept a new client connection
        int clientSocket = accept(serverSocket, reinterpret_cast<struct sockaddr *>(&clientAddress), &clientAddressLength);

        if (clientSocket == -1)
        {
            std::cerr << "Failed to accept client connection." << std::endl;
            continue;
        }

        std::cout << "Connected Client Port Number: " << ntohs(clientAddress.sin_port) << std::endl;
        std::cout << "Connected Client IP: " << inet_ntoa(clientAddress.sin_addr) << std::endl;

        // Create a new ClientHandler instance
        ClientHandler *client = new ClientHandler(clientSocket, clientHandlersList, mtx);

        // Add the client for handling in a separate thread
        clientHandlersList.push_back(client);

        // Start the Client Thread
        // std::thread clientThread(*client);
        // clientThread.detach();

        // Start the Client Thread
        std::thread clientThread(
            [&client]()
            {
                client->operate();
            });

        clientThread.detach();
    };

    // Close all client sockets and delete the handlers
    for (auto client : clientHandlersList)
    {
        close(client->clientSocket);
        delete client;
    }

    // Close the server socket
    close(serverSocket);

    return 0;
}
