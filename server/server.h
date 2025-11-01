#include <iostream>
#include <string>
#include <string_view>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std::literals;

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

struct ServerParams {
    ServerParams& SetDomain(int value) {
        domain = value;
        return *this;
    }

    ServerParams& SetType(int value) {
        type = value;
        return *this;
    }

    ServerParams& SetProtocol(int value) {
        protocol = value;
        return *this;
    }

    ServerParams& SetAddress(sockaddr_in value) {
        address = value;
        return *this;
    }
    
    ServerParams& SetMaxQueueLength(int value) {
        max_queue_length = value;
        return *this;
    }

    int domain;  // communication domain
    int type;  // type of communication semantics
    int protocol;  // protocol family for communication
    sockaddr_in address;  // struct with socket address 
    int max_queue_length;  // max length of queue with connections
};


class Server {
public:
    explicit Server(ServerParams params) : params_{params} {
        if (!CreateSocket()) {
            throw "Error creating socket"s;
        }
        std::cout << "Create socket\n"s;
        if (BindSocket()) {
            close(socket_);
            throw "Error binding socket"s;
        }
        std::cout << "Bind socket\n"s;
    }

    ~Server() {
        StopServer();
    }

    void StartServer() {
        if (!StartListening()) {
            close(socket_);
            throw "Error on listening port"s;
        }
        std::cout << "Start listening\n"s;
    }

    void ProcessConnection(std::string_view stop_word) {
        int addrlen = sizeof(params_.address);
        bool continue_connection = true;

        while (continue_connection) {
            int client;
            // accept clietn connection
            if ((client = accept(socket_, (struct sockaddr *)&params_.address, (socklen_t*)&addrlen)) < 0) {
                throw "Error accept connection"s;
                close(socket_);
            }
            std::cout << "-----------------------------------\n"s;
            std::cout << "Begin connection\n"s;
            std::cout << "Client connect\n"s;

            // read data from client
            //int bytes_read = read(client, buffer_, BUFFER_SIZE);
            int bytes_read = recv(client, buffer_, BUFFER_SIZE, 0);

            std::string word(buffer_, buffer_ + bytes_read);
            if (bytes_read > 0) {
                std::cout << "Receive from client: "s << word;
                // send echo-а эхо‑abswer
                //write(client, buffer_, bytes_read);
                send(client, buffer_, bytes_read, 0);
                std::cout << "Send to client: "s << word;
            }
            // close connection
            close(client);
            std::cout << "End connection with client\n"s;
            
            if (word.find(std::string(stop_word)) != word.npos) {
                std::cout << "-----------------------------------\n"s;
                std::cout << "Stop server\n"s;
                StopServer();
                continue_connection = false;
            }
        }
    }

    // stoping server
    void StopServer() {
        close(socket_);
    }

private:
    // create socket
    bool CreateSocket() {
        socket_ = socket(
                         params_.domain,
                         params_.type,
                         params_.protocol
        );
        return socket_ != -1;
    }
    // bind socket to IP and port
    bool BindSocket() {
        int res = bind(
                       socket_,
                       (const struct sockaddr*)&params_.address,
                       sizeof(params_.address)
        );
        return res != 0;
    }
    // waiting for connections
    bool StartListening() {
        return listen(socket_, params_.max_queue_length) == 0;
    }

    ServerParams params_;
    int socket_{};
    char buffer_[BUFFER_SIZE] = {0};
};