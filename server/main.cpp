#include "server.h"

int main() {
    std::string stop_word;
    std::cin >> stop_word;

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    ServerParams server_params = ServerParams()
                                .SetDomain(AF_INET)
                                .SetType(SOCK_STREAM)
                                .SetProtocol(0)
                                .SetAddress(address)
                                .SetMaxQueueLength(5);
    
    Server server(server_params);

    server.StartServer();
    server.ProcessConnection(stop_word);
}