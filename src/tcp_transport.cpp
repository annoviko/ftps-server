#include "tcp_transport.h"

#include "easylogging++.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <stdexcept>
#include <iostream>


tcp_transport::tcp_transport(void) {
    m_socket = socket(AF_INET, SOCK_STREAM, 0);
    m_secure = false;
    m_address = "";
    m_port = 0;
}


tcp_transport::tcp_transport(const int tcp_socket_descr) :
        m_socket(tcp_socket_descr),
        m_secure(false),
        m_address(""),
        m_port(0)
{
    socklen_t length = sizeof(struct sockaddr);
    struct sockaddr_in socket_info;

    if (getsockname(m_socket, (sockaddr *) &socket_info, &length) < 0) {
        std::cout << "TCP transport cannot extract info about socket during creating." << std::endl;
    }
    else {
        char * socket_address = inet_ntoa(socket_info.sin_addr);
        int socket_port = ntohs(socket_info.sin_port);

        m_address = std::string(socket_address);
        m_port = socket_port;
    }
}


tcp_transport::tcp_transport(tcp_transport && other_transport) {
    m_socket    = std::move(other_transport.m_socket);
    m_session   = std::move(other_transport.m_session);
    m_secure    = std::move(other_transport.m_secure);
    m_address   = std::move(other_transport.m_address);
    m_port      = std::move(other_transport.m_port);

    other_transport.m_socket    = 0;
}


tcp_transport::~tcp_transport(void) { }


ssize_t tcp_transport::push(const char * byte_sequence, const size_t sequence_length) const {
    if (m_secure) {
        return m_session.push(byte_sequence, sequence_length);
    }
    else {
        return send(m_socket, byte_sequence, sequence_length, 0);
    }
}


ssize_t tcp_transport::pull(const size_t receive_buffer_length, void * receive_buffer) const {
    if (m_secure) {
        return m_session.pull(receive_buffer_length, receive_buffer);
    }
    else {
        return recv(m_socket, receive_buffer, receive_buffer_length, 0);
    }
}


void tcp_transport::close(void) {
    if (m_secure) {
        m_session.close();
    }

    ::close(m_socket);
    m_socket = 0;
}


void tcp_transport::set_tls(void) {
    m_session.bind(m_socket);
}


tcp_transport & tcp_transport::operator=(tcp_transport && other_transport) {
    if (this != &other_transport) {
        m_socket    = std::move(other_transport.m_socket);
        m_session   = std::move(other_transport.m_session);
        m_secure    = std::move(other_transport.m_secure);
        m_address   = std::move(other_transport.m_address);
        m_port      = std::move(other_transport.m_port);

        other_transport.m_socket    = 0;
        other_transport.m_secure    = false;
        other_transport.m_port      = 0;
    }

    return *this;
}
