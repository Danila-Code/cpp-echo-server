#include <fstream>

#include "server.h"

int main() {
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
    std::ofstream log_file("server.log", std::ios::app);
    Logger logger(log_file);
    
    Server server(server_params, logger);
    
    server.StartServer();
    server.ProcessConnection();
}