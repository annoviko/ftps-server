#include "tcp_listener.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


tcp_listener::tcp_listener(void) : tcp_transport(), m_queue_size(0) { }


tcp_listener::tcp_listener(const std::string & server_address, const int server_port, const int server_queue_size) {
    m_address = server_address;
    m_port = server_port;
    m_queue_size = server_queue_size;
}


tcp_listener::tcp_listener(tcp_listener && other_listener) : tcp_transport(std::move(other_listener)) {
    m_queue_size = std::move(other_listener.m_queue_size);
    other_listener.m_queue_size = 0;
}


tcp_listener::~tcp_listener(void) { }


int tcp_listener::bind(void) {
    int operation_result = OPERATION_SUCCESS;

    struct sockaddr_in ip4_server_address;
    ip4_server_address.sin_family = AF_INET;
    ip4_server_address.sin_port = htons((uint16_t) m_port);

    inet_pton(AF_INET, m_address.c_str(), &ip4_server_address.sin_addr);

    if (::bind(m_socket, (struct sockaddr *) &ip4_server_address, sizeof(ip4_server_address)) != 0) {
        operation_result = OPERATION_FAILURE;
    }
    else {
        listen(m_socket, m_queue_size);
    }

    return operation_result;
}


int tcp_listener::accept(tcp_transport & transport) const {
    int operation_result = OPERATION_SUCCESS;

    struct sockaddr_in ipv4_addr_client;
    int ipv4_addr_client_len = sizeof(struct sockaddr_in);

    int client_socket = ::accept(m_socket, (struct sockaddr *) &ipv4_addr_client, (socklen_t *) &ipv4_addr_client_len);
    if (client_socket < 0) {
        operation_result = OPERATION_FAILURE;
    }
    else {
        transport = tcp_transport(client_socket);
    }

    return operation_result;
}


tcp_listener & tcp_listener::operator =(tcp_listener && other_transport) {
    static_cast<tcp_transport &>(*this) = std::move(other_transport);

    m_queue_size = std::move(other_transport.m_queue_size);
    other_transport.m_queue_size = 0;

    return *this;
}
