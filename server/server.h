#include <iostream>
#include <string>
#include <string_view>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "logger.h"

using namespace std::literals;

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

const std::string HTML_PAGE =
    "<html lang=\"ru\">\n"
    "<head>\n"
    "<meta charset=\"utf-8\">\n"
    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
    "<title>Мой первый HTTP-сервер</title>\n"
    "</head>\n"
    "<body>\n"
    "<header>С++ сервер</header>\n"
    "<h1>Привет от С++!</h1>\n"
    "</header>\n"
    "<main>\n"
    "</main>\n"
    "<footer>\n"
    "</footer>\n"
    "</body>\n"
    "</html>\n"s;
const std::string HTTP_RESPONSE = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/html; charset=utf-8\r\n"s;

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
    explicit Server(ServerParams params, Logger& logger)
            : params_{params}, logger_{logger} {
        if (!CreateSocket()) {
            throw "Error creating socket"s;
        }
        logger_("Create socket"sv);
        if (!BindSocket()) {
            close(socket_);
            throw "Error binding socket"s;
        }
        logger_("Bind socket"sv);
    }

    ~Server() {
        StopServer();
    }

    void StartServer() {
        if (!StartListening()) {
            close(socket_);
            throw "Error on listening port"s;
        }
        logger_("Start listening"sv);
    }

    void ProcessConnection() {
        int addrlen = sizeof(params_.address);

        while (true) {
            int client;
            // accept clietn connection
            if ((client = accept(socket_, (struct sockaddr *)&params_.address, (socklen_t*)&addrlen)) < 0) {
                throw "Error accept connection"s;
                close(socket_);
            }
            logger_("Client connect"sv);
            // read data from client
            int bytes_read = recv(client, buffer_, BUFFER_SIZE, 0);

            std::string word(buffer_, buffer_ + bytes_read);
            if (bytes_read > 0) {
                logger_("Receive from client: "s + word);
                // send response with html page
                std::string content_length = "Content-Length: "s
                    + std::to_string(HTML_PAGE.size())
                    + "\r\nConnection: close\r\n\r\n"s;
                std::string response{HTTP_RESPONSE + content_length + HTML_PAGE};
                send(client, response.c_str(), response.size(), 0);
                logger_("Send to client: "s + HTTP_RESPONSE);
            }
            // close connection
            close(client);
            logger_("End connection with client"sv);
        }
    }

    // stoping server
    void StopServer() {
        logger_("Stop server"sv);
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
        return res == 0;
    }
    // waiting for connections
    bool StartListening() {
        return listen(socket_, params_.max_queue_length) == 0;
    }

    ServerParams params_;
    Logger& logger_;
    int socket_{};
    char buffer_[BUFFER_SIZE] = {0};
};