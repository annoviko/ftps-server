#include "tcp_client.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


tcp_client::tcp_client(void) { }


tcp_client::tcp_client(const int tcp_socket_descr) : tcp_transport(tcp_socket_descr) { }


tcp_client::tcp_client(tcp_client && other_client) : tcp_transport(std::move(other_client)) { }


tcp_client::~tcp_client(void) { }


int tcp_client::connect(const std::string & server_address, const int server_port) {
    int operation_result = OPERATION_SUCCESS;

    struct sockaddr_in client_address;
    client_address.sin_family = AF_INET;
    client_address.sin_port = htons((uint16_t) server_port);
    client_address.sin_addr.s_addr = inet_addr(server_address.c_str());

    if (::connect(m_socket, (struct sockaddr *) &client_address, sizeof(client_address)) < 0) {
        operation_result = OPERATION_FAILURE;
    }
    else {
        m_address = server_address;
        m_port = server_port;
    }

    return operation_result;
}


tcp_client & tcp_client::operator =(tcp_client && other_transport) {
    static_cast<tcp_transport &>(*this) = std::move(other_transport);
    return *this;
}
